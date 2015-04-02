
#include "FWCore/PluginManager/interface/PluginFactory.h"

#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/Framework/interface/ConsumesCollector.h"

#include "EDM2PXLIO/Core/interface/ValueMapAccessorFactory.h"


EDM_REGISTER_PLUGINFACTORY(edm2pxlio::ValueMapAccessorFactory,"edm2pxlio::ValueMapAccessorFactory");
