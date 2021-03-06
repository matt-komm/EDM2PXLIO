#ifndef _PRIMARYVERTEXPROVIDER_H_
#define _PRIMARYVERTEXPROVIDER_H_

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

#include <iostream>

class PrimaryVertexProvider: public Provider
{
	protected:
    	const reco::Vertex* primaryVertex_;
    	edm::InputTag vertexInputTag_;
	public:
		PrimaryVertexProvider():
			 primaryVertex_(nullptr),
			 vertexInputTag_()
		{
		}
		
		virtual std::string typeName() const
		{
		    return "PrimaryVertexProvider";
		}

		virtual void parseParameter(const edm::ParameterSet& iConfig)
		{
			if (iConfig.exists("primaryVertex"))
			{
				vertexInputTag_ = iConfig.getParameter<edm::InputTag>("primaryVertex");
			}
		}

		virtual void process(const edm::Event* edmEvent, const edm::EventSetup* iSetup, pxl::Event* pxlEvent)
		{
			if (vertexInputTag_.label().length()>0) {
				edm::Handle<edm::View<reco::Vertex>> vertexList;
				edmEvent->getByLabel(vertexInputTag_,vertexList);
				primaryVertex_ = &(*vertexList)[0];
			}
		}

		virtual const reco::Vertex* getPrimaryVertex()
		{
			return primaryVertex_;
		}

		virtual ~PrimaryVertexProvider()
		{
		    if (primaryVertex_!=nullptr)
		    {
			    delete primaryVertex_;
		    }
		}
};

#endif
