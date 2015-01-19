#ifndef __EDM2PXLIO_CORE_CONVERTER_H__
#define __EDM2PXLIO_CORE_CONVERTER_H__

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

class Converter
{
	protected:
		//std::vector<Provider*> providerList_;
		const std::string _name;

	public:
		Converter(const std::string& name, const edm::ParameterSet&, edm::ConsumesCollector&):
		    _name(name)
		{
		}
		
		inline std::string getName() const
		{
		    return _name;
		}

        /*
		template<class ProviderClass>
		ProviderClass* createProvider()
		{
		    
			ProviderClass* new_provider = new ProviderClass();
			for (unsigned int i = 0; i < providerList_.size(); ++i)
		    {
		        if (new_provider->typeName()==providerList_[i]->typeName())
		        {
		            ProviderClass* old_provider = dynamic_cast<ProviderClass*>(providerList_[i]);
		            if (old_provider)
		            {
		                delete new_provider;
		                return old_provider;
	                }
		        }
		    }
			
			providerList_.push_back(new_provider);
			return new_provider;
		}
		*/

		virtual void convert(const edm::Event* edmEvent, const edm::EventSetup* iSetup, pxl::Event* pxlEvent) const
		{
		    /*
			for (unsigned iprovider=0; iprovider<providerList_.size();++iprovider)
			{
				providerList_[iprovider]->process(edmEvent,iSetup,pxlEvent);
			}
			*/
		}

		virtual ~Converter()
		{
		}
};

}

#endif
