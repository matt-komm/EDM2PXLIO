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
#include "EDM2PXLIO/EDM2PXLIO/src/converters/PatMuon2Pxlio.h"
#include "EDM2PXLIO/EDM2PXLIO/src/converters/PatElectron2Pxlio.h"
#include "EDM2PXLIO/EDM2PXLIO/src/converters/PatJet2Pxlio.h"
#include "EDM2PXLIO/EDM2PXLIO/src/converters/PatMET2Pxlio.h"

#include "EDM2PXLIO/EDM2PXLIO/src/converters/Trigger2Pxlio.h"

#include "EDM2PXLIO/EDM2PXLIO/src/converters/GenParticle2Pxlio.h"

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

      pxl::OutputFile pxlFile_;
      
      PatMuon2Pxlio muonCollection_;
      PatElectron2Pxlio electronCollection_;
      PatJet2Pxlio jetCollection_;
      PatMET2Pxlio metCollection_;
      
      GenParticle2Pxlio genParticleCollection_;
      
      Trigger2Pxlio triggerCollection_;
      
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
    pxlFile_(iConfig.getUntrackedParameter<std::string>("fileName")),
    muonCollection_("muon"),
    electronCollection_("electron"),
    jetCollection_("jet"),
    metCollection_("met"),
    genParticleCollection_("gen"),
    triggerCollection_("trigger")
{
    muonCollection_.parseParameter(iConfig);
    electronCollection_.parseParameter(iConfig);
    jetCollection_.parseParameter(iConfig);
    metCollection_.parseParameter(iConfig);
    genParticleCollection_.parseParameter(iConfig);
    triggerCollection_.parseParameter(iConfig);
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
    pxlEvent.setUserRecord<unsigned int>("Run", iEvent.run());
    pxlEvent.setUserRecord<unsigned int>("Event number", iEvent.id().event());
    //add lumi
    
    muonCollection_.convert(&iEvent,&iSetup,&pxlEvent);
    electronCollection_.convert(&iEvent,&iSetup,&pxlEvent);
    jetCollection_.convert(&iEvent,&iSetup,&pxlEvent);
    metCollection_.convert(&iEvent,&iSetup,&pxlEvent);
    genParticleCollection_.convert(&iEvent,&iSetup,&pxlEvent);
    
    triggerCollection_.convert(&iEvent,&iSetup,&pxlEvent);
    
    pxlFile_.streamObject(&pxlEvent);
    pxlFile_.writeFileSection();
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
    pxlFile_.close();
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
