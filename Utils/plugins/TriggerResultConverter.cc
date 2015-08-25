#include "TriggerResultConverter.h"
#include "EDM2PXLIO/Core/interface/ConverterFactory.h"

namespace edm2pxlio
{

TriggerResultConverter::TriggerResultConverter(const std::string& name, const edm::ParameterSet& globalConfig, edm::ConsumesCollector& consumesCollector):
    Base(name, globalConfig, consumesCollector)
{
    if (globalConfig.exists(name))
	{
	    const edm::ParameterSet& triggerConfig = globalConfig.getParameter<edm::ParameterSet>(name);   
	
        if (triggerConfig.exists("regex")) 
        {
            std::vector<std::string> list = triggerConfig.getParameter<std::vector<std::string>>("regex");
            for (unsigned iregex=0; iregex<list.size(); ++iregex)
            {
                _regexList.push_back(boost::regex(list[iregex]));
            }
        }
        
        if (triggerConfig.exists("prefix")) 
        {
            _prefixes = triggerConfig.getParameter<std::vector<std::string>>("prefix");
        }
        
    }
    else
    {
    }
}

void TriggerResultConverter::convert(const edm::Event* edmEvent, const edm::EventSetup* iSetup, pxl::Event* pxlEvent) const
{
    Base::convert(edmEvent, iSetup, pxlEvent);

    for (unsigned index=0; index<Base::size(); ++index)
    {
        const edm::Handle<edm::TriggerResults> trigResults = Base::getCollection(edmEvent,index);
        pxl::EventView* pxlEventView = Base::findEventView(pxlEvent,Base::getEventViewName(index));
        if (trigResults.product())
        {
            const edm::TriggerNames& trigNames = edmEvent->triggerNames(*trigResults);
            for (unsigned inames=0; inames<trigNames.size(); ++inames) 
            {
                const std::string pathName=trigNames.triggerName(inames);
                bool accept = false;
                for (unsigned iregex=0; (iregex<_regexList.size()) && !accept; ++iregex)
                {
                    accept = accept || regex_search(pathName,_regexList[iregex]);
                    if (accept)
                    {
                        break;
                    }
                }
                if (accept)
                {
                    bool passTrig=trigResults->accept(trigNames.triggerIndex(pathName));
                    
                    if (_prefixes.size()==Base::size())
                    {
                        pxlEventView->setUserRecord(_prefixes[index]+pathName,passTrig);
                    }
                    else
                    {
                        pxlEventView->setUserRecord(pathName,passTrig);
                    }
                }
            }
        }
    }
}

TriggerResultConverter::~TriggerResultConverter()
{
}

}

DEFINE_EDM_PLUGIN(edm2pxlio::ConverterFactory, edm2pxlio::TriggerResultConverter, "TriggerResultConverter");



