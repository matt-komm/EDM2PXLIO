#ifndef _CANDIDATE2PXLIO_H_
#define _CANDIDATE2PXLIO_H_

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

#include <DataFormats/Candidate/interface/Candidate.h>
#include <DataFormats/Candidate/interface/LeafCandidate.h>

#include "PhysicsTools/PatUtils/interface/StringParserTools.h"

#include "Pxl/Pxl/interface/Pxl.h"

#include "EDM2PXLIO/EDM2PXLIO/src/common/Collection2Pxlio.h"

#include "EDM2PXLIO/EDM2PXLIO/src/common/CollectionClass2Pxlio.h"

#include <PhysicsTools/CandUtils/interface/EventShapeVariables.h>

#include "EDM2PXLIO/EDM2PXLIO/src/common/ConverterFactory.h"


class Candidate2Pxlio: public CollectionClass2Pxlio<reco::LeafCandidate>
{
    protected:

    public:
        Candidate2Pxlio(std::string name):
            CollectionClass2Pxlio<reco::LeafCandidate>(name)
        {
        }
        
        static void init()
        {
            ConverterFactory* fac = ConverterFactory::getInstance();
            fac->registerConverter(new ConverterProducerTmpl<Candidate2Pxlio>("Candidate2Pxlio"));
        }
                
        virtual void convertObject(const reco::LeafCandidate& candidate, pxl::Particle* pxlParticle)
        {
            CollectionClass2Pxlio<reco::LeafCandidate>::convertObject(candidate, pxlParticle);
            pxlParticle->setP4(candidate.px(),candidate.py(),candidate.pz(),candidate.energy());
            
            pxlParticle->setUserRecord<int>("charge",candidate.charge());
            pxlParticle->setUserRecord<double>("vx",candidate.vx());
            pxlParticle->setUserRecord<double>("vy",candidate.vy());
            pxlParticle->setUserRecord<double>("vz",candidate.vz());
            
            
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
        
        ~Candidate2Pxlio()
        {
        }
};

#endif

