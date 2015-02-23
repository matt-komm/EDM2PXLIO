#include "EDM2PXLIO/Provider/interface/PrimaryVertexProvider.h"

#include <iostream>

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
    else
    {
        edm::LogWarning("PrimaryVertexProvider") << "Instance requested but input 'primaryVertex' missing";
    }
}

void PrimaryVertexProvider::process(const edm::Event* edmEvent, const edm::EventSetup* iSetup)
{
    if (_vertexInputTag.label().length()>0)
    {
        edm::Handle<edm::View<reco::Vertex>> primaryVertices;
        edmEvent->getByToken(_token,primaryVertices);
        if (primaryVertices->size()>0)
        {
            _primaryVertex = &(*primaryVertices)[0];
        }
        else
        {
            _primaryVertex=nullptr;
        }
    }
}

PrimaryVertexProvider::~PrimaryVertexProvider()
{
}

}

