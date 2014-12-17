// -*- C++ -*-
//
// Package:    EDM2PXLIO
// Class:      EDM2PXLIO
// 
/**\class EDM2PXLIO EDM2PXLIO.cc EDM2PXLIO/EDM2PXLIO/src/EDM2PXLIO.cc

 Description: [one line class summary]

 Implementation:
     [Notes on implementation]
*/
//
// Original Author:  Matthias Komm
//         Created:  Fri Jun 28 18:09:52 CEST 2013
// $Id$
//
//


// system include files
#include <memory>

// user include files
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/EDAnalyzer.h"

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/Framework/interface/MakerMacros.h"

#include "FWCore/ParameterSet/interface/ParameterSet.h"

#include "FWCore/Utilities/interface/InputTag.h"

#include "DataFormats/Common/interface/Handle.h"

#include "DataFormats/PatCandidates/interface/Muon.h"

#include "PhysicsTools/PatUtils/interface/StringParserTools.h"

#include "Pxl/Pxl/interface/pxl/core.hh"
#include "Pxl/Pxl/interface/pxl/hep.hh"

#include "EDM2PXLIO/EDM2PXLIO/src/common/Collection2Pxlio.h"
#include "EDM2PXLIO/EDM2PXLIO/src/common/ConverterFactory.h"
#include "EDM2PXLIO/EDM2PXLIO/src/converters/pat/PatMuon2Pxlio.h"
#include "EDM2PXLIO/EDM2PXLIO/src/converters/pat/PatElectron2Pxlio.h"
#include "EDM2PXLIO/EDM2PXLIO/src/converters/pat/PatJet2Pxlio.h"
#include "EDM2PXLIO/EDM2PXLIO/src/converters/pat/PatMET2Pxlio.h"

#include "EDM2PXLIO/EDM2PXLIO/src/converters/trigger/Trigger2Pxlio.h"

#include "EDM2PXLIO/EDM2PXLIO/src/converters/gen/GenParticle2Pxlio.h"
#include "EDM2PXLIO/EDM2PXLIO/src/converters/gen/GenJet2Pxlio.h"

#include "EDM2PXLIO/EDM2PXLIO/src/converters/reco/Candidate2Pxlio.h"

#include "EDM2PXLIO/EDM2PXLIO/src/converters/utils/ValueList2Pxlio.h"

#include "EDM2PXLIO/EDM2PXLIO/src/converters/tracking/SimTrack2Pxlio.h"
#include "EDM2PXLIO/EDM2PXLIO/src/converters/tracking/SimVertex2Pxlio.h"
#include "EDM2PXLIO/EDM2PXLIO/src/converters/tracking/TrajectorySeed2Pxlio.h"
#include "EDM2PXLIO/EDM2PXLIO/src/converters/tracking/RecoTrack2Pxlio.h"

//missing: primary vertex

//
// class declaration
//

class EDM2PXLIO : public edm::EDAnalyzer {
    public:
        explicit EDM2PXLIO(const edm::ParameterSet&);
        ~EDM2PXLIO();

        static void fillDescriptions(edm::ConfigurationDescriptions& descriptions);


    private:
        struct SelectedProcessPaths
        {
            std::string processName;
            std::vector<std::string> paths;
        };

        virtual void beginJob() ;
        virtual void analyze(const edm::Event&, const edm::EventSetup&);
        virtual void endJob() ;

        virtual void beginRun(edm::Run const&, edm::EventSetup const&);
        virtual void endRun(edm::Run const&, edm::EventSetup const&);
        virtual void beginLuminosityBlock(edm::LuminosityBlock const&, edm::EventSetup const&);
        virtual void endLuminosityBlock(edm::LuminosityBlock const&, edm::EventSetup const&);

        bool checkPath(const edm::Event&, pxl::Event&);
        std::vector<SelectedProcessPaths> _selectedProcessPaths;
        
        pxl::OutputFile* _pxlFile;
        std::string _process;



        std::vector<Converter*> _converters; 

      // ----------member data ---------------------------
};


//
// constants, enums and typedefs
//

//
// static data member definitions
//

//
// constructors and destructor
//
EDM2PXLIO::EDM2PXLIO(const edm::ParameterSet& iConfig):
    _pxlFile(0)
{
    if (iConfig.exists("selectEvents"))
    {
        const std::vector<edm::ParameterSet>& selectEventPSets = iConfig.getParameter<std::vector<edm::ParameterSet>>("selectEvents");
        for (unsigned int iset=0; iset<selectEventPSets.size(); ++iset)
        {
            SelectedProcessPaths selectedProcessPath;
            selectedProcessPath.processName=selectEventPSets[iset].getParameter<std::string>("process"); 
            selectedProcessPath.paths=selectEventPSets[iset].getParameter<std::vector<std::string>>("paths");
            _selectedProcessPaths.push_back(selectedProcessPath);
        }
    }
    else
    {
        SelectedProcessPaths selectedProcessPath;
        selectedProcessPath.processName="PAT";
        selectedProcessPath.paths.push_back("p0");
        _selectedProcessPaths.push_back(selectedProcessPath);
        edm::LogWarning("no selectEvents configured") << "default process 'PAT' with path 'p0' will be used";
    }
    if (iConfig.exists("outFileName")) {
        _pxlFile = new pxl::OutputFile(iConfig.getParameter<std::string>("outFileName"));
    } else {
        edm::LogWarning("no output file name configured") << "default name 'data.pxlio' will be used";
        _pxlFile = new pxl::OutputFile("data.pxlio");
    }
    
    if (iConfig.exists("process")) {
        _process = iConfig.getParameter<std::string>("process");
    } else {
        _process = "";
    }

    GenParticle2Pxlio::init();
    GenJet2Pxlio::init();
    
    PatMuon2Pxlio::init();
    PatElectron2Pxlio::init();
    PatJet2Pxlio::init();
    PatMET2Pxlio::init();
    
    Candidate2Pxlio::init();
    
    ValueList2Pxlio::init();
    Trigger2Pxlio::init();
    
    SimTrack2Pxlio::init();
    SimVertex2Pxlio::init();
    TrajectorySeed2Pxlio::init();
    RecoTrack2Pxlio::init();
    
    
    const std::vector<std::string> psetNames = iConfig.getParameterNamesForType<edm::ParameterSet>();
    for (unsigned int iname=0; iname< psetNames.size(); ++iname)
    {
        const edm::ParameterSet& localConf = iConfig.getParameter<edm::ParameterSet>(psetNames[iname]);
        if (localConf.exists("type"))
        {
            std::string typeName = localConf.getParameter<std::string>("type");
            if (ConverterFactory::getInstance()->hasConverter(typeName))
            {   
                _converters.push_back(ConverterFactory::getInstance()->createConverter(typeName,psetNames[iname]));
            }
            else
            {
                edm::LogWarning("converter type not recognized: ") << typeName;
            }
        }
        
    }
    
    
    for (unsigned int iconverter = 0; iconverter<_converters.size(); ++iconverter)
    {
        _converters[iconverter]->parseParameter(iConfig);
    }
    
}


EDM2PXLIO::~EDM2PXLIO()
{
 
   // do anything here that needs to be done at desctruction time
   // (e.g. close files, deallocate resources etc.)

}


//
// member functions
//

// ------------ method called for each event  ------------
void
EDM2PXLIO::analyze(const edm::Event& iEvent, const edm::EventSetup& iSetup)
{
    
    pxl::Event pxlEvent;
    if (!checkPath(iEvent,pxlEvent))
    {
        return;
    }
    pxlEvent.setUserRecord("Run", iEvent.run());
    pxlEvent.setUserRecord("Event number", (uint64_t)iEvent.id().event());
    pxlEvent.setUserRecord("LuminosityBlock",iEvent.luminosityBlock());
    pxlEvent.setUserRecord("isRealData",iEvent.isRealData());
    if (_process.length()>0)
    {
        pxlEvent.setUserRecord("Process", _process);
    }
    
    for (unsigned int iconverter = 0; iconverter<_converters.size(); ++iconverter)
    {
        _converters[iconverter]->convert(&iEvent,&iSetup,&pxlEvent);
    }
    
    _pxlFile->streamObject(&pxlEvent);
    _pxlFile->writeFileSection();
}

bool
EDM2PXLIO::checkPath(const edm::Event& iEvent, pxl::Event& pxlEvent)
{
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


// ------------ method called once each job just before starting event loop  ------------
void 
EDM2PXLIO::beginJob()
{
}

// ------------ method called once each job just after ending the event loop  ------------
void 
EDM2PXLIO::endJob() 
{
    _pxlFile->close();
    delete _pxlFile;
    
    /*
    TODO: make all destructors virtual first!
    for (unsigned int iconverter = 0; iconverter<_converters.size(); ++iconverter)
    {
        delete _converters[iconverter];
    }
    */
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
