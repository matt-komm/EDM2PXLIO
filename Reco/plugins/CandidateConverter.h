#ifndef __EDM2PXLIO_RECO_CANDIDATECONVERTER_H__
#define __EDM2PXLIO_RECO_CANDIDATECONVERTER_H__

// user include files

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

namespace edm2pxlio
{

class CandidateConverter: public CollectionClassConverter<reco::Candidate>
{
    private:
        typedef CollectionClassConverter<reco::Candidate> Base;
    protected:

    public:
        CandidateConverter(const std::string& name, const edm::ParameterSet& globalConfig, edm::ConsumesCollector& consumesCollector);    
        virtual void convertObject(const reco::Candidate& candidate, pxl::Particle* pxlParticle);
        virtual ~CandidateConverter();
};

}

#endif

