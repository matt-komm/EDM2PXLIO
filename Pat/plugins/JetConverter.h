#ifndef __EDM2PXLIO_PAT_JETCONVERTER_H__
#define __EDM2PXLIO_PAT_JETCONVERTER_H__

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

#include "DataFormats/PatCandidates/interface/Jet.h"

namespace edm2pxlio
{

class JetConverter:
    public CollectionClassConverter<pat::Jet>
{
    private:
        typedef CollectionClassConverter<pat::Jet> Base;
    protected:

    public:
        JetConverter(const std::string& name, const edm::ParameterSet& globalConfig, edm::ConsumesCollector& consumesCollector);
        virtual void convertObject(const pat::Jet& patObject, pxl::Particle* pxlParticle) const;
        ~JetConverter();
};

}

#endif

