#ifndef _SIMTRACK2PXLIO_H_
#define _SIMTRACK2PXLIO_H_

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

#include "Pxl/Pxl/interface/pxl/core.hh"
#include "Pxl/Pxl/interface/pxl/hep.hh"

#include "EDM2PXLIO/EDM2PXLIO/src/common/CollectionClass2Pxlio.h"

#include "SimDataFormats/Track/interface/SimTrack.h"

#include "EDM2PXLIO/EDM2PXLIO/src/common/ConverterFactory.h"


class SimTrack2Pxlio: public CollectionClass2Pxlio<SimTrack>
{
    protected:
    public:
        SimTrack2Pxlio(std::string name):
            CollectionClass2Pxlio<SimTrack>(name)
        {
            CollectionClass2Pxlio<SimTrack>::setDefaultEventView("Simulation");
        }
        
        static void init()
        {
            ConverterFactory* fac = ConverterFactory::getInstance();
            fac->registerConverter(new ConverterProducerTmpl<SimTrack2Pxlio>("SimTrack2Pxlio"));
        }
        
                
        virtual void convertObject(const SimTrack& classObject, pxl::Particle* pxlParticle)
        {
            CollectionClass2Pxlio<SimTrack>::convertObject(classObject, pxlParticle);
            const math::XYZTLorentzVectorD& momentum = classObject.momentum();
            pxlParticle->setP4(momentum.X(),momentum.Y(),momentum.Z(),momentum.T());
            pxlParticle->setCharge(classObject.charge());
            
        }
        
        virtual void convertCollection(const edm::Handle<edm::View<SimTrack>> classObjectList, std::vector<pxl::Particle*>& pxlParticleList)
        {
            CollectionClass2Pxlio<SimTrack>::convertCollection(classObjectList, pxlParticleList);
        }
        
        ~SimTrack2Pxlio()
        {
        }
};

#endif

