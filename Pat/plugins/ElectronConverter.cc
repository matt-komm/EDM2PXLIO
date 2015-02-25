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
    pxlParticle->setUserRecord("dB",patObject.dB());
    const std::vector<pat::Electron::IdPair>& electronIds = patObject.electronIDs();
    for (unsigned int i = 0; i < electronIds.size(); ++i)
    {
        pxlParticle->setUserRecord(electronIds[i].first,(bool)electronIds[i].second);
    }
    
    pxlParticle->setUserRecord("ecalDrivenSeed",patObject.ecalDrivenSeed());
    pxlParticle->setUserRecord("trackerDrivenSeed",patObject.trackerDrivenSeed());
    pxlParticle->setUserRecord("isInEB-EE",(1.4442 < fabs(patObject.eta())) && (fabs(patObject.eta()) < 1.5660));
    
    pxlParticle->setUserRecord("passConversionVeto",patObject.passConversionVeto());
    //pxlParticle->setUserRecord("numberOfHits",patObject.gsfTrack()->trackerExpectedHitsInner().numberOfHits());

    
    if (patObject.gsfTrack().get()!=nullptr)
    {
        const reco::Track* gsfTrack = patObject.gsfTrack().get();
        pxlParticle->setUserRecord("chi2",gsfTrack->chi2());
        pxlParticle->setUserRecord("ndof",gsfTrack->ndof());
        pxlParticle->setUserRecord("lostHits",gsfTrack->lost());
        
        
        
        if (_primaryVertexProvider->getPrimaryVertex())
        {
            double dz =fabs(patObject.gsfTrack()->dz(_primaryVertexProvider->getPrimaryVertex()->position()));
            double dxy =fabs(patObject.gsfTrack()->dxy(_primaryVertexProvider->getPrimaryVertex()->position()));
            pxlParticle->setUserRecord("dz",dz);
            pxlParticle->setUserRecord("dxy",dxy);
        }
    }
    
    pxlParticle->setUserRecord("caloIso",patObject.caloIso());
    pxlParticle->setUserRecord("ecalIso",patObject.ecalIso());
    pxlParticle->setUserRecord("hcalIso",patObject.hcalIso());
    pxlParticle->setUserRecord("trackIso",patObject.trackIso());

    
    pxlParticle->setUserRecord("R03TkSumPt",patObject.dr03TkSumPt());
    pxlParticle->setUserRecord("R03EcalRecHitSumEt",patObject.dr03EcalRecHitSumEt());
    pxlParticle->setUserRecord("R03HcalTowerSumEt",patObject.dr03HcalTowerSumEt());
    

    pxlParticle->setUserRecord("chargedHadronIso",patObject.chargedHadronIso());
    pxlParticle->setUserRecord("neutralHadronIso",patObject.neutralHadronIso());
    pxlParticle->setUserRecord("photonIso",patObject.photonIso());
    pxlParticle->setUserRecord("superClusterEta",patObject.superCluster()->eta());
    
    pxlParticle->setUserRecord("sigmaIetaIeta",patObject.sigmaIetaIeta());
    pxlParticle->setUserRecord("hadronicOverEm",patObject.hadronicOverEm());
    pxlParticle->setUserRecord("fbrem",patObject.fbrem());

    /*
    pxlParticle->setUserRecord("patPt",patObject.pt());
    pxlParticle->setUserRecord("patPx",patObject.px());
    pxlParticle->setUserRecord("patPy",patObject.py());
    pxlParticle->setUserRecord("patPz",patObject.pz());
    pxlParticle->setUserRecord("patEta",patObject.eta());
    pxlParticle->setUserRecord("patE",patObject.energy());
    pxlParticle->setUserRecord("patEt",patObject.et());
    */
}

}

DEFINE_EDM_PLUGIN(edm2pxlio::ConverterFactory, edm2pxlio::ElectronConverter, "ElectronConverter");
