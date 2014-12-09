#ifndef _SIMVERTEX2PXLIO_H_
#define _SIMVERTEX2PXLIO_H_

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

#include "SimDataFormats/Vertex/interface/SimVertex.h"

#include "EDM2PXLIO/EDM2PXLIO/src/common/ConverterFactory.h"


class SimVertex2Pxlio: public CollectionClass2Pxlio<SimVertex>
{
    protected:
    public:
        SimVertex2Pxlio(std::string name):
            CollectionClass2Pxlio<SimVertex>(name)
        {
            CollectionClass2Pxlio<SimVertex>::setDefaultEventView("Simulation");
        }
        
        static void init()
        {
            ConverterFactory* fac = ConverterFactory::getInstance();
            fac->registerConverter(new ConverterProducerTmpl<SimVertex2Pxlio>("SimVertex2Pxlio"));
        }
        
                
        virtual void convertObject(const SimVertex& classObject, pxl::Particle* pxlParticle)
        {
            CollectionClass2Pxlio<SimVertex>::convertObject(classObject, pxlParticle);
        }
        
        virtual void convertCollection(const edm::Handle<edm::View<SimVertex>> classObjectList, std::vector<pxl::Particle*>& pxlParticleList)
        {
            CollectionClass2Pxlio<SimVertex>::convertCollection(classObjectList, pxlParticleList);
        }
        
        ~SimVertex2Pxlio()
        {
        }
};

#endif

