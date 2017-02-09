#ifndef __EDM2PXLIO_CORE_PROVIDERFACTORY_H__
#define __EDM2PXLIO_CORE_PROVIDERFACTORY_H__

#include "EDM2PXLIO/Core/interface/Provider.h"

#include <unordered_map>
#include <typeinfo>
#include <vector>
#include <iostream>
#include <memory>

namespace edm2pxlio
{

class ProviderFactory
{
    protected:
        std::unordered_map<long,std::shared_ptr<Provider>> _providers;

        ProviderFactory()
        {
        }

    public:
        ProviderFactory(const ProviderFactory&) = delete;
        ProviderFactory(ProviderFactory&&) = default;

        static ProviderFactory& getInstance()
        {
            static ProviderFactory factory;
            return factory;
        }

        template<class PROVIDER> static std::shared_ptr<PROVIDER> get(const edm::ParameterSet& globalConfig, edm::ConsumesCollector& consumesCollector)
        {
            return getInstance().create<PROVIDER>(globalConfig, consumesCollector);
        }
        
        std::vector<std::shared_ptr<Provider>> getProviderList()
        {
            std::vector<std::shared_ptr<Provider>> providerList;
            for (auto it = _providers.cbegin(); it != _providers.cend(); ++it)
            {
                providerList.push_back(it->second);
            }
            return providerList;
        }

        template<class PROVIDER>
        std::shared_ptr<PROVIDER> create(const edm::ParameterSet& globalConfig, edm::ConsumesCollector& consumesCollector)
        {
            long hash = typeid(PROVIDER).hash_code();
            auto it = _providers.find(hash);
            if (it!=_providers.end())
            {
                std::shared_ptr<PROVIDER> provider = std::dynamic_pointer_cast<PROVIDER>(it->second);
                return provider;
            }
            else
            {
                std::shared_ptr<PROVIDER> provider(new PROVIDER(globalConfig,consumesCollector));
                _providers[hash]=provider;
                return provider;
            }
            return nullptr;
        }
        
        ~ProviderFactory()
        {
            _providers.clear();
        }
};

}

#endif

