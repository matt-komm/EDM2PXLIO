#include "FWCore/Framework/interface/EDAnalyzer.h"

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/Framework/interface/ConsumesCollector.h"

#include "FWCore/PluginManager/interface/ModuleDef.h"

#include "FWCore/MessageLogger/interface/MessageLogger.h"

#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/ParameterSet/interface/ParameterSetDescription.h"
#include "FWCore/ParameterSet/interface/ConfigurationDescriptions.h"


#include "FWCore/Utilities/interface/InputTag.h"

#include "Pxl/Pxl/interface/pxl/core.hh"
#include "Pxl/Pxl/interface/pxl/hep.hh"

#include "EDM2PXLIO/Core/interface/Converter.h"
#include "EDM2PXLIO/Core/interface/ConverterFactory.h"

#include "EDM2PXLIO/Core/interface/Provider.h"
#include "EDM2PXLIO/Core/interface/ProviderFactory.h"

#include "EDM2PXLIO/Provider/interface/FilterPathProvider.h"


#include <string>
#include <vector>


class EDM2PXLIO:
    public edm::EDAnalyzer
{
    public:
        explicit EDM2PXLIO(const edm::ParameterSet&);
        ~EDM2PXLIO();

        static void fillDescriptions(edm::ConfigurationDescriptions& descriptions);


    private:
        
        pxl::OutputFile* _pxlFile;
        std::string _processName;
        
        std::vector<edm2pxlio::Converter*> _converters; 
        std::vector<edm2pxlio::Provider*> _providers;
        
        edm2pxlio::FilterPathProvider* _filterPathProvider;
        std::vector<std::string> _filterPaths;
        
        
        virtual void beginJob();
        virtual void analyze(const edm::Event&, const edm::EventSetup&);
        virtual void endJob();

        virtual void beginRun(edm::Run const&, edm::EventSetup const&);
        virtual void endRun(edm::Run const&, edm::EventSetup const&);
        virtual void beginLuminosityBlock(edm::LuminosityBlock const&, edm::EventSetup const&);
        virtual void endLuminosityBlock(edm::LuminosityBlock const&, edm::EventSetup const&);

};


EDM2PXLIO::EDM2PXLIO(const edm::ParameterSet& globalConfig):
    _pxlFile(nullptr),
    _filterPathProvider(nullptr)
{
    edm::ConsumesCollector consumeCollect = consumesCollector();
    
    _filterPathProvider = edm2pxlio::ProviderFactory::get<edm2pxlio::FilterPathProvider>(globalConfig,consumeCollect);
    if (globalConfig.exists("filterPaths"))
    {
        _filterPaths = globalConfig.getParameter<std::vector<std::string>>("filterPaths");
    }
    

    if (globalConfig.exists("outFileName")) {
        _pxlFile = new pxl::OutputFile(globalConfig.getParameter<std::string>("outFileName"));
    } else {
        edm::LogWarning("no output file name configured") << "default name 'out.pxlio' will be used";
        _pxlFile = new pxl::OutputFile("out.pxlio");
    }
    _pxlFile->setCompressionMode(3);
    _pxlFile->setMaxNObjects(2000);
    _pxlFile->setMaxSize(20485760); //20MB   
    if (globalConfig.exists("processName")) {
        _processName = globalConfig.getParameter<std::string>("processName");
    } else {
        _processName = "";
    }

    
    const std::vector<std::string> psetNames = globalConfig.getParameterNamesForType<edm::ParameterSet>();
    for (unsigned int iname=0; iname< psetNames.size(); ++iname)
    {
        const std::string& converterName = psetNames[iname];
        const edm::ParameterSet& localConf = globalConfig.getParameter<edm::ParameterSet>(converterName);
        if (localConf.exists("type"))
        {
            std::string typeName = localConf.getParameter<std::string>("type");
            edm2pxlio::Converter* converter = edm2pxlio::ConverterFactory::get()->tryToCreate(typeName,converterName,globalConfig,consumeCollect);
            if (converter)
            {
                _converters.push_back(converter);
            }
            else
            {
                edm::LogWarning("Converter plugin not found: ") << typeName;
            }
        }
    }
}


EDM2PXLIO::~EDM2PXLIO()
{
}

// ------------ method called once each job just before starting event loop  ------------
void
EDM2PXLIO::beginJob()
{
    _providers = edm2pxlio::ProviderFactory::getInstance().getProviderList();
}

void
EDM2PXLIO::analyze(const edm::Event& iEvent, const edm::EventSetup& iSetup)
{
    try
    {
        pxl::Event pxlEvent;

        pxlEvent.setUserRecord("Run", (unsigned int)iEvent.run());
        pxlEvent.setUserRecord("Event number", (uint64_t)iEvent.id().event());
        pxlEvent.setUserRecord("LuminosityBlock",(unsigned int)iEvent.luminosityBlock());
        pxlEvent.setUserRecord("isRealData",iEvent.isRealData());
        if (_processName.length()>0)
        {
            pxlEvent.setUserRecord("ProcessName", _processName);
        }
        for (unsigned int iprovider = 0; iprovider<_providers.size(); ++iprovider)
        {
            _providers[iprovider]->process(&iEvent,&iSetup);
        }
        
        if (_filterPathProvider->accept(_filterPaths))
        {
            for (unsigned int iconverter = 0; iconverter<_converters.size(); ++iconverter)
            {
                _converters[iconverter]->beginEvent(&iEvent,&iSetup);
                _converters[iconverter]->convert(&iEvent,&iSetup,&pxlEvent);
                _converters[iconverter]->endEvent(&iEvent,&iSetup);
            }
            _pxlFile->streamObject(&pxlEvent);
        }
    } 
    catch (...)
    {
        //store the last event before the exception
        _pxlFile->writeFileSection();
        _pxlFile->close();
        //rethrow the original exception
        throw;
    }
    
    
}

// ------------ method called once each job just after ending the event loop  ------------
void 
EDM2PXLIO::endJob() 
{
    _pxlFile->writeFileSection();
    _pxlFile->close();
    delete _pxlFile;
}

// ------------ method called when starting to processes a run  ------------
void 
EDM2PXLIO::beginRun(edm::Run const&, edm::EventSetup const&)
{
}

// ------------ method called when ending the processing of a run  ------------
void 
EDM2PXLIO::endRun(edm::Run const&, edm::EventSetup const&)
{
}

// ------------ method called when starting to processes a luminosity block  ------------
void 
EDM2PXLIO::beginLuminosityBlock(edm::LuminosityBlock const&, edm::EventSetup const&)
{
}

// ------------ method called when ending the processing of a luminosity block  ------------
void 
EDM2PXLIO::endLuminosityBlock(edm::LuminosityBlock const&, edm::EventSetup const&)
{
}

// ------------ method fills 'descriptions' with the allowed parameters for the module  ------------
void
EDM2PXLIO::fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
  //The following says we do not know what parameters are allowed so do no validation
  // Please change this to state exactly what you do use, even if it is no parameters
  edm::ParameterSetDescription desc;
  desc.setUnknown();
  descriptions.addDefault(desc);
}

//define this as a plug-in
DEFINE_FWK_MODULE(EDM2PXLIO);
