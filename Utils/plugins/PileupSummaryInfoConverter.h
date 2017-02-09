#ifndef __EDM2PXLIO_UTILS_PILEUPSUMMARYINFOCONVERTER_H__
#define __EDM2PXLIO_UTILS_PILEUPSUMMARYINFOCONVERTER_H__

// user include files

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/EventSetup.h"

#include "FWCore/ParameterSet/interface/ParameterSet.h"

#include "FWCore/Utilities/interface/InputTag.h"

#include "DataFormats/Common/interface/Handle.h"
#include "DataFormats/Common/interface/View.h"

#include "SimDataFormats/PileupSummaryInfo/interface/PileupSummaryInfo.h"

#include "EDM2PXLIO/Provider/interface/PrimaryVertexProvider.h"
#include "EDM2PXLIO/Provider/interface/RhoProvider.h"

#include "Pxl/Pxl/interface/pxl/core.hh"
#include "Pxl/Pxl/interface/pxl/hep.hh"

#include "EDM2PXLIO/Core/interface/CollectionClassConverter.h"

namespace edm2pxlio
{

class PileupSummaryInfoConverter: 
    public CollectionConverter<std::vector<PileupSummaryInfo>>
{
    private:
        typedef CollectionConverter<std::vector<PileupSummaryInfo>> Base;
    protected:
        std::shared_ptr<PrimaryVertexProvider> _primaryVertexProvider;
        std::shared_ptr<RhoProvider> _rhoProvider;
    public:
        PileupSummaryInfoConverter(const std::string& name, const edm::ParameterSet& globalConfig, edm::ConsumesCollector& consumesCollector);    
        virtual void convert(const edm::Event* edmEvent, const edm::EventSetup* iSetup, pxl::Event* pxlEvent) const;
        virtual ~PileupSummaryInfoConverter();
};

}

#endif

