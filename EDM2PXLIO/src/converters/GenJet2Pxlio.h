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
#include "DataFormats/Math/interface/LorentzVector.h"

#include "FWCore/ParameterSet/interface/ParameterSet.h"

#include "FWCore/Utilities/interface/InputTag.h"

#include "DataFormats/Common/interface/Handle.h"

#include "DataFormats/JetReco/interface/GenJet.h"

#include "PhysicsTools/PatUtils/interface/StringParserTools.h"

#include "Pxl/Pxl/interface/Pxl.h"

#include "EDM2PXLIO/EDM2PXLIO/src/Collection2Pxlio.h"

#include "EDM2PXLIO/EDM2PXLIO/src/converters/Pat2Pxlio.h"

#include <PhysicsTools/CandUtils/interface/EventShapeVariables.h>

bool myfunction (pxl::Particle* i,pxl::Particle* j) { return (i.getPt()<j.getPt()); }

class GenJet2Pxlio: public Pat2Pxlio<reco::GenJet>
{
    protected:

    public:
        GenJet2Pxlio(std::string name):
            Pat2Pxlio<reco::GenJet>(name)
        {
        }
                
        virtual void convertObject(const reco::GenJet& genJet, pxl::Particle* pxlParticle)
        {
            pxlParticle->setUserRecord<int>("charge",genJet.charge());
            pxlParticle->setUserRecord<double>("vx",genJet.vx());
            pxlParticle->setUserRecord<double>("vy",genJet.vy());
            pxlParticle->setUserRecord<double>("vz",genJet.vz());
            pxlParticle->setUserRecord<int>("numberOfDaughters",genJet.numberOfDaughters());
            pxlParticle->setUserRecord<int>("nConstituents",genJet.nConstituents());
            pxlParticle->setUserRecord<double>("maxDistance",genJet.maxDistance());
            pxlParticle->setUserRecord<double>("emEnergy",genJet.emEnergy());
            pxlParticle->setUserRecord<double>("hadEnergy",genJet.hadEnergy());
            pxlParticle->setUserRecord<double>("constituentEtaPhiSpread",genJet.constituentEtaPhiSpread());
            pxlParticle->setUserRecord<double>("constituentPtDistribution",genJet.constituentPtDistribution());
            pxlParticle->setUserRecord<double>("etaetaMoment",genJet.etaetaMoment());
            pxlParticle->setUserRecord<double>("etaphiMoment",genJet.etaphiMoment());
            pxlParticle->setUserRecord<double>("phiphiMoment",genJet.phiphiMoment());
            pxlParticle->setUserRecord<double>("jetArea",genJet.jetArea());
            
            
            double sumDR=0.0;
            double jetMeanPt = 0.0;
            double jetMeanMass = 0.0;
            std::vector<const reco::GenParticle*> constituents = genJet.getGenConstituents();
            std::vector<math::XYZVector> list;
            std::vector<edm::Ptr<reco::Candidate>> can= genJet.getJetConstituents();
            
            for( unsigned int iConst=0; iConst<constituents.size(); ++iConst ) 
            {
                reco::Candidate::LorentzVector iConstituent = constituents[iConst]->p4();
                math::XYZVector ivec(iConstituent);
                list.push_back(ivec);
                float pt = iConstituent.Pt();
                float m = iConstituent.M();
                double dR = reco::deltaR (genJet, *(constituents[iConst]));
                sumDR+=dR;
                jetMeanPt+=pt*dR;
                jetMeanMass+=m*dR;
            }
            EventShapeVariables evVar(list);
            pxlParticle->setUserRecord<double>("jetMeanPt",jetMeanPt/sumDR);
            pxlParticle->setUserRecord<double>("jetMeanMass",jetMeanMass/sumDR);
            pxlParticle->setUserRecord<double>("sphericity",evVar.sphericity());
            pxlParticle->setUserRecord<double>("C",evVar.C());
            pxlParticle->setUserRecord<double>("D",evVar.D());
            pxlParticle->setUserRecord<double>("aplanarity",evVar.aplanarity());
            pxlParticle->setUserRecord<double>("D",evVar.D());
        }
        
        ~GenJet2Pxlio()
        {
        }
};

#endif

