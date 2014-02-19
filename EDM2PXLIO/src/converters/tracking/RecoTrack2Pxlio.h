#ifndef _RECOTRACK2PXLIO_H_
#define _RECOTRACK2PXLIO_H_

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

#include "PhysicsTools/PatUtils/interface/StringParserTools.h"

#include "Pxl/Pxl/interface/Pxl.h"

#include "EDM2PXLIO/EDM2PXLIO/src/common/CollectionClass2Pxlio.h"

#include "DataFormats/TrackReco/interface/Track.h"

#include "EDM2PXLIO/EDM2PXLIO/src/common/ConverterFactory.h"


class RecoTrack2Pxlio: public CollectionClass2Pxlio<reco::Track>
{
    protected:
    public:
        RecoTrack2Pxlio(std::string name):
            CollectionClass2Pxlio<reco::Track>(name)
        {
            CollectionClass2Pxlio<reco::Track>::setDefaultEventView("Simulation");
        }
        
        static void init()
        {
            ConverterFactory* fac = ConverterFactory::getInstance();
            fac->registerConverter(new ConverterProducerTmpl<RecoTrack2Pxlio>("RecoTrack2Pxlio"));
        }
                
        virtual void convertObject(const reco::Track& classObject, pxl::Particle* pxlParticle)
        {
            CollectionClass2Pxlio<reco::Track>::convertObject(classObject, pxlParticle);
        }
        
        virtual void convertCollection(const edm::Handle<edm::View<reco::Track>> classObjectList, std::vector<pxl::Particle*>& pxlParticleList)
        {
            CollectionClass2Pxlio<reco::Track>::convertCollection(classObjectList, pxlParticleList);
        }
        
        ~RecoTrack2Pxlio()
        {
        }
};

#endif

