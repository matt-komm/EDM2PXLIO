import FWCore.ParameterSet.Config as cms

process = cms.Process("Demo")

process.load("FWCore.MessageService.MessageLogger_cfi")

process.maxEvents = cms.untracked.PSet( input = cms.untracked.int32(-1) )

process.source = cms.Source("PoolSource",
    # replace 'myfile.root' with the source file you want to use
    fileNames = cms.untracked.vstring(
        'file:myfile.root'
    )
)

process.load('SimGeneral.HepPDTESSource.pythiapdt_cfi')
process.pat2pxlio=cms.EDAnalyzer('EDM2PXLIO',
    fileName=cms.untracked.string("data.pxlio"),
    
    muonSrcs=cms.VInputTag("nonIsolatedLoosePatMuons"),
    muonNames=cms.vstring("Muon"),
    
    electronSrcs=cms.VInputTag("nonIsolatedLoosePatElectrons"),
    electronNames=cms.vstring("Electron"),
    
    jetSrcs=cms.VInputTag("analysisPatJets"),
    jetNames=cms.vstring("Jet"),
    
    metSrcs=cms.VInputTag("patPFMet","patMETs"),
    metNames=cms.vstring("rawMET","MET"),
    
    genSrcs=cms.VInputTag("genParticles"),
    
    triggerRegex=cms.vstring("HLT_Mu","HLT_IsoMu","HLT_Ele")
)


process.p = cms.Path(process.demo)
