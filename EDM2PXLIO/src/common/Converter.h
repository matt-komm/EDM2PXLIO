#ifndef _CONVERTER_H_
#define _CONVERTER_H_

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

#include "DataFormats/PatCandidates/interface/Muon.h"

#include "PhysicsTools/PatUtils/interface/StringParserTools.h"

#include "Pxl/Pxl/interface/pxl/core.hh"
#include "Pxl/Pxl/interface/pxl/hep.hh"

#include "EDM2PXLIO/EDM2PXLIO/src/provider/Provider.h"

class Converter
{
	protected:
		std::vector<Provider*> providerList_;

	public:
		Converter()
		{
		}

		template<class ProviderClass>
		ProviderClass* createProvider()
		{
			ProviderClass* provider = new ProviderClass();
			providerList_.push_back(provider);
			return provider;
		}

		virtual void parseParameter(const edm::ParameterSet& iConfig)
		{
			for (unsigned iprovider=0; iprovider<providerList_.size();++iprovider)
			{
				providerList_[iprovider]->parseParameter(iConfig);
			}
		}

		virtual void convert(const edm::Event* edmEvent, const edm::EventSetup* iSetup, pxl::Event* pxlEvent)
		{
			for (unsigned iprovider=0; iprovider<providerList_.size();++iprovider)
			{
				providerList_[iprovider]->process(edmEvent,iSetup,pxlEvent);
			}
		}

		~Converter()
		{
		}
};

#endif
