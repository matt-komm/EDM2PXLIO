import FWCore.ParameterSet.Config as cms

process = cms.Process("S2")

process.load("Configuration.EventContent.EventContent_cff")
process.load('Configuration.StandardSequences.Geometry_cff')
process.load('Configuration.StandardSequences.MagneticField_38T_cff')
process.load('Configuration.StandardSequences.FrontierConditions_GlobalTag_cff')
process.GlobalTag.globaltag = 'POSTLS172_V3::All'

process.load("FWCore.MessageLogger.MessageLogger_cfi")
process.MessageLogger.cerr.FwkReport.reportEvery = 10
process.MessageLogger.suppressWarning = cms.untracked.vstring('ecalLaserCorrFilter','manystripclus53X','toomanystripclus53X')
process.options = cms.untracked.PSet( wantSummary = cms.untracked.bool(False) )
process.options.allowUnscheduled = cms.untracked.bool(False)

process.source = cms.Source("PoolSource",
    fileNames = cms.untracked.vstring(
        "file:test.root"
    )
)
process.maxEvents = cms.untracked.PSet( input = cms.untracked.int32(100) )


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


from RecoJets.JetProducers.ak4PFJets_cfi import ak4PFJets
from RecoJets.JetProducers.ak4GenJets_cfi import ak4GenJets
from RecoMET.METProducers.PFMET_cfi import pfMet



#select isolated collections
process.selectedMuons = cms.EDFilter("CandPtrSelector", 
    src = cms.InputTag("slimmedMuons"), 
    cut = cms.string(
        '''
        abs(eta)<2.5 && pt>10. &&
        (pfIsolationR04().sumChargedHadronPt+
        max(0.,pfIsolationR04().sumNeutralHadronEt+
        pfIsolationR04().sumPhotonEt-
        0.50*pfIsolationR04().sumPUPt))/pt < 0.20 &&
        (isPFMuon && (isGlobalMuon || isTrackerMuon) )
        '''
    )
)
process.selectedElectrons = cms.EDFilter("CandPtrSelector", 
    src = cms.InputTag("slimmedElectrons"), 
    cut = cms.string(
        '''
        abs(eta)<2.5 && pt>20. &&
        gsfTrack.isAvailable() &&
        gsfTrack.hitPattern().numberOfLostHits(\'MISSING_INNER_HITS\') < 2 &&
        (pfIsolationVariables().sumChargedHadronPt+
        max(0.,pfIsolationVariables().sumNeutralHadronEt+
        pfIsolationVariables().sumPhotonEt-
        0.5*pfIsolationVariables().sumPUPt))/pt < 0.15
        '''
    )
)
#do projections
process.pfCHS = cms.EDFilter("CandPtrSelector", 
    src = cms.InputTag("packedPFCandidates"), 
    cut = cms.string("fromPV")
)
process.pfNoMuonCHS = cms.EDProducer("CandPtrProjector", 
    src = cms.InputTag("pfCHS"), 
    veto = cms.InputTag("selectedMuons")
)
process.pfNoElectronsCHS = cms.EDProducer("CandPtrProjector", 
    src = cms.InputTag("pfNoMuonCHS"), 
    veto = cms.InputTag("selectedElectrons")
)

process.ak4PFJetsCHS = ak4PFJets.clone(
    src = 'pfNoElectronsCHS', 
    doAreaFastjet = True
) # no idea while doArea is false by default, but it's True in RECO so we have to set it




# get the tracks, primary & secondary vertices from packedPFCandidates
process.unpackedTracksAndVertices = cms.EDProducer('PATTrackAndVertexUnpacker',
    slimmedVertices = cms.InputTag("offlineSlimmedPrimaryVertices"),
    slimmedSecondaryVertices = cms.InputTag("slimmedSecondaryVertices"),
    additionalTracks= cms.InputTag("lostTracks"),
    packedCandidates = cms.InputTag("packedPFCandidates")
)

process.load('RecoBTag.Configuration.RecoBTag_cff')
process.load('RecoJets.Configuration.RecoJetAssociations_cff')
process.load("PhysicsTools.PatAlgos.producersLayer1.jetProducer_cff")

process.patJets.addJetCharge = False #so jet charge module can be removed?
process.patJets.addBTagInfo = True
process.patJets.getJetMCFlavour = False
process.patJets.addAssociatedTracks = False
process.patJetPartonMatch.matched = "prunedGenParticles"
process.patJetCorrFactors.primaryVertices = "offlineSlimmedPrimaryVertices"

process.ak4JetTracksAssociatorAtVertexPF.jets = cms.InputTag("ak4PFJetsCHS")
process.ak4JetTracksAssociatorAtVertexPF.tracks = cms.InputTag("unpackedTracksAndVertices")
process.impactParameterTagInfos.primaryVertex = cms.InputTag("unpackedTracksAndVertices")
process.pfImpactParameterTagInfos.primaryVertex = cms.InputTag("unpackedTracksAndVertices")
process.pfImpactParameterTagInfos.candidates=cms.InputTag("packedPFCandidates") #no clue
process.inclusiveSecondaryVertexFinderTagInfos.extSVCollection = cms.InputTag("unpackedTracksAndVertices","secondary","")


process.patJetGenJetMatch.matched = "slimmedGenJets"
process.patJetPartonMatch.matched = "prunedGenParticles"
process.patJetCorrFactors.primaryVertices = "offlineSlimmedPrimaryVertices"

process.patJetPartons.particles = "prunedGenParticles"
process.patJetPartonsLegacy.src = "prunedGenParticles" # if using legacy jet flavour (not used by default)
process.patJetPartonsLegacy.skipFirstN = cms.uint32(0) # do not skip first 6 particles, we already pruned some!
process.patJetPartonsLegacy.acceptNoDaughters = cms.bool(True) # as we drop intermediate stuff, we need to accept quarks with no siblings



#this may not make sense, slimmed object may not be soft
process.softMuonTagInfos.primaryVertex=cms.InputTag("offlineSlimmedPrimaryVertices")
process.softMuonTagInfos.jets=cms.InputTag("ak4PFJetsCHS")
process.softMuonTagInfos.leptons=cms.InputTag("slimmedMuons")
process.softPFMuonsTagInfos.primaryVertex=cms.InputTag("offlineSlimmedPrimaryVertices")
process.softPFMuonsTagInfos.jets=cms.InputTag("ak4PFJetsCHS")
process.softPFMuonsTagInfos.leptons=cms.InputTag("slimmedMuons")


process.softPFElectronsTagInfos.primaryVertex=cms.InputTag("offlineSlimmedPrimaryVertices")
process.softPFElectronsTagInfos.jets=cms.InputTag("ak4PFJetsCHS")
process.softPFElectronsTagInfos.leptons=cms.InputTag("slimmedElectrons")

    



process.OUT = cms.OutputModule("PoolOutputModule",
    fileName = cms.untracked.string('projected.root'),
    outputCommands = cms.untracked.vstring('keep *','keep patJets_patJetsAK4PF_*_*','keep patJets_patJetsAK4PFCHS_*_*','keep *_*_*_PAT','keep *_*_*_S2')
)






process.pat2pxlio=cms.EDAnalyzer('EDM2PXLIO',
    outFileName=cms.string("projected.pxlio"),
    process=cms.string("tChannel"),
    selectEvents=cms.VPSet(
        cms.PSet(
            process=cms.string("S2"),
            paths=cms.vstring("p0"),
        )
    ),
    primaryVertex = cms.InputTag("offlineSlimmedPrimaryVertices"),

    muons = cms.PSet(
        type=cms.string("MuonConverter"),
        srcs=cms.VInputTag(cms.InputTag("slimmedMuons")),
        names=cms.vstring("Muon")
    ),
    
    electrons = cms.PSet(
        type=cms.string("ElectronConverter"),
        srcs=cms.VInputTag(cms.InputTag("slimmedElectrons")),
        names=cms.vstring("Electron")
    ),
                                 
    jets = cms.PSet(
        type=cms.string("JetConverter"),
        srcs=cms.VInputTag(cms.InputTag("slimmedJets"),cms.InputTag("patJets")),
        names=cms.vstring("Jet","ReclusteredJet"),
        select=cms.string("pt>10"),
    ),
    
    genParticles= cms.PSet(
        type=cms.string("GenParticleConverter"),
        srcs=cms.VInputTag(cms.InputTag("lessGenParticles")),
        targetEventViews=cms.vstring("Generated")
    )
    
)
'''
primaryVertex=cms.InputTag("offlineSlimmedPrimaryVertices"),

genCollection = cms.PSet(
    type=cms.string("GenParticle2Pxlio"),
    srcs=cms.VInputTag(cms.InputTag("lessGenParticles")),
    GenEventInfo=cms.InputTag('generator'),
    LHEEvent=cms.InputTag('source')
),

jets = cms.PSet(
    type=cms.string("PatJet2Pxlio"),
    srcs=cms.VInputTag(cms.InputTag("slimmedJets"),cms.InputTag("patJets")),
    names=cms.vstring("Jet","ReclusteredJet"),
    select=cms.string("pt>10"),
    
    #userRecords=cms.PSet(
    #    abseta=cms.string("abs(eta)")
    #)
),


muons = cms.PSet(
    type=cms.string("PatMuon2Pxlio"),
    srcs=cms.VInputTag(cms.InputTag("slimmedMuons")),
    names=cms.vstring("Muon")
),

electrons = cms.PSet(
    type=cms.string("PatElectron2Pxlio"),
    srcs=cms.VInputTag(cms.InputTag("slimmedElectrons")),
    names=cms.vstring("Electron")
),

mets = cms.PSet(
    type=cms.string("PatMET2Pxlio"),
    srcs=cms.VInputTag(cms.InputTag("slimmedMETs")),
    names=cms.vstring("MET")
),

triggers = cms.PSet(
    type=cms.string("Trigger2Pxlio"),
    srcs=cms.VInputTag(cms.InputTag("TriggerResults")),
    regex=cms.vstring("Iso[0-9a-zA-z]*")
)
#TODO: add prescale info from: pat::PackedTriggerPrescales "patTrigger" 
'''


process.p0=cms.Path(
    process.lessGenParticles*
    process.pfCHS*
    process.selectedMuons*process.pfNoMuonCHS*
    process.selectedElectrons*process.pfNoElectronsCHS*
    process.ak4PFJetsCHS*process.unpackedTracksAndVertices*
    
    #process.recoJetAssociations*
    #only need this product
    process.ak4JetTracksAssociatorAtVertexPF* 
    
    
    
    process.btagging* 
    
    
    
    process.makePatJets
    
    
)

process.endpath= cms.EndPath(process.OUT*process.pat2pxlio)


