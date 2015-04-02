#ifndef __EDM2PXLIO_CORE_VALUEMAPACCESSOR_H__
#define __EDM2PXLIO_CORE_VALUEMAPACCESSOR_H__

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/Framework/interface/ConsumesCollector.h"

#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/Utilities/interface/InputTag.h"

#include "DataFormats/Common/interface/Handle.h"

#include "Pxl/Pxl/interface/pxl/core.hh"
#include "Pxl/Pxl/interface/pxl/hep.hh"

#include "DataFormats/Provenance/interface/ProductID.h"
#include "DataFormats/Common/interface/ValueMap.h"



#include <string>

namespace edm
{
    class Event;
    class EventSetup;
    class ParameterSet;
    class ConsumesCollector;
}

namespace pxl
{
    class Event;
}

namespace edm2pxlio
{

class ValueMapAccessor
{
    protected:
        const std::string _name;

    public:
        ValueMapAccessor(const std::string& name, const edm::ParameterSet&, edm::ConsumesCollector&):
            _name(name)
        {
        }

        inline std::string getName() const
        {
            return _name;
        }

        virtual void convert(const edm::Event*, const edm::EventSetup*, pxl::Event*)
        {
        }
        
        virtual void accessValues(const edm::ProductID&, size_t, pxl::Particle*)
        {
        }
        
        virtual ~ValueMapAccessor()
        {
        }
};

template<class TYPE>
class ValueMapAccessorTmpl:
    public ValueMapAccessor
{
    protected:
        const edm::ValueMap<TYPE>* _valueMap;
        edm::EDGetTokenT<edm::ValueMap<TYPE>> _valueMapToken;
    public:
        ValueMapAccessorTmpl(const std::string& name, const edm::ParameterSet& config, edm::ConsumesCollector& consumesCollector):
            ValueMapAccessor(name,config,consumesCollector)
        {
            _valueMapToken = consumesCollector.consumes<edm::ValueMap<TYPE>>(config.getParameter<edm::InputTag>("src"));
        }
        
        virtual void convert(const edm::Event* edmEvent, const edm::EventSetup*, pxl::Event*)
        {
            edm::Handle<edm::ValueMap<TYPE>> collectionObjects;
            edmEvent->getByToken(_valueMapToken,collectionObjects);
            _valueMap = collectionObjects.product();
        }
        
        virtual void accessValues(const edm::ProductID& id, size_t key, pxl::Particle* particle)
        {
            convertTypeValues(_valueMap->get(id,key),particle);
        }
        
        virtual void convertTypeValues(const TYPE&, pxl::Particle*)
        {
        }
};

}

#endif
