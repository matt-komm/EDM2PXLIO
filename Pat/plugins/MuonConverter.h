#ifndef __EDM2PXLIO_PAT_MUONCONVERTER_H__
#define __EDM2PXLIO_PAT_MUONCONVERTER_H__

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

#include "EDM2PXLIO/Provider/interface/PrimaryVertexProvider.h"

#include "DataFormats/PatCandidates/interface/Muon.h"

#include "DataFormats/VertexReco/interface/Vertex.h"

namespace edm2pxlio
{

class MuonConverter:
    public CollectionClassConverter<pat::Muon>
{
    private:
        typedef CollectionClassConverter<pat::Muon> Base;
    protected:
        PrimaryVertexProvider* _primaryVertexProvider;
    public:
        MuonConverter(const std::string& name, const edm::ParameterSet& globalConfig, edm::ConsumesCollector& consumesCollector);
        virtual void convertObject(const pat::Muon& patObject, pxl::Particle* pxlParticle) const;
        ~MuonConverter();
};

}

#endif

