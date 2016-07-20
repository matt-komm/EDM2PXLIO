#ifndef __EDM2PXLIO_PLT_PLTJETCONVERTER_H__
#define __EDM2PXLIO_PLT_PLTJETCONVERTER_H__

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/EventSetup.h"

#include "FWCore/ParameterSet/interface/ParameterSet.h"

#include "FWCore/Utilities/interface/InputTag.h"

#include "DataFormats/Common/interface/Handle.h"
#include "DataFormats/Common/interface/View.h"

#include "DataFormats/Candidate/interface/Candidate.h"

#include "Pxl/Pxl/interface/pxl/core.hh"
#include "Pxl/Pxl/interface/pxl/hep.hh"

#include "EDM2PXLIO/Core/interface/CollectionClassConverter.h"

#include "UserCode/ParticleLevelTools/interface/Jet.h"

namespace edm2pxlio
{

class PltJetConverter:
    public CollectionClassConverter<plt::Jet>
{
    private:
        typedef CollectionClassConverter<plt::Jet> Base;
    protected:
        
    public:
        PltJetConverter(const std::string& name, const edm::ParameterSet& globalConfig, edm::ConsumesCollector& consumesCollector);
        virtual void convertObject(const plt::Jet& pltJet, pxl::Particle* pxlParticle) const;
        ~PltJetConverter();
};

}

#endif

