import FWCore.ParameterSet.Config as cms
from FWCore.ParameterSet.VarParsing import VarParsing

import os

options = VarParsing ('analysis')

#TODO: forward option to plugins
options.register(
    'isData',
    False,
    VarParsing.multiplicity.singleton,
    VarParsing.varType.bool,
    "is data"
)

options.register(
    'reHLT',
    False,
    VarParsing.multiplicity.singleton,
    VarParsing.varType.bool,
    "is data"
)

options.register(
    'isReRecoData',
    False,
    VarParsing.multiplicity.singleton,
    VarParsing.varType.bool,
    "is rereco data"
)

options.register(
    'onlyFiltered',
    False,
    VarParsing.multiplicity.singleton,
    VarParsing.varType.bool,
    "keep only filtered events"
)

options.register(
    'noFilter',
    False,
    VarParsing.multiplicity.singleton,
    VarParsing.varType.bool,
    "apply no filtering"
)

options.register(
    'addSys',
    True,
    VarParsing.multiplicity.singleton,
    VarParsing.varType.bool,
    "adds JEC/JER uncertainties"
)

options.register(
    'noGen',
    False,
    VarParsing.multiplicity.singleton,
    VarParsing.varType.bool,
    "removes storing of gen objects"
)

options.register(
    'noLHE',
    False,
    VarParsing.multiplicity.singleton,
    VarParsing.varType.bool,
    "removes LHE weights"
)

options.register(
    'addPL',
    False,
    VarParsing.multiplicity.singleton,
    VarParsing.varType.bool,
    "adds particle level objects"
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

process = cms.Process("DX")
process.options = cms.untracked.PSet( wantSummary = cms.untracked.bool(True) )
process.options.allowUnscheduled = cms.untracked.bool(True)
#process.options.numberOfThreads = cms.untracked.uint32(4)
#process.options.numberOfStreams = cms.untracked.uint32(4)

process.load('Configuration.StandardSequences.Services_cff')
process.load('Configuration.EventContent.EventContent_cff')
process.load('Configuration.StandardSequences.GeometryDB_cff')
process.load('Configuration.StandardSequences.MagneticField_38T_cff')
process.load('CondCore.DBCommon.CondDBSetup_cfi')
process.load('Configuration.StandardSequences.FrontierConditions_GlobalTag_cff')

#from CondCore.DBCommon.CondDBSetup_cfi import CondDBSetup

if options.isData and not options.isReRecoData:
    process.GlobalTag.globaltag = '80X_dataRun2_2016SeptRepro_v7'
    
    ### frontier database ###
    process.dbJEC = cms.ESSource("PoolDBESSource",
        #CondDBSetup,
        toGet = cms.VPSet(
            cms.PSet(
                record = cms.string('JetCorrectionsRecord'),
                tag    = cms.string('JetCorrectorParametersCollection_Summer16_23Sep2016AllV4_DATA_AK4PFchs'),
                #tag    = cms.string('JetCorrectorParametersCollection_Spring16_23Sep2016AllV2_DATA_AK4PFchs'),
                label  = cms.untracked.string('AK4PFchs')
            ),
        ),
        #connect = cms.string('frontier://FrontierProd/CMS_CONDITIONS'),
        connect = cms.string('sqlite:Summer16_23Sep2016AllV4_DATA.db'),
        #connect = cms.string('sqlite:Spring16_23Sep2016AllV2_DATA.db')
    )
elif options.isData and options.isReRecoData:
    process.GlobalTag.globaltag = '80X_dataRun2_Prompt_v16'
    
    ### frontier database ###
    process.dbJEC = cms.ESSource("PoolDBESSource",
        #CondDBSetup,
        toGet = cms.VPSet(
            cms.PSet(
                record = cms.string('JetCorrectionsRecord'),
                tag    = cms.string('JetCorrectorParametersCollection_Summer16_23Sep2016AllV4_DATA_AK4PFchs'),
                #tag    = cms.string('JetCorrectorParametersCollection_Spring16_23Sep2016AllV2_DATA_AK4PFchs'),
                label  = cms.untracked.string('AK4PFchs')
            ),
        ),
        #connect = cms.string('frontier://FrontierProd/CMS_CONDITIONS'),
        connect = cms.string('sqlite:Summer16_23Sep2016AllV4_DATA.db'),
        #connect = cms.string('sqlite:Spring16_23Sep2016AllV2_DATA.db')
    )
else:
    process.GlobalTag.globaltag = '80X_mcRun2_asymptotic_2016_TrancheIV_v6'
    
    ### frontier database ###
    process.dbJEC = cms.ESSource("PoolDBESSource",
        #CondDBSetup,
        toGet = cms.VPSet(
            cms.PSet(
                record = cms.string('JetCorrectionsRecord'),
                tag    = cms.string('JetCorrectorParametersCollection_Summer16_23Sep2016V4_MC_AK4PFchs'),
                #tag    = cms.string('JetCorrectorParametersCollection_Spring16_23Sep2016V2_MC_AK4PFchs'),
                label  = cms.untracked.string('AK4PFchs')
            ),
        ),
        #connect = cms.string('frontier://FrontierProd/CMS_CONDITIONS'),
        connect = cms.string('sqlite:Summer16_23Sep2016V4_MC.db')
        #connect = cms.string('sqlite:Spring16_23Sep2016V2_MC.db')
    )
    
    process.dbJES = cms.ESSource("PoolDBESSource",
        #CondDBSetup,
        toGet = cms.VPSet(
            cms.PSet(
                record = cms.string('JetResolutionRcd'),
                tag    = cms.string('JR_Summer16_25nsV1_80X_MC_PtResolution_AK4PFchs'),
                label  = cms.untracked.string('AK4PFchs_pt')
            ),
            cms.PSet(
                record = cms.string('JetResolutionScaleFactorRcd'),
                tag    = cms.string('JR_Summer16_25nsV1_80X_MC_SF_AK4PFchs'),
                label  = cms.untracked.string('AK4PFchs')
            ),
        ),
        #connect = cms.string('frontier://FrontierProd/CMS_CONDITIONS'),
        connect = cms.string('sqlite:Summer16_25nsV1_80X_MC.db')
    )


process.es_prefer_jec = cms.ESPrefer("PoolDBESSource",'dbJEC')

if not options.isData:
    process.es_prefer_jer = cms.ESPrefer('PoolDBESSource', 'dbJES')

print "JECs -------------------"
print "\t",process.dbJEC.toGet[0].tag
print "\t",process.dbJEC.connect

if not options.isData:
    process.es_prefer_jer = cms.ESPrefer('PoolDBESSource', 'dbJES')

    print "JER -------------------"
    for s in process.dbJES.toGet:
        print "\t",s.tag
    print "\t",process.dbJES.connect
print " ------------------- "



process.load("FWCore.MessageLogger.MessageLogger_cfi")
process.MessageLogger.cerr.FwkReport.reportEvery = 100
process.MessageLogger.suppressWarning = cms.untracked.vstring('ecalLaserCorrFilter','manystripclus53X','toomanystripclus53X')



process.source = cms.Source("PoolSource",
    inputCommands = cms.untracked.vstring("keep *", "drop LHERunInfoProduct_*_*_*"), #this produces otherwise a memleak
    fileNames = cms.untracked.vstring(
        '/store/mc/RunIIFall15MiniAODv2/ST_t-channel_4f_leptonDecays_13TeV-amcatnlo-pythia8_TuneCUETP8M1/MINIAODSIM/PU25nsData2015v1_76X_mcRun2_asymptotic_v12-v1/00000/4E432BC0-F8BB-E511-B2DF-002590760A10.root'
    ),
    #skipEvents = cms.untracked.uint32(24900)
    #lumisToProcess = cms.untracked.VLuminosityBlockRange('251244:96-251244:121'),
)
    
    
if options.isData:
    process.maxEvents = cms.untracked.PSet( input = cms.untracked.int32(200) )
else:
    process.maxEvents = cms.untracked.PSet( input = cms.untracked.int32(200) )
    
### bad muon filter ###
#process.load("RecoMET.METFilters.badGlobalMuonTaggersMiniAOD_cff")
#process.DX_hasBadCloneMuons = cms.Path(process.cloneGlobalMuonTagger)
#process.DX_hasBadMuons = cms.Path(process.badGlobalMuonTagger)

process.DX_plain=cms.Path()
process.DX_filtered_mu=cms.Path()
process.DX_filtered_ele=cms.Path()

#process.DX_plain.isMC=cms.bool(not options.isData)
#process.DX_filtered_mu.isMC=cms.bool(not options.isData)
#process.DX_filtered_ele.isMC=cms.bool(not options.isData)

def addModule(m):
    process.DX_plain+=m
    process.DX_filtered_mu+=m
    process.DX_filtered_ele+=m


### selectors ###
skimMuSequence = cms.Sequence()
process.DX_filtered_mu+=skimMuSequence
skimEleSequence = cms.Sequence()
process.DX_filtered_ele+=skimEleSequence



### gen particle pruner ###

process.load('SimGeneral.HepPDTESSource.pythiapdt_cfi')

process.lessGenParticles = cms.EDProducer("GenParticlePruner",
    src=cms.InputTag("prunedGenParticles"),
    select=cms.vstring(
        "drop  *",
        #"++keep abs(status)>20 & abs(status) < 30", #keeps all particles from the hard matrix element and their mothers
        "++keep isHardProcess",
        "keep++ abs(pdgId)=24 & (isHardProcess | fromHardProcessFinalState)", #keep W-boson decay
        "keep++ abs(pdgId)=23 & (isHardProcess | fromHardProcessFinalState)", #keep Z-boson decay
        "keep++ abs(pdgId)=22 & (isHardProcess | fromHardProcessFinalState)", #keep gamma-boson decay
        "drop abs(status)>30 & (abs(pdgId)!=24 | abs(pdgId)!=23 | abs(pdgId)!=22) & (!isHardProcess | !fromHardProcessFinalState)", #drop all intermediate from decay 
        
        #keep a few other statuses
        "keep (isLastCopyBeforeFSR | isLastCopy | fromHardProcessBeforeFSR) & (abs(pdgId)<7)",
        #"keep isLastCopyBeforeFSR & (abs(pdgId)=6 | abs(pdgId)=5)",
        #"keep fromHardProcessBeforeFSR & (abs(pdgId)=6 | abs(pdgId)=5)",
        #"keep isFirstCopy"
        
        #keep prompt leptons/photon
        "keep (isPromptFinalState) & ((abs(pdgId)>10 & abs(pdgId)<19) || abs(pdgId)=22)",
        
        #"drop abs(pdgId)==5 & isLastCopy"
        #"keep++ abs(pdgId)=5"
    )
)
addModule(process.lessGenParticles)


#file service for EventWeight and PU info
if not options.isData:
    process.TFileService = cms.Service("TFileService", 
        fileName = cms.string("info.root")
    )

    process.eventAndPuInfo = cms.EDAnalyzer("EventInfoCollector",
        GenEventInfo=cms.InputTag("generator"),
        PileupSummaryInfo=cms.InputTag("slimmedAddPileupInfo")
    )
    process.DX_plain+=process.eventAndPuInfo



filteredPath = ["DX_filtered_mu","DX_filtered_ele"]

process.pat2pxlio=cms.EDAnalyzer('EDM2PXLIO',
    outFileName=cms.string("output.pxlio"),
    processName=cms.string(options.processName),
    filterPaths=cms.vstring(filteredPath),
    primaryVertex = cms.InputTag("offlineSlimmedPrimaryVertices"),
    rho = cms.InputTag("fixedGridRhoFastjetAll"),
)

if not options.onlyFiltered:
    process.pat2pxlio.filterPaths.append("DX_plain")
    
    


#add gen particles/jets
if not options.isData:
    setattr(process.pat2pxlio,"genParticles",cms.PSet(
        type=cms.string("GenParticleConverter"),
        srcs=cms.VInputTag(cms.InputTag("lessGenParticles") if not options.noGen else ()),
        targetEventViews=cms.vstring("Generated"),
        LHEEvent=cms.InputTag("externalLHEProducer"),
        skipLHEweights = cms.bool(options.noLHE),
        GenEventInfo=cms.InputTag("generator"),
        filterPaths=cms.vstring(filteredPath),
    ))

    
    
#add particle level reco

print "Adding particle level objects"

from TopQuarkAnalysis.TopEventProducers.producers.pseudoTop_cfi import pseudoTop

process.genMerger = cms.EDProducer("MergedGenParticleProducer",
    inputPruned = cms.InputTag("prunedGenParticles"),
    inputPacked = cms.InputTag("packedGenParticles")
)
process.hepmcProducer = cms.EDProducer("GenParticles2HepMCConverter",
    genParticles = cms.InputTag("genMerger"),
    genEventInfo = cms.InputTag("generator"),
    signalParticlePdgIds = cms.vint32(6,-6)
)

process.rivetPseudoTop = pseudoTop.clone(
    src = cms.InputTag("hepmcProducer","unsmeared"),
    projection = cms.string("PseudoTop"),
    
    maxEta = cms.double(5.), # HF range. Maximum 6.0 on MiniAOD
    
    usePromptFinalStates = cms.bool(True), # for leptons, photons, neutrinos
    excludePromptLeptonsFromJetClustering = cms.bool(True),
    
    leptonConeSize = cms.double(0.1),
    minLeptonPt = cms.double(20.),
    maxLeptonEta = cms.double(2.5),
    
    jetConeSize = cms.double(0.4),
    minJetPt = cms.double(20.),
    maxJetEta = cms.double(5.0),
    
    runTopReconstruction = cms.bool(False),
)
addModule(process.rivetPseudoTop)
setattr(process.pat2pxlio,"rivetLeptons",cms.PSet(
    type=cms.string("GenJetConverter"),
    useNameDB = cms.bool(False),
    srcs=cms.VInputTag(cms.InputTag("rivetPseudoTop","leptons") if not options.noGen else ()),
    names=cms.vstring("Lepton"),
    targetEventViews=cms.vstring("PTR"),
    filterPaths=cms.vstring(filteredPath),
))
setattr(process.pat2pxlio,"rivetJets",cms.PSet(
    type=cms.string("GenJetConverter"),
    srcs=cms.VInputTag(cms.InputTag("rivetPseudoTop","jets") if not options.noGen else ()),
    names=cms.vstring("Jet"),
    targetEventViews=cms.vstring("PTR"),
    filterPaths=cms.vstring(filteredPath),
))
setattr(process.pat2pxlio,"rivetNeutrino",cms.PSet(
    type=cms.string("GenParticleConverter"),
    useNameDB = cms.bool(False),
    srcs=cms.VInputTag(cms.InputTag("rivetPseudoTop","neutrinos") if not options.noGen else ()),
    names=cms.vstring("Neutrino"),
    targetEventViews=cms.vstring("PTR"),
    filterPaths=cms.vstring(filteredPath),
))


process.endpath= cms.EndPath()

process.endpath+=process.pat2pxlio
'''
print "-------------------------------------"
print "WARNING: root output module in cfg!!!"
print "-------------------------------------"
process.OUT = cms.OutputModule("PoolOutputModule",
    fileName = cms.untracked.string('output.root'),
    outputCommands = cms.untracked.vstring(
        "keep *",
        "drop *_*_*_DX",
        "keep *_slimmedElectrons_*_*"
     ),
    dropMetaData = cms.untracked.string('ALL'),
)
process.endpath+= process.OUT
'''
