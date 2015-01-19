#include "FWCore/PluginManager/interface/PluginFactoryBase.h"
#include "FWCore/PluginManager/interface/PluginFactory.h"

#include "EDM2PXLIO/Core/interface/ConverterFactory.h"


EDM_REGISTER_PLUGINFACTORY(edm2pxlio::ConverterFactory,"edm2pxlio::ConverterFactory");
