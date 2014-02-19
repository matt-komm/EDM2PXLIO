#ifndef _VALUELIST2PXLIO_H_
#define _VALUELIST2PXLIO_H_

// system include files
#include <memory>
#include <string>
#include <sstream>
#include <vector>

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

#include "Pxl/Pxl/interface/Pxl.h"

#include "EDM2PXLIO/EDM2PXLIO/src/common/Collection2Pxlio.h"

#include "EDM2PXLIO/EDM2PXLIO/src/common/ConverterFactory.h"

class ValueList2Pxlio: public Collection2Pxlio<std::vector<double>>
{
    protected:

    public:
        ValueList2Pxlio(std::string name):
            Collection2Pxlio<std::vector<double>>(name)
        {
        }
        
        static void init()
        {
            ConverterFactory* fac = ConverterFactory::getInstance();
            fac->registerConverter(new ConverterProducerTmpl<ValueList2Pxlio>("ValueList2Pxlio"));
        }
        
        virtual void convert(const edm::Event* edmEvent, const edm::EventSetup* iSetup, pxl::Event* pxlEvent)
        {
            for (unsigned int isrc = 0; isrc<Collection2Pxlio<std::vector<double>>::size(); ++isrc)
            {
                edm::Handle<std::vector<double>> valueList = Collection2Pxlio<std::vector<double>>::getCollection(edmEvent,isrc);
                pxl::EventView* pxlEventView = Collection2Pxlio<std::vector<double>>::findEventView(pxlEvent,Collection2Pxlio<std::vector<double>>::getEventViewName(isrc));
                std::string name = getCollectionName(isrc);
                //pxl::BasicNVector vec(valueList->size());
                for (unsigned int ivalue=0; ivalue<valueList->size();++ivalue)
                {
                    //vec.setElement(ivalue,(*valueList)[ivalue]);
                    std::stringstream ss;
                    std::string s;
                    ss << ivalue;
                    s = ss.str();
                    pxlEventView->setUserRecord<double>(name+"_"+s,(*valueList)[ivalue]);
                }
                //pxlEventView->setUserRecord<pxl::BasicNVector>(name,vec);
            }
            
        }
        
        ~ValueList2Pxlio()
        {
        }
};

#endif

