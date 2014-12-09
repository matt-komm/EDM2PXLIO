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

#include "Pxl/Pxl/interface/pxl/core.hh"
#include "Pxl/Pxl/interface/pxl/hep.hh"

#include "EDM2PXLIO/EDM2PXLIO/src/common/Collection2Pxlio.h"

#include "EDM2PXLIO/EDM2PXLIO/src/common/CollectionClass2Pxlio.h"

#include <iostream>

#include "EDM2PXLIO/EDM2PXLIO/src/provider/PrimaryVertexProvider.h"

#include "EDM2PXLIO/EDM2PXLIO/src/provider/EffectiveAreaIsolationProvider.h"

#include "EDM2PXLIO/EDM2PXLIO/src/common/ConverterFactory.h"

class PatElectron2Pxlio: public CollectionClass2Pxlio<pat::Electron>
{
    protected:
        PrimaryVertexProvider* primaryVertexProvider_;
        EffectiveAreaIsolationProvider* effectiveAreaIsolationProvider_;

    public:
        PatElectron2Pxlio(std::string name):
            CollectionClass2Pxlio<pat::Electron>(name),
            primaryVertexProvider_(0),
            effectiveAreaIsolationProvider_(0)
        {
            primaryVertexProvider_=createProvider<PrimaryVertexProvider>();
            effectiveAreaIsolationProvider_=createProvider<EffectiveAreaIsolationProvider>();
        }
        
        static void init()
        {
            ConverterFactory* fac = ConverterFactory::getInstance();
            fac->registerConverter(new ConverterProducerTmpl<PatElectron2Pxlio>("PatElectron2Pxlio"));
        }

        virtual void convertObject(const pat::Electron& patObject, pxl::Particle* pxlParticle)
        {
            CollectionClass2Pxlio<pat::Electron>::convertObject(patObject, pxlParticle);
            pxlParticle->setP4(patObject.px(),patObject.py(),patObject.pz(),patObject.energy());
            pxlParticle->setCharge(patObject.charge());
            pxlParticle->setUserRecord("dB",patObject.dB());
            const std::vector<pat::Electron::IdPair>& electronIds = patObject.electronIDs();
            for (unsigned int i = 0; i < electronIds.size(); ++i)
            {
                pxlParticle->setUserRecord(electronIds[i].first,electronIds[i].second);
            }
            
            pxlParticle->setUserRecord("ecalDrivenSeed",patObject.ecalDrivenSeed());
            pxlParticle->setUserRecord("trackerDrivenSeed",patObject.trackerDrivenSeed());
            pxlParticle->setUserRecord("isInEB-EE",(1.4442 < fabs(patObject.eta())) && (fabs(patObject.eta()) < 1.5660));
            
            pxlParticle->setUserRecord("passConversionVeto",patObject.passConversionVeto());
            //pxlParticle->setUserRecord("numberOfHits",patObject.gsfTrack()->trackerExpectedHitsInner().numberOfHits());

            if (primaryVertexProvider_->getPrimaryVertex())
            {
                double dz =fabs(patObject.gsfTrack()->dz(primaryVertexProvider_->getPrimaryVertex()->position()));
                double dxy =fabs(patObject.gsfTrack()->dxy(primaryVertexProvider_->getPrimaryVertex()->position()));
                pxlParticle->setUserRecord("dz",dz);
                pxlParticle->setUserRecord("dxy",dxy);
            }

            pxlParticle->setUserRecord("chargedHadronIso",patObject.chargedHadronIso());
            pxlParticle->setUserRecord("neutralHadronIso",patObject.neutralHadronIso());
            pxlParticle->setUserRecord("photonIso",patObject.photonIso());
            pxlParticle->setUserRecord("superClusterEta",patObject.superCluster()->eta());
            
            pxlParticle->setUserRecord("sigmaIetaIeta",patObject.sigmaIetaIeta());
            pxlParticle->setUserRecord("hadronicOverEm",patObject.hadronicOverEm());
            pxlParticle->setUserRecord("dr03TkSumPt",patObject.dr03TkSumPt());
            pxlParticle->setUserRecord("dr03EcalRecHitSumEt",patObject.dr03EcalRecHitSumEt());
            pxlParticle->setUserRecord("dr03HcalTowerSumEt",patObject.dr03HcalTowerSumEt());
            //pxlParticle->setUserRecord("numberOfLostHits",patObject.gsfTrack()->trackerExpectedHitsInner().numberOfLostHits());
        
            pxlParticle->setUserRecord("patPt",patObject.pt());
            pxlParticle->setUserRecord("patPx",patObject.px());
            pxlParticle->setUserRecord("patPy",patObject.py());
            pxlParticle->setUserRecord("patPz",patObject.pz());
            pxlParticle->setUserRecord("patEta",patObject.eta());
            pxlParticle->setUserRecord("patE",patObject.energy());
            pxlParticle->setUserRecord("patEt",patObject.et());
        
        }
        
        virtual void convertP4(const pat::Electron& patObject, pxl::Particle* pxlParticle)
        {
            pxlParticle->setP4(patObject.ecalDrivenMomentum().px(),patObject.ecalDrivenMomentum().py(),patObject.ecalDrivenMomentum().pz(),patObject.ecalDrivenMomentum().energy());
        }
        
        virtual void convertCollection(const edm::Handle<edm::View<pat::Electron>> patObjectList, std::vector<pxl::Particle*>& pxlParticleList)
        {
            CollectionClass2Pxlio<pat::Electron>::convertCollection(patObjectList, pxlParticleList);
            
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
                        pxlParticleList[iel]->setUserRecord("ea03",ea03);
                    }

                }
            }

        }
        
        ~PatElectron2Pxlio()
        {
        }
};

#endif

