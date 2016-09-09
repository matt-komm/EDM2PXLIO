#include "EDM2PXLIO/Plt/plugins/PltJetConverter.h"
#include "EDM2PXLIO/Core/interface/ConverterFactory.h"
#include "EDM2PXLIO/Core/interface/ProviderFactory.h"

#include "PhysicsTools/CandUtils/interface/EventShapeVariables.h"

#include <functional>

namespace edm2pxlio
{

PltJetConverter::PltJetConverter(const std::string& name, const edm::ParameterSet& globalConfig, edm::ConsumesCollector& consumesCollector):
    Base(name, globalConfig, consumesCollector)
{
    if (globalConfig.exists(_name))
    {
        const edm::ParameterSet& iConfig = globalConfig.getParameter<edm::ParameterSet>(_name);   
    }
}

void PltJetConverter::convertObject(const plt::Jet& pltJet, pxl::Particle* pxlParticle) const
{
    const plt::Jet::LorentzVector& vec = pltJet.p4();
    pxlParticle->setP4(vec.px(),vec.py(),vec.pz(),vec.E());
    pxlParticle->setUserRecord("charge",pltJet.charge());
    
    pxlParticle->setUserRecord("JC02",pltJet.jetCharge(0.2));
    pxlParticle->setUserRecord("JC05",pltJet.jetCharge(0.5));
    pxlParticle->setUserRecord("JC07",pltJet.jetCharge(0.7));
    pxlParticle->setUserRecord("JC10",pltJet.jetCharge(1.0));
    
    pxlParticle->setUserRecord("electronFraction",pltJet.electronFraction());
    pxlParticle->setUserRecord("muonFraction",pltJet.muonFraction());
    pxlParticle->setUserRecord("photonFraction",pltJet.photonFraction());
    pxlParticle->setUserRecord("neutrinoFraction",pltJet.neutrinoFraction());
    pxlParticle->setUserRecord("hadronFraction",pltJet.hadronFraction());
    
    std::vector<std::string> ghostTagNames = pltJet.getGhostTagTypes();
    for (const std::string& ghostTag: ghostTagNames)
    {
        pxlParticle->setUserRecord(ghostTag,pltJet.getNumberOfGhostTags(ghostTag));
    }
    
}

PltJetConverter::~PltJetConverter()
{
}

}

DEFINE_EDM_PLUGIN(edm2pxlio::ConverterFactory, edm2pxlio::PltJetConverter, "PltJetConverter");
