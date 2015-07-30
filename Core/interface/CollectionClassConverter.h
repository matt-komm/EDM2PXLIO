#ifndef __EDM2PXLIO_CORE_COLLECTIONCLASSCONVERTER_H__
#define __EDM2PXLIO_CORE_COLLECTIONCLASSCONVERTER_H__

// system include files
#include <unordered_map>
#include <utility>

// user include files

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/EventSetup.h"

#include "FWCore/ParameterSet/interface/ParameterSet.h"

#include "FWCore/Utilities/interface/InputTag.h"

#include "DataFormats/Common/interface/Handle.h"
#include "DataFormats/Common/interface/View.h"

#include "PhysicsTools/PatUtils/interface/StringParserTools.h"

#include "Pxl/Pxl/interface/pxl/core.hh"
#include "Pxl/Pxl/interface/pxl/hep.hh"

#include "EDM2PXLIO/Core/interface/CollectionConverter.h"

#include "EDM2PXLIO/Core/interface/ValueMapAccessor.h"
#include "EDM2PXLIO/Core/interface/ValueMapAccessorFactory.h"

#include "EDM2PXLIO/Core/interface/TriggerResultFilter.h"

namespace edm2pxlio
{

template<class Class>
class CollectionClassConverter: public CollectionConverter<edm::View<Class>>
{
    private:
        typedef CollectionConverter<edm::View<Class>> Base;
    protected:
        std::vector<std::pair<std::string,StringObjectFunction<Class>*>> _userRecordsFcts;
        StringCutObjectSelector<Class>* _cutFct;
        std::vector<ValueMapAccessor*> _valueMapAccessors;
        TriggerResultFilter _triggerResultFilter;

    public:
        CollectionClassConverter(const std::string& name, const edm::ParameterSet& globalConfig, edm::ConsumesCollector& consumesCollector):
            Base(name, globalConfig, consumesCollector),
            _cutFct(nullptr)
        {
            if (globalConfig.exists(Base::getName()))
            {
                const edm::ParameterSet& iConfig = globalConfig.getParameter<edm::ParameterSet>(Base::getName());
                if (iConfig.exists("userRecords"))
                {
                    const edm::ParameterSet& urConfig = iConfig.getParameter<edm::ParameterSet>("userRecords");
                    const std::vector<std::string> userRecordNames = urConfig.getParameterNamesForType<std::string>();
                    for (unsigned int iur=0; iur< userRecordNames.size(); ++iur)
                    {
                        StringObjectFunction<Class>* fct = new StringObjectFunction<Class>(urConfig.getParameter<std::string>(userRecordNames[iur]),true);
                        _userRecordsFcts.push_back(std::make_pair(userRecordNames[iur],fct));
                    }
                }
                if (iConfig.exists("select"))
                {
                    _cutFct = new StringCutObjectSelector<Class>(iConfig.getParameter<std::string>("select"), true);
                }
                
                if (iConfig.exists("valueMaps"))
                {
                    const edm::ParameterSet& vmConfigs = iConfig.getParameter<edm::ParameterSet>("valueMaps");
                    const std::vector<std::string> valueMapConfigs = vmConfigs.getParameterNamesForType<edm::ParameterSet>();
                    for (unsigned int ivm=0; ivm< valueMapConfigs.size(); ++ivm)
                    {
                        const std::string& vmPluginName = valueMapConfigs[ivm];
                        const edm::ParameterSet& vmConfig = vmConfigs.getParameter<edm::ParameterSet>(vmPluginName);
                        const std::string vmPluginType = vmConfig.getParameter<std::string>("type");
                        
                        edm2pxlio::ValueMapAccessor* accessor = edm2pxlio::ValueMapAccessorFactory::get()->tryToCreate(vmPluginType,vmPluginName,vmConfig,consumesCollector);
                        if (accessor)
                        {
                            _valueMapAccessors.push_back(accessor);
                        }
                        else
                        {
                            edm::LogWarning("ValueMapAccessor plugin not found: ") << vmPluginType;
                        }
                    }
                }
                
                if (iConfig.exists("triggerFilter"))
                {
                    _triggerResultFilter.parseConfiguration(iConfig.getParameter<std::vector<edm::ParameterSet>>("triggerFilter"));
                }
            }
        }
        
        void fillUserRecords(const Class& classObject, pxl::Object* pxlObject) const
        {
            for (unsigned iur=0; iur<_userRecordsFcts.size();++iur)
            {
                pxlObject->setUserRecord(_userRecordsFcts[iur].first,(*_userRecordsFcts[iur].second)(classObject));
            }
        }

        virtual void convert(const edm::Event* edmEvent, const edm::EventSetup* iSetup, pxl::Event* pxlEvent) const
        {
            Base::convert(edmEvent, iSetup, pxlEvent);
            if (!_triggerResultFilter.checkPath(*edmEvent))
            {
                return;
            }
            
            for (unsigned int ivm = 0; ivm < _valueMapAccessors.size(); ++ ivm)
            {
                _valueMapAccessors[ivm]->convert(edmEvent,iSetup,pxlEvent);
            }

            for (unsigned index=0; index<CollectionConverter<edm::View<Class>>::size(); ++index)
            {

                const edm::Handle<edm::View<Class>> collection = Base::getCollection(edmEvent,index);
                pxl::EventView* pxlEventView = Base::findEventView(pxlEvent,Base::getEventViewName(index));
                unsigned int nSkipped=0;
                if (collection.product())
                {
                    std::vector<pxl::Particle*> pxlParticles;
                    for (unsigned iparticle=0; iparticle< collection->size(); ++iparticle) 
                    {
                        const Class& classObject = (*collection)[iparticle];
                        if (_cutFct!=nullptr)
                        {
                            if (!((*_cutFct)(classObject)))
                            {
                                ++nSkipped;
                                continue;
                            }
                        }
                        pxl::Particle* pxlParticle = pxlEventView->create<pxl::Particle>();
                        pxlParticles.push_back(pxlParticle);
                        pxlParticle->setName(Base::getCollectionName(index));
                        pxlParticle->setUserRecord("index",iparticle);
                        
                        this->convertObject(classObject,pxlParticle);
                        fillUserRecords(classObject,pxlParticle);
                        
                        for (unsigned int ivm = 0; ivm < _valueMapAccessors.size(); ++ ivm)
                        {
                            _valueMapAccessors[ivm]->accessValues(collection->id(),collection->refAt(iparticle).key(), pxlParticle);
                        }
                    }
                    if (collection->size()!=(pxlParticles.size()+nSkipped))
                    {
                        throw cms::Exception(Base::getName()+"::convert")
                        << "converted pxl particles differ in size compared to input collection '"
                        <<collection.provenance()->moduleLabel ()<<"'"
                        <<": N(collection)="<<collection->size()<<", "
                        <<"N(particles)="<<pxlParticles.size()<<", "
                        <<"N(skipped)="<<nSkipped;
                    }
                    this->convertCollection(collection,pxlParticles);
                }
            }
        }

        virtual void convertObject(const Class& classObject, pxl::Particle* pxlParticle) const
        {
        }

        virtual void convertCollection(const edm::Handle<edm::View<Class>> classCollection, std::vector<pxl::Particle*>& pxlParticles) const
        {
        }

        virtual ~CollectionClassConverter()
        {
        }
};

}

#endif

