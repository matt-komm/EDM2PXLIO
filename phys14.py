import FWCore.ParameterSet.Config as cms
from FWCore.ParameterSet.VarParsing import VarParsing

options = VarParsing ('analysis')

#TODO: forward option to plugins
options.register(
    'isData',
    False,
    VarParsing.multiplicity.singleton,
    VarParsing.varType.bool,
    "is data"
)

#TODO: forward option to plugins
options.register(
    'isFSim',
    False,
    VarParsing.multiplicity.singleton,
    VarParsing.varType.bool,
    "is fastsim"
)

options.register(
    'processName',
    "unknown",
    VarParsing.multiplicity.singleton,
    VarParsing.varType.string,
    "name of process"
)
				  
options.parseArguments()



process = cms.Process("STEA")
process.options = cms.untracked.PSet( wantSummary = cms.untracked.bool(True) )
process.options.allowUnscheduled = cms.untracked.bool(False)
#process.options.numberOfThreads = cms.untracked.uint32(4)
#process.options.numberOfStreams = cms.untracked.uint32(4)

process.load('Configuration.EventContent.EventContent_cff')
process.load('Configuration.Geometry.GeometryIdeal_cff')
process.load('Configuration.StandardSequences.MagneticField_38T_cff')
process.load('Configuration.StandardSequences.FrontierConditions_GlobalTag_cff')
process.GlobalTag.globaltag = 'PHYS14_25_V2'

process.load("FWCore.MessageLogger.MessageLogger_cfi")
process.MessageLogger.cerr.FwkReport.reportEvery = 1000
process.MessageLogger.suppressWarning = cms.untracked.vstring('ecalLaserCorrFilter','manystripclus53X','toomanystripclus53X')

process.source = cms.Source("PoolSource",
    fileNames = cms.untracked.vstring(
        "root://eoscms////eos/cms/store/mc/Phys14DR/TToLeptons_t-channel-CSA14_Tune4C_13TeV-aMCatNLO-tauola/MINIAODSIM/PU20bx25_PHYS14_25_V1-v1/00000/0260CBE1-9F6A-E411-88C8-E0CB4E29C514.root"
        #'root://eoscms////eos/cms/store/mc/Phys14DR/TToLeptons_t-channel-CSA14_Tune4C_13TeV-aMCatNLO-tauola/MINIAODSIM/PU20bx25_PHYS14_25_V1-v1/00000/0260CBE1-9F6A-E411-88C8-E0CB4E29C514.root'
        #'root://eoscms///eos/cms/store/mc/Spring14miniaod/TToLeptons_t-channel-CSA14_Tune4C_13TeV-aMCatNLO-tauola/MINIAODSIM/PU20bx25_POSTLS170_V5-v2/00000/0082EB4E-0D23-E411-9129-FA163E4A4545.root'
        #'root://xrootd.unl.edu//store/mc/Phys14DR/TTJets_MSDecaysCKM_central_Tune4C_13TeV-madgraph-tauola/MINIAODSIM/PU4bx50_PHYS14_25_V1-v1/00000/003B199E-0F81-E411-8E76-0025905A60B0.root'
        #'root://xrootd.unl.edu//store/mc/Phys14DR/Tbar_tW-channel-DR_Tune4C_13TeV-CSA14-powheg-tauola/MINIAODSIM/PU20bx25_PHYS14_25_V1-v1/00000/0AFEB3CA-9D72-E411-8ACC-20CF305B057E.root'
        #'root://xrootd.unl.edu//store/mc/Phys14DR/QCD_Pt-20toInf_MuEnrichedPt15_PionKaonDecay_Tune4C_13TeV_pythia8/MINIAODSIM/PU20bx25_PHYS14_25_V1-v3/10000/381E5CF2-8BA7-E411-B4ED-0025B3E05C2C.root'
        #'root://xrootd.unl.edu//store/mc/Phys14DR/TTJets_MSDecaysCKM_central_Tune4C_13TeV-madgraph-tauola/MINIAODSIM/PU20bx25_PHYS14_25_V1-v1/00000/00C90EFC-3074-E411-A845-002590DB9262.root'
    )
)
process.maxEvents = cms.untracked.PSet( input = cms.untracked.int32(-1) )


process.goodOfflinePrimaryVertices = cms.EDFilter('FirstVertexFilter',
    src = cms.InputTag('offlineSlimmedPrimaryVertices'),
    cut = cms.string('!isFake & ndof >= 4. & abs(z) < 24. & position.Rho < 2.')
)


process.load('SimGeneral.HepPDTESSource.pythiapdt_cfi')
process.lessGenParticles = cms.EDProducer("GenParticlePruner",
    src=cms.InputTag("prunedGenParticles"),
    select=cms.vstring(
        "drop  *",
        "++keep abs(status)>20 & abs(status) < 30", #keeps all particles from the hard matrix element and their mothers
        "keep++ abs(pdgId)=24", #keep W-boson decay
        "drop abs(status)>30", #drop all intermediate from decay
    )
)

from PhysicsTools.SelectorUtils.tools.vid_id_tools import *

process.load("RecoEgamma.ElectronIdentification.egmGsfElectronIDs_cfi")
# overwrite a default parameter: for miniAOD, the collection name is a slimmed one
process.egmGsfElectronIDs.physicsObjectSrc = cms.InputTag('slimmedElectrons')

from PhysicsTools.SelectorUtils.centralIDRegistry import central_id_registry
process.egmGsfElectronIDSequence = cms.Sequence(process.egmGsfElectronIDs)

# Define which IDs we want to produce
# Each of these two example IDs contains all four standard 
# cut-based ID working points (only two WP of the PU20bx25 are actually used here).
my_id_modules = ['RecoEgamma.ElectronIdentification.Identification.cutBasedElectronID_PHYS14_PU20bx25_V1_miniAOD_cff']
#Add them to the VID producer
for idmod in my_id_modules:
    setupAllVIDIdsInModule(process,idmod,setupVIDElectronSelection)



process.pat2pxlio=cms.EDAnalyzer('EDM2PXLIO',
    outFileName=cms.string("output.pxlio"),
    processName=cms.string(options.processName),
    selectEvents=cms.VPSet(
        cms.PSet(
            process=cms.string("STEA"),
            paths=cms.vstring(
                "STEA_filtered",
                "STEA_plain"
            ),
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
        names=cms.vstring("Electron"),
        valueMaps = cms.PSet(
            phys14eleIDVeto=cms.PSet(
                type=cms.string("ValueMapAccessorBool"),
                src=cms.InputTag("egmGsfElectronIDs","cutBasedElectronID-PHYS14-PU20bx25-V1-miniAOD-standalone-veto"),
            ),
            phys14eleIDLoose=cms.PSet(
                type=cms.string("ValueMapAccessorBool"),
                src=cms.InputTag("egmGsfElectronIDs","cutBasedElectronID-PHYS14-PU20bx25-V1-miniAOD-standalone-loose"),
            ),
            phys14eleIDTight=cms.PSet(
                type=cms.string("ValueMapAccessorBool"),
                src=cms.InputTag("egmGsfElectronIDs","cutBasedElectronID-PHYS14-PU20bx25-V1-miniAOD-standalone-tight"),
            ),
        ),
        
    ),
                                 
    jets = cms.PSet(
        type=cms.string("JetConverter"),
        srcs=cms.VInputTag(cms.InputTag("slimmedJets")),
        names=cms.vstring("Jet"),
    ),
    
    genParticles= cms.PSet(
        type=cms.string("GenParticleConverter"),
        srcs=cms.VInputTag(cms.InputTag("lessGenParticles")),
        targetEventViews=cms.vstring("Generated"),
        #LHEEvent=cms.InputTag("source","","LHEFile"),
        GenEventInfo=cms.InputTag("generator","","SIM")
    ),
    
    genjets = cms.PSet(
        type=cms.string("GenJetConverter"),
        srcs=cms.VInputTag(cms.InputTag("slimmedGenJets")),
        names=cms.vstring("GenJet"),
        targetEventViews=cms.vstring("GenJets"),
    ),
                             
    mets = cms.PSet(
        type=cms.string("METConverter"),
        srcs=cms.VInputTag(cms.InputTag("slimmedMETs")),
        names=cms.vstring("MET")
    ),
    
    triggersHLT = cms.PSet(
        type=cms.string("TriggerResultConverter"),
        srcs=cms.VInputTag(cms.InputTag("TriggerResults","","HLT")),
        regex=cms.vstring("HLT_Iso[0-9a-zA-z]*","HLT_Ele[0-9a-zA-z]*")
    ),
    
    triggersPAT = cms.PSet(
        type=cms.string("TriggerResultConverter"),
        srcs=cms.VInputTag(cms.InputTag("TriggerResults","","PAT")),
        regex=cms.vstring("[0-9a-zA-z]*")
    ),
    
    triggersSTEA = cms.PSet(
        type=cms.string("TriggerResultConverter"),
        srcs=cms.VInputTag(cms.InputTag("TriggerResults","","STEA")),
        regex=cms.vstring("STEA_filtered")
    ),
    
    puInfo = cms.PSet(
        type=cms.string("PileupSummaryInfoConverter"),
        srcs=cms.VInputTag(cms.InputTag("addPileupInfo","","HLT")),
        names=cms.vstring("PU")
    ),
)


process.STEA_plain=cms.Path(
    process.lessGenParticles
    *process.egmGsfElectronIDSequence
)

process.STEA_filtered=cms.Path(
    process.lessGenParticles
    *process.goodOfflinePrimaryVertices
    *process.egmGsfElectronIDSequence
)

process.endpath= cms.EndPath(process.pat2pxlio)


