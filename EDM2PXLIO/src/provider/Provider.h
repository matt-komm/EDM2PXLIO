#ifndef _PROVIDER_H_
#define _PROVIDER_H_

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

#include <iostream>

class Provider
{
	protected:
	public:
		Provider()
		{

		}

		virtual void parseParameter(const edm::ParameterSet& iConfig)
		{
		}

		virtual void process(const edm::Event* edmEvent, const edm::EventSetup* iSetup, pxl::Event* pxlEvent)
		{
		}

		~Provider()
		{

		}
};

#endif
