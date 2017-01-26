#ifndef __EDM2PXLIO_RECO_GENPARTICLECONVERTER_H__
#define __EDM2PXLIO_RECO_GENPARTICLECONVERTER_H__

// user include files

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/EventSetup.h"

#include "FWCore/ParameterSet/interface/ParameterSet.h"

#include "FWCore/Utilities/interface/InputTag.h"

#include "DataFormats/Common/interface/Handle.h"
#include "FWCore/Framework/interface/ESHandle.h"
#include "DataFormats/Common/interface/View.h"

#include "SimDataFormats/GeneratorProducts/interface/GenEventInfoProduct.h"
#include "SimDataFormats/GeneratorProducts/interface/LHEEventProduct.h"
#include "SimGeneral/HepPDTRecord/interface/ParticleDataTable.h"
#include "DataFormats/HepMCCandidate/interface/GenParticle.h"

#include "Pxl/Pxl/interface/pxl/core.hh"
#include "Pxl/Pxl/interface/pxl/hep.hh"

#include "boost/math/special_functions/sign.hpp"

#include "EDM2PXLIO/Core/interface/CollectionClassConverter.h"

#include <unordered_map>

namespace edm2pxlio
{

class GenParticleConverter: 
    public CollectionClassConverter<reco::GenParticle>
{
    private:
        typedef CollectionClassConverter<reco::GenParticle> Base;
    protected:
    
        edm::InputTag _genEventInfoProductInputTag;
        edm::EDGetTokenT<GenEventInfoProduct>  _genEventInfoProductToken;
        edm::InputTag _lheEventProductInputTag;
        edm::EDGetTokenT<LHEEventProduct>  _lheEventProductToken;
        bool _useNameDB;
        bool _skipLHEweights;

    public:
        GenParticleConverter(const std::string& name, const edm::ParameterSet& globalConfig, edm::ConsumesCollector& consumesCollector);

        virtual void convert(const edm::Event* edmEvent, const edm::EventSetup* iSetup, pxl::Event* pxlEvent) const;
        
        virtual void convertObject(const reco::GenParticle& genObject, pxl::Particle* pxlParticle) const;
        virtual ~GenParticleConverter();
};

}

#endif

