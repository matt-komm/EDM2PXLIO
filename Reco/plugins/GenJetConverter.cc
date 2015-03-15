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
    
    pxlParticle->setUserRecord("numberOfDaughters",genJet.numberOfDaughters());
    pxlParticle->setUserRecord("nConstituents",genJet.nConstituents());
    pxlParticle->setUserRecord("maxDistance",(float)genJet.maxDistance());
    pxlParticle->setUserRecord("emEnergy",(float)genJet.emEnergy());
    pxlParticle->setUserRecord("hadEnergy",(float)genJet.hadEnergy());
    pxlParticle->setUserRecord("invisibleEnergy",(float)genJet.invisibleEnergy());
    pxlParticle->setUserRecord("constituentEtaPhiSpread",(float)genJet.constituentEtaPhiSpread());
    pxlParticle->setUserRecord("constituentPtDistribution",(float)genJet.constituentPtDistribution());
    pxlParticle->setUserRecord("etaetaMoment",(float)genJet.etaetaMoment());
    pxlParticle->setUserRecord("etaphiMoment",(float)genJet.etaphiMoment());
    pxlParticle->setUserRecord("phiphiMoment",(float)genJet.phiphiMoment());
    pxlParticle->setUserRecord("jetArea",(float)genJet.jetArea());
}

GenJetConverter::~GenJetConverter()
{
}

}

DEFINE_EDM_PLUGIN(edm2pxlio::ConverterFactory, edm2pxlio::GenJetConverter, "GenJetConverter");
