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

#include "EDM2PXLIO/EDM2PXLIO/src/Collection2Pxlio.h"
//#include "EDM2PXLIO/EDM2PXLIO/src/converters/PatMuon2Pxlio.h"
//#include "EDM2PXLIO/EDM2PXLIO/src/converters/PatElectron2Pxlio.h"
//#include "EDM2PXLIO/EDM2PXLIO/src/converters/PatJet2Pxlio.h"
//#include "EDM2PXLIO/EDM2PXLIO/src/converters/PatMET2Pxlio.h"

//#include "EDM2PXLIO/EDM2PXLIO/src/converters/Trigger2Pxlio.h"

#include "EDM2PXLIO/EDM2PXLIO/src/converters/GenParticle2Pxlio.h"
#include "EDM2PXLIO/EDM2PXLIO/src/converters/GenJet2Pxlio.h"

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
      
      //PatMuon2Pxlio muonCollection_;
      //PatElectron2Pxlio electronCollection_;
      //PatJet2Pxlio jetCollection_;
      //PatMET2Pxlio metCollection_;
      
      GenParticle2Pxlio genParticleCollection_;
      GenJet2Pxlio genJetCollection_;
      
      //Trigger2Pxlio triggerCollection_;
      

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
    pxlFile_(0),
    //muonCollection_("muon"),
    //electronCollection_("electron"),
    //jetCollection_("jet"),
    //metCollection_("met"),
    genParticleCollection_("gen"),
    genJetCollection_("genjet")
    //triggerCollection_("trigger")
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

    //muonCollection_.parseParameter(iConfig);
    //electronCollection_.parseParameter(iConfig);
    //jetCollection_.parseParameter(iConfig);
    //metCollection_.parseParameter(iConfig);
    genParticleCollection_.parseParameter(iConfig);
    genJetCollection_.parseParameter(iConfig);
    //triggerCollection_.parseParameter(iConfig);
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
    
    //muonCollection_.convert(&iEvent,&iSetup,&pxlEvent);
    //electronCollection_.convert(&iEvent,&iSetup,&pxlEvent);
    //jetCollection_.convert(&iEvent,&iSetup,&pxlEvent);
    //metCollection_.convert(&iEvent,&iSetup,&pxlEvent);
    genParticleCollection_.convert(&iEvent,&iSetup,&pxlEvent);
    genJetCollection_.convert(&iEvent,&iSetup,&pxlEvent);
    //triggerCollection_.convert(&iEvent,&iSetup,&pxlEvent);
    
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
