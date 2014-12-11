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

#include "Pxl/Pxl/interface/pxl/core.hh"
#include "Pxl/Pxl/interface/pxl/hep.hh"

#include "EDM2PXLIO/EDM2PXLIO/src/common/Collection2Pxlio.h"

#include "EDM2PXLIO/EDM2PXLIO/src/common/CollectionClass2Pxlio.h"

#include "EDM2PXLIO/EDM2PXLIO/src/provider/PrimaryVertexProvider.h"

#include "EDM2PXLIO/EDM2PXLIO/src/common/ConverterFactory.h"

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
            
        static void init()
        {
            ConverterFactory* fac = ConverterFactory::getInstance();
            fac->registerConverter(new ConverterProducerTmpl<PatMuon2Pxlio>("PatMuon2Pxlio"));
        }
                
        virtual void convertObject(const pat::Muon& patObject, pxl::Particle* pxlParticle)
        {
            CollectionClass2Pxlio<pat::Muon>::convertObject(patObject, pxlParticle);
            pxlParticle->setP4(patObject.px(),patObject.py(),patObject.pz(),patObject.energy());
            pxlParticle->setCharge(patObject.charge());
            
            
            pxlParticle->setUserRecord("R04sumChargedHadronPt",patObject.pfIsolationR04().sumChargedHadronPt);
            pxlParticle->setUserRecord("R04sumNeutralHadronPt",patObject.pfIsolationR04().sumNeutralHadronEt);
            pxlParticle->setUserRecord("R04sumPhotonIso",patObject.pfIsolationR04().sumPhotonEt);
            pxlParticle->setUserRecord("R04sumPUPt",patObject.pfIsolationR04().sumPUPt);
            
            pxlParticle->setUserRecord("isGlobalMuon",patObject.isGlobalMuon());
            pxlParticle->setUserRecord("isTrackerMuon",patObject.isTrackerMuon());
            pxlParticle->setUserRecord("isPFMuon",patObject.isPFMuon());
            
            pxlParticle->setUserRecord("numberOfMatchedStations",patObject.numberOfMatchedStations());
            
            //ref to embedded Track (tracker+muon chambers)
            if (patObject.combinedMuon().get()!=nullptr)
            {
                const reco::Track* combinedTrack = patObject.combinedMuon().get();
                
                pxlParticle->setUserRecord("numberOfValidMuonHits",combinedTrack->hitPattern().numberOfValidMuonHits());
                pxlParticle->setUserRecord("numberOfLostMuonHits",combinedTrack->hitPattern().numberOfLostMuonHits());
                pxlParticle->setUserRecord("chi2",combinedTrack->normalizedChi2());
                pxlParticle->setUserRecord("ndof",combinedTrack->ndof());
            }
            
            //ref to embedded Track (tracker only)
            if (patObject.track().get()!=nullptr)
            {
                const reco::Track* trackerTrack = patObject.track().get();
                pxlParticle->setUserRecord("trackerLayersWithMeasurement",trackerTrack->hitPattern().trackerLayersWithMeasurement());
                pxlParticle->setUserRecord("numberOfValidPixelHits",trackerTrack->hitPattern().numberOfValidPixelHits());
                
                if (primaryVertexProvider_->getPrimaryVertex()!=nullptr)
                {
                    double dz = trackerTrack->dz(primaryVertexProvider_->getPrimaryVertex()->position());
                    double dxy = trackerTrack->dxy(primaryVertexProvider_->getPrimaryVertex()->position());
                    pxlParticle->setUserRecord("dz",dz);
                    pxlParticle->setUserRecord("dxy",dxy);
                    pxlParticle->setUserRecord("isTightMuon",patObject.isTightMuon(*primaryVertexProvider_->getPrimaryVertex()));
                }     
            }
                       
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

