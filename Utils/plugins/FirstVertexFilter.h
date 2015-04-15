#pragma once

#include <FWCore/Framework/interface/EDFilter.h>
#include <FWCore/Framework/interface/Event.h>

#include <FWCore/ParameterSet/interface/ParameterSet.h>
#include <FWCore/ParameterSet/interface/ConfigurationDescriptions.h>

#include <DataFormats/VertexReco/interface/VertexFwd.h>
#include <DataFormats/VertexReco/interface/Vertex.h>
#include <CommonTools/Utils/interface/StringCutObjectSelector.h>

#include <string>


/**
 * \class FirstVertexFilter
 * \author Andrey Popov
 * \brief This CMSSW plugin performs selection on the first primary vertex
 *
 * The plugin performs string-based filtering on the first vertex in the collection given. The
 * vertices that pass the selection are put into the event content in a separate collection.
 */
class FirstVertexFilter: public edm::EDFilter
{
public:
    /// Constructor
    FirstVertexFilter(edm::ParameterSet const &cfg);

public:
    /// A method to verify plugin's configuration
    static void fillDescriptions(edm::ConfigurationDescriptions &descriptions);
    
    /**
     * \brief Checks if the event should be kept depending on properties of the first vertex
     * 
     * Stores vertices that pass the selection in a separate collection.
     */
    virtual bool filter(edm::Event &event, edm::EventSetup const &eventSetup);
    
private:
    /// Input collection of vertices
    edm::EDGetTokenT<reco::VertexCollection> verticesToken;
    
    /**
     * \brief Selector to filter vertices
     * 
     * String-based selectors are described in [1].
     * [1] https://twiki.cern.ch/twiki/bin/view/CMSPublic/SWGuidePhysicsCutParser
     */
    StringCutObjectSelector<reco::Vertex> const selector;
};
