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
    
    pxlParticle->setUserRecord("dB",PRECISION(patObject.dB()));
    
    pxlParticle->setUserRecord("ecalDrivenSeed",patObject.ecalDrivenSeed());
    pxlParticle->setUserRecord("trackerDrivenSeed",patObject.trackerDrivenSeed());
    pxlParticle->setUserRecord("isInEB-EE",(1.4442 < fabs(patObject.eta())) && (fabs(patObject.eta()) < 1.5660));
    
    pxlParticle->setUserRecord("passConversionVeto",patObject.passConversionVeto());
    //pxlParticle->setUserRecord("numberOfHits",patObject.gsfTrack()->trackerExpectedHitsInner().numberOfHits());

    
    if (patObject.gsfTrack().get()!=nullptr)
    {
        const reco::Track* gsfTrack = patObject.gsfTrack().get();
        pxlParticle->setUserRecord("chi2",PRECISION(gsfTrack->chi2()));
        pxlParticle->setUserRecord("ndof",gsfTrack->ndof());
        pxlParticle->setUserRecord("lostHits",gsfTrack->lost());
        
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
    

    pxlParticle->setUserRecord("chargedHadronIso",PRECISION(patObject.chargedHadronIso()));
    pxlParticle->setUserRecord("neutralHadronIso",PRECISION(patObject.neutralHadronIso()));
    pxlParticle->setUserRecord("photonIso",PRECISION(patObject.photonIso()));
    pxlParticle->setUserRecord("superClusterEta",PRECISION(patObject.superCluster()->eta()));
    
    pxlParticle->setUserRecord("sigmaIetaIeta",PRECISION(patObject.sigmaIetaIeta()));
    pxlParticle->setUserRecord("hadronicOverEm",PRECISION(patObject.hadronicOverEm()));
    pxlParticle->setUserRecord("fbrem",PRECISION(patObject.fbrem()));

}

}

DEFINE_EDM_PLUGIN(edm2pxlio::ConverterFactory, edm2pxlio::ElectronConverter, "ElectronConverter");
