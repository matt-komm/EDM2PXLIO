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

#include "DataFormats/PatCandidates/interface/MET.h"

namespace edm2pxlio
{

class METConverter:
    public CollectionClassConverter<pat::MET>
{
    private:
        typedef CollectionClassConverter<pat::MET> Base;
    public:
        METConverter(const std::string& name, const edm::ParameterSet& globalConfig, edm::ConsumesCollector& consumesCollector):
            Base(name, globalConfig, consumesCollector)
        {
        }
        
        virtual void convertObject(const pat::MET& patObject, pxl::Particle* pxlParticle) const
        {
            Base::convertObject(patObject,pxlParticle);
            pxlParticle->setP4(patObject.px(),patObject.py(),patObject.pz(),patObject.energy());
            /*
            if (patObject.isCaloMET())
            {
                pxlParticle->setUserRecord("metSignificance",patObject.metSignificance());
                pxlParticle->setUserRecord("emEtFraction",patObject.emEtFraction());
                pxlParticle->setUserRecord("emEtInEB",patObject.emEtInEB());
                pxlParticle->setUserRecord("emEtInEE",patObject.emEtInEE());
                pxlParticle->setUserRecord("emEtInHF",patObject.emEtInHF());
                pxlParticle->setUserRecord("etFractionHadronic",patObject.etFractionHadronic());
                pxlParticle->setUserRecord("hadEtInHB",patObject.hadEtInHB());
                pxlParticle->setUserRecord("hadEtInHE",patObject.hadEtInHE());
                pxlParticle->setUserRecord("hadEtInHF",patObject.hadEtInHF());
                pxlParticle->setUserRecord("hadEtInHO",patObject.hadEtInHO());
                pxlParticle->setUserRecord("maxEtInEmTowers",patObject.maxEtInEmTowers());
                pxlParticle->setUserRecord("maxEtInHadTowers",patObject.maxEtInHadTowers());
            }


            if (patObject.isPFMET())
            {
                pxlParticle->setUserRecord("MuonEtFraction",patObject.MuonEtFraction());
                pxlParticle->setUserRecord("NeutralEMFraction",patObject.NeutralEMFraction());
                pxlParticle->setUserRecord("NeutralHadEtFraction",patObject.NeutralHadEtFraction());
            }
            */
        }
        
        ~METConverter()
        {
        }
};

}

#endif

