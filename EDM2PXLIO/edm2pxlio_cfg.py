import FWCore.ParameterSet.Config as cms

process = cms.Process("Demo")

process.load("FWCore.MessageService.MessageLogger_cfi")

process.maxEvents = cms.untracked.PSet( input = cms.untracked.int32(100) )

process.source = cms.Source("PoolSource",
    # replace 'myfile.root' with the source file you want to use
    fileNames = cms.untracked.vstring(
        'file:test.root'
    )
)
'''
process.output = cms.OutputModule("PoolOutputModule",
    splitLevel = cms.untracked.int32(0),
    eventAutoFlushCompressedSize = cms.untracked.int32(1048576),
    #outputCommands = process.FEVTDEBUGHLTEventContent.outputCommands,
    fileName = cms.untracked.string('test.root')
)
'''

process.load('SimGeneral.HepPDTESSource.pythiapdt_cfi')
process.lessGenParticles = cms.EDProducer("GenParticlePruner",
    src=cms.InputTag("prunedGenParticles"),
    select=cms.vstring(
        "drop  *",
        "keep abs(status)>20 & abs(status) < 30", #keeps all particles from the hard matrix element
        "keep++ abs(pdgId)=24", #keep W-boson decay
        "drop abs(status)>30" #drop all intermediate from decay
        )
    )


process.pat2pxlio=cms.EDAnalyzer('EDM2PXLIO',
    OutFileName=cms.untracked.string("test.pxlio"),
    process=cms.untracked.string("tChannel"),
    SelectEvents=cms.untracked.VPSet(
        cms.untracked.PSet(
            process=cms.untracked.string("Demo"),
            paths=cms.untracked.vstring("p0"), #selects all events
        )
    ),
    
    genCollection = cms.PSet(
        type=cms.string("GenParticle2Pxlio"),
        srcs=cms.VInputTag(cms.InputTag("lessGenParticles")),
        GenEventInfo=cms.InputTag('generator'),
        LHEEvent=cms.InputTag('source')
    ),
    
    jets = cms.PSet(
        type=cms.string("PatJet2Pxlio"),
        srcs=cms.VInputTag(cms.InputTag("slimmedJets")),
        names=cms.vstring("Jets")
    )
    
    

)

process.p0 = cms.Path(process.lessGenParticles)

process.end=cms.EndPath(process.pat2pxlio)
