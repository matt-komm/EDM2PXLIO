#ifndef _TRAJECTORYSEED2PXLIO_H_
#define _TRAJECTORYSEED2PXLIO_H_

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

#include "DataFormats/TrajectorySeed/interface/TrajectorySeed.h"

#include "EDM2PXLIO/EDM2PXLIO/src/common/ConverterFactory.h"

class TrajectorySeed2Pxlio: public CollectionClass2Pxlio<TrajectorySeed>
{
    protected:
    public:
        TrajectorySeed2Pxlio(std::string name):
            CollectionClass2Pxlio<TrajectorySeed>(name)
        {
            CollectionClass2Pxlio<TrajectorySeed>::setDefaultEventView("Simulation");
        }
        
        static void init()
        {
            ConverterFactory* fac = ConverterFactory::getInstance();
            fac->registerConverter(new ConverterProducerTmpl<TrajectorySeed2Pxlio>("TrajectorySeed2Pxlio"));
        }
                
        virtual void convertObject(const TrajectorySeed& classObject, pxl::Particle* pxlParticle)
        {
            CollectionClass2Pxlio<TrajectorySeed>::convertObject(classObject, pxlParticle);
        }
        
        virtual void convertCollection(const edm::Handle<edm::View<TrajectorySeed>> classObjectList, std::vector<pxl::Particle*>& pxlParticleList)
        {
            CollectionClass2Pxlio<TrajectorySeed>::convertCollection(classObjectList, pxlParticleList);
        }
        
        ~TrajectorySeed2Pxlio()
        {
        }
};

#endif

