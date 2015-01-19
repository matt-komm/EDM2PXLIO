
#include "FWCore/PluginManager/interface/PluginFactory.h"

#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/Framework/interface/ConsumesCollector.h"

#include "EDM2PXLIO/Core/interface/ConverterFactory.h"


EDM_REGISTER_PLUGINFACTORY(edm2pxlio::ConverterFactory,"edm2pxlio::ConverterFactory");
