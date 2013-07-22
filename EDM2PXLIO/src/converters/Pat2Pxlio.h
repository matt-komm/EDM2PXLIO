#ifndef _PAT2PXLIO_H_
#define _PAT2PXLIO_H_

// system include files
#include <memory>

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

#include "EDM2PXLIO/EDM2PXLIO/src/Collection2Pxlio.h"

template<class PatClass>
class Pat2Pxlio: public Collection2Pxlio<edm::View<PatClass>>
{
    protected:
        std::vector<pxl::Particle*> pxlCollection;
        
    public:
        Pat2Pxlio(std::string name):
            Collection2Pxlio<edm::View<PatClass>>(name)

        {
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
                    for (unsigned iparticle=0; iparticle< collection->size(); ++iparticle) {
                        const PatClass patObject = (*collection)[iparticle];
                        pxl::Particle* pxlParticle = pxlEventView->create<pxl::Particle>();
                        pxlCollection.push_back(pxlParticle);
                        pxlParticle->setName(Collection2Pxlio<edm::View<PatClass>>::getCollectionName(index));
                        convertP4(patObject,pxlParticle);
                        convertObject(patObject,pxlParticle);
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
        
        virtual void convertCollection(const edm::Handle<edm::View<PatClass>> patObjectList, std::vector<pxl::Particle*> pxlParticleList)
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

