#ifndef __EDM2PXLIO_RECO_PACKEDCANDIDATECONVERTER_H__
#define __EDM2PXLIO_RECO_PACKEDCANDIDATECONVERTER_H__

// user include files

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/EventSetup.h"

#include "FWCore/ParameterSet/interface/ParameterSet.h"

#include "FWCore/Utilities/interface/InputTag.h"

#include "DataFormats/Common/interface/Handle.h"
#include "DataFormats/Common/interface/View.h"

#include "DataFormats/PatCandidates/interface/PackedCandidate.h"
#include "DataFormats/PatCandidates/interface/Jet.h"

#include "DataFormats/VertexReco/interface/Vertex.h"

#include "Pxl/Pxl/interface/pxl/core.hh"
#include "Pxl/Pxl/interface/pxl/hep.hh"

#include "EDM2PXLIO/Core/interface/CollectionClassConverter.h"

#include "FWCore/ServiceRegistry/interface/Service.h"
#include "CommonTools/UtilAlgos/interface/TFileService.h"

#include "TH2F.h"
#include "TH1F.h"

#include <vector>

namespace edm2pxlio
{

class PackedCandidateConverter: 
    public CollectionClassConverter<pat::PackedCandidate>
{
    private:
        typedef CollectionClassConverter<pat::PackedCandidate> Base;
    protected:
        //edm::Service<TFileService> fs;
        //TH2F* candPtEta;
        //TH2F* candPhiEta;
        
        //std::vector<TH2F*> candPtPhiEta;
        
        std::vector<edm::InputTag> _jetToMatchInputTags;
        std::vector<edm::EDGetTokenT<edm::View<pat::Jet>>> _jetToMatchToken;
        
        std::vector<edm::InputTag> _vertexToMatchInputTags;
        std::vector<edm::EDGetTokenT<edm::View<reco::Vertex>>> _vertexToMatchToken;
    public:
        PackedCandidateConverter(const std::string& name, const edm::ParameterSet& globalConfig, edm::ConsumesCollector& consumesCollector);    
        virtual void convert(const edm::Event* edmEvent, const edm::EventSetup* iSetup, pxl::Event* pxlEvent) const;
        virtual ~PackedCandidateConverter();
        
        void addObservables(pxl::Particle* particle, pxl::EventView* pxlEventView, std::vector<pxl::Vertex*>& vertices, const std::vector<const pat::PackedCandidate*>& cands) const;
};

}

#endif

