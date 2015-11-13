#ifndef __EDM2PXLIO_PROVIDER_RHOPROVIDER_H__
#define __EDM2PXLIO_PROVIDER_RHOPROVIDER_H__

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/Framework/interface/MakerMacros.h"

#include "FWCore/ParameterSet/interface/ParameterSet.h"

#include "FWCore/Utilities/interface/InputTag.h"

#include "DataFormats/Common/interface/Handle.h"

#include "FWCore/Framework/interface/ConsumesCollector.h"

#include "DataFormats/VertexReco/interface/Vertex.h"

#include "EDM2PXLIO/Core/interface/Provider.h"

namespace edm2pxlio
{

class RhoProvider: 
    public Provider
{
    protected:
        edm::InputTag _rhoInputTag;
        double _rho;
        edm::EDGetTokenT<double> _rhoToken;
    public:
        RhoProvider(const edm::ParameterSet& globalConfig, edm::ConsumesCollector& consumesCollector);

        virtual void process(const edm::Event* edmEvent, const edm::EventSetup* iSetup);

        inline double getRho() const
        {
            return _rho;
        }

        virtual ~RhoProvider();
};

}

#endif
