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
        pxlParticle->setUserRecord(bDiscriminators[i].first,bDiscriminators[i].second);
    }
    pxlParticle->setUserRecord("numberOfDaughters",patObject.numberOfDaughters());

    pxlParticle->setUserRecord("neutralEmEnergy",PRECISION(patObject.neutralEmEnergy()));
    pxlParticle->setUserRecord("chargedEmEnergy",PRECISION(patObject.chargedEmEnergy()));
    pxlParticle->setUserRecord("chargedHadronEnergy",PRECISION(patObject.chargedHadronEnergy()));
    pxlParticle->setUserRecord("neutralHadronEnergy",PRECISION(patObject.neutralHadronEnergy()));
    pxlParticle->setUserRecord("HFHadronEnergy",PRECISION(patObject.HFHadronEnergy()));

    pxlParticle->setUserRecord("neutralEmEnergyFraction",PRECISION(patObject.neutralEmEnergyFraction()));
    pxlParticle->setUserRecord("chargedEmEnergyFraction",PRECISION(patObject.chargedEmEnergyFraction()));
    pxlParticle->setUserRecord("chargedHadronEnergyFraction",PRECISION(patObject.chargedHadronEnergyFraction()));
    pxlParticle->setUserRecord("neutralHadronEnergyFraction",PRECISION(patObject.neutralHadronEnergyFraction()));
    pxlParticle->setUserRecord("HFHadronEnergyFraction",PRECISION(patObject.HFHadronEnergyFraction()));

    pxlParticle->setUserRecord("chargedMultiplicity",patObject.chargedMultiplicity());
    pxlParticle->setUserRecord("neutralMultiplicity",patObject.neutralMultiplicity());
    pxlParticle->setUserRecord("muonMultiplicity",patObject.muonMultiplicity());
    pxlParticle->setUserRecord("electronMultiplicity",patObject.electronMultiplicity());

    const reco::JetFlavourInfo& flavorInfo = patObject.jetFlavourInfo();
    pxlParticle->setUserRecord("partonFlavour",flavorInfo.getPartonFlavour());

    
    for (unsigned int iname = 0; iname < patObject.userFloatNames ().size(); ++ iname)
    {
        float value = patObject.userFloat(patObject.userFloatNames()[iname]);
        if (fabs(value)>0.0)
        {
            pxlParticle->setUserRecord("user_"+patObject.userFloatNames()[iname],value);
        }
    }
    

    /*
    const reco::SecondaryVertexTagInfo *svTagInfo = patObject.tagInfoSecondaryVertex();
    if (svTagInfo  &&  svTagInfo->nVertices() > 0)
    {
        pxlParticle->setUserRecord("secondaryVertexMass",svTagInfo->secondaryVertex(0).p4().mass());
    }
    */
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
