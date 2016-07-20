#ifndef __EDM2PXLIO_PAT_JETCONVERTER_H__
#define __EDM2PXLIO_PAT_JETCONVERTER_H__

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/Framework/interface/ESHandle.h"

#include "FWCore/ParameterSet/interface/ParameterSet.h"

#include "FWCore/Utilities/interface/InputTag.h"

#include "DataFormats/Common/interface/Handle.h"
#include "DataFormats/Common/interface/View.h"

#include "DataFormats/Candidate/interface/Candidate.h"

#include "SimGeneral/HepPDTRecord/interface/ParticleDataTable.h"

#include "Pxl/Pxl/interface/pxl/core.hh"
#include "Pxl/Pxl/interface/pxl/hep.hh"

#include "EDM2PXLIO/Core/interface/CollectionClassConverter.h"

#include "DataFormats/PatCandidates/interface/PackedGenParticle.h"
#include "DataFormats/HepMCCandidate/interface/GenParticle.h"

namespace edm2pxlio
{

class PackedGenParticleConverter:
    public CollectionClassConverter<pat::PackedGenParticle>
{
    private:
        typedef CollectionClassConverter<pat::PackedGenParticle> Base;
    protected:
        bool _basicsOnly;
        bool _useNameDB;
        const ParticleDataTable* _pdt;
    public:
        PackedGenParticleConverter(const std::string& name, const edm::ParameterSet& globalConfig, edm::ConsumesCollector& consumesCollector);
        void beginEvent(const edm::Event* edmEvent, const edm::EventSetup* iSetup);
        void convertObject(const pat::PackedGenParticle& genObject, pxl::Particle* pxlParticle) const;
        void endEvent(const edm::Event* edmEvent, const edm::EventSetup* iSetup);
        ~PackedGenParticleConverter();
};

}

#endif

