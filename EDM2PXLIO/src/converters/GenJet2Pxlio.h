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

#include "DataFormats/JetReco/interface/GenJet.h"

#include "PhysicsTools/PatUtils/interface/StringParserTools.h"

#include "Pxl/Pxl/interface/Pxl.h"

#include "EDM2PXLIO/EDM2PXLIO/src/Collection2Pxlio.h"

#include "EDM2PXLIO/EDM2PXLIO/src/converters/Pat2Pxlio.h"

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
        }
        
        ~GenJet2Pxlio()
        {
        }
};

#endif

