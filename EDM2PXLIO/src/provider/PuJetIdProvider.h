#ifndef _PUJETIDPROVIDER_H_
#define _PUJETIDPROVIDER_H_

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

#include "Pxl/Pxl/interface/pxl/core.hh"
#include "Pxl/Pxl/interface/pxl/hep.hh"

#include "EDM2PXLIO/EDM2PXLIO/src/provider/Provider.h"

#include "CMGTools/External/interface/PileupJetIdentifier.h"

#include <iostream>

class PuJetIdProvider: public Provider
{
    protected:
        edm::InputTag puJetInputTag_;
        const edm::ValueMap<StoredPileupJetIdentifier>* jetIDs_;

    public:
        PuJetIdProvider():
            Provider(),
            puJetInputTag_(),
            jetIDs_(nullptr)
        {
        }
        
        virtual std::string typeName() const
		{
		    return "PuJetIdProvider";
		}

        virtual void parseParameter(const edm::ParameterSet& iConfig)
        {
            if (iConfig.exists("puJetId"))
            {
                puJetInputTag_ = iConfig.getParameter<edm::InputTag>("puJetId");
            }
        }

        virtual void process(const edm::Event* edmEvent, const edm::EventSetup* iSetup, pxl::Event* pxlEvent)
        {
            if (puJetInputTag_.label().length()>0)
            {
                edm::Handle<edm::ValueMap<StoredPileupJetIdentifier> > jetIDs;
                edmEvent->getByLabel(puJetInputTag_, jetIDs);
                jetIDs_=jetIDs.product();
            }
        }

        virtual const edm::ValueMap<StoredPileupJetIdentifier>* getPuJetIds()
        {
            return jetIDs_;
        }

        virtual ~PuJetIdProvider()
        {
            if (jetIDs_!=nullptr)
            {
                delete jetIDs_;
            }
        }
};

#endif
