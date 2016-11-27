#include "EDM2PXLIO/Pat/plugins/ElectronConverter.h"
#include "EDM2PXLIO/Core/interface/ConverterFactory.h"
#include "EDM2PXLIO/Core/interface/ProviderFactory.h"

namespace edm2pxlio
{

ElectronConverter::ElectronConverter(const std::string& name, const edm::ParameterSet& globalConfig, edm::ConsumesCollector& consumesCollector):
    Base(name, globalConfig, consumesCollector),
    _primaryVertexProvider(nullptr),
    _rhoProvider(nullptr),
    _effAreas(nullptr)
{
    _primaryVertexProvider=ProviderFactory::get<PrimaryVertexProvider>(globalConfig,consumesCollector);
    _rhoProvider=ProviderFactory::get<RhoProvider>(globalConfig,consumesCollector);

    const edm::ParameterSet& iConfig = globalConfig.getParameter<edm::ParameterSet>(_name);
    if (iConfig.exists("effAreasConfigFile"))
    {
        _effAreas.reset(new EffectiveAreas( (iConfig.getParameter<edm::FileInPath>("effAreasConfigFile")).fullPath()));
    }
    else
    {
        edm::LogWarning(_name) << "no effAreasConfigFile defined - isolation wont be stored";    
    }
}

float ElectronConverter::calculateRelIso(const pat::Electron& patObject) const
{
    //taken from https://github.com/ikrav/cmssw/blob/egm_id_80X_v1/RecoEgamma/ElectronIdentification/plugins/cuts/GsfEleEffAreaPFIsoCut.cc#L83-L94
    
    // Compute the combined isolation with effective area correction
    const reco::GsfElectron::PflowIsolationVariables& pfIso = patObject.pfIsolationVariables();
    const float chad = pfIso.sumChargedHadronPt;
    const float nhad = pfIso.sumNeutralHadronEt;
    const float pho = pfIso.sumPhotonEt;
    const float  eA = _effAreas->getEffectiveArea(std::abs(patObject.superCluster()->eta()));
    const float rho = _rhoProvider ? _rhoProvider->getRho() : 0; // std::max likes float arguments
    const float iso = chad + std::max(0.0f, nhad + pho - rho*eA);

    // Apply the cut and return the result
    // Scale by pT if the relative isolation is requested but avoid division by 0
    return iso/patObject.pt();
}


void ElectronConverter::convertObject(const pat::Electron& patObject, pxl::Particle* pxlParticle) const
{
    Base::convertObject(patObject, pxlParticle);
    pxlParticle->setP4(patObject.px(),patObject.py(),patObject.pz(),patObject.energy());
    pxlParticle->setPdgNumber(patObject.pdgId());
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
    
    
    pxlParticle->setUserRecord("full5x5_sigmaIphiIphi",PRECISION(patObject.full5x5_sigmaIphiIphi()));
    pxlParticle->setUserRecord("full5x5_sigmaIetaIphi",PRECISION(patObject.full5x5_sigmaIetaIphi()));
    pxlParticle->setUserRecord("full5x5_sigmaIetaIeta",PRECISION(patObject.full5x5_sigmaIetaIeta()));
    //dEtaIn
    pxlParticle->setUserRecord("deltaEtaSuperClusterTrackAtVtx",PRECISION(patObject.deltaEtaSuperClusterTrackAtVtx()));
    //dPhiIn
    pxlParticle->setUserRecord("deltaPhiSuperClusterTrackAtVtx",PRECISION(patObject.deltaPhiSuperClusterTrackAtVtx()));
    //hOverE
    pxlParticle->setUserRecord("hadronicOverEm",PRECISION(patObject.hadronicOverEm()));
    //ooEmooP='1/E - 1/p'
    const double ecal_energy_inverse = 1.0/patObject.ecalEnergy();
    const double eSCoverP = patObject.eSuperClusterOverP();
    
    pxlParticle->setUserRecord("ooEmooP",PRECISION(std::abs(1.0 - eSCoverP)*ecal_energy_inverse));
    

    pxlParticle->setUserRecord("passConversionVeto",patObject.passConversionVeto());
    
    if (_effAreas)
    {
        pxlParticle->setUserRecord("effAreaRelIso",calculateRelIso(patObject));
        pxlParticle->setUserRecord("effArea",_effAreas->getEffectiveArea(std::abs(patObject.superCluster()->eta())));
    }
    
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
        pxlParticle->setUserRecord("numberOfMissingInnerHits",gsfTrack->hitPattern().numberOfHits(reco::HitPattern::MISSING_INNER_HITS)); 
        pxlParticle->setUserRecord("numberOfMissingOuterHits",gsfTrack->hitPattern().numberOfHits(reco::HitPattern::MISSING_OUTER_HITS)); 
        
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
    /*
    pxlParticle->setUserRecord("R03TkSumPt",PRECISION(patObject.dr03TkSumPt()));
    pxlParticle->setUserRecord("R03EcalRecHitSumEt",PRECISION(patObject.dr03EcalRecHitSumEt()));
    pxlParticle->setUserRecord("R03HcalTowerSumEt",PRECISION(patObject.dr03HcalTowerSumEt()));
    pxlParticle->setUserRecord("R03HcalTowerSumEtBc",PRECISION(patObject.dr03HcalTowerSumEtBc()));
    
    pxlParticle->setUserRecord("R04TkSumPt",PRECISION(patObject.dr04TkSumPt()));
    pxlParticle->setUserRecord("R04EcalRecHitSumEt",PRECISION(patObject.dr04EcalRecHitSumEt()));
    pxlParticle->setUserRecord("R04HcalTowerSumEt",PRECISION(patObject.dr04HcalTowerSumEt()));
    pxlParticle->setUserRecord("R04HcalTowerSumEtBc",PRECISION(patObject.dr04HcalTowerSumEtBc()));
    */
    pxlParticle->setUserRecord("chargedHadronIso",PRECISION(patObject.chargedHadronIso()));
    pxlParticle->setUserRecord("neutralHadronIso",PRECISION(patObject.neutralHadronIso()));
    pxlParticle->setUserRecord("photonIso",PRECISION(patObject.photonIso()));
    pxlParticle->setUserRecord("particleIso",PRECISION(patObject.particleIso()));
    
    pxlParticle->setUserRecord("puChargedHadronIso",PRECISION(patObject.puChargedHadronIso()));
    
    pxlParticle->setUserRecord("superClusterEta",PRECISION(patObject.superCluster()->eta()));
    pxlParticle->setUserRecord("superClusterPhi",PRECISION(patObject.superCluster()->phi()));
    
    pxlParticle->setUserRecord("fbrem",PRECISION(patObject.fbrem()));
    
    const reco::GsfElectron::PflowIsolationVariables& pfIso = patObject.pfIsolationVariables();
    pxlParticle->setUserRecord("pfsumChargedHadronPt",pfIso.sumChargedHadronPt);
    pxlParticle->setUserRecord("pfsumNeutralHadronEt",pfIso.sumNeutralHadronEt);
    pxlParticle->setUserRecord("pfsumPhotonEt",pfIso.sumPhotonEt);
    pxlParticle->setUserRecord("pfsumPUPt",pfIso.sumPUPt);
    pxlParticle->setUserRecord("pfsumChargedParticlePt",pfIso.sumChargedParticlePt); 

}

}

DEFINE_EDM_PLUGIN(edm2pxlio::ConverterFactory, edm2pxlio::ElectronConverter, "ElectronConverter");
