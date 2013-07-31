#ifndef _EFFECTIVEAREAISOLATIONPROVIDER_H_
#define _EFFECTIVEAREAISOLATIONPROVIDER_H_

// system include files
#include <memory>

// user include files
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/EDAnalyzer.h"

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/Framework/interface/MakerMacros.h"

#include "FWCore/ParameterSet/interface/ParameterSet.h"

#include "FWCore/Utilities/interface/InputTag.h"

#include "DataFormats/Common/interface/Handle.h"

#include "DataFormats/PatCandidates/interface/Electron.h"

#include "DataFormats/VertexReco/interface/Vertex.h"

#include "PhysicsTools/PatUtils/interface/StringParserTools.h"

#include "Pxl/Pxl/interface/Pxl.h"

#include "EDM2PXLIO/EDM2PXLIO/src/provider/Provider.h"

#include <iostream>

class EffectiveAreaIsolationProvider: public Provider
{
    protected:
        edm::InputTag earhoInputTag_;
        const edm::ValueMap<double>* earho_;

    public:
        EffectiveAreaIsolationProvider():
            Provider(),
            earhoInputTag_(),
            earho_(0)
        {
        }

        virtual void parseParameter(const edm::ParameterSet& iConfig)
        {
            if (iConfig.exists("elPFIsoValueEA03"))
            {
                earhoInputTag_ = iConfig.getParameter<edm::InputTag>("elPFIsoValueEA03");
            }
        }

        virtual void process(const edm::Event* edmEvent, const edm::EventSetup* iSetup, pxl::Event* pxlEvent)
        {

            if (earhoInputTag_.label().length()>0)
            {
                edm::Handle<edm::ValueMap<double>> earho;
                edmEvent->getByLabel(earhoInputTag_,earho);
                earho_=earho.product();
            }
        }

        virtual const edm::ValueMap<double>* getEARho()
        {
            return earho_;
        }

        ~EffectiveAreaIsolationProvider()
        {
            delete earho_;
        }
};

#endif
