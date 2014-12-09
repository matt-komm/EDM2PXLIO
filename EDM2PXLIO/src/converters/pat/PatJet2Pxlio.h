#ifndef _PATJET2PXLIO_H_
#define _PATJET2PXLIO_H_

// system include files
#include <memory>

// user include files
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/EDAnalyzer.h"

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Utilities/interface/Exception.h"
#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/Framework/interface/MakerMacros.h"

#include "FWCore/ParameterSet/interface/ParameterSet.h"

#include "FWCore/Utilities/interface/InputTag.h"

#include "DataFormats/Common/interface/Handle.h"

#include "DataFormats/PatCandidates/interface/Jet.h"

#include "PhysicsTools/PatUtils/interface/StringParserTools.h"

#include "Pxl/Pxl/interface/pxl/core.hh"
#include "Pxl/Pxl/interface/pxl/hep.hh"

#include "EDM2PXLIO/EDM2PXLIO/src/common/Collection2Pxlio.h"

#include "EDM2PXLIO/EDM2PXLIO/src/common/CollectionClass2Pxlio.h"

#include "EDM2PXLIO/EDM2PXLIO/src/common/ConverterFactory.h"

class PatJet2Pxlio: public CollectionClass2Pxlio<pat::Jet>
{
    protected:

    public:
        PatJet2Pxlio(std::string name):
            CollectionClass2Pxlio<pat::Jet>(name)
        {
        }
        
        static void init()
        {
            ConverterFactory* fac = ConverterFactory::getInstance();
            fac->registerConverter(new ConverterProducerTmpl<PatJet2Pxlio>("PatJet2Pxlio"));
        }
                
        virtual void convertObject(const pat::Jet& patObject, pxl::Particle* pxlParticle)
        {

            CollectionClass2Pxlio<pat::Jet>::convertObject(patObject, pxlParticle);
            pxlParticle->setP4(patObject.px(),patObject.py(),patObject.pz(),patObject.energy());
            pxlParticle->setUserRecord("trackCountingHighPurBJetTags",patObject.bDiscriminator("trackCountingHighPurBJetTags"));
            pxlParticle->setUserRecord("trackCountingHighEffBJetTags",patObject.bDiscriminator("trackCountingHighEffBJetTags"));
            pxlParticle->setUserRecord("combinedSecondaryVertexBJetTags",patObject.bDiscriminator("combinedSecondaryVertexBJetTags"));
            pxlParticle->setUserRecord("jetBProbabilityBJetTags",patObject.bDiscriminator("jetBProbabilityBJetTags"));
            pxlParticle->setUserRecord("jetProbabilityBJetTags",patObject.bDiscriminator("jetProbabilityBJetTags"));
            
            pxlParticle->setUserRecord("numberOfDaughters",patObject.numberOfDaughters());
            
            pxlParticle->setUserRecord("neutralEmEnergy",patObject.neutralEmEnergy());
            pxlParticle->setUserRecord("chargedEmEnergy",patObject.chargedEmEnergy());
            pxlParticle->setUserRecord("chargedHadronEnergy",patObject.chargedHadronEnergy());
            pxlParticle->setUserRecord("neutralHadronEnergy",patObject.neutralHadronEnergy());
            pxlParticle->setUserRecord("HFHadronEnergy",patObject.HFHadronEnergy());
            
            pxlParticle->setUserRecord("neutralEmEnergyFraction",patObject.neutralEmEnergyFraction());
            pxlParticle->setUserRecord("chargedEmEnergyFraction",patObject.chargedEmEnergyFraction());
            pxlParticle->setUserRecord("chargedHadronEnergyFraction",patObject.chargedHadronEnergyFraction());
            pxlParticle->setUserRecord("neutralHadronEnergyFraction",patObject.neutralHadronEnergyFraction());
            pxlParticle->setUserRecord("HFHadronEnergyFraction",patObject.HFHadronEnergyFraction());
            
            pxlParticle->setUserRecord("chargedMultiplicity",patObject.chargedMultiplicity());
            pxlParticle->setUserRecord("neutralMultiplicity",patObject.neutralMultiplicity());
            pxlParticle->setUserRecord("muonMultiplicity",patObject.muonMultiplicity());
            pxlParticle->setUserRecord("electronMultiplicity",patObject.electronMultiplicity());
            
            float nhf =  ( patObject.neutralHadronEnergy() + patObject.HFHadronEnergy() ) /patObject.energy();
            pxlParticle->setUserRecord("neutralHadronEnergyFraction",nhf);
            
            const reco::SecondaryVertexTagInfo *svTagInfo = patObject.tagInfoSecondaryVertex();
            if (svTagInfo  &&  svTagInfo->nVertices() > 0)
            {
                pxlParticle->setUserRecord("secondaryVertexMass",svTagInfo->secondaryVertex(0).p4().mass());
            }
            if (patObject.genParton()) 
            {
                pxlParticle->setUserRecord("partonFlavour",patObject.partonFlavour());
            }

        }
        
        virtual void convertCollection(const edm::Handle<edm::View<pat::Jet>> patObjectList, std::vector<pxl::Particle*>& pxlParticleList)
        {
            CollectionClass2Pxlio<pat::Jet>::convertCollection(patObjectList, pxlParticleList);
        }
        
        ~PatJet2Pxlio()
        {
        }
};

#endif

