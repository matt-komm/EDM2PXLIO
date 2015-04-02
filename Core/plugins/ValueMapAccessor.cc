#include "EDM2PXLIO/Core/interface/ValueMapAccessor.h"
#include "EDM2PXLIO/Core/interface/ValueMapAccessorFactory.h"

namespace edm2pxlio
{

template<class TYPE>
class DefaultTypeValueMapAccessorTmpl:
    public ValueMapAccessorTmpl<TYPE>
{
    public:
        DefaultTypeValueMapAccessorTmpl(const std::string& name, const edm::ParameterSet& config, edm::ConsumesCollector& consumesCollector):
            ValueMapAccessorTmpl<TYPE>(name,config,consumesCollector)
        {
        }
        
        virtual void convertTypeValues(const TYPE& value, pxl::Particle* particle)
        {
            particle->setUserRecord(ValueMapAccessorTmpl<TYPE>::getName(),value);
        }
    
};

typedef DefaultTypeValueMapAccessorTmpl<bool> ValueMapAccessorBool;
typedef DefaultTypeValueMapAccessorTmpl<float> ValueMapAccessorFloat;
typedef DefaultTypeValueMapAccessorTmpl<int> ValueMapAccessorInt;
typedef DefaultTypeValueMapAccessorTmpl<unsigned int> ValueMapAccessorUInt;

}

DEFINE_EDM_PLUGIN(edm2pxlio::ValueMapAccessorFactory, edm2pxlio::ValueMapAccessorBool, "ValueMapAccessorBool");
DEFINE_EDM_PLUGIN(edm2pxlio::ValueMapAccessorFactory, edm2pxlio::ValueMapAccessorFloat, "ValueMapAccessorFloat");
DEFINE_EDM_PLUGIN(edm2pxlio::ValueMapAccessorFactory, edm2pxlio::ValueMapAccessorInt, "ValueMapAccessorInt");
DEFINE_EDM_PLUGIN(edm2pxlio::ValueMapAccessorFactory, edm2pxlio::ValueMapAccessorUInt, "ValueMapAccessorUInt");

