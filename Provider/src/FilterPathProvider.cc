#include "EDM2PXLIO/Provider/interface/FilterPathProvider.h"


namespace edm2pxlio
{

FilterPathProvider::FilterPathProvider(const edm::ParameterSet& globalConfig, edm::ConsumesCollector& consumesCollector):
    Provider(globalConfig, consumesCollector),
    _trigByName(nullptr),
    _triggerResultToken(consumesCollector.consumes<edm::TriggerResults>(edm::InputTag("TriggerResults")))
{
}

void FilterPathProvider::process(const edm::Event* edmEvent, const edm::EventSetup* iSetup)
{
    edm::Handle<edm::TriggerResults> triggerResult;
    edmEvent->getByToken(_triggerResultToken,triggerResult);
    const edm::TriggerNames& trigNames = edmEvent->triggerNames(*triggerResult);
    _trigByName.reset(new edm::TriggerResultsByName(triggerResult.product(),&trigNames));
}

bool FilterPathProvider::accept(const std::vector<std::string>& paths) const
{
    if (paths.size()==0 || _trigByName==nullptr)
    {
        return true;
    }
    for (unsigned ipath=0; ipath<paths.size();++ipath)
    {
        if (_trigByName->accept(paths[ipath]))
        {
            return true;
        }
    }
    return false;
}

FilterPathProvider::~FilterPathProvider()
{
}

}

