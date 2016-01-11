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
process.load('Configuration.StandardSequences.FrontierConditions_GlobalTag_condDBv2_cff')
#process.load('Configuration.StandardSequences.FrontierConditions_GlobalTag_cff')


if options.isData:
    process.GlobalTag.globaltag = '76X_dataRun2_v15' #=76miniaodv1 default
    
    ### frontier database ###
    from CondCore.DBCommon.CondDBSetup_cfi import *
    process.frontierDB = cms.ESSource("PoolDBESSource",
        CondDBSetup,
        toGet = cms.VPSet(
            cms.PSet(
                record = cms.string('JetCorrectionsRecord'),
                tag    = cms.string('JetCorrectorParametersCollection_Summer15_V3_DATA_AK4PFchs'), #should correspond to V6 (V3 is here an internal name inside the GT)
                label  = cms.untracked.string('AK4PF')
            ),
        ),
        connect = cms.string('frontier://FrontierProd/CMS_CONDITIONS'),
    )
else:
    process.GlobalTag.globaltag = '76X_mcRun2_asymptotic_v12' #=76miniaodv1 default
    
    ### frontier database ###
    from CondCore.DBCommon.CondDBSetup_cfi import *
    process.frontierDB = cms.ESSource("PoolDBESSource",
        CondDBSetup,
        toGet = cms.VPSet(
            cms.PSet(
                record = cms.string('JetCorrectionsRecord'),
                tag    = cms.string('JetCorrectorParametersCollection_Summer15_25nsV6_MC_AK4PFchs'),
                label  = cms.untracked.string('AK4PF')
            ),
        ),
        connect = cms.string('frontier://FrontierProd/CMS_CONDITIONS'),
    )

process.es_prefer_jec = cms.ESPrefer("PoolDBESSource",'frontierDB')

process.load("FWCore.MessageLogger.MessageLogger_cfi")
process.MessageLogger.cerr.FwkReport.reportEvery = 100
process.MessageLogger.suppressWarning = cms.untracked.vstring('ecalLaserCorrFilter','manystripclus53X','toomanystripclus53X')


if options.isData and not options.isReRecoData:
    process.source = cms.Source("PoolSource",
        fileNames = cms.untracked.vstring(
        '/store/data/Run2015C_25ns/SingleMuon/MINIAOD/16Dec2015-v1/00000/307CA4CC-E1AF-E511-9A6E-20CF3027A5AE.root'
        ),
        lumisToProcess = cms.untracked.VLuminosityBlockRange('254852:47-254852:94'),
    )
elif options.isData and options.isReRecoData:
    process.source = cms.Source("PoolSource",
        fileNames = cms.untracked.vstring(
            #'root://xrootd.unl.edu//store/data/Run2015B/SingleMuon/MINIAOD/PromptReco-v1/000/251/244/00000/68275270-7C27-E511-B1F0-02163E011A46.root' #{golden run: 251244:96-251244:121}        ),
        )
        #lumisToProcess = cms.untracked.VLuminosityBlockRange('251244:96-251244:121'),
    )
else:
    process.source = cms.Source("PoolSource",
        fileNames = cms.untracked.vstring(
            #'root://xrootd.unl.edu//store/mc/RunIISpring15DR74/TT_TuneCUETP8M1_13TeV-powheg-pythia8/MINIAODSIM/Asympt50ns_MCRUN2_74_V9A-v1/50000/02CF0510-4CFF-E411-A715-0025905A6090.root'
            #'root://xrootd.unl.edu//store/data/Run2015B/SingleMuon/MINIAOD/PromptReco-v1/000/251/244/00000/68275270-7C27-E511-B1F0-02163E011A46.root' #{golden run: 251244:96-251244:121}
            #'root://xrootd.unl.edu//store/mc/RunIISpring15MiniAODv2/WJetsToLNu_TuneCUETP8M1_13TeV-madgraphMLM-pythia8/MINIAODSIM/74X_mcRun2_asymptotic_v2-v1/10000/000EB5C7-6D70-E511-8EF1-BCAEC51FDEED.root'
            'root://xrootd.unl.edu//store/mc/RunIIFall15MiniAODv1/ST_t-channel_5f_leptonDecays_13TeV-amcatnlo-pythia8_TuneCUETP8M1/MINIAODSIM/PU25nsData2015v1_76X_mcRun2_asymptotic_v12-v1/00000/3A84F237-C3A8-E511-A846-002590D9D8B4.root',
            
            #'root://xrootd.unl.edu//store/mc/RunIISpring15DR74/QCD_Pt-20toInf_MuEnrichedPt15_TuneCUETP8M1_13TeV_pythia8/MINIAODSIM/Asympt50ns_MCRUN2_74_V9A-v2/00000/023FCE0B-300A-E511-BE68-001E673973C8.root'
        ),
        #lumisToProcess = cms.untracked.VLuminosityBlockRange('251244:96-251244:121'),
    )
    
    
if options.isData:
    process.maxEvents = cms.untracked.PSet( input = cms.untracked.int32(1000) )
else:
    process.maxEvents = cms.untracked.PSet( input = cms.untracked.int32(500) )

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
addFilter(cms.InputTag("slimmedJets"),"pt>30.0",minN=1)



### gen particle pruner ###

process.load('SimGeneral.HepPDTESSource.pythiapdt_cfi')

if not options.isData:
    process.lessGenParticles = cms.EDProducer("GenParticlePruner",
        src=cms.InputTag("prunedGenParticles"),
        select=cms.vstring(
            "drop  *",
            "++keep abs(status)>20 & abs(status) < 30", #keeps all particles from the hard matrix element and their mothers
            "keep++ abs(pdgId)=24", #keep W-boson decay
            "drop abs(status)>30", #drop all intermediate from decay 
        )
    )
    addModule(process.lessGenParticles)


### electron IDs ###

from PhysicsTools.SelectorUtils.tools.vid_id_tools import *
switchOnVIDElectronIdProducer(process, DataFormat.MiniAOD)
for eleID in [
    #'RecoEgamma.ElectronIdentification.Identification.cutBasedElectronID_PHYS14_PU20bx25_V2_cff',
    #'RecoEgamma.ElectronIdentification.Identification.cutBasedElectronID_Spring15_50ns_V1_cff',
    'RecoEgamma.ElectronIdentification.Identification.cutBasedElectronID_Spring15_25ns_V1_cff',
    #'RecoEgamma.ElectronIdentification.Identification.heepElectronID_HEEPV51_cff'
]:
    setupAllVIDIdsInModule(process,eleID,setupVIDElectronSelection)

addModule(process.egmGsfElectronIDSequence)



### JEC ###

from PhysicsTools.PatAlgos.producersLayer1.jetUpdater_cff import patJetCorrFactorsUpdated
process.jec25nsSummer15V6 = patJetCorrFactorsUpdated.clone(
    src = cms.InputTag("slimmedJets"),
    levels = [
        'L1FastJet', 
        'L2Relative', 
        'L3Absolute'
    ],
    payload = 'AK4PFchs' 
)
if options.isData:
    process.jec25nsSummer15V6.levels.append('L2L3Residual')
    
from PhysicsTools.PatAlgos.producersLayer1.jetUpdater_cff import patJetsUpdated
process.slimmedJetsJEC = patJetsUpdated.clone(
    jetSource = cms.InputTag("slimmedJets"),
    jetCorrFactorsSource = cms.VInputTag(
        cms.InputTag("jec25nsSummer15V6")
    )
)
process.reapplyJEC = cms.Sequence(process.jec25nsSummer15V6*process.slimmedJetsJEC)
addModule(process.reapplyJEC)

### JER
process.smearJets = cms.EDProducer("JetResolutionSmearer",
    src=cms.InputTag("slimmedJetsJEC"),
    SFs=cms.VPSet(
        cms.PSet(factor=cms.double(1.061),select=cms.string("abs(eta)>0.0 && abs(eta)<=0.8")),
        cms.PSet(factor=cms.double(1.088),select=cms.string("abs(eta)>0.8 && abs(eta)<=1.3")),
        cms.PSet(factor=cms.double(1.106),select=cms.string("abs(eta)>1.3 && abs(eta)<=1.9")),
        cms.PSet(factor=cms.double(1.126),select=cms.string("abs(eta)>1.9 && abs(eta)<=2.5")),
        cms.PSet(factor=cms.double(1.343),select=cms.string("abs(eta)>2.5 && abs(eta)<=3.0")),
        cms.PSet(factor=cms.double(1.303),select=cms.string("abs(eta)>3.0 && abs(eta)<=3.2")),
        cms.PSet(factor=cms.double(1.320),select=cms.string("abs(eta)>3.2 && abs(eta)<=5.0"))
    )
)
addModule(process.smearJets)


### Q/G likelihood



### MET uncertainty ###

from PhysicsTools.PatUtils.tools.runMETCorrectionsAndUncertainties import runMetCorAndUncFromMiniAOD
runMetCorAndUncFromMiniAOD(process, 
    #metType="PF",
    jetCollUnskimmed="smearJets",
    jetColl="smearJets",
    #photonColl="slimmedPhotons",
    #electronColl="slimmedElectrons",
    #muonColl="slimmedMuons",
    #tauColl="slimmedTaus",
    #pfCandColl = "packedPFCandidates",
    #jetFlav="AK4PFchs",
    #jetCleaning="LepClean",
    isData=options.isData,
    #jetConfig=False,
    #jetCorLabelL3=cms.InputTag('ak4PFCHSL1FastL2L3Corrector'),
    #jetCorLabelRes=cms.InputTag('ak4PFCHSL1FastL2L3ResidualCorrector'),
    jecUncFile="EDM2PXLIO/Core/data/Summer15_25nsV6_MC_Uncertainty_AK4PFchs.txt",
    #postfix="new"
)

#skip reapplying of JEC in tool
process.patJets = cms.EDFilter("PATJetSelector",
    cut=cms.string("pt>10"),
    src=cms.InputTag("smearJets"),
)

process.shiftedPatJetEnDown.jetCorrUncertaintyTag="" #bug
process.shiftedPatJetEnUp.jetCorrUncertaintyTag="" #bug


### down jer
process.patJetsResDown = cms.EDProducer("JetResolutionSmearer",
    src=cms.InputTag("patJets"),
    SFs=cms.VPSet(
        cms.PSet(factor=cms.double(1.-0.023/1.061),select=cms.string("abs(eta)>0.0 && abs(eta)<=0.8")),
        cms.PSet(factor=cms.double(1.-0.029/1.088),select=cms.string("abs(eta)>0.8 && abs(eta)<=1.3")),
        cms.PSet(factor=cms.double(1.-0.030/1.106),select=cms.string("abs(eta)>1.3 && abs(eta)<=1.9")),
        cms.PSet(factor=cms.double(1.-0.094/1.126),select=cms.string("abs(eta)>1.9 && abs(eta)<=2.5")),
        cms.PSet(factor=cms.double(1.-0.123/1.343),select=cms.string("abs(eta)>2.5 && abs(eta)<=3.0")),
        cms.PSet(factor=cms.double(1.-0.111/1.303),select=cms.string("abs(eta)>3.0 && abs(eta)<=3.2")),
        cms.PSet(factor=cms.double(1.-0.286/1.320),select=cms.string("abs(eta)>3.2 && abs(eta)<=5.0"))
    )
)
### up jer
process.patJetsResUp = cms.EDProducer("JetResolutionSmearer",
    src=cms.InputTag("patJets"),
    SFs=cms.VPSet(
        cms.PSet(factor=cms.double(1.+0.023/1.061),select=cms.string("abs(eta)>0.0 && abs(eta)<=0.8")),
        cms.PSet(factor=cms.double(1.+0.029/1.088),select=cms.string("abs(eta)>0.8 && abs(eta)<=1.3")),
        cms.PSet(factor=cms.double(1.+0.030/1.106),select=cms.string("abs(eta)>1.3 && abs(eta)<=1.9")),
        cms.PSet(factor=cms.double(1.+0.094/1.126),select=cms.string("abs(eta)>1.9 && abs(eta)<=2.5")),
        cms.PSet(factor=cms.double(1.+0.123/1.343),select=cms.string("abs(eta)>2.5 && abs(eta)<=3.0")),
        cms.PSet(factor=cms.double(1.+0.111/1.303),select=cms.string("abs(eta)>3.0 && abs(eta)<=3.2")),
        cms.PSet(factor=cms.double(1.+0.286/1.320),select=cms.string("abs(eta)>3.2 && abs(eta)<=5.0"))
     )
)

'''
def removeL2L3Residual(process,postfix):
    getattr(process,"patPFMetT1T2Corr"+postfix).jetCorrLabelRes = cms.InputTag("L3Absolute")
    getattr(process,"patPFMetT1T2SmearCorr"+postfix).jetCorrLabelRes = cms.InputTag("L3Absolute")
    getattr(process,"patPFMetT2Corr"+postfix).jetCorrLabelRes = cms.InputTag("L3Absolute")
    getattr(process,"patPFMetT2SmearCorr"+postfix).jetCorrLabelRes = cms.InputTag("L3Absolute")
    getattr(process,"shiftedPatJetEnDown"+postfix).jetCorrLabelUpToL3Res = cms.InputTag("ak4PFCHSL1FastL2L3Corrector")
    getattr(process,"shiftedPatJetEnUp"+postfix).jetCorrLabelUpToL3Res = cms.InputTag("ak4PFCHSL1FastL2L3Corrector")

from PhysicsTools.PatUtils.tools.runMETCorrectionsAndUncertainties import runMetCorAndUncFromMiniAOD

runMetCorAndUncFromMiniAOD(process,
    isData=options.isData,
    jecUncFile="EDM2PXLIO/Core/data/Summer15_25nsV2_MC_Uncertainty_AK4PFchs.txt",
    #pfCandColl=cms.InputTag("slimmedJets"),
    postfix=""
)
if not options.isData:
    removeL2L3Residual(process,"")

'''

'''
runMetCorAndUncFromMiniAOD(process,
    isData=options.isData,
    jecUncFile="EDM2PXLIO/Core/data/Summer15_50nsV4_UncertaintySources_AK4PFchs.txt",
    #pfCandColl=cms.InputTag("slimmedJets"),
    postfix="Smeared"
)
if not options.isData:
    removeL2L3Residual(process,"Smeared")
#putting 'Smeared' into the collection name will run JER variations hopefully: need nominal smearing and JES uncertainty for 13TeV
'''

### recluster jets ###
'''
process.pfCHS = cms.EDFilter("CandPtrSelector", src = cms.InputTag("packedPFCandidates"), cut = cms.string("fromPV"))
from RecoJets.JetProducers.ak4PFJets_cfi import ak4PFJets
process.ak4PFJetsCHS = ak4PFJets.clone(
    src = cms.InputTag('pfCHS'), 
    doAreaFastjet = cms.bool(True)
)

from PhysicsTools.PatAlgos.tools.jetTools import *

addJetCollection(
    process,
    labelName = 'AK4PFCHS',
    jetSource = cms.InputTag('ak4PFJetsCHS'),
    pvSource = cms.InputTag('offlineSlimmedPrimaryVertices'),
    pfCandidates = cms.InputTag('packedPFCandidates'),
    svSource = cms.InputTag('slimmedSecondaryVertices'),
    btagDiscriminators = ['pfCombinedInclusiveSecondaryVertexV2BJetTags'],
    jetCorrections = ('AK4PFchs', ['L1FastJet', 'L2Relative', 'L3Absolute'], 'None'),
    genJetCollection = cms.InputTag('slimmedGenJets'),
    genParticles = cms.InputTag('prunedGenParticles'),
    algo = 'AK',
    rParam = 0.4
)

getattr(process,'selectedPatJetsAK4PFCHS').cut = cms.string('pt > 10')
addModule(process.selectedPatJetsAK4PFCHS)
'''

### MET excluding HF ###

'''
process.candidatesNoHF = cms.EDFilter("CandPtrSelector",
    src=cms.InputTag("packedPFCandidates"),
    cut=cms.string("abs(pdgId)!=1 && abs(pdgId)!=2 && abs(eta)<3.0")
)
runMetCorAndUncFromMiniAOD(process,
    isData=options.isData,
    pfCandColl=cms.InputTag("candidatesNoHF"),
    jecUncFile="EDM2PXLIO/Core/data/Summer15_25nsV2_MC_Uncertainty_AK4PFchs.txt",
    postfix="NoHF"
)
if not options.isData:
    removeL2L3Residual(process,"NoHF")
'''

### MET Significance ###

from RecoMET.METProducers.METSignificanceParams_cfi import METSignificanceParams

'''
process.slimmedMETSignificance = cms.EDProducer(
    "METSignificanceProducer",
    srcLeptons = cms.VInputTag(
        'slimmedElectrons',
        'slimmedMuons',
        'slimmedPhotons'
    ),
    srcPfJets = cms.InputTag('slimmedJets'),
    srcMet = cms.InputTag('slimmedMETs'),
    srcPFCandidates = cms.InputTag('packedPFCandidates'),

    parameters = METSignificanceParams
)
addModule(process.slimmedMETSignificance)
'''
'''
process.jetsNoHF = cms.EDFilter("CandPtrSelector",
    src=cms.InputTag("slimmedJets"),
    cut=cms.string("abs(eta)<3.0")
)
addModule(process.jetsNoHF)
process.slimmedMETnoHFSignificance = cms.EDProducer(
    "METSignificanceProducer",
    srcLeptons = cms.VInputTag(
        'slimmedElectrons',
        'slimmedMuons',
        'slimmedPhotons'
    ),
    srcPfJets = cms.InputTag('jetsNoHF'),
    srcMet = cms.InputTag('slimmedMETsNoHF'),
    srcPFCandidates = cms.InputTag('packedPFCandidates'),

    parameters = METSignificanceParams
)
addModule(process.slimmedMETnoHFSignificance)
'''

### HEHB Noise filter rerunning
'''
process.load('CommonTools.RecoAlgos.HBHENoiseFilterResultProducer_cfi')
process.HBHENoiseFilterResultProducer.minZeros = cms.int32(99999)

process.applyBaselineHBHENoiseFilter = cms.EDFilter('BooleanFlagFilter',
   inputLabel = cms.InputTag('HBHENoiseFilterResultProducer','HBHENoiseFilterResult'),
   reverseDecision = cms.bool(False)
)

process.STEA_HBHENoiseFilter = cms.Path(
    process.HBHENoiseFilterResultProducer
    *process.applyBaselineHBHENoiseFilter
)
'''
### Primary vertex filter


process.goodVertexFilter = cms.EDFilter("GoodVertexFilter",
    vertexCollection = cms.InputTag('offlineSlimmedPrimaryVertices'),
    minimumNDOF = cms.uint32(4) ,
    maxAbsZ = cms.double(24),
    maxd0 = cms.double(2)
)
process.DX_primaryVertexFilter = cms.Path(process.goodVertexFilter)



### OUTPUT ###

#file service for EventWeight and PU info
if not options.isData:
    process.TFileService = cms.Service("TFileService", 
        fileName = cms.string("info.root")
    )

    process.eventAndPuInfo = cms.EDAnalyzer("EventInfoCollector",
        GenEventInfo=cms.InputTag("generator","","SIM"),
        PileupSummaryInfo=cms.InputTag("slimmedAddPileupInfo")
    )
    process.DX_plain+=process.eventAndPuInfo



filterPSet = cms.VPSet(
    cms.PSet(
        process=cms.string("DX"),
        paths=cms.vstring(
            "DX_filtered"
        ),
    )
)

process.pat2pxlio=cms.EDAnalyzer('EDM2PXLIO',
    outFileName=cms.string("output.pxlio"),
    processName=cms.string(options.processName),
    selectEvents=cms.VPSet(
        cms.PSet(
            process=cms.string("DX"),
            paths=cms.vstring(
                "DX_filtered"
            ),
        )
    ),
    primaryVertex = cms.InputTag("offlineSlimmedPrimaryVertices"),
    rho = cms.InputTag("fixedGridRhoFastjetAll"),
)

if not options.onlyFiltered:
    process.pat2pxlio.selectEvents[0].paths.append("DX_plain")


setattr(process.pat2pxlio,"muons",cms.PSet(
    type=cms.string("MuonConverter"),
    srcs=cms.VInputTag(cms.InputTag("slimmedMuons")),
    names=cms.vstring("Muon"),
    valueMaps = cms.PSet()
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
        
    )
))

if options.isData:
    setattr(process.pat2pxlio,"patJets",cms.PSet(
        type=cms.string("JetConverter"),
        srcs=cms.VInputTag(
            cms.InputTag("slimmedJetsJEC"),
        ),
        names=cms.vstring(
            "Jet",
        ),
        select=cms.string("pt>15.0"),
        valueMaps=cms.PSet(),
        triggerFilter=filterPSet,
    ))
    setattr(process.pat2pxlio,"patMET",cms.PSet(
        type=cms.string("METConverter"),
        srcs=cms.VInputTag(
            cms.InputTag("patPFMetT1"),
        ),
        names=cms.vstring(
            "MET",
        )
    ))
else:
    setattr(process.pat2pxlio,"patJets",cms.PSet(
        type=cms.string("JetConverter"),
        srcs=cms.VInputTag(
            cms.InputTag("slimmedJetsJEC"),
        ),
        names=cms.vstring(
            "Jet",
        ),
        select=cms.string("pt>15.0"),
        valueMaps=cms.PSet(),
        triggerFilter=filterPSet,
        basicVariablesOnly=cms.bool(True)
    ))
    setattr(process.pat2pxlio,"smearedPatJets",cms.PSet(
        type=cms.string("JetConverter"),
        srcs=cms.VInputTag(
            cms.InputTag("selectedPatJets")
        ),
        names=cms.vstring(
            "JetRes",
        ),
        select=cms.string("pt>15.0"),
        valueMaps=cms.PSet(),
        triggerFilter=filterPSet
    ))
    setattr(process.pat2pxlio,"patMET",cms.PSet(
        type=cms.string("METConverter"),
        srcs=cms.VInputTag(
            cms.InputTag("patPFMetT1"),
        ),
        names=cms.vstring(
            "METRes",
        )
    ))

    ### add sys variations
    setattr(process.pat2pxlio,"sysJets",cms.PSet(
        type=cms.string("JetConverter"),
        srcs=cms.VInputTag(
            cms.InputTag("shiftedPatJetEnDown"),
            cms.InputTag("shiftedPatJetEnUp"),
            cms.InputTag("shiftedPatJetResDown"),
            cms.InputTag("shiftedPatJetResUp"),
        ),
        names=cms.vstring(
            "JetEnDown",
            "JetEnUp",
            "JetResDown",
            "JetResUp"
        ),
        select=cms.string("pt>15.0"),
        valueMaps=cms.PSet(),
        triggerFilter=filterPSet
    ))
    
    setattr(process.pat2pxlio,"sysMETs",cms.PSet(
        type=cms.string("METConverter"),
        srcs=cms.VInputTag(
            #cms.InputTag("patPFMetT1ElectronEnDownnew"),
            #cms.InputTag("patPFMetT1ElectronEnUpnew"),
            cms.InputTag("patPFMetT1JetEnDown"),
            cms.InputTag("patPFMetT1JetEnUp"),
            cms.InputTag("patPFMetT1JetResDown"),
            cms.InputTag("patPFMetT1JetResUp"),
            #cms.InputTag("patPFMetT1MuonEnDownnew"),
            #cms.InputTag("patPFMetT1MuonEnUpnew")
        ),
        names=cms.vstring(
            #"METElectronEnDown",
            #"METElectronEnUp",
            "METJetEnDown",
            "METJetEnUp",
            "METJetResDown",
            "METJetResUp",
            #"METMuonEnDown",
            #"METMuonEnUp",
        )
    ))

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

setattr(process.pat2pxlio,"puInfo",cms.PSet(
    type=cms.string("PileupSummaryInfoConverter"),
    srcs=cms.VInputTag(cms.InputTag("slimmedAddPileupInfo")),
    names=cms.vstring("PU")
))





if options.isData:
    setattr(process.pat2pxlio,"triggersRECO",cms.PSet(
        type=cms.string("TriggerResultConverter"),
        srcs=cms.VInputTag(cms.InputTag("TriggerResults","","RECO")),
        regex=cms.vstring("[0-9a-zA-z_]*"),
        prefix=cms.vstring(["RECO_"])
    ))
    if options.isReRecoData:
        setattr(process.pat2pxlio,"triggersPAT",cms.PSet(
            type=cms.string("TriggerResultConverter"),
            srcs=cms.VInputTag(cms.InputTag("TriggerResults","","PAT")),
            regex=cms.vstring("[0-9a-zA-z_]*"),
            prefix=cms.vstring(["PAT_"])
        ))
    
else:
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
    ))
            
    setattr(process.pat2pxlio,"genjets",cms.PSet(
        type=cms.string("GenJetConverter"),
        srcs=cms.VInputTag(cms.InputTag("slimmedGenJets")),
        names=cms.vstring("GenJet"),
        targetEventViews=cms.vstring("GenJets"),
        triggerFilter=filterPSet,
    ))
    
process.endpath= cms.EndPath()

process.endpath+=process.pat2pxlio
'''
print "-------------------------------------"
print "WARNING: root output module in cfg!!!"
print "-------------------------------------"
process.OUT = cms.OutputModule("PoolOutputModule",
    fileName = cms.untracked.string('output.root'),
    outputCommands = cms.untracked.vstring( "drop *"),
    dropMetaData = cms.untracked.string('ALL'),
)
process.endpath+= process.OUT
'''
