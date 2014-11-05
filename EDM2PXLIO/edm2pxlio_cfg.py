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
process.prunedGenParticles = cms.EDProducer("GenParticlePruner",
    src=cms.InputTag("genParticles"),
    select=cms.vstring(
        "drop  *",
        "keep status = 3", #keeps all particles from the hard matrix element
        "+keep abs(pdgId) = 15 & status = 1" #keeps intermediate decaying tau
        )
    )
process.pat2pxlio=cms.EDAnalyzer('EDM2PXLIO',
    OutFileName=cms.untracked.string("data.pxlio"),
    process=cms.untracked.string("tChannel"),
    SelectEvents=cms.untracked.VPSet(
        cms.untracked.PSet(
            process=cms.untracked.string("Demo"),
            paths=cms.untracked.vstring("p0"),
        )
    ),
    
    genCollection = cms.PSet(
        type=cms.string("GenParticle2Pxlio"),
        srcs=cms.VInputTag(cms.InputTag("prunedGenParticles")),
        EventInfo=cms.InputTag('generator')
    ),
    
    genJets = cms.PSet(
        type=cms.string("GenJet2Pxlio"),
        srcs=cms.VInputTag("ak5GenJets","kt4GenJets","kt6GenJets"),
        names=cms.vstring("AK5GenJets","KT4GenJets","KT6GenJets"),
        targetEventViews=cms.vstring("GenJets")
    ),
    
    

)


process.p0 = cms.Path(process.prunedGenParticles)

process.end=cms.EndPath(process.pat2pxlio)
