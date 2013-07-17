#ifndef _PATMET2PXLIO_H_
#define _PATMET2PXLIO_H_

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

#include "DataFormats/PatCandidates/interface/MET.h"

#include "DataFormats/VertexReco/interface/Vertex.h"

#include "PhysicsTools/PatUtils/interface/StringParserTools.h"

#include "Pxl/Pxl/interface/Pxl.h"

#include "EDM2PXLIO/EDM2PXLIO/src/Collection2Pxlio.h"

#include "EDM2PXLIO/EDM2PXLIO/src/converters/Pat2Pxlio.h"

class PatMET2Pxlio: public Pat2Pxlio<pat::MET>
{
    public:
        PatMET2Pxlio(std::string name):
            Pat2Pxlio<pat::MET>(name)
        {
        }
        
        virtual void convert(const edm::Event* edmEvent, const edm::EventSetup* iSetup, pxl::Event* pxlEvent)
        {
            for (unsigned index=0; index<Collection2Pxlio<std::vector<pat::MET>>::size(); ++index)
            {
                const std::vector<pat::MET>* collection = Collection2Pxlio<std::vector<pat::MET>>::getCollection(edmEvent,index);
                pxl::EventView* pxlEventView = Collection2Pxlio<std::vector<pat::MET>>::findEventView(pxlEvent,Collection2Pxlio<std::vector<pat::MET>>::getEventViewName(index));
                
                if (collection) {
                    
                    const pat::MET patObject = collection->front();
                    pxl::Particle* pxlParticle = pxlEventView->create<pxl::Particle>();
                    pxlCollection.push_back(pxlParticle);
                    pxlParticle->setName(Collection2Pxlio<std::vector<pat::MET>>::getCollectionName(index));
                    convertP4(patObject,pxlParticle);
                    convertObject(patObject,pxlParticle);

                }
            }
        }
        
        virtual void convertObject(const pat::MET& patObject, pxl::Particle* pxlParticle)
        {
        }
        
        
        ~PatMET2Pxlio()
        {
        }
};

#endif

