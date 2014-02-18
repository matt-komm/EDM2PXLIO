#ifndef _PATELECTRON2PXLIO_H_
#define _PATELECTRON2PXLIO_H_

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

#include "DataFormats/PatCandidates/interface/Electron.h"

#include "DataFormats/VertexReco/interface/Vertex.h"

#include "PhysicsTools/PatUtils/interface/StringParserTools.h"

#include "Pxl/Pxl/interface/Pxl.h"

#include "EDM2PXLIO/EDM2PXLIO/src/common/Collection2Pxlio.h"

#include "EDM2PXLIO/EDM2PXLIO/src/converters/pat/Pat2Pxlio.h"

#include <iostream>

#include "EDM2PXLIO/EDM2PXLIO/src/provider/PrimaryVertexProvider.h"

#include "EDM2PXLIO/EDM2PXLIO/src/provider/EffectiveAreaIsolationProvider.h"

class PatElectron2Pxlio: public Pat2Pxlio<pat::Electron>
{
    protected:
        PrimaryVertexProvider* primaryVertexProvider_;
        EffectiveAreaIsolationProvider* effectiveAreaIsolationProvider_;

    public:
        PatElectron2Pxlio(std::string name):
            Pat2Pxlio<pat::Electron>(name),
            primaryVertexProvider_(0),
            effectiveAreaIsolationProvider_(0)
        {
            primaryVertexProvider_=createProvider<PrimaryVertexProvider>();
            effectiveAreaIsolationProvider_=createProvider<EffectiveAreaIsolationProvider>();
        }

        virtual void convertObject(const pat::Electron& patObject, pxl::Particle* pxlParticle)
        {
            Pat2Pxlio<pat::Electron>::convertObject(patObject, pxlParticle);
            pxlParticle->setCharge(patObject.charge());
            pxlParticle->setUserRecord<float>("dB",patObject.dB());
            pxlParticle->setUserRecord<float>("mva",patObject.electronID("mvaTrigV0"));
            pxlParticle->setUserRecord<bool>("ecalDrivenSeed",patObject.ecalDrivenSeed());
            pxlParticle->setUserRecord<bool>("trackerDrivenSeed",patObject.trackerDrivenSeed());
            pxlParticle->setUserRecord<bool>("isInEB-EE",(1.4442 < fabs(patObject.eta())) && (fabs(patObject.eta()) < 1.5660));
            
            pxlParticle->setUserRecord<bool>("passConversionVeto",patObject.passConversionVeto());
            pxlParticle->setUserRecord<int>("numberOfHits",patObject.gsfTrack()->trackerExpectedHitsInner().numberOfHits());

            if (primaryVertexProvider_->getPrimaryVertex())
            {
                double dz =fabs(patObject.gsfTrack()->dz(primaryVertexProvider_->getPrimaryVertex()->position()));
                double dxy =fabs(patObject.gsfTrack()->dxy(primaryVertexProvider_->getPrimaryVertex()->position()));
                pxlParticle->setUserRecord<float>("dz",dz);
                pxlParticle->setUserRecord<float>("dxy",dxy);
            }

            pxlParticle->setUserRecord<float>("chargedHadronIso",patObject.chargedHadronIso());
            pxlParticle->setUserRecord<float>("neutralHadronIso",patObject.neutralHadronIso());
            pxlParticle->setUserRecord<float>("photonIso",patObject.photonIso());
            pxlParticle->setUserRecord<float>("superClusterEta",patObject.superCluster()->eta());
            
            pxlParticle->setUserRecord<float>("sigmaIetaIeta",patObject.sigmaIetaIeta());
            pxlParticle->setUserRecord<float>("hadronicOverEm",patObject.hadronicOverEm());
            pxlParticle->setUserRecord<float>("dr03TkSumPt",patObject.dr03TkSumPt());
            pxlParticle->setUserRecord<float>("dr03EcalRecHitSumEt",patObject.dr03EcalRecHitSumEt());
            pxlParticle->setUserRecord<float>("dr03HcalTowerSumEt",patObject.dr03HcalTowerSumEt());
            pxlParticle->setUserRecord<float>("numberOfLostHits",patObject.gsfTrack()->trackerExpectedHitsInner().numberOfLostHits());
        
            pxlParticle->setUserRecord<float>("patPt",patObject.pt());
            pxlParticle->setUserRecord<float>("patPx",patObject.px());
            pxlParticle->setUserRecord<float>("patPy",patObject.py());
            pxlParticle->setUserRecord<float>("patPz",patObject.pz());
            pxlParticle->setUserRecord<float>("patEta",patObject.eta());
            pxlParticle->setUserRecord<float>("patE",patObject.energy());
            pxlParticle->setUserRecord<float>("patEt",patObject.et());
        
        }
        
        virtual void convertP4(const pat::Electron& patObject, pxl::Particle* pxlParticle)
        {
            pxlParticle->setP4(patObject.ecalDrivenMomentum().px(),patObject.ecalDrivenMomentum().py(),patObject.ecalDrivenMomentum().pz(),patObject.ecalDrivenMomentum().energy());
        }
        
        virtual void convertCollection(const edm::Handle<edm::View<pat::Electron>> patObjectList, std::vector<pxl::Particle*>& pxlParticleList)
        {
            Pat2Pxlio<pat::Electron>::convertCollection(patObjectList, pxlParticleList);
            if (effectiveAreaIsolationProvider_->getEARho())
            {
                //besides reco eletrons, gsf electrons are also present in the map so the numbers do not match...
                /*
                if (effectiveAreaIsolationProvider_->getEARho()->size()!=patObjectList->size())
                {
                    throw cms::Exception("PatElectron2Pxlio::convertCollection") << "electron ea03 value map differs in size compared to provided pat electrons";
                }
                */
                for (unsigned iel=0; iel<patObjectList->size(); ++iel)
                {
                    const pat::Electron el = (*patObjectList)[iel];
                    if (effectiveAreaIsolationProvider_->getEARho()->contains(el.originalObjectRef().id()))
                    {
                        double ea03 = (*effectiveAreaIsolationProvider_->getEARho())[el.originalObjectRef()];
                        pxlParticleList[iel]->setUserRecord<float>("ea03",ea03);
                    }

                }
            }

        }
        
        ~PatElectron2Pxlio()
        {
        }
};

#endif

