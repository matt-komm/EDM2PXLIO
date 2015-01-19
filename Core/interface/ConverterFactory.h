#ifndef __EDM2PXLIO_CORE_CONVERTERFACTORY_H__
#define __EDM2PXLIO_CORE_CONVERTERFACTORY_H__

#include "FWCore/PluginManager/interface/PluginFactoryBase.h"
#include "FWCore/PluginManager/interface/PluginFactory.h"
#include "EDM2PXLIO/Core/interface/Converter.h"

#include <string>

namespace edm
{
    class ParameterSet;
    class ConsumesCollector;
}

namespace edm2pxlio
{
    typedef edmplugin::PluginFactory<edm2pxlio::Converter*(const std::string&, const edm::ParameterSet&, edm::ConsumesCollector&)> ConverterFactory;
}
#endif

