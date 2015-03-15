#include "GenJetConverter.h"
#include "EDM2PXLIO/Core/interface/ConverterFactory.h"


namespace edm2pxlio
{

GenJetConverter::GenJetConverter(const std::string& name, const edm::ParameterSet& globalConfig, edm::ConsumesCollector& consumesCollector):
    Base(name, globalConfig, consumesCollector)
{
}
        
void GenJetConverter::convertObject(const reco::GenJet& genJet, pxl::Particle* pxlParticle) const
{
    Base::convertObject(genJet, pxlParticle);
    pxlParticle->setP4(genJet.px(),genJet.py(),genJet.pz(),genJet.energy());
    
    pxlParticle->setUserRecord("charge",genJet.charge());
    pxlParticle->setUserRecord("vx",genJet.vx());
    pxlParticle->setUserRecord("vy",genJet.vy());
    pxlParticle->setUserRecord("vz",genJet.vz());
    pxlParticle->setUserRecord("numberOfDaughters",genJet.numberOfDaughters());
    pxlParticle->setUserRecord("nConstituents",genJet.nConstituents());
    pxlParticle->setUserRecord("maxDistance",genJet.maxDistance());
    pxlParticle->setUserRecord("emEnergy",genJet.emEnergy());
    pxlParticle->setUserRecord("hadEnergy",genJet.hadEnergy());
    pxlParticle->setUserRecord("invisibleEnergy",genJet.invisibleEnergy());
    pxlParticle->setUserRecord("constituentEtaPhiSpread",genJet.constituentEtaPhiSpread());
    pxlParticle->setUserRecord("constituentPtDistribution",genJet.constituentPtDistribution());
    pxlParticle->setUserRecord("etaetaMoment",genJet.etaetaMoment());
    pxlParticle->setUserRecord("etaphiMoment",genJet.etaphiMoment());
    pxlParticle->setUserRecord("phiphiMoment",genJet.phiphiMoment());
    pxlParticle->setUserRecord("jetArea",genJet.jetArea());
}

GenJetConverter::~GenJetConverter()
{
}

}

DEFINE_EDM_PLUGIN(edm2pxlio::ConverterFactory, edm2pxlio::GenJetConverter, "GenJetConverter");
