#include "EDM2PXLIO/Pat/plugins/JetConverter.h"
#include "EDM2PXLIO/Core/interface/ConverterFactory.h"
#include "EDM2PXLIO/Core/interface/ProviderFactory.h"

namespace edm2pxlio
{

JetConverter::JetConverter(const std::string& name, const edm::ParameterSet& globalConfig, edm::ConsumesCollector& consumesCollector):
    Base(name, globalConfig, consumesCollector)
{
}

void JetConverter::convertObject(const pat::Jet& patObject, pxl::Particle* pxlParticle) const
{
    Base::convertObject(patObject, pxlParticle);
    pxlParticle->setP4(patObject.px(),patObject.py(),patObject.pz(),patObject.energy());

    const std::vector<std::pair<std::string, float>>& bDiscriminators = patObject.getPairDiscri();
    for (unsigned int i = 0; i<bDiscriminators.size(); ++i)
    {
        pxlParticle->setUserRecord(bDiscriminators[i].first,PRECISION(bDiscriminators[i].second));
    }

    pxlParticle->setUserRecord("nConstituents",patObject.nConstituents());
    pxlParticle->setUserRecord("n60",patObject.n60());
    pxlParticle->setUserRecord("n90",patObject.n90());
    
    pxlParticle->setUserRecord("chargedMultiplicity",patObject.chargedMultiplicity());
    
    pxlParticle->setUserRecord("chargedHadronMultiplicity",patObject.chargedHadronMultiplicity ());
    if (patObject.chargedHadronMultiplicity ()>0)
    {
        pxlParticle->setUserRecord("chargedHadronEnergy",PRECISION(patObject.chargedHadronEnergy()));
        pxlParticle->setUserRecord("chargedHadronEnergyFraction",PRECISION(patObject.chargedHadronEnergyFraction()));
    }
    
    pxlParticle->setUserRecord("neutralHadronMultiplicity",patObject.neutralHadronMultiplicity());
    if (patObject.neutralHadronMultiplicity()>0)
    {
        pxlParticle->setUserRecord("neutralHadronEnergy",PRECISION(patObject.neutralHadronEnergy()));
        pxlParticle->setUserRecord("neutralHadronEnergyFraction",PRECISION(patObject.neutralHadronEnergyFraction()));
    } 
    
    pxlParticle->setUserRecord("neutralMultiplicity",patObject.neutralMultiplicity());
    if (patObject.neutralMultiplicity()>0)
    {
        pxlParticle->setUserRecord("neutralEmEnergy",PRECISION(patObject.neutralEmEnergy()));
        pxlParticle->setUserRecord("neutralEmEnergyFraction",PRECISION(patObject.neutralEmEnergyFraction()));
    }
    
    pxlParticle->setUserRecord("photonMultiplicity",patObject.photonMultiplicity());
    if (patObject.photonMultiplicity()>0)
    {
        pxlParticle->setUserRecord("photonEnergy",PRECISION(patObject.photonEnergy()));
        pxlParticle->setUserRecord("photonEnergyFraction",PRECISION(patObject.photonEnergyFraction()));
    }
    
    pxlParticle->setUserRecord("HFHadronMultiplicity",patObject.HFHadronMultiplicity());
    if (patObject.HFHadronMultiplicity()>0)
    {
        pxlParticle->setUserRecord("HFHadronEnergy",PRECISION(patObject.HFHadronEnergy()));
        pxlParticle->setUserRecord("HFHadronEnergyFraction",PRECISION(patObject.HFHadronEnergyFraction()));
    }
    
    pxlParticle->setUserRecord("muonMultiplicity",patObject.muonMultiplicity());
    if (patObject.muonMultiplicity()>0)
    {
        //same as 'chargedMuEnergy'
        pxlParticle->setUserRecord("muonEnergy",PRECISION(patObject.muonEnergy()));
        pxlParticle->setUserRecord("muonEnergyFraction",PRECISION(patObject.muonEnergyFraction()));
    }
    
    pxlParticle->setUserRecord("electronMultiplicity",patObject.electronMultiplicity());
    if (patObject.electronMultiplicity()>0)
    {
        //same as 'chargedEmEnergy'
        pxlParticle->setUserRecord("electronEnergy",PRECISION(patObject.electronEnergy()));
        pxlParticle->setUserRecord("electronEnergyFraction",PRECISION(patObject.electronEnergyFraction()));
    }
    
    const reco::JetFlavourInfo& flavorInfo = patObject.jetFlavourInfo();
    pxlParticle->setUserRecord("partonFlavour",flavorInfo.getPartonFlavour());
    
    pxlParticle->setUserRecord("jetArea",PRECISION(patObject.jetArea()));
    pxlParticle->setUserRecord("jetCharge",PRECISION(patObject.jetCharge()));
    
    for (unsigned int iname = 0; iname < patObject.userFloatNames ().size(); ++ iname)
    {
        float value = patObject.userFloat(patObject.userFloatNames()[iname]);
        if (fabs(value)>0.0)
        {
            pxlParticle->setUserRecord("user_"+patObject.userFloatNames()[iname],PRECISION(value));
        }
    }
    
    if (patObject.genParton())
    {
        pxlParticle->setUserRecord("partonFlavour",patObject.partonFlavour());
    }


}

JetConverter::~JetConverter()
{
}

}

DEFINE_EDM_PLUGIN(edm2pxlio::ConverterFactory, edm2pxlio::JetConverter, "JetConverter");
