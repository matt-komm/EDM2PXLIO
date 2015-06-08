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
typedef DefaultTypeValueMapAccessorTmpl<double> ValueMapAccessorDouble;
typedef DefaultTypeValueMapAccessorTmpl<int> ValueMapAccessorInt;
typedef DefaultTypeValueMapAccessorTmpl<unsigned int> ValueMapAccessorUInt;


}

DEFINE_EDM_PLUGIN(edm2pxlio::ValueMapAccessorFactory, edm2pxlio::ValueMapAccessorBool, "ValueMapAccessorBool");
DEFINE_EDM_PLUGIN(edm2pxlio::ValueMapAccessorFactory, edm2pxlio::ValueMapAccessorFloat, "ValueMapAccessorFloat");
DEFINE_EDM_PLUGIN(edm2pxlio::ValueMapAccessorFactory, edm2pxlio::ValueMapAccessorDouble, "ValueMapAccessorDouble");
DEFINE_EDM_PLUGIN(edm2pxlio::ValueMapAccessorFactory, edm2pxlio::ValueMapAccessorInt, "ValueMapAccessorInt");
DEFINE_EDM_PLUGIN(edm2pxlio::ValueMapAccessorFactory, edm2pxlio::ValueMapAccessorUInt, "ValueMapAccessorUInt");

#include "DataFormats/Math/interface/LorentzVector.h"

namespace edm2pxlio
{
class LorentzVectorValueMapAccessor:
    public ValueMapAccessorTmpl<math::XYZTLorentzVector>
{
    public:
        LorentzVectorValueMapAccessor(const std::string& name, const edm::ParameterSet& config, edm::ConsumesCollector& consumesCollector):
            ValueMapAccessorTmpl<math::XYZTLorentzVector>(name,config,consumesCollector)
        {
        }
        
        virtual void convertTypeValues(const math::XYZTLorentzVector& value, pxl::Particle* particle)
        {
            pxl::LorentzVector vec(value.Px(),value.Py(),value.Pz(),value.E());
            particle->setUserRecord(ValueMapAccessorTmpl<math::XYZTLorentzVector>::getName(),vec);
        }
};
}

DEFINE_EDM_PLUGIN(edm2pxlio::ValueMapAccessorFactory, edm2pxlio::LorentzVectorValueMapAccessor, "ValueMapAccessorLorentzVector");

