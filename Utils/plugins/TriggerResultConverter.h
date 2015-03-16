#ifndef __EDM2PXLIO_UTILS_TRIGGERCONVERTER_H__
#define __EDM2PXLIO_UTILS_TRIGGERCONVERTER_H__

// user include files
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/EventSetup.h"

#include "FWCore/ParameterSet/interface/ParameterSet.h"

#include "FWCore/Utilities/interface/InputTag.h"

#include "DataFormats/Common/interface/Handle.h"
#include "DataFormats/Common/interface/View.h"

#include "Pxl/Pxl/interface/pxl/core.hh"
#include "Pxl/Pxl/interface/pxl/hep.hh"

#include "EDM2PXLIO/Core/interface/CollectionClassConverter.h"

#include "FWCore/Common/interface/TriggerNames.h"
#include "DataFormats/Common/interface/TriggerResults.h"

#include "boost/regex.hpp"

namespace edm2pxlio
{

class TriggerResultConverter: 
    public CollectionConverter<edm::TriggerResults>
{
    private:
        typedef CollectionConverter<edm::TriggerResults> Base;

    protected:
        std::vector<boost::regex> _regexList;

    public:
        TriggerResultConverter(const std::string& name, const edm::ParameterSet& globalConfig, edm::ConsumesCollector& consumesCollector);
        
        virtual void convert(const edm::Event* edmEvent, const edm::EventSetup* iSetup, pxl::Event* pxlEvent) const;
        
        ~TriggerResultConverter();
};

}

#endif

