#ifndef __EDM2PXLIO_CORE_VALUEMAPACCESSORFACTORY_H__
#define __EDM2PXLIO_CORE_VALUEMAPACCESSORFACTORY_H__

#include "FWCore/PluginManager/interface/PluginFactory.h"

#include "EDM2PXLIO/Core/interface/ValueMapAccessor.h"

#include <string>

namespace edm
{
    class ParameterSet;
    class ConsumesCollector;
}

namespace edm2pxlio
{
    typedef edmplugin::PluginFactory<edm2pxlio::ValueMapAccessor*(const std::string&, const edm::ParameterSet&, edm::ConsumesCollector&)> ValueMapAccessorFactory;
}
#endif

