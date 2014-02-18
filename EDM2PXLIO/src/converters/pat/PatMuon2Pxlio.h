#ifndef _PATMUON2PXLIO_H_
#define _PATMUON2PXLIO_H_

// system include files
#include <memory>

// user include files
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/EDAnalyzer.h"

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/Framework/interface/MakerMacros.h"

#include "FWCore/ParameterSet/interface/ParameterSet.h"

#include "FWCore/Utilities/interface/InputTag.h"

#include "DataFormats/Common/interface/Handle.h"

#include "DataFormats/PatCandidates/interface/Muon.h"

#include "DataFormats/VertexReco/interface/Vertex.h"

#include "PhysicsTools/PatUtils/interface/StringParserTools.h"

#include "Pxl/Pxl/interface/Pxl.h"

#include "EDM2PXLIO/EDM2PXLIO/src/common/Collection2Pxlio.h"

#include "EDM2PXLIO/EDM2PXLIO/src/common/CollectionClass2Pxlio.h"

#include "EDM2PXLIO/EDM2PXLIO/src/provider/PrimaryVertexProvider.h"

class PatMuon2Pxlio: public CollectionClass2Pxlio<pat::Muon>
{
    protected:
        PrimaryVertexProvider* primaryVertexProvider_;
    public:
        PatMuon2Pxlio(std::string name):
            CollectionClass2Pxlio<pat::Muon>(name),
            primaryVertexProvider_(0)
        {
            primaryVertexProvider_=createProvider<PrimaryVertexProvider>();
        }
                
        virtual void convertObject(const pat::Muon& patObject, pxl::Particle* pxlParticle)
        {
            CollectionClass2Pxlio<pat::Muon>::convertObject(patObject, pxlParticle);
        
            pxlParticle->setCharge(patObject.charge());
            
            float relIso = (patObject.chargedHadronIso() + std::max(0., patObject.neutralHadronIso() +patObject.photonIso() - 0.5*patObject.puChargedHadronIso()))/patObject.pt();
            pxlParticle->setUserRecord<float>("relIso",relIso);
            
            pxlParticle->setUserRecord<bool>("isGlobalMuon",patObject.isGlobalMuon());
            pxlParticle->setUserRecord<bool>("isTrackerMuon",patObject.isTrackerMuon());
            pxlParticle->setUserRecord<bool>("isPFMuon",patObject.isPFMuon());
            
            pxlParticle->setUserRecord<int>("numberOfValidMuonHits",patObject.globalTrack()->hitPattern().numberOfValidMuonHits());
            pxlParticle->setUserRecord<int>("numberOfMatchedStations",patObject.numberOfMatchedStations());
            pxlParticle->setUserRecord<int>("numberOfValidPixelHits",patObject.innerTrack()->hitPattern().numberOfValidPixelHits());
            pxlParticle->setUserRecord<int>("trackerLayersWithMeasurement",patObject.track()->hitPattern().trackerLayersWithMeasurement());


            if (primaryVertexProvider_->getPrimaryVertex())
            {
                double dz = patObject.innerTrack()->dz(primaryVertexProvider_->getPrimaryVertex()->position());
                double dxy = patObject.innerTrack()->dxy(primaryVertexProvider_->getPrimaryVertex()->position());
                pxlParticle->setUserRecord<float>("dz",dz);
                pxlParticle->setUserRecord<float>("dxy",dxy);
                pxlParticle->setUserRecord<bool>("isTightMuon",patObject.isTightMuon(*primaryVertexProvider_->getPrimaryVertex()));
            }

            pxlParticle->setUserRecord<float>("chi2",patObject.globalTrack()->normalizedChi2());

            pxlParticle->setUserRecord<float>("chargedHadronIso",patObject.chargedHadronIso());
            pxlParticle->setUserRecord<float>("neutralHadronIso",patObject.neutralHadronIso());
            pxlParticle->setUserRecord<float>("photonIso",patObject.photonIso());
            pxlParticle->setUserRecord<float>("puChargedHadronIso",patObject.puChargedHadronIso());
        }
        
        virtual void convertCollection(const edm::Handle<edm::View<pat::Muon>> patObjectList, std::vector<pxl::Particle*>& pxlParticleList)
        {
            CollectionClass2Pxlio<pat::Muon>::convertCollection(patObjectList, pxlParticleList);
        }
        
        ~PatMuon2Pxlio()
        {
        }
};

#endif

