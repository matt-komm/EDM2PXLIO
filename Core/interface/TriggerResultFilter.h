#ifndef __EDM2PXLIO_CORE_TRIGGERRESULTFILTER_H__
#define __EDM2PXLIO_CORE_TRIGGERRESULTFILTER_H__


#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/EventSetup.h"

#include "FWCore/Utilities/interface/InputTag.h"

#include "FWCore/Common/interface/TriggerResultsByName.h"
#include "FWCore/Common/interface/TriggerNames.h"

#include "DataFormats/Common/interface/TriggerResults.h"
#include "DataFormats/Common/interface/Handle.h"



namespace edm2pxlio
{

struct SelectedProcessPaths
{
    std::string processName;
    std::vector<std::string> paths;
    edm::EDGetTokenT<edm::TriggerResults> token;
};

class TriggerResultFilter
{
    public: 

    protected:
        std::vector<SelectedProcessPaths> _selectedProcessPaths;
    public:
        
        inline void addSelectedProcessPaths(const SelectedProcessPaths& selectedProcessPaths)
        {
            _selectedProcessPaths.push_back(selectedProcessPaths);
        }
        void parseConfiguration(const std::vector<edm::ParameterSet>& selectEventPSets, edm::ConsumesCollector& consumesCollector)
        {
            for (unsigned int iset=0; iset<selectEventPSets.size(); ++iset)
            {
                SelectedProcessPaths selectedProcessPaths;
                selectedProcessPaths.processName=selectEventPSets[iset].getParameter<std::string>("process"); 
		        selectedProcessPaths.token = consumesCollector.consumes<edm::TriggerResults>(edm::InputTag("TriggerResults","",selectedProcessPaths.processName));
                selectedProcessPaths.paths=selectEventPSets[iset].getParameter<std::vector<std::string>>("paths");
                this->addSelectedProcessPaths(selectedProcessPaths);
            }
        }
        
        bool checkPath(const edm::Event&) const;
};


bool
TriggerResultFilter::checkPath(const edm::Event& iEvent) const
{
    if (_selectedProcessPaths.size()==0)
    {   
        return true;
    }
    
    bool accept=false;
    for (unsigned int iprocess =0; iprocess < _selectedProcessPaths.size(); ++iprocess)
    {
        const SelectedProcessPaths& selectedProcessPath = _selectedProcessPaths[iprocess];
        edm::Handle<edm::TriggerResults> result;
        iEvent.getByToken(selectedProcessPath.token,result);
        if (! result.isValid())
        {
            edm::LogWarning("trigger result for process "+selectedProcessPath.processName+" not valid") << " will be skip";
            continue;
        }
        const std::vector<std::string>& paths = selectedProcessPath.paths;
        
        const edm::TriggerNames& trigNames = iEvent.triggerNames(*result);
        edm::TriggerResultsByName trigByName(result.product(),&trigNames);

        for (unsigned ipath=0; ipath<paths.size();++ipath)
        {
            //this will throw if path does not exists
            accept = accept || trigByName.accept(paths[ipath]);
        }
    }
    return accept;
}

}

#endif

