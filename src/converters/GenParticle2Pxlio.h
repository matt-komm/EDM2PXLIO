#ifndef _GENPARTICLE2PXLIO_H_
#define _GENPARTICLE2PXLIO_H_

// system include files
#include <memory>

// user include files
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/EDAnalyzer.h"

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/Framework/interface/ESHandle.h"
#include "FWCore/Framework/interface/MakerMacros.h"

#include "FWCore/ParameterSet/interface/ParameterSet.h"

#include "FWCore/Utilities/interface/InputTag.h"

#include "DataFormats/Common/interface/Handle.h"

#include "SimGeneral/HepPDTRecord/interface/ParticleDataTable.h"

//#include "DataFormats/HepMCCandidate/interface/GenParticle.h"

#include "PhysicsTools/PatUtils/interface/StringParserTools.h"

#include "Pxl/Pxl/interface/Pxl.h"

#include "EDM2PXLIO/EDM2PXLIO/src/Collection2Pxlio.h"

#include "EDM2PXLIO/EDM2PXLIO/src/converters/Pat2Pxlio.h"

class GenParticle2Pxlio: public Collection2Pxlio<std::vector<reco::GenParticle>>
{
    protected:
        edm::ESHandle<ParticleDataTable> pdt_;
        std::vector<pxl::Particle*> pxlCollection;
    public:
        GenParticle2Pxlio(std::string name):
            Collection2Pxlio<std::vector<reco::GenParticle>>(name)
        {
        }
        
        virtual void convert(const edm::Event* edmEvent, const edm::EventSetup* iSetup, pxl::Event* pxlEvent)
        {
            iSetup->getData(pdt_);
            for (unsigned index=0; index<Collection2Pxlio<std::vector<reco::GenParticle>>::size(); ++index)
            {
                const std::vector<reco::GenParticle>* collection = Collection2Pxlio<std::vector<reco::GenParticle>>::getCollection(edmEvent,index);
                pxl::EventView* pxlEventView = Collection2Pxlio<std::vector<reco::GenParticle>>::findEventView(pxlEvent,Collection2Pxlio<std::vector<reco::GenParticle>>::getEventViewName(index));
                
                if (collection) {
                    for (unsigned iparticle=0; iparticle< collection->size(); ++iparticle) {
                        const reco::GenParticle genObject = (*collection)[iparticle];
                        pxl::Particle* pxlParticle = pxlEventView->create<pxl::Particle>();
                        pxlCollection.push_back(pxlParticle);
                        pxlParticle->setName(getNameFromID(genObject.pdgId()));
                        convertP4(genObject,pxlParticle);
                        convertObject(genObject,pxlParticle);
                        pxlCollection.push_back(pxlParticle);
                    }
                    convertCollection(collection,pxlCollection);
                }
            }
        }
        
        virtual std::string getNameFromID(int id)
        {
            const ParticleData* pd = pdt_->particle(id);
            if (!pd)
            {
                std::ostringstream name;
                name << "ID " << id;
                return name.str();
            } else {
                return pd->name();
            }
        }
        
        virtual void convertObject(const reco::GenParticle& genObject, pxl::Particle* pxlParticle)
        {
        }
        
        virtual void convertCollection(const std::vector<reco::GenParticle>* genObjectList, std::vector<pxl::Particle*> pxlParticleList)
        {
        }
        
        virtual void convertP4(const reco::GenParticle& genObject, pxl::Particle* pxlParticle)
        {
            pxlParticle->setP4(genObject.px(),genObject.py(),genObject.pz(),genObject.energy());
        }
        
        ~GenParticle2Pxlio()
        {
        }
};

#endif

