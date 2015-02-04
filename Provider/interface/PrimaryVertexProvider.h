#ifndef __EDM2PXLIO_PROVIDER_PRIMARYVERTEXPROVIDER_H__
#define __EDM2PXLIO_PROVIDER_PRIMARYVERTEXPROVIDER_H__

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/Framework/interface/MakerMacros.h"

#include "FWCore/ParameterSet/interface/ParameterSet.h"

#include "FWCore/Utilities/interface/InputTag.h"

#include "DataFormats/Common/interface/Handle.h"

#include "FWCore/Framework/interface/ConsumesCollector.h"

#include "DataFormats/VertexReco/interface/Vertex.h"

#include "EDM2PXLIO/Core/interface/Provider.h"

namespace edm2pxlio
{

class PrimaryVertexProvider: 
    public Provider
{
	protected:
	    edm::InputTag _vertexInputTag;
    	const reco::Vertex* _primaryVertex;
    	edm::EDGetTokenT<edm::View<reco::Vertex>> _token;
	public:
		PrimaryVertexProvider(const edm::ParameterSet& globalConfig, edm::ConsumesCollector& consumesCollector);

		virtual void process(const edm::Event* edmEvent, const edm::EventSetup* iSetup);

		inline const reco::Vertex* getPrimaryVertex() const
		{
			return _primaryVertex;
		}

		virtual ~PrimaryVertexProvider();
};

}

#endif
