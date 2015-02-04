#ifndef __EDM2PXLIO_CORE_PROVIDERFACTORY_H__
#define __EDM2PXLIO_CORE_PROVIDERFACTORY_H__

#include "EDM2PXLIO/Core/interface/Provider.h"

#include <unordered_map>
#include <typeinfo>

namespace edm2pxlio
{

class ProviderFactory
{
    protected:
        std::unordered_map<long,Provider*> _providers;
    
        ProviderFactory()
        {
        }
    public:
        template<class PROVIDER> static PROVIDER* get(const edm::ParameterSet& globalConfig, edm::ConsumesCollector& consumesCollector)
        {
            static thread_local ProviderFactory factory;
            return factory.create<PROVIDER>(globalConfig, consumesCollector);
            return nullptr;
        }
        
        template<class PROVIDER>
        PROVIDER* create(const edm::ParameterSet& globalConfig, edm::ConsumesCollector& consumesCollector)
        {
            
            long hash = typeid(PROVIDER).hash_code();
            std::unordered_map<long,Provider*>::iterator it = _providers.find(hash);
            if (it!=_providers.end())
            {
                PROVIDER* provider = dynamic_cast<PROVIDER*>(it->second);
                return provider;
            }
            else
            {
                PROVIDER* provider = new PROVIDER(globalConfig,consumesCollector);
                _providers[hash]=provider;
                return provider;
            }
        }
        
};

}

#endif

