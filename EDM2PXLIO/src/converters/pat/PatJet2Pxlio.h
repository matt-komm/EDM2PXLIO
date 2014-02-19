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

#include "Pxl/Pxl/interface/Pxl.h"

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
            pxlParticle->setUserRecord<float>("trackCountingHighPurBJetTags",patObject.bDiscriminator("trackCountingHighPurBJetTags"));
            pxlParticle->setUserRecord<float>("trackCountingHighEffBJetTags",patObject.bDiscriminator("trackCountingHighEffBJetTags"));
            pxlParticle->setUserRecord<float>("combinedSecondaryVertexBJetTags",patObject.bDiscriminator("combinedSecondaryVertexBJetTags"));
            pxlParticle->setUserRecord<float>("jetBProbabilityBJetTags",patObject.bDiscriminator("jetBProbabilityBJetTags"));
            pxlParticle->setUserRecord<float>("jetProbabilityBJetTags",patObject.bDiscriminator("jetProbabilityBJetTags"));
            
            pxlParticle->setUserRecord<int>("numberOfDaughters",patObject.numberOfDaughters());
            
            pxlParticle->setUserRecord<float>("neutralEmEnergy",patObject.neutralEmEnergy());
            pxlParticle->setUserRecord<float>("chargedEmEnergy",patObject.chargedEmEnergy());
            pxlParticle->setUserRecord<float>("chargedHadronEnergy",patObject.chargedHadronEnergy());
            pxlParticle->setUserRecord<float>("neutralHadronEnergy",patObject.neutralHadronEnergy());
            pxlParticle->setUserRecord<float>("HFHadronEnergy",patObject.HFHadronEnergy());
            
            pxlParticle->setUserRecord<float>("neutralEmEnergyFraction",patObject.neutralEmEnergyFraction());
            pxlParticle->setUserRecord<float>("chargedEmEnergyFraction",patObject.chargedEmEnergyFraction());
            pxlParticle->setUserRecord<float>("chargedHadronEnergyFraction",patObject.chargedHadronEnergyFraction());
            pxlParticle->setUserRecord<float>("neutralHadronEnergyFraction",patObject.neutralHadronEnergyFraction());
            pxlParticle->setUserRecord<float>("HFHadronEnergyFraction",patObject.HFHadronEnergyFraction());
            
            pxlParticle->setUserRecord<float>("chargedMultiplicity",patObject.chargedMultiplicity());
            pxlParticle->setUserRecord<float>("neutralMultiplicity",patObject.neutralMultiplicity());
            pxlParticle->setUserRecord<float>("muonMultiplicity",patObject.muonMultiplicity());
            pxlParticle->setUserRecord<float>("electronMultiplicity",patObject.electronMultiplicity());
            
            float nhf =  ( patObject.neutralHadronEnergy() + patObject.HFHadronEnergy() ) /patObject.energy();
            pxlParticle->setUserRecord<float>("neutralHadronEnergyFraction",nhf);
            
            const reco::SecondaryVertexTagInfo *svTagInfo = patObject.tagInfoSecondaryVertex();
            if (svTagInfo  &&  svTagInfo->nVertices() > 0)
            {
                pxlParticle->setUserRecord<float>("secondaryVertexMass",svTagInfo->secondaryVertex(0).p4().mass());
            }
            if (patObject.genParton()) 
            {
                pxlParticle->setUserRecord<int>("partonFlavour",patObject.partonFlavour());
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

