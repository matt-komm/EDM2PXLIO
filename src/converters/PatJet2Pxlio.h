#ifndef _PATJET2PXLIO_H_
#define _PATJET2PXLIO_H_

// system include files
#include <memory>

// user include files
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/EDAnalyzer.h"

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"

#include "FWCore/ParameterSet/interface/ParameterSet.h"

#include "FWCore/Utilities/interface/InputTag.h"

#include "DataFormats/Common/interface/Handle.h"

#include "DataFormats/PatCandidates/interface/Jet.h"

#include "PhysicsTools/PatUtils/interface/StringParserTools.h"

#include "Pxl/Pxl/interface/Pxl.h"

#include "EDM2PXLIO/EDM2PXLIO/src/Collection2Pxlio.h"

#include "EDM2PXLIO/EDM2PXLIO/src/converters/Pat2Pxlio.h"

#include "CMGTools/External/interface/PileupJetIdentifier.h"

class PatJet2Pxlio: public Pat2Pxlio<pat::Jet>
{
    protected:
        std::string puJetInputTag_;
        const edm::ValueMap<StoredPileupJetIdentifier>* jetIDs_;
    public:
        PatJet2Pxlio(std::string name, std::string puJetInputTag=std::string()):
            Pat2Pxlio<pat::Jet>(name),
            puJetInputTag_(puJetInputTag)
        {
        }
        
        virtual void convert(const edm::Event* edmEvent, pxl::Event* pxlEvent)
        {
            
            if (puJetInputTag_.length()>0) 
            {
                edm::Handle<edm::ValueMap<StoredPileupJetIdentifier> > jetIDs;
                edmEvent->getByLabel(puJetInputTag_, jetIDs);
                jetIDs_=jetIDs.product();
            }
            Pat2Pxlio<pat::Jet>::convert(edmEvent,pxlEvent);
        } 
                
        virtual void convertObject(const pat::Jet& patObject, pxl::Particle* pxlParticle)
        {
            pxlParticle->setUserRecord<int>("numberOfDaughters",patObject.numberOfDaughters());
            pxlParticle->setUserRecord<int>("electronMultiplicity",patObject.electronMultiplicity());
            pxlParticle->setUserRecord<int>("muonMultiplicity",patObject.muonMultiplicity());
            pxlParticle->setUserRecord<float>("trackCountingHighPurBJetTags",patObject.bDiscriminator("trackCountingHighPurBJetTags"));
            pxlParticle->setUserRecord<float>("trackCountingHighEffBJetTags",patObject.bDiscriminator("trackCountingHighEffBJetTags"));
            pxlParticle->setUserRecord<float>("combinedSecondaryVertexBJetTags",patObject.bDiscriminator("combinedSecondaryVertexBJetTags"));
            pxlParticle->setUserRecord<float>("jetBProbabilityBJetTags",patObject.bDiscriminator("jetBProbabilityBJetTags"));
            pxlParticle->setUserRecord<float>("jetProbabilityBJetTags",patObject.bDiscriminator("jetProbabilityBJetTags"));
            
            
            float nhf =  ( patObject.neutralHadronEnergy() + patObject.HFHadronEnergy() ) /patObject.energy();
	        pxlParticle->setUserRecord<float>("neutralHadronEnergyFraction",nhf);
	        
	        pxlParticle->setUserRecord<float>("neutralEmEnergyFraction",patObject.neutralEmEnergyFraction());
	        pxlParticle->setUserRecord<float>("chargedEmEnergyFraction",patObject.chargedEmEnergyFraction());
	        pxlParticle->setUserRecord<float>("chargedHadronEnergyFraction",patObject.chargedHadronEnergyFraction());
	        pxlParticle->setUserRecord<float>("chargedMultiplicity",patObject.chargedMultiplicity());
	        pxlParticle->setUserRecord<float>("neutralHadronEnergyFraction",patObject.neutralHadronEnergyFraction());
	        pxlParticle->setUserRecord<float>("neutralHadronEnergyFraction",patObject.neutralHadronEnergyFraction());

            
            
            
            const reco::SecondaryVertexTagInfo *svTagInfo = patObject.tagInfoSecondaryVertex();
            if (svTagInfo  &&  svTagInfo->nVertices() > 0)
            {
                pxlParticle->setUserRecord<float>("secondaryVertexMass",svTagInfo->secondaryVertex(0).p4().mass());
            }
            if (patObject.genParton()) 
            {
                pxlParticle->setUserRecord<int>("partonFlavour",patObject.partonFlavour());
            }
            
            if (jetIDs_)
            {
                pxlParticle->setUserRecord<float>("RMS",((*jetIDs_)[patObject.originalObjectRef()]).RMS());
            }
        }
        
        virtual void convertCollection(const std::vector<pat::Jet>* patObjectList, std::vector<pxl::Particle*> pxlParticleList)
        {
        }
        
        ~PatJet2Pxlio()
        {
        }
};

#endif

