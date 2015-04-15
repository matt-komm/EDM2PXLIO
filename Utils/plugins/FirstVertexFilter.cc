#include "FirstVertexFilter.h"

#include <FWCore/ParameterSet/interface/ParameterSetDescription.h>
#include <FWCore/Utilities/interface/InputTag.h>

#include <FWCore/Framework/interface/MakerMacros.h>

#include <string>
#include <memory>


using namespace std;


FirstVertexFilter::FirstVertexFilter(edm::ParameterSet const &cfg):
    selector(cfg.getParameter<std::string>("cut"))
{
    verticesToken = consumes<reco::VertexCollection>(cfg.getParameter<edm::InputTag>("src"));
    
    produces<reco::VertexCollection>();
}


void FirstVertexFilter::fillDescriptions(edm::ConfigurationDescriptions &descriptions)
{
    edm::ParameterSetDescription desc;
    desc.add<edm::InputTag>("src", edm::InputTag("offlineSlimmedPrimaryVertices"))->
     setComment("Source collection of vertices.");
    desc.add<string>("cut", "!isFake & ndof >= 4. & abs(z) < 24. & position.Rho < 2.")->
     setComment("Selection to apply to the vertices.");
    
    descriptions.add("firstVertexFilter", desc);
}


bool FirstVertexFilter::filter(edm::Event &event, edm::EventSetup const &eventSetup)
{
    // Read a collection of vertices
    edm::Handle<reco::VertexCollection> vertices;
    event.getByToken(verticesToken, vertices);
    
    
    // Find vertices that pass the selection
    std::unique_ptr<reco::VertexCollection> selectedVertices(new reco::VertexCollection);
    
    for (reco::Vertex const &v: *vertices)
    {
        if (selector(v))
            selectedVertices->emplace_back(v);
    }
    
    
    // Write the collection of selected vertices in the event
    event.put(move(selectedVertices));
    
    
    // Evaluate decision of the filter
    return selector(vertices->front());
}


DEFINE_FWK_MODULE(FirstVertexFilter);
