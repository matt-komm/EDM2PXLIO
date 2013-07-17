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

#include "DataFormats/VertexReco/interface/Vertex.h"

#include "PhysicsTools/PatUtils/interface/StringParserTools.h"

#include "Pxl/Pxl/interface/Pxl.h"

#include "EDM2PXLIO/EDM2PXLIO/src/Collection2Pxlio.h"

#include "EDM2PXLIO/EDM2PXLIO/src/converters/Pat2Pxlio.h"

class PatMuon2Pxlio: public Pat2Pxlio<pat::Muon>
{
    protected:
        const reco::Vertex* primaryVertex_;
        std::string vertexInputTag_;
    public:
        PatMuon2Pxlio(std::string name,std::string vertexInputTag=std::string()):
            Pat2Pxlio<pat::Muon>(name),
            vertexInputTag_(vertexInputTag)
        {
        }
        
        virtual void convert(const edm::Event* edmEvent, const edm::EventSetup* iSetup, pxl::Event* pxlEvent)
        {
            
            if (vertexInputTag_.length()>0) {
                edm::Handle<std::vector<reco::Vertex>> vertexList;
                edmEvent->getByLabel(vertexInputTag_,vertexList);
                primaryVertex_ = &(*vertexList)[0];
            }
            Pat2Pxlio<pat::Muon>::convert(edmEvent,iSetup,pxlEvent);
            
        }
                
        virtual void convertObject(const pat::Muon& patObject, pxl::Particle* pxlParticle)
        {
            pxlParticle->setCharge(patObject.charge());
            
            float relIso = (patObject.chargedHadronIso() + std::max(0., patObject.neutralHadronIso() +patObject.photonIso() - 0.5*patObject.puChargedHadronIso()))/patObject.pt();
            pxlParticle->setUserRecord<float>("relIso",relIso);
            
            pxlParticle->setUserRecord<float>("dB",patObject.dB());
            pxlParticle->setUserRecord<bool>("isGlobalMuon",patObject.isGlobalMuon());
            pxlParticle->setUserRecord<bool>("isTrackerMuon",patObject.isTrackerMuon());
            pxlParticle->setUserRecord<bool>("isPFMuon",patObject.isTrackerMuon());
            pxlParticle->setUserRecord<int>("numberOfValidMuonHits",patObject.globalTrack()->hitPattern().numberOfValidMuonHits());
            pxlParticle->setUserRecord<int>("numberOfMatchedStations",patObject.numberOfMatchedStations());
            pxlParticle->setUserRecord<int>("numberOfValidPixelHits",patObject.innerTrack()->hitPattern().numberOfValidPixelHits());
            pxlParticle->setUserRecord<int>("trackerLayersWithMeasurement",patObject.track()->hitPattern().trackerLayersWithMeasurement());
            if (primaryVertex_)
            {
                double dz = patObject.innerTrack()->dz(primaryVertex_->position());
                pxlParticle->setUserRecord<float>("dz",dz);
            }
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

