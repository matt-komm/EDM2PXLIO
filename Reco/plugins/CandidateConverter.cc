#include "EDM2PXLIO/Reco/plugins/CandidateConverter.h"
#include "EDM2PXLIO/Core/interface/ConverterFactory.h"

namespace edm2pxlio
{

CandidateConverter::CandidateConverter(const std::string& name, const edm::ParameterSet& globalConfig, edm::ConsumesCollector& consumesCollector):
    Base(name, globalConfig, consumesCollector)
{
}
                
void CandidateConverter::convertObject(const reco::Candidate& candidate, pxl::Particle* pxlParticle) const
{
    Base::convertObject(candidate, pxlParticle);
    pxlParticle->setP4(candidate.px(),candidate.py(),candidate.pz(),candidate.energy());
    
    pxlParticle->setUserRecord("charge",candidate.charge());
}
        
CandidateConverter::~CandidateConverter()
{
}

}

DEFINE_EDM_PLUGIN(edm2pxlio::ConverterFactory, edm2pxlio::CandidateConverter, "CandidateConverter");




