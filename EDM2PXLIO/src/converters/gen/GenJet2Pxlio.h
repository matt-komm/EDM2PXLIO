#ifndef _GENJET2PXLIO_H_
#define _GENJET2PXLIO_H_

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

#include "Pxl/Pxl/interface/pxl/core.hh"
#include "Pxl/Pxl/interface/pxl/hep.hh"

#include "EDM2PXLIO/EDM2PXLIO/src/common/Collection2Pxlio.h"

#include "EDM2PXLIO/EDM2PXLIO/src/common/CollectionClass2Pxlio.h"

#include <PhysicsTools/CandUtils/interface/EventShapeVariables.h>

#include "EDM2PXLIO/EDM2PXLIO/src/common/ConverterFactory.h"

bool myfunction (pxl::Particle* i,pxl::Particle* j) { return (i->getPt()<j->getPt()); }

class GenJet2Pxlio: public CollectionClass2Pxlio<reco::GenJet>
{
    protected:

    public:
        GenJet2Pxlio(std::string name):
            CollectionClass2Pxlio<reco::GenJet>(name)
        {
        }
        
        static void init()
        {
            ConverterFactory* fac = ConverterFactory::getInstance();
            fac->registerConverter(new ConverterProducerTmpl<GenJet2Pxlio>("GenJet2Pxlio"));
        }
                
        virtual void convertObject(const reco::GenJet& genJet, pxl::Particle* pxlParticle)
        {
            CollectionClass2Pxlio<reco::GenJet>::convertObject(genJet, pxlParticle);
            pxlParticle->setP4(genJet.px(),genJet.py(),genJet.pz(),genJet.energy());
            
            pxlParticle->setUserRecord("charge",genJet.charge());
            pxlParticle->setUserRecord("vx",genJet.vx());
            pxlParticle->setUserRecord("vy",genJet.vy());
            pxlParticle->setUserRecord("vz",genJet.vz());
            pxlParticle->setUserRecord("numberOfDaughters",genJet.numberOfDaughters());
            pxlParticle->setUserRecord("nConstituents",genJet.nConstituents());
            pxlParticle->setUserRecord("maxDistance",genJet.maxDistance());
            pxlParticle->setUserRecord("emEnergy",genJet.emEnergy());
            pxlParticle->setUserRecord("hadEnergy",genJet.hadEnergy());
            pxlParticle->setUserRecord("constituentEtaPhiSpread",genJet.constituentEtaPhiSpread());
            pxlParticle->setUserRecord("constituentPtDistribution",genJet.constituentPtDistribution());
            pxlParticle->setUserRecord("etaetaMoment",genJet.etaetaMoment());
            pxlParticle->setUserRecord("etaphiMoment",genJet.etaphiMoment());
            pxlParticle->setUserRecord("phiphiMoment",genJet.phiphiMoment());
            pxlParticle->setUserRecord("jetArea",genJet.jetArea());
            
            
            /* calculates variables sensitive to substructure
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
            */
        }
        
        ~GenJet2Pxlio()
        {
        }
};

#endif

