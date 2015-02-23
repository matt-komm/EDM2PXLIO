#ifndef __EDM2PXLIO_CORE_PROVIDER_H__
#define __EDM2PXLIO_CORE_PROVIDER_H__

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

class Provider
{
    protected:

    public:
        Provider(const edm::ParameterSet& globalConfig, edm::ConsumesCollector& consumesCollector)
        {
        }

        virtual void process(const edm::Event* edmEvent, const edm::EventSetup* iSetup)
        {
        }


        virtual ~Provider()
        {
        }
};

}

#endif
