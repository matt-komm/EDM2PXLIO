import FWCore.ParameterSet.Config as cms
from FWCore.ParameterSet.VarParsing import VarParsing

import os

options = VarParsing ('analysis')

options.register(
    'onlyFiltered',
    False,
    VarParsing.multiplicity.singleton,
    VarParsing.varType.bool,
    "keep only filtered events"
)

options.register(
    'processName',
    "unknown",
    VarParsing.multiplicity.singleton,
    VarParsing.varType.string,
    "name of process"
)
				  
options.parseArguments()

process = cms.Process("DX")
process.options = cms.untracked.PSet( wantSummary = cms.untracked.bool(True) )
process.options.allowUnscheduled = cms.untracked.bool(True)
#process.options.numberOfThreads = cms.untracked.uint32(4)
#process.options.numberOfStreams = cms.untracked.uint32(4)

process.load('Configuration.StandardSequences.Services_cff')
process.load('Configuration.EventContent.EventContent_cff')
process.load('Configuration.StandardSequences.GeometryDB_cff')
process.load('Configuration.StandardSequences.MagneticField_38T_cff')
process.load('Configuration.StandardSequences.FrontierConditions_GlobalTag_condDBv2_cff')
#process.load('Configuration.StandardSequences.FrontierConditions_GlobalTag_cff')


process.GlobalTag.globaltag = '80X_mcRun2_asymptotic_2016_miniAODv2'

### frontier database ###
from CondCore.DBCommon.CondDBSetup_cfi import *
process.frontierDB = cms.ESSource("PoolDBESSource",
    #CondDBSetup,
    toGet = cms.VPSet(
        cms.PSet(
            record = cms.string('JetCorrectionsRecord'),
            tag    = cms.string('JetCorrectorParametersCollection_Spring16_25nsV3_MC_AK4PFchs'),
            label  = cms.untracked.string('AK4PFchs')
        ),
    ),
    #connect = cms.string('frontier://FrontierProd/CMS_CONDITIONS'),
    connect = cms.string('sqlite:Spring16_25nsV3_MC.db')
)

process.es_prefer_jec = cms.ESPrefer("PoolDBESSource",'frontierDB')

process.load("FWCore.MessageLogger.MessageLogger_cfi")
process.MessageLogger.cerr.FwkReport.reportEvery = 100
process.MessageLogger.suppressWarning = cms.untracked.vstring('ecalLaserCorrFilter','manystripclus53X','toomanystripclus53X')

process.source = cms.Source("PoolSource",
    fileNames = cms.untracked.vstring(
        'root://cms-xrd-global.cern.ch//store/mc/RunIISpring16MiniAODv2/ST_t-channel_top_4f_leptonDecays_13TeV-powheg-pythia8_TuneCUETP8M1/MINIAODSIM/PUSpring16_80X_mcRun2_asymptotic_2016_miniAODv2_v0-v1/70000/005CECC7-5340-E611-A703-1CC1DE18C984.root',
        #'root://cms-xrd-global.cern.ch//store/mc/RunIISpring16MiniAODv2/TT_TuneCUETP8M1_13TeV-powheg-pythia8/MINIAODSIM/PUSpring16_80X_mcRun2_asymptotic_2016_miniAODv2_v0_ext4-v1/00000/004A0552-3929-E611-BD44-0025905A48F0.root'
    ),
    #lumisToProcess = cms.untracked.VLuminosityBlockRange('251244:96-251244:121'),
)
    
process.maxEvents = cms.untracked.PSet( input = cms.untracked.int32(200) )

process.DX_plain=cms.Path()
process.DX_filtered=cms.Path()


def addModule(m):
    process.DX_plain+=m
    process.DX_filtered+=m


### selectors ###
process.skimSequence = cms.Sequence()
process.DX_filtered+=process.skimSequence

def addFilter(inputTag,cutString,minN=None):
    name = str(inputTag._InputTag__moduleLabel)
    
    if minN!=None:
        name+="min"+str(minN)
        
    selector = cms.EDFilter("CandViewSelector",
        src = inputTag,
        cut = cms.string(cutString)
    )
    selectorName="select"+name
    setattr(process,selectorName,selector)
    process.skimSequence+=selector
  
    if minN!=None:
        selectorMinFilter = cms.EDFilter("CandViewCountFilter",
            src = cms.InputTag(selectorName),
            minNumber = cms.uint32(minN)
        )
        selectorMinFilterName="minFilter"+name
        setattr(process,selectorMinFilterName,selectorMinFilter)
        process.skimSequence+=selectorMinFilter
    
    
addFilter(cms.InputTag("slimmedMuons"),"pt>15.0",minN=1)
addFilter(cms.InputTag("slimmedJets"),"pt>15.0",minN=2)
addFilter(cms.InputTag("slimmedJets"),"pt>20.0",minN=1)



### gen particle pruner ###

process.load('SimGeneral.HepPDTESSource.pythiapdt_cfi')


process.lessGenParticles = cms.EDProducer("GenParticlePruner",
    src=cms.InputTag("prunedGenParticles"),
    select=cms.vstring(
        "drop  *",
        #"++keep abs(status)>20 & abs(status) < 30", #keeps all particles from the hard matrix element and their mothers
        "++keep isHardProcess",
        "keep++ abs(pdgId)=24", #keep W-boson decay
        "keep++ abs(pdgId)=23", #keep Z-boson decay
        "drop abs(status)>30", #drop all intermediate from decay 
        
        #keep a few other statuses
        "keep isLastCopy & (abs(pdgId)=6 | abs(pdgId)=5)",
        "keep isLastCopyBeforeFSR & (abs(pdgId)=6 | abs(pdgId)=5)",
        "keep fromHardProcessBeforeFSR & (abs(pdgId)=6 | abs(pdgId)=5)",
        #"keep isFirstCopy"
        
        #keep prompt leptons
        "keep (isPromptFinalState) & abs(pdgId)>10 & abs(pdgId)<19",
        
        #"keep++ abs(pdgId)=5"
    )
)
addModule(process.lessGenParticles)

### particle level

pSeq = cms.Sequence()

process.fiducialParticles = cms.EDFilter("PackedGenParticleSelector",
    src = cms.InputTag("packedGenParticles"),
    cut = cms.string("abs(eta)<5.0 && (status==1)")
)
pSeq+=process.fiducialParticles

process.promtLeptons = cms.EDFilter("PackedGenParticleSelector",
    src = cms.InputTag("fiducialParticles"),
    cut = cms.string(
        "abs(eta)<5.0"
        + "&& (isPromptFinalState || isDirectPromptTauDecayProductFinalState)"
        + "&& (abs(pdgId)==11 || abs(pdgId)==13 || abs(pdgId)==22)"
    )
)
process.dressedLeptons = cms.EDProducer("JetClustering",
    src=cms.InputTag("promtLeptons"),
    jetR=cms.double(0.1),
    minJetPt=cms.double(15.0)
)
pSeq+=process.promtLeptons+process.dressedLeptons

process.promtNu = cms.EDFilter("PackedGenParticleSelector",
    src = cms.InputTag("fiducialParticles"),
    cut = cms.string(
        "abs(eta)<5.0"
        + "&& (isPromptFinalState || isDirectPromptTauDecayProductFinalState)"
        + "&& (abs(pdgId)==12 || abs(pdgId)==14 || abs(pdgId)==16)"
    )
)
process.allNu = cms.EDFilter("PackedGenParticleSelector",
    src = cms.InputTag("fiducialParticles"),
    cut = cms.string(
        "abs(eta)<5.0"
        + "&& (abs(pdgId)==12 || abs(pdgId)==14 || abs(pdgId)==16)"
    )
)
pSeq+=process.promtNu+process.allNu

process.ghostBHadrons = cms.EDProducer("BHadronSelector",
    src = cms.InputTag("prunedGenParticles"),
    cut = cms.string("pt>5")
)
pSeq+=process.ghostBHadrons

process.stableParticles = cms.EDProducer("PackedGenParticleProjection",
    combine = cms.VInputTag(
        cms.InputTag("fiducialParticles"),
    ),
)
process.particleJets = cms.EDProducer("JetClustering",
    src=cms.InputTag("stableParticles"),
    jetR=cms.double(0.4),
    minJetPt=cms.double(20.0),
    ghosts = cms.PSet(
        bHadrons = cms.InputTag("ghostBHadrons")
    ),
)
pSeq+=process.stableParticles+process.particleJets

process.stableParticlesExL = cms.EDProducer("PackedGenParticleProjection",
    combine = cms.VInputTag(
        cms.InputTag("fiducialParticles"),
    ),
    exclude = cms.VInputTag(
        cms.InputTag("promtLeptons"),
    ),
)
process.particleJetsExL = cms.EDProducer("JetClustering",
    src=cms.InputTag("stableParticlesExL"),
    jetR=cms.double(0.4),
    minJetPt=cms.double(20.0),
    ghosts = cms.PSet(
        bHadrons = cms.InputTag("ghostBHadrons")
    ),
)
pSeq+=process.stableParticlesExL+process.particleJetsExL

process.stableParticlesExLN = cms.EDProducer("PackedGenParticleProjection",
    combine = cms.VInputTag(
        cms.InputTag("fiducialParticles"),
    ),
    exclude = cms.VInputTag(
        cms.InputTag("promtLeptons"),
        cms.InputTag("promtNu"),
    ),
)
process.particleJetsExLN = cms.EDProducer("JetClustering",
    src=cms.InputTag("stableParticlesExLN"),
    jetR=cms.double(0.4),
    minJetPt=cms.double(20.0),
    ghosts = cms.PSet(
        bHadrons = cms.InputTag("ghostBHadrons")
    ),
)
pSeq+=process.stableParticlesExLN+process.particleJetsExLN

process.stableParticlesExLNN = cms.EDProducer("PackedGenParticleProjection",
    combine = cms.VInputTag(
        cms.InputTag("fiducialParticles"),
    ),
    exclude = cms.VInputTag(
        cms.InputTag("promtLeptons"),
        cms.InputTag("allNu"),
    ),
)
process.particleJetsExLNN = cms.EDProducer("JetClustering",
    src=cms.InputTag("stableParticlesExLNN"),
    jetR=cms.double(0.4),
    minJetPt=cms.double(20.0),
    ghosts = cms.PSet(
        bHadrons = cms.InputTag("ghostBHadrons")
    ),
)
pSeq+=process.stableParticlesExLNN+process.particleJetsExLNN


addModule(pSeq)
###TODO: MET from only prompt or allNu; Btagging



### electron IDs ###

from PhysicsTools.SelectorUtils.tools.vid_id_tools import *
switchOnVIDElectronIdProducer(process, DataFormat.MiniAOD)
for eleID in [
    'RecoEgamma.ElectronIdentification.Identification.cutBasedElectronID_Spring15_25ns_V1_cff',
]:
    setupAllVIDIdsInModule(process,eleID,setupVIDElectronSelection)

addModule(process.egmGsfElectronIDSequence)



### JEC ###

from PhysicsTools.PatAlgos.producersLayer1.jetUpdater_cff import updatedPatJetCorrFactors
process.jec25nsSpring16V3 = updatedPatJetCorrFactors.clone(
    src = cms.InputTag("slimmedJets"),
    levels = [
        'L1FastJet', 
        'L2Relative', 
        'L3Absolute'
    ],
    payload = 'AK4PFchs' 
)
    
from PhysicsTools.PatAlgos.producersLayer1.jetUpdater_cff import updatedPatJets
process.slimmedJetsJEC = updatedPatJets.clone(
    jetSource = cms.InputTag("slimmedJets"),
    jetCorrFactorsSource = cms.VInputTag(
        cms.InputTag("jec25nsSpring16V3")
    )
)
process.reapplyJEC = cms.Sequence(process.jec25nsSpring16V3*process.slimmedJetsJEC)
addModule(process.reapplyJEC)


### recalculate MET & Uncertainties ###

from PhysicsTools.PatUtils.tools.runMETCorrectionsAndUncertainties import runMetCorAndUncFromMiniAOD

runMetCorAndUncFromMiniAOD(
    process,
    isData=False
)

### Q/G Likelihood ###

### MET Significance ###

### PV selection ###

process.goodVertexFilter = cms.EDFilter("GoodVertexFilter",
    vertexCollection = cms.InputTag('offlineSlimmedPrimaryVertices'),
    minimumNDOF = cms.uint32(4) ,
    maxAbsZ = cms.double(24),
    maxd0 = cms.double(2)
)
process.DX_primaryVertexFilter = cms.Path(process.goodVertexFilter)



### OUTPUT ###


filteredPath = "DX_filtered"

process.pat2pxlio=cms.EDAnalyzer('EDM2PXLIO',
    outFileName=cms.string("output.pxlio"),
    processName=cms.string(options.processName),
    filterPaths=cms.vstring(filteredPath),
    primaryVertex = cms.InputTag("offlineSlimmedPrimaryVertices"),
    rho = cms.InputTag("fixedGridRhoFastjetAll"),
)

if not options.onlyFiltered:
    process.pat2pxlio.filterPaths.append("DX_plain")
    
    


setattr(process.pat2pxlio,"muons",cms.PSet(
    type=cms.string("MuonConverter"),
    srcs=cms.VInputTag(cms.InputTag("slimmedMuons")),
    names=cms.vstring("Muon"),
    valueMaps = cms.PSet(),
    select=cms.string("pt>10.0"),
))

setattr(process.pat2pxlio,"electrons",cms.PSet(
    type=cms.string("ElectronConverter"),
    srcs=cms.VInputTag(cms.InputTag("slimmedElectrons")),
    names=cms.vstring("Electron"),
    valueMaps=cms.PSet(
        spring15eleIDVeto25ns = cms.PSet(
            type=cms.string("ValueMapAccessorBool"),
            src=cms.InputTag("egmGsfElectronIDs","cutBasedElectronID-Spring15-25ns-V1-standalone-veto")
        ),
        spring15eleIDTight25ns = cms.PSet(
            type=cms.string("ValueMapAccessorBool"),
            src=cms.InputTag("egmGsfElectronIDs","cutBasedElectronID-Spring15-25ns-V1-standalone-tight")
        ),
        
    ),
    select=cms.string("pt>10.0"),
))

setattr(process.pat2pxlio,"patJets",cms.PSet(
    type=cms.string("JetConverter"),
    srcs=cms.VInputTag(
        cms.InputTag("patJetsReapplyJEC"),
        
        #cms.InputTag("shiftedPatJetEnDown"), # this cannot be used - jets are already cleaned against leptons for T1
        #cms.InputTag("shiftedPatJetEnUp"), # this cannot be used - jets are already cleaned against leptons for T1
    ),
    names=cms.vstring(
        "Jet",
        #"JetEnDown",
        #"JetEnUp"
    ),
    select=cms.string("pt>15.0"),
    valueMaps=cms.PSet(
    ),
    filterPaths=cms.vstring(filteredPath),
))
setattr(process.pat2pxlio,"patMET",cms.PSet(
    type=cms.string("METConverter"),
    srcs=cms.VInputTag(
        cms.InputTag("patPFMetT1"),
        #cms.InputTag("patPFMetT1JetEnDown"),
        #cms.InputTag("patPFMetT1JetEnUp"),
    ),
    names=cms.vstring(
        "MET",
        #"METEnDown",
        #"METEnUp"
    )
))


#add PU info
setattr(process.pat2pxlio,"puInfo",cms.PSet(
    type=cms.string("PileupSummaryInfoConverter"),
    srcs=cms.VInputTag(cms.InputTag("slimmedAddPileupInfo")),
    names=cms.vstring("PU")
))

#add trigger info
setattr(process.pat2pxlio,"triggersHLT",cms.PSet(
    type=cms.string("TriggerResultConverter"),
    srcs=cms.VInputTag(cms.InputTag("TriggerResults","","HLT")),
    regex=cms.vstring("HLT_Iso[0-9a-zA-z_]*","HLT_Ele[0-9a-zA-z_]*")
))

setattr(process.pat2pxlio,"triggersDX",cms.PSet(
    type=cms.string("TriggerResultConverter"),
    srcs=cms.VInputTag(cms.InputTag("TriggerResults","","DX")),
    regex=cms.vstring("[0-9a-zA-z_]*")
))

setattr(process.pat2pxlio,"triggersPAT",cms.PSet(
    type=cms.string("TriggerResultConverter"),
    srcs=cms.VInputTag(cms.InputTag("TriggerResults","","PAT")),
    regex=cms.vstring("[0-9a-zA-z_]*"),
    prefix=cms.vstring(["PAT_"])
))

setattr(process.pat2pxlio,"genParticles",cms.PSet(
    type=cms.string("GenParticleConverter"),
    srcs=cms.VInputTag(cms.InputTag("lessGenParticles")),
    targetEventViews=cms.vstring("Generated"),
    LHEEvent=cms.InputTag("externalLHEProducer"),
    GenEventInfo=cms.InputTag("generator","","SIM"),
    filterPaths=cms.vstring(filteredPath),
))


setattr(process.pat2pxlio,"dressedLeptons",cms.PSet(
    type=cms.string("PltJetConverter"),
    srcs=cms.VInputTag(cms.InputTag("dressedLeptons")),
    names=cms.vstring("Lepton"),
    targetEventViews=cms.vstring("ParticleLevel"),
))


setattr(process.pat2pxlio,"particleJets",cms.PSet(
    type=cms.string("PltJetConverter"),
    srcs=cms.VInputTag(cms.InputTag("particleJets")),
    names=cms.vstring("Jet"),
    targetEventViews=cms.vstring("ParticleLevel"),
))

setattr(process.pat2pxlio,"particleJetsExL",cms.PSet(
    type=cms.string("PltJetConverter"),
    srcs=cms.VInputTag(cms.InputTag("particleJetsExL")),
    names=cms.vstring("JetExL"),
    targetEventViews=cms.vstring("ParticleLevel"),
))

setattr(process.pat2pxlio,"particleJetsExLN",cms.PSet(
    type=cms.string("PltJetConverter"),
    srcs=cms.VInputTag(cms.InputTag("particleJetsExLN")),
    names=cms.vstring("JetExLN"),
    targetEventViews=cms.vstring("ParticleLevel"),
))

setattr(process.pat2pxlio,"particleJetsExLNN",cms.PSet(
    type=cms.string("PltJetConverter"),
    srcs=cms.VInputTag(cms.InputTag("particleJetsExLNN")),
    names=cms.vstring("JetExLNN"),
    targetEventViews=cms.vstring("ParticleLevel"),
))



setattr(process.pat2pxlio,"ghostBHadrons",cms.PSet(
    type=cms.string("PackedGenParticleConverter"),
    srcs=cms.VInputTag(cms.InputTag("ghostBHadrons")),
    names=cms.vstring("GhostB"),
    targetEventViews=cms.vstring("GhostBs"),
))

setattr(process.pat2pxlio,"ghostBHadrons",cms.PSet(
    type=cms.string("PackedGenParticleConverter"),
    srcs=cms.VInputTag(cms.InputTag("ghostBHadrons")),
    names=cms.vstring("GhostB"),
    targetEventViews=cms.vstring("GhostBs"),
))

setattr(process.pat2pxlio,"promtNu",cms.PSet(
    type=cms.string("PackedGenParticleConverter"),
    srcs=cms.VInputTag(cms.InputTag("promtNu")),
    names=cms.vstring("Nu"),
    targetEventViews=cms.vstring("PromptNu"),
))

setattr(process.pat2pxlio,"allNu",cms.PSet(
    type=cms.string("PackedGenParticleConverter"),
    srcs=cms.VInputTag(cms.InputTag("allNu")),
    names=cms.vstring("Nu"),
    targetEventViews=cms.vstring("AllNu"),
))


process.endpath= cms.EndPath()

process.endpath+=process.pat2pxlio
'''
print "-------------------------------------"
print "WARNING: root output module in cfg!!!"
print "-------------------------------------"
process.OUT = cms.OutputModule("PoolOutputModule",
    fileName = cms.untracked.string('output.root'),
    outputCommands = cms.untracked.vstring( "keep *","drop *_*_*_DX", "keep *_QGL*_*_*"),
    dropMetaData = cms.untracked.string('ALL'),
)
process.endpath+= process.OUT
'''
