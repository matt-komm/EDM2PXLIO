#ifndef __EDM2PXLIO_PROVIDER_FILTERPATHPROVIDER_H_
#define __EDM2PXLIO_PROVIDER_FILTERPATHPROVIDER_H_


#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/Framework/interface/ConsumesCollector.h"

#include "FWCore/Utilities/interface/InputTag.h"

#include "FWCore/Common/interface/TriggerResultsByName.h"
#include "FWCore/Common/interface/TriggerNames.h"

#include "DataFormats/Common/interface/TriggerResults.h"
#include "DataFormats/Common/interface/Handle.h"

#include "EDM2PXLIO/Core/interface/Provider.h"

#include <memory>

namespace edm2pxlio
{

class FilterPathProvider: 
    public Provider
{
    protected:
        std::unique_ptr<edm::TriggerResultsByName> _trigByName;
        edm::EDGetTokenT<edm::TriggerResults> _triggerResultToken;
    public:
        
        FilterPathProvider(const edm::ParameterSet& globalConfig, edm::ConsumesCollector& consumesCollector);

        virtual void process(const edm::Event* edmEvent, const edm::EventSetup* iSetup);

        bool accept(const std::vector<std::string>& paths) const;

        virtual ~FilterPathProvider();
};

}

#endif
