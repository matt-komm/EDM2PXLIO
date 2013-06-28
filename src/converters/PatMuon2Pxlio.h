#ifndef _PATMUON2PXLIO_H_
#define _PATMUON2PXLIO_H_

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

#include "DataFormats/PatCandidates/interface/Muon.h"

#include "PhysicsTools/PatUtils/interface/StringParserTools.h"

#include "Pxl/Pxl/interface/Pxl.h"

#include "EDM2PXLIO/EDM2PXLIO/src/Collection2Pxlio.h"

#include "EDM2PXLIO/EDM2PXLIO/src/converters/Pat2Pxlio.h"

class PatMuon2Pxlio: public Pat2Pxlio<pat::Muon>
{
    public:
        PatMuon2Pxlio(std::string name):
            Pat2Pxlio<pat::Muon>(name)
        {
        }
                
        virtual void convertObject(const pat::Muon patObject, pxl::Particle* pxlParticle)
        {
            pxlParticle->setUserRecord<float>("chi2",patObject.globalTrack()->normalizedChi2());
        }
        
        virtual void convertCollection(const std::vector<pat::Muon>* patObjectList, std::vector<pxl::Particle*> pxlParticleList)
        {
        }
        
        ~PatMuon2Pxlio()
        {
        }
};

#endif

