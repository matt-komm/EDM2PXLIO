#ifndef __EDM2PXLIO_CORE_TRIGGERRESULTFILTER_H__
#define __EDM2PXLIO_CORE_TRIGGERRESULTFILTER_H__


#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/EventSetup.h"

namespace edm2pxlio
{

struct SelectedProcessPaths
{
    std::string processName;
    std::vector<std::string> paths;
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
        void parseConfiguration(const std::vector<edm::ParameterSet>& selectEventPSets)
        {
            for (unsigned int iset=0; iset<selectEventPSets.size(); ++iset)
            {
                SelectedProcessPaths selectedProcessPaths;
                selectedProcessPaths.processName=selectEventPSets[iset].getParameter<std::string>("process"); 
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
        edm::TriggerResultsByName result = iEvent.triggerResultsByName(selectedProcessPath.processName);
        if (! result.isValid())
        {
            edm::LogWarning("trigger result not valid") << "event will not be processed";
            return false;
        }
        const std::vector<std::string>& paths = selectedProcessPath.paths;
        if (paths.size()==0)
        {
            //accept all events
            return true;
        }
        for (unsigned ipath=0; ipath<paths.size();++ipath)
        {
            if (!result.wasrun(paths[ipath]))
            {
                edm::LogWarning("TriggerResults has no cms.Path named: ") << paths[ipath] << " in process '"<<selectedProcessPath.processName<<"'. The result of this path will be ignored.";
            } else {
                accept = accept || result.accept(paths[ipath]);
            }
        }
    }
    return accept;
}

}

#endif

