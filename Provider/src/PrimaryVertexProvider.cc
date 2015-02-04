#include "EDM2PXLIO/Provider/interface/PrimaryVertexProvider.h"

namespace edm2pxlio
{

PrimaryVertexProvider::PrimaryVertexProvider(const edm::ParameterSet& globalConfig, edm::ConsumesCollector& consumesCollector):
	 Provider(globalConfig, consumesCollector),
	 _vertexInputTag(),
	 _primaryVertex(nullptr)
{
    if (globalConfig.exists("primaryVertex"))
    {
        _vertexInputTag = globalConfig.getParameter<edm::InputTag>("primaryVertex");
        _token = consumesCollector.consumes<edm::View<reco::Vertex>>(_vertexInputTag);
    }
}

void PrimaryVertexProvider::process(const edm::Event* edmEvent, const edm::EventSetup* iSetup)
{
	if (_vertexInputTag.label().length()>0) 
	{
		edm::Handle<edm::View<reco::Vertex>> primaryVertices;
        edmEvent->getByToken(_token,primaryVertices);
		_primaryVertex = &(*primaryVertices)[0];
	}
}

PrimaryVertexProvider::~PrimaryVertexProvider()
{
    if (_primaryVertex!=nullptr)
    {
	    delete _primaryVertex;
    }
}

}

