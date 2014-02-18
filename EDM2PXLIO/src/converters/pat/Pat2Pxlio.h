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

template<class PatClass>
class Pat2Pxlio: public Collection2Pxlio<edm::View<PatClass>>
{
    protected:
        std::vector<pxl::Particle*> pxlCollection;
        std::vector<std::string> userRecordNames_;
        std::vector<StringObjectFunction<PatClass>*> userRecordsFunc_;

    public:
        Pat2Pxlio(std::string name):
            Collection2Pxlio<edm::View<PatClass>>(name)
        {
        }

        virtual void parseParameter(const edm::ParameterSet& iConfig)
        {
            Collection2Pxlio<edm::View<PatClass>>::parseParameter(iConfig);
            if (iConfig.exists(Collection2Pxlio<edm::View<PatClass>>::name_+"UserRecords"))
            {
                std::vector<std::string> urList = iConfig.getParameter<std::vector<std::string>>(Collection2Pxlio<edm::View<PatClass>>::name_+"UserRecords");
                if (urList.size()%2!=0) {
                    return;
                }
                for (unsigned cnt=0; cnt< urList.size()-1;cnt+=2)
                {
                    StringObjectFunction<PatClass>* func = new StringObjectFunction<PatClass>(urList[cnt+1],true);
                    userRecordNames_.push_back(urList[cnt]);
                    userRecordsFunc_.push_back(func);
                }
            }
        }

        void fillUserRecords(const PatClass& patObject,pxl::Object* pxlObject)
        {
            for (unsigned cnt=0; cnt<userRecordNames_.size();++cnt)
            {
                pxlObject->setUserRecord<float>(userRecordNames_[cnt],(*userRecordsFunc_[cnt])(patObject));
            }

        }

        virtual void convert(const edm::Event* edmEvent, const edm::EventSetup* iSetup, pxl::Event* pxlEvent)
        {
            Collection2Pxlio<edm::View<PatClass>>::convert(edmEvent, iSetup, pxlEvent);
            for (unsigned index=0; index<Collection2Pxlio<edm::View<PatClass>>::size(); ++index)
            {
                const edm::Handle<edm::View<PatClass>> collection = Collection2Pxlio<edm::View<PatClass>>::getCollection(edmEvent,index);
                pxl::EventView* pxlEventView = Collection2Pxlio<edm::View<PatClass>>::findEventView(pxlEvent,Collection2Pxlio<edm::View<PatClass>>::getEventViewName(index));
                pxlCollection.clear();
                if (collection.product()) {
                    for (unsigned iparticle=0; iparticle< collection->size(); ++iparticle) 
                    {
                        const PatClass patObject = (*collection)[iparticle];
                        pxl::Particle* pxlParticle = pxlEventView->create<pxl::Particle>();
                        pxlCollection.push_back(pxlParticle);
                        pxlParticle->setName(Collection2Pxlio<edm::View<PatClass>>::getCollectionName(index));
                        convertP4(patObject,pxlParticle);
                        convertObject(patObject,pxlParticle);
                        fillUserRecords(patObject,pxlParticle);
                    }
                    if (collection->size()!=pxlCollection.size())
                    {
                        throw cms::Exception("Pat2Pxlio::convert") << "converted pxl particles differ in size compared to input collection";
                    }
                    convertCollection(collection,pxlCollection);
                }
            }
        }

        virtual void convertObject(const PatClass& patObject, pxl::Particle* pxlParticle)
        {

        }

        virtual void convertCollection(const edm::Handle<edm::View<PatClass>> patObjectList, std::vector<pxl::Particle*>& pxlParticleList)
        {
        }

        virtual void convertP4(const PatClass& patObject, pxl::Particle* pxlParticle)
        {
            pxlParticle->setP4(patObject.px(),patObject.py(),patObject.pz(),patObject.energy());
        }

        ~Pat2Pxlio()
        {
        }
};

#endif

