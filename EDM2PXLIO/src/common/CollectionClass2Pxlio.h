#ifndef _PAT2PXLIO_H_
#define _PAT2PXLIO_H_

// system include files
#include <memory>
#include <unordered_map>

// user include files
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/EDAnalyzer.h"

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/Framework/interface/MakerMacros.h"

#include "FWCore/ParameterSet/interface/ParameterSet.h"

#include "FWCore/Utilities/interface/InputTag.h"

#include "DataFormats/Common/interface/Handle.h"

#include "DataFormats/PatCandidates/interface/Muon.h"

#include "PhysicsTools/PatUtils/interface/StringParserTools.h"

#include "Pxl/Pxl/interface/Pxl.h"

#include "EDM2PXLIO/EDM2PXLIO/src/common/Collection2Pxlio.h"

template<class Class>
class CollectionClass2Pxlio: public Collection2Pxlio<edm::View<Class>>
{
    protected:
        std::vector<pxl::Particle*> pxlCollection;
        std::vector<std::string> userRecordNames_;
        std::vector<StringObjectFunction<Class>*> userRecordsFunc_;

    public:
        CollectionClass2Pxlio(std::string name):
            Collection2Pxlio<edm::View<Class>>(name)
        {
        }

        virtual void parseParameter(const edm::ParameterSet& iConfig)
        {
            Collection2Pxlio<edm::View<Class>>::parseParameter(iConfig);
            if (iConfig.exists(Collection2Pxlio<edm::View<Class>>::name_+"UserRecords"))
            {
                std::vector<std::string> urList = iConfig.getParameter<std::vector<std::string>>(Collection2Pxlio<edm::View<Class>>::name_+"UserRecords");
                if (urList.size()%2!=0) {
                    return;
                }
                for (unsigned cnt=0; cnt< urList.size()-1;cnt+=2)
                {
                    StringObjectFunction<Class>* func = new StringObjectFunction<Class>(urList[cnt+1],true);
                    userRecordNames_.push_back(urList[cnt]);
                    userRecordsFunc_.push_back(func);
                }
            }
        }

        void fillUserRecords(const Class& classObject,pxl::Object* pxlObject)
        {
            for (unsigned cnt=0; cnt<userRecordNames_.size();++cnt)
            {
                pxlObject->setUserRecord<float>(userRecordNames_[cnt],(*userRecordsFunc_[cnt])(classObject));
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
                if (collection.product()) {
                    for (unsigned iparticle=0; iparticle< collection->size(); ++iparticle) 
                    {
                        const Class classObject = (*collection)[iparticle];
                        pxl::Particle* pxlParticle = pxlEventView->create<pxl::Particle>();
                        pxlCollection.push_back(pxlParticle);
                        pxlParticle->setName(Collection2Pxlio<edm::View<Class>>::getCollectionName(index));
                        convertP4(classObject,pxlParticle);
                        convertObject(classObject,pxlParticle);
                        fillUserRecords(classObject,pxlParticle);
                    }
                    if (collection->size()!=pxlCollection.size())
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

        virtual void convertP4(const Class& classObject, pxl::Particle* pxlParticle)
        {
            pxlParticle->setP4(classObject.px(),classObject.py(),classObject.pz(),classObject.energy());
        }

        ~CollectionClass2Pxlio()
        {
        }
};

#endif

