#ifndef _COLLECTIONCLASS2PXLIO_H_
#define _COLLECTIONCLASS2PXLIO_H_

// system include files
#include <memory>
#include <unordered_map>
#include <utility>

// user include files
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/EDAnalyzer.h"

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/Framework/interface/MakerMacros.h"

#include "FWCore/ParameterSet/interface/ParameterSet.h"

#include "FWCore/Utilities/interface/InputTag.h"

#include "DataFormats/Common/interface/Handle.h"

#include "PhysicsTools/PatUtils/interface/StringParserTools.h"

#include "Pxl/Pxl/interface/pxl/core.hh"
#include "Pxl/Pxl/interface/pxl/hep.hh"

#include "EDM2PXLIO/EDM2PXLIO/src/common/Collection2Pxlio.h"

template<class Class>
class CollectionClass2Pxlio: public Collection2Pxlio<edm::View<Class>>
{
    protected:
        std::vector<pxl::Particle*> pxlCollection;
        std::vector<std::pair<std::string,StringObjectFunction<Class>*>> _userRecordsFcts;
        StringCutObjectSelector<Class>* _cutFct;

    public:
        CollectionClass2Pxlio(std::string name):
            Collection2Pxlio<edm::View<Class>>(name),
            _cutFct(nullptr)
        {
        }

        virtual void parseParameter(const edm::ParameterSet& globalConfig)
        {
            Collection2Pxlio<edm::View<Class>>::parseParameter(globalConfig);
            if (globalConfig.exists(Collection2Pxlio<edm::View<Class>>::name_))
            {
                const edm::ParameterSet& iConfig = globalConfig.getParameter<edm::ParameterSet>(Collection2Pxlio<edm::View<Class>>::name_);
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
            }
        }
        void fillUserRecords(const Class& classObject,pxl::Object* pxlObject)
        {
            for (unsigned iur=0; iur<_userRecordsFcts.size();++iur)
            {
                pxlObject->setUserRecord(_userRecordsFcts[iur].first,(*_userRecordsFcts[iur].second)(classObject));
            }
        }

        virtual void convert(const edm::Event* edmEvent, const edm::EventSetup* iSetup, pxl::Event* pxlEvent)
        {
            Collection2Pxlio<edm::View<Class>>::convert(edmEvent, iSetup, pxlEvent);
            for (unsigned index=0; index<Collection2Pxlio<edm::View<Class>>::size(); ++index)
            {
                const edm::Handle<edm::View<Class>> collection = Collection2Pxlio<edm::View<Class>>::getCollection(edmEvent,index);
                pxl::EventView* pxlEventView = Collection2Pxlio<edm::View<Class>>::findEventView(pxlEvent,Collection2Pxlio<edm::View<Class>>::getEventViewName(index));
                pxlCollection.clear();
                unsigned int nSkipped=0;
                if (collection.product()) {
                    for (unsigned iparticle=0; iparticle< collection->size(); ++iparticle) 
                    {
                        const Class classObject = (*collection)[iparticle];
                        if (_cutFct)
                        {
                            if (!((*_cutFct)(classObject)))
                            {
                                ++nSkipped;
                                continue;
                            }
                        }
                        pxl::Particle* pxlParticle = pxlEventView->create<pxl::Particle>();
                        pxlCollection.push_back(pxlParticle);
                        pxlParticle->setName(Collection2Pxlio<edm::View<Class>>::getCollectionName(index));
                        
                        convertObject(classObject,pxlParticle);
                        fillUserRecords(classObject,pxlParticle);
                    }
                    if (collection->size()!=(pxlCollection.size()+nSkipped))
                    {
                        throw cms::Exception("Pat2Pxlio::convert") << "converted pxl particles differ in size compared to input collection";
                    }
                    convertCollection(collection,pxlCollection);
                }
            }
        }

        virtual void convertObject(const Class& classObject, pxl::Particle* pxlParticle)
        {
        }

        virtual void convertCollection(const edm::Handle<edm::View<Class>> classObjectList, std::vector<pxl::Particle*>& pxlParticleList)
        {
        }

        ~CollectionClass2Pxlio()
        {
        }
};

#endif

