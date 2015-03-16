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
    
    pxlParticle->setUserRecord("emEnergy",PRECISION(genJet.emEnergy()));
    pxlParticle->setUserRecord("hadEnergy",PRECISION(genJet.hadEnergy()));
    pxlParticle->setUserRecord("invisibleEnergy",PRECISION(genJet.invisibleEnergy()));

    
}   

GenJetConverter::~GenJetConverter()
{
}

}

DEFINE_EDM_PLUGIN(edm2pxlio::ConverterFactory, edm2pxlio::GenJetConverter, "GenJetConverter");
