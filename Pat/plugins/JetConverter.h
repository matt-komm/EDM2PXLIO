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
#include "DataFormats/HepMCCandidate/interface/GenParticle.h"

namespace edm2pxlio
{

class JetChargeCalculator
{
    public:
        enum MEASURE
        {
            DR,PT,E,ONE,MASSDROP,COS
        };
        
        static const std::function<bool(const reco::Candidate*)> SELECT_CHARGED;
        static const std::function<bool(const reco::Candidate*)> SELECT_VERTEX;
        static const std::function<bool(const reco::Candidate*)> SELECT_NOTVERTEX;
        static const std::function<bool(const reco::Candidate*)> SELECT_ALL;
        
        static double calculate(const pat::Jet& jet, MEASURE measure, double exp=1.0, double bias=0.0, std::function<bool(const reco::Candidate*)> selector=[](const reco::Candidate*){return true;});
};

class JetConverter:
    public CollectionClassConverter<pat::Jet>
{
    private:
        typedef CollectionClassConverter<pat::Jet> Base;
    protected:
        bool _basicsOnly;
    public:
        JetConverter(const std::string& name, const edm::ParameterSet& globalConfig, edm::ConsumesCollector& consumesCollector);
        void calculateJetShapes(const pat::Jet& patObject, pxl::Particle* pxlParticle) const;
        virtual void convertObject(const pat::Jet& patObject, pxl::Particle* pxlParticle) const;
        ~JetConverter();
};

}

#endif

