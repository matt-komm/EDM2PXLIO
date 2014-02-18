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

#include "Pxl/Pxl/interface/Pxl.h"

#include "EDM2PXLIO/EDM2PXLIO/src/common/Collection2Pxlio.h"
#include "EDM2PXLIO/EDM2PXLIO/src/converters/pat/PatMuon2Pxlio.h"
#include "EDM2PXLIO/EDM2PXLIO/src/converters/pat/PatElectron2Pxlio.h"
#include "EDM2PXLIO/EDM2PXLIO/src/converters/pat/PatJet2Pxlio.h"
#include "EDM2PXLIO/EDM2PXLIO/src/converters/pat/PatMET2Pxlio.h"

#include "EDM2PXLIO/EDM2PXLIO/src/converters/trigger/Trigger2Pxlio.h"


#include "EDM2PXLIO/EDM2PXLIO/src/converters/gen/GenParticle2Pxlio.h"
#include "EDM2PXLIO/EDM2PXLIO/src/converters/gen/GenJet2Pxlio.h"

#include "EDM2PXLIO/EDM2PXLIO/src/converters/utils/ValueList2Pxlio.h"
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
      virtual void beginJob() ;
      virtual void analyze(const edm::Event&, const edm::EventSetup&);
      virtual void endJob() ;

      virtual void beginRun(edm::Run const&, edm::EventSetup const&);
      virtual void endRun(edm::Run const&, edm::EventSetup const&);
      virtual void beginLuminosityBlock(edm::LuminosityBlock const&, edm::EventSetup const&);
      virtual void endLuminosityBlock(edm::LuminosityBlock const&, edm::EventSetup const&);

      bool checkPath(const edm::Event&, pxl::Event&);
      std::vector<std::string> outputPathNames_;
      pxl::OutputFile* pxlFile_;
      std::string process_;
      
      
        
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
    pxlFile_(0)
{
    if (iConfig.exists("SelectEventsFromPath")) {
        outputPathNames_ = iConfig.getParameter<std::vector<std::string> >("SelectEventsFromPath");
    } else {
        edm::LogWarning("no SelectEventsFromPath configured") << "default path 'p0' will be used";
        outputPathNames_.push_back("p0");
    }
    if (iConfig.exists("OutFileName")) {
        pxlFile_ = new pxl::OutputFile(iConfig.getUntrackedParameter<std::string>("OutFileName"));
    } else {
        edm::LogWarning("no output file name configured") << "default name 'data.pxlio' will be used";
        pxlFile_ = new pxl::OutputFile("data.pxlio");
    }
    
    if (iConfig.exists("process")) {
        process_ = iConfig.getUntrackedParameter<std::string>("process");
    } else {
        process_ = "";
    }
    
    _converters.push_back(new GenParticle2Pxlio("gen"));
    _converters.push_back(new GenJet2Pxlio("genjets"));
    
    _converters.push_back(new PatMuon2Pxlio("muon"));
    _converters.push_back(new PatElectron2Pxlio("electron"));
    _converters.push_back(new PatJet2Pxlio("jet"));
    _converters.push_back(new PatMET2Pxlio("met"));
    
    _converters.push_back(new ValueList2Pxlio("valuelist"));
    _converters.push_back(new Trigger2Pxlio("trigger"));

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
    pxlEvent.setUserRecord<unsigned int>("Run", iEvent.run());
    pxlEvent.setUserRecord<unsigned int>("Event number", iEvent.id().event());
    pxlEvent.setUserRecord<unsigned int>("LuminosityBlock",iEvent.luminosityBlock());
    pxlEvent.setUserRecord<bool>("isRealData",iEvent.isRealData());
    if (process_.length()>0) 
    {
        pxlEvent.setUserRecord<std::string>("Process", process_);
    }
    
    for (unsigned int iconverter = 0; iconverter<_converters.size(); ++iconverter)
    {
        _converters[iconverter]->convert(&iEvent,&iSetup,&pxlEvent);
    }
    
    pxlFile_->streamObject(&pxlEvent);
    pxlFile_->writeFileSection();
}

bool
EDM2PXLIO::checkPath(const edm::Event& iEvent, pxl::Event& pxlEvent)
{
    bool accept=false;
    std::string pathLabel("SIM");
    edm::TriggerResultsByName result = iEvent.triggerResultsByName(pathLabel);
    if (! result.isValid())
    {
        edm::LogWarning("trigger result not valid") << "event will not be processed";
        return false;
    }
    for (unsigned ipath=0; ipath<outputPathNames_.size();++ipath)
    {
        if (! result.wasrun(outputPathNames_[ipath]))
        {
            edm::LogWarning("TriggerResults('PAT') has no cms.Path named") << outputPathNames_[ipath] << ". The result of this path will be ignored.";
        } else {
            //pxlEvent->setUserRecord<bool>(outputPathNames_[ipath],result.accept(outputPathNames_[ipath]));
            accept = accept || result.accept(outputPathNames_[ipath]);
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
    pxlFile_->close();
    delete pxlFile_;
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
