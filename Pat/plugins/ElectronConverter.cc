#include "EDM2PXLIO/Pat/plugins/ElectronConverter.h"
#include "EDM2PXLIO/Core/interface/ConverterFactory.h"
#include "EDM2PXLIO/Core/interface/ProviderFactory.h"

namespace edm2pxlio
{

ElectronConverter::ElectronConverter(const std::string& name, const edm::ParameterSet& globalConfig, edm::ConsumesCollector& consumesCollector):
    Base(name, globalConfig, consumesCollector),
    _primaryVertexProvider(nullptr)
{
    _primaryVertexProvider=ProviderFactory::get<PrimaryVertexProvider>(globalConfig,consumesCollector);
}


void ElectronConverter::convertObject(const pat::Electron& patObject, pxl::Particle* pxlParticle) const
{
    Base::convertObject(patObject, pxlParticle);
    pxlParticle->setP4(patObject.px(),patObject.py(),patObject.pz(),patObject.energy());
    pxlParticle->setCharge(patObject.charge());
    
    const std::vector<pat::Electron::IdPair>& electronIds = patObject.electronIDs();
    for (unsigned int i = 0; i < electronIds.size(); ++i)
    {
        pxlParticle->setUserRecord(electronIds[i].first,bool(electronIds[i].second));
    }
    
    pxlParticle->setUserRecord("isPF",patObject.isPF());
    pxlParticle->setUserRecord("ecalDrivenSeed",patObject.ecalDrivenSeed());
    pxlParticle->setUserRecord("trackerDrivenSeed",patObject.trackerDrivenSeed());
    pxlParticle->setUserRecord("isEB",patObject.isEB());
    pxlParticle->setUserRecord("isEE",patObject.isEE());
    pxlParticle->setUserRecord("isEBEEGap",patObject.isEBEEGap());
    
    
    //http://cmslxr.fnal.gov/lxr/source/EgammaAnalysis/ElectronTools/src/EGammaCutBasedEleId.cc#0104
    //full5x5_sigmaIetaIeta
    pxlParticle->setUserRecord("full5x5_sigmaIetaIphi",PRECISION(patObject.full5x5_sigmaIetaIphi()));
    //dEtaIn
    pxlParticle->setUserRecord("deltaEtaSuperClusterTrackAtVtx",PRECISION(patObject.deltaEtaSuperClusterTrackAtVtx()));
    //dPhiIn
    pxlParticle->setUserRecord("deltaPhiSuperClusterTrackAtVtx",PRECISION(patObject.deltaPhiSuperClusterTrackAtVtx()));
    //hOverE
    pxlParticle->setUserRecord("hadronicOverEm",PRECISION(patObject.hadronicOverEm()));
    //ooEmooP='1/E - 1/p'
    pxlParticle->setUserRecord("ooEmooP",PRECISION((1.0/patObject.ecalEnergy() - patObject.eSuperClusterOverP()/patObject.ecalEnergy())));
    
    pxlParticle->setUserRecord("sigmaIetaIeta",PRECISION(patObject.sigmaIetaIeta()));
    pxlParticle->setUserRecord("passConversionVeto",patObject.passConversionVeto());
    
    if (patObject.gsfTrack().get()!=nullptr)
    {
        const reco::GsfTrack* gsfTrack = patObject.gsfTrack().get();
        pxlParticle->setUserRecord("chi2",PRECISION(gsfTrack->chi2()));
        pxlParticle->setUserRecord("ndof",gsfTrack->ndof());
        pxlParticle->setUserRecord("lostHits",gsfTrack->lost());
        
        pxlParticle->setUserRecord("trackerLayersWithMeasurement",gsfTrack->hitPattern().trackerLayersWithMeasurement());
        pxlParticle->setUserRecord("numberOfValidTrackerHits",gsfTrack->hitPattern().numberOfValidTrackerHits());

        pxlParticle->setUserRecord("pixelLayersWithMeasurement",gsfTrack->hitPattern().pixelLayersWithMeasurement());
        pxlParticle->setUserRecord("numberOfValidPixelHits",gsfTrack->hitPattern().numberOfValidPixelHits());
        pxlParticle->setUserRecord("numberOfMissingHitsInner",gsfTrack->hitPattern().numberOfHits(reco::HitPattern::MISSING_INNER_HITS)); 
        pxlParticle->setUserRecord("numberOfMissingHitsOuter",gsfTrack->hitPattern().numberOfHits(reco::HitPattern::MISSING_OUTER_HITS)); 
        
        if (_primaryVertexProvider->getPrimaryVertex())
        {
            double dz =fabs(patObject.gsfTrack()->dz(_primaryVertexProvider->getPrimaryVertex()->position()));
            double dxy =fabs(patObject.gsfTrack()->dxy(_primaryVertexProvider->getPrimaryVertex()->position()));
            pxlParticle->setUserRecord("dz",PRECISION(dz));
            pxlParticle->setUserRecord("dxy",PRECISION(dxy));
        }
    }
    
    pxlParticle->setUserRecord("caloIso",PRECISION(patObject.caloIso()));
    pxlParticle->setUserRecord("ecalIso",PRECISION(patObject.ecalIso()));
    pxlParticle->setUserRecord("hcalIso",PRECISION(patObject.hcalIso()));
    pxlParticle->setUserRecord("trackIso",PRECISION(patObject.trackIso()));

    pxlParticle->setUserRecord("R03TkSumPt",PRECISION(patObject.dr03TkSumPt()));
    pxlParticle->setUserRecord("R03EcalRecHitSumEt",PRECISION(patObject.dr03EcalRecHitSumEt()));
    pxlParticle->setUserRecord("R03HcalTowerSumEt",PRECISION(patObject.dr03HcalTowerSumEt()));
    pxlParticle->setUserRecord("R03HcalTowerSumEt",PRECISION(patObject.dr03HcalTowerSumEtBc()));
    
    pxlParticle->setUserRecord("R04TkSumPt",PRECISION(patObject.dr04TkSumPt()));
    pxlParticle->setUserRecord("R04EcalRecHitSumEt",PRECISION(patObject.dr04EcalRecHitSumEt()));
    pxlParticle->setUserRecord("R04HcalTowerSumEt",PRECISION(patObject.dr04HcalTowerSumEt()));
    pxlParticle->setUserRecord("R04HcalTowerSumEt",PRECISION(patObject.dr04HcalTowerSumEtBc()));
    
    pxlParticle->setUserRecord("chargedHadronIso",PRECISION(patObject.chargedHadronIso()));
    pxlParticle->setUserRecord("neutralHadronIso",PRECISION(patObject.neutralHadronIso()));
    pxlParticle->setUserRecord("photonIso",PRECISION(patObject.photonIso()));
    pxlParticle->setUserRecord("particleIso",PRECISION(patObject.particleIso()));
    
    pxlParticle->setUserRecord("puChargedHadronIso",PRECISION(patObject.puChargedHadronIso()));
    
    pxlParticle->setUserRecord("superClusterEta",PRECISION(patObject.superCluster()->eta()));
    
    pxlParticle->setUserRecord("fbrem",PRECISION(patObject.fbrem()));

}

}

DEFINE_EDM_PLUGIN(edm2pxlio::ConverterFactory, edm2pxlio::ElectronConverter, "ElectronConverter");
