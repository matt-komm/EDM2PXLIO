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

process = cms.Process("STEA")
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
    process.GlobalTag.globaltag = '74X_dataRun2_Prompt_v1'
    #process.GlobalTag.globaltag = '74X_dataRun2_Prompt_v1'
else:
    process.GlobalTag.globaltag = 'MCRUN2_74_V9A'


process.load("FWCore.MessageLogger.MessageLogger_cfi")
process.MessageLogger.cerr.FwkReport.reportEvery = 100
process.MessageLogger.suppressWarning = cms.untracked.vstring('ecalLaserCorrFilter','manystripclus53X','toomanystripclus53X')


'''
#process.load('JetMETCorrections.Configuration.DefaultJEC_cff')
from CondCore.DBCommon.CondDBSetup_cfi import *
process.conditionsFromDB = cms.ESSource("PoolDBESSource",CondDBSetup,
    connect = cms.string("frontier://FrontierProd/CMS_CONDITIONS"),
    toGet =  cms.VPSet(
        #JEC
        cms.PSet(
            record = cms.string("JetCorrectionsRecord"),
            tag = cms.string("JetCorrectorParametersCollection_CSA14_V4_MC_AK4PFchs"),
            label=cms.untracked.string("AK4PFchs")
        ),
        #BTAG SF
        #cms.PSet(
        #    record = cms.string("PerformancePayloadRecord"),
        #    tag = cms.string("BTagTTBARMCBTAGCSVtable_v8_offline"),
        #    label=cms.untracked.string("BtagScaleFactorsCSV")
        #),
        #BTAG WP
        #cms.PSet(
        #    record = cms.string("PerformanceWPRecord"),
        #    tag = cms.string("BTagTTBARMCBTAGCSVwp_v8_offline"),
        #    label=cms.untracked.string("BtagWorkingPointsCSV")
        #),
    )
)

process.jesUp = cms.EDProducer("JESUncertainty",
    jecES=cms.string("AK4PFchs"),
    jetSrc=cms.InputTag("slimmedJets"),
    metSrc=cms.InputTag("slimmedMETs"),
    delta=cms.double(1.0)
)
process.jesDown = cms.EDProducer("JESUncertainty",
    jecES=cms.string("AK4PFchs"),
    jetSrc=cms.InputTag("slimmedJets"),
    metSrc=cms.InputTag("slimmedMETs"),
    delta=cms.double(-1.0)
) 

process.btaggingSF = cms.EDProducer("BtagUncertainty",
    workingPointES=cms.string("BtagWorkingPointsCSV"),
    scaleFactorES=cms.string("BtagScaleFactorsCSV"),
    jetSrc=cms.InputTag("slimmedJets"),
)
'''


if options.isData:
    process.source = cms.Source("PoolSource",
        fileNames = cms.untracked.vstring(
            #'root://xrootd.unl.edu//store/data/Run2015B/SingleMuon/MINIAOD/PromptReco-v1/000/251/244/00000/68275270-7C27-E511-B1F0-02163E011A46.root' #{golden run: 251244:96-251244:121}        ),
            'root://xrootd.unl.edu//store/data/Run2015B/SingleMuon/MINIAOD/17Jul2015-v1/30000/16B50792-172E-E511-B0C8-0025905C43EC.root')
        #lumisToProcess = cms.untracked.VLuminosityBlockRange('251244:96-251244:121'),
    )
else:
    process.source = cms.Source("PoolSource",
        fileNames = cms.untracked.vstring(
            #'root://xrootd.unl.edu//store/mc/RunIISpring15DR74/TT_TuneCUETP8M1_13TeV-powheg-pythia8/MINIAODSIM/Asympt50ns_MCRUN2_74_V9A-v1/50000/02CF0510-4CFF-E411-A715-0025905A6090.root'
            #'root://xrootd.unl.edu//store/data/Run2015B/SingleMuon/MINIAOD/PromptReco-v1/000/251/244/00000/68275270-7C27-E511-B1F0-02163E011A46.root' #{golden run: 251244:96-251244:121}
            'root://xrootd.unl.edu//store/mc/RunIISpring15DR74/ST_t-channel_4f_leptonDecays_13TeV-amcatnlo-pythia8_TuneCUETP8M1/MINIAODSIM/Asympt50ns_MCRUN2_74_V9A-v1/60000/046CAA30-1103-E511-94E8-7845C4FC3B0C.root'
            #'root://xrootd.unl.edu//store/mc/RunIISpring15DR74/QCD_Pt-20toInf_MuEnrichedPt15_TuneCUETP8M1_13TeV_pythia8/MINIAODSIM/Asympt50ns_MCRUN2_74_V9A-v2/00000/023FCE0B-300A-E511-BE68-001E673973C8.root'
        ),
        #lumisToProcess = cms.untracked.VLuminosityBlockRange('251244:96-251244:121'),
    )
    
    
    
process.maxEvents = cms.untracked.PSet( input = cms.untracked.int32(100) )


process.STEA_plain=cms.Path()
process.STEA_filtered=cms.Path()


def addModule(m):
    process.STEA_plain+=m
    process.STEA_filtered+=m


### selectors ###
process.skimSequence = cms.Sequence()
process.STEA_filtered+=process.skimSequence

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
addFilter(cms.InputTag("slimmedJets"),"pt>10.0",minN=2)
addFilter(cms.InputTag("slimmedJets"),"pt>30.0",minN=1)


### gen particle pruner ###

process.load('SimGeneral.HepPDTESSource.pythiapdt_cfi')

if not options.isData:
    process.lessGenParticles = cms.EDProducer("GenParticlePruner",
        src=cms.InputTag("prunedGenParticles"),
        select=cms.vstring(
            "drop  *",
            #"keep++ abs(pdgId)=5", #keep all b decay chains
            #"keep++ abs(pdgId)=4", #keep all c decay chains
            #"drop abs(pdgId)>30",
            #"drop abs(pdgId)=310", #drop K short
            "++keep abs(status)>20 & abs(status) < 30", #keeps all particles from the hard matrix element and their mothers
            "keep++ abs(pdgId)=24", #keep W-boson decay
            "drop abs(status)>30", #drop all intermediate from decay
            #"drop abs(status)>30", #drop all intermediate from decay
            #"keep abs(pdgId)>510 & abs(pdgId)<558", #keep B/bb-mesons
            #"keep abs(pdgId)>410 & abs(pdgId)<446", #keep C/cc-mesons
            
        )
    )
    addModule(process.lessGenParticles)


### electron IDs ###

from PhysicsTools.SelectorUtils.tools.vid_id_tools import *
switchOnVIDElectronIdProducer(process, DataFormat.MiniAOD)
for eleID in [
    'RecoEgamma.ElectronIdentification.Identification.cutBasedElectronID_PHYS14_PU20bx25_V2_cff',
    'RecoEgamma.ElectronIdentification.Identification.cutBasedElectronID_Spring15_50ns_V1_cff',
    'RecoEgamma.ElectronIdentification.Identification.cutBasedElectronID_Spring15_25ns_V1_cff',
    #'RecoEgamma.ElectronIdentification.Identification.heepElectronID_HEEPV51_cff'
]:
    setupAllVIDIdsInModule(process,eleID,setupVIDElectronSelection)

addModule(process.egmGsfElectronIDSequence)



### JEC ###

from CondCore.DBCommon.CondDBSetup_cfi import *

if options.isData:
    era="Summer15_50nsV4_DATA"
else:
    era="Summer15_50nsV4_MC"
    
dBFile = era+".db"
process.jec = cms.ESSource("PoolDBESSource",CondDBSetup,
    connect = cms.string( "sqlite_file:"+dBFile ),
    toGet =  cms.VPSet(
        cms.PSet(
            record = cms.string("JetCorrectionsRecord"),
            tag = cms.string("JetCorrectorParametersCollection_"+era+"_AK4PF"),
            label= cms.untracked.string("AK4PF")
            ),
        cms.PSet(
            record = cms.string("JetCorrectionsRecord"),
            tag = cms.string("JetCorrectorParametersCollection_"+era+"_AK4PFchs"),
            label= cms.untracked.string("AK4PFchs")
            ),
    )    
)

process.es_prefer_jec = cms.ESPrefer("PoolDBESSource",'jec')


from PhysicsTools.PatAlgos.producersLayer1.jetUpdater_cff import patJetCorrFactorsUpdated
process.slimmedJetJECCorrFactors = patJetCorrFactorsUpdated.clone(
    src = cms.InputTag("slimmedJets"),
    levels = [
        'L1FastJet', 
        'L2Relative', 
        'L3Absolute'
    ],
    payload = 'AK4PFchs'
) 
from PhysicsTools.PatAlgos.producersLayer1.jetUpdater_cff import patJetsUpdated
process.slimmedJetsJEC = patJetsUpdated.clone(
    jetSource = cms.InputTag("slimmedJets"),
    jetCorrFactorsSource = cms.VInputTag(cms.InputTag("slimmedJetJECCorrFactors"))
)
process.reapplyJEC = cms.Sequence(process.slimmedJetJECCorrFactors*process.slimmedJetsJEC)
addModule(process.reapplyJEC)


### MET uncertainty ###

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
    jecUncFile="EDM2PXLIO/Core/data/Summer15_50nsV4_UncertaintySources_AK4PFchs.txt",
    #pfCandColl=cms.InputTag("slimmedJets"),
    postfix=""
)
if not options.isData:
    removeL2L3Residual(process,"")

'''
runMetCorAndUncFromMiniAOD(process,
    isData=options.isData,
    jecUncFile="EDM2PXLIO/Core/data/Summer15_50nsV4_UncertaintySources_AK4PFchs.txt",
    #pfCandColl=cms.InputTag("slimmedJets"),
    postfix="Smeared"
)
if not options.isData:
    removeL2L3Residual(process,"Smeared")
'''

#putting 'Smeared' into the collection name will run JER variations hopefully: need nominal smearing and JES uncertainty for 13TeV


#skip smearing - not sure this is correct and up-to-date
#process.shiftedPatJetResDown.src=process.patJets




### MET excluding HF ###

process.candidatesNoHF = cms.EDFilter("CandPtrSelector",
    src=cms.InputTag("packedPFCandidates"),
    cut=cms.string("abs(pdgId)!=1 && abs(pdgId)!=2 && abs(eta)<3.0")
)
runMetCorAndUncFromMiniAOD(process,
    isData=options.isData,
    pfCandColl=cms.InputTag("candidatesNoHF"),
    jecUncFile="EDM2PXLIO/Core/data/Summer15_50nsV4_UncertaintySources_AK4PFchs.txt",
    postfix="NoHF"
)
if not options.isData:
    removeL2L3Residual(process,"NoHF")
    
'''
runMetCorAndUncFromMiniAOD(process,
    isData=options.isData,
    pfCandColl=cms.InputTag("noHFCandidates"),
    jecUncFile="EDM2PXLIO/Core/data/Summer15_50nsV4_UncertaintySources_AK4PFchs.txt",
    postfix="NoHFSmeared"
)
if not options.isData:
    removeL2L3Residual(process,"NoHFSmeared")
'''

### MET Significance ###

from RecoMET.METProducers.METSignificanceParams_cfi import METSignificanceParams

process.slimmedMETSignificance = cms.EDProducer(
    "METSignificanceProducer",
    srcLeptons = cms.VInputTag(
        'slimmedElectrons',
        'slimmedMuons',
        'slimmedPhotons'
    ),
    srcPfJets = cms.InputTag('slimmedJetsJEC'),
    srcMet = cms.InputTag('slimmedMETs','','STEA'),
    srcPFCandidates = cms.InputTag('packedPFCandidates'),

    parameters = METSignificanceParams
)
addModule(process.slimmedMETSignificance)

process.slimmedMETSignificanceNoHF = cms.EDProducer(
    "METSignificanceProducer",
    srcLeptons = cms.VInputTag(
        'slimmedElectrons',
        'slimmedMuons',
        'slimmedPhotons'
    ),
    srcPfJets = cms.InputTag('patJetsNoHF'),
    srcMet = cms.InputTag('slimmedMETsNoHF'),
    srcPFCandidates = cms.InputTag('candidatesNoHF'),

    parameters = METSignificanceParams
)
addModule(process.slimmedMETSignificance)



### HEHB Noise filter rerunning

process.load('CommonTools.RecoAlgos.HBHENoiseFilterResultProducer_cfi')
process.HBHENoiseFilterResultProducer.minZeros = cms.int32(99999)

process.applyBaselineHBHENoiseFilter = cms.EDFilter('BooleanFlagFilter',
   inputLabel = cms.InputTag('HBHENoiseFilterResultProducer','HBHENoiseFilterResult'),
   reverseDecision = cms.bool(False)
)

process.HBHENoiseFilter = cms.Path(
    process.HBHENoiseFilterResultProducer
    *process.applyBaselineHBHENoiseFilter
)

### Primary vertex filter


process.goodVertexFilter = cms.EDFilter("GoodVertexFilter",
    vertexCollection = cms.InputTag('offlineSlimmedPrimaryVertices'),
    minimumNDOF = cms.uint32(4) ,
    maxAbsZ = cms.double(24),
    maxd0 = cms.double(2)
)
process.primaryVertexFilter = cms.Path(process.goodVertexFilter)



### OUTPUT ###


filterPSet = cms.VPSet(
    cms.PSet(
        process=cms.string("STEA"),
        paths=cms.vstring(
            "STEA_filtered"
        ),
    )
)

process.pat2pxlio=cms.EDAnalyzer('EDM2PXLIO',
        outFileName=cms.string("output.pxlio"),
        processName=cms.string(options.processName),
        selectEvents=cms.VPSet(
            cms.PSet(
                process=cms.string("STEA"),
                paths=cms.vstring(
                    "STEA_filtered"
                ),
            )
        ),
        primaryVertex = cms.InputTag("offlineSlimmedPrimaryVertices")
)

if not options.onlyFiltered:
    process.pat2pxlio.selectEvents[0].paths.append("STEA_plain")


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
        phys14eleIDVeto = cms.PSet(
            type=cms.string("ValueMapAccessorBool"),
            src=cms.InputTag("egmGsfElectronIDs","cutBasedElectronID-PHYS14-PU20bx25-V2-standalone-veto")
        ),
        phys14eleIDTight = cms.PSet(
            type=cms.string("ValueMapAccessorBool"),
            src=cms.InputTag("egmGsfElectronIDs","cutBasedElectronID-PHYS14-PU20bx25-V2-standalone-tight")
        ),
        sprint15eleIDVeto50ns = cms.PSet(
            type=cms.string("ValueMapAccessorBool"),
            src=cms.InputTag("egmGsfElectronIDs","cutBasedElectronID-Spring15-50ns-V1-standalone-veto")
        ),
        sprint15eleIDTight50ns = cms.PSet(
            type=cms.string("ValueMapAccessorBool"),
            src=cms.InputTag("egmGsfElectronIDs","cutBasedElectronID-Spring15-50ns-V1-standalone-tight")
        ),
        sprint15eleIDVeto25ns = cms.PSet(
            type=cms.string("ValueMapAccessorBool"),
            src=cms.InputTag("egmGsfElectronIDs","cutBasedElectronID-Spring15-25ns-V1-standalone-veto")
        ),
        sprint15eleIDTight25ns = cms.PSet(
            type=cms.string("ValueMapAccessorBool"),
            src=cms.InputTag("egmGsfElectronIDs","cutBasedElectronID-Spring15-25ns-V1-standalone-tight")
        ),
        
    )
))
'''                                     
setattr(process.pat2pxlio,"jets",cms.PSet(
    type=cms.string("JetConverter"),
    srcs=cms.VInputTag(cms.InputTag("slimmedJets")),
    names=cms.vstring("Jet"),
    select=cms.string("pt>20.0"),
    valueMaps=cms.PSet(),
    triggerFilter=filterPSet
))
'''

setattr(process.pat2pxlio,"slimmedJetsJEC",cms.PSet(
    type=cms.string("JetConverter"),
    srcs=cms.VInputTag(cms.InputTag("slimmedJetsJEC")),
    names=cms.vstring("Jet"),
    select=cms.string("pt>10.0"),
    valueMaps=cms.PSet(),
    triggerFilter=filterPSet
))


setattr(process.pat2pxlio,"remets",cms.PSet(
    type=cms.string("METConverter"),
    srcs=cms.VInputTag(
        cms.InputTag("slimmedMETs","","STEA"),
        cms.InputTag("slimmedMETsNoHF","","STEA")
    ),
    names=cms.vstring(
        "MET",
        "METnoHF"
    ),
    metSignificances=cms.VInputTag(
        cms.InputTag("slimmedMETSignificance","METSignificance"),
        cms.InputTag("slimmedMETSignificanceNoHF","METSignificance"), 
    ),
    
    addSysVariations=cms.bool(True)
    
))


setattr(process.pat2pxlio,"jetendown",cms.PSet(
    type=cms.string("JetConverter"),
    srcs=cms.VInputTag(cms.InputTag("shiftedPatJetEnDown")),
    names=cms.vstring("Jet"),
    select=cms.string("pt>10.0"),
    valueMaps=cms.PSet(),
    triggerFilter=filterPSet,
    basicVariablesOnly=cms.bool(True),
    targetEventViews=cms.vstring("JetEnDown"),
))

setattr(process.pat2pxlio,"jetenup",cms.PSet(
    type=cms.string("JetConverter"),
    srcs=cms.VInputTag(cms.InputTag("shiftedPatJetEnUp")),
    names=cms.vstring("Jet"),
    select=cms.string("pt>10.0"),
    valueMaps=cms.PSet(),
    triggerFilter=filterPSet,
    basicVariablesOnly=cms.bool(True),
    targetEventViews=cms.vstring("JetEnUp"),
))


'''
setattr(process.pat2pxlio,"jetssmeared",cms.PSet(
    type=cms.string("JetConverter"),
    srcs=cms.VInputTag(cms.InputTag("patJetsSmeared")),
    names=cms.vstring("Jet"),
    select=cms.string("pt>10.0"),
    valueMaps=cms.PSet(),
    triggerFilter=filterPSet,
    basicVariablesOnly=cms.bool(True),
    targetEventViews=cms.vstring("Smeared"),
))

setattr(process.pat2pxlio,"remetssmeared",cms.PSet(
    type=cms.string("METConverter"),
    srcs=cms.VInputTag(
        cms.InputTag("slimmedMETsSmeared","","STEA"),
        cms.InputTag("slimmedMETsNoHFSmeared","","STEA")
    ),
    names=cms.vstring(
        "MET",
        "METnoHF"
    ),
    addSysVariations=cms.bool(True),
    targetEventViews=cms.vstring("Smeared"),
))
'''

setattr(process.pat2pxlio,"triggersHLT",cms.PSet(
    type=cms.string("TriggerResultConverter"),
    srcs=cms.VInputTag(cms.InputTag("TriggerResults","","HLT")),
    regex=cms.vstring("HLT_Iso[0-9a-zA-z_]*","HLT_Ele[0-9a-zA-z_]*")
))

setattr(process.pat2pxlio,"triggersSTEA",cms.PSet(
    type=cms.string("TriggerResultConverter"),
    srcs=cms.VInputTag(cms.InputTag("TriggerResults","","STEA")),
    regex=cms.vstring("[0-9a-zA-z_]*")
))

setattr(process.pat2pxlio,"puInfo",cms.PSet(
    type=cms.string("PileupSummaryInfoConverter"),
    srcs=cms.VInputTag(cms.InputTag("addPileupInfo","","HLT")),
    names=cms.vstring("PU")
))


if options.isData:
    setattr(process.pat2pxlio,"triggersRECO",cms.PSet(
        type=cms.string("TriggerResultConverter"),
        srcs=cms.VInputTag(cms.InputTag("TriggerResults","","RECO")),
        regex=cms.vstring("[0-9a-zA-z_]*")
    ))
    '''
    setattr(process.pat2pxlio,"mets",cms.PSet(
        type=cms.string("METConverter"),
        srcs=cms.VInputTag(cms.InputTag("slimmedMETs","","RECO")),
        names=cms.vstring("MET"),
        metSignificances=cms.VInputTag(cms.InputTag("slimmedMETSignificance","METSignificance"))
    ))
    '''
    
else:
    setattr(process.pat2pxlio,"triggersPAT",cms.PSet(
        type=cms.string("TriggerResultConverter"),
        srcs=cms.VInputTag(cms.InputTag("TriggerResults","","PAT")),
        regex=cms.vstring("[0-9a-zA-z_]*")
    ))
    '''
    setattr(process.pat2pxlio,"mets",cms.PSet(
        type=cms.string("METConverter"),
        srcs=cms.VInputTag(cms.InputTag("slimmedMETs","","PAT")),
        names=cms.vstring("MET"),
        metSignificances=cms.VInputTag(cms.InputTag("slimmedMETSignificance","METSignificance"))
    ))
    '''
    setattr(process.pat2pxlio,"genParticles",cms.PSet(
        type=cms.string("GenParticleConverter"),
        srcs=cms.VInputTag(cms.InputTag("lessGenParticles")),
        targetEventViews=cms.vstring("Generated"),
        LHEEvent=cms.InputTag("externalLHEProducer"),
        GenEventInfo=cms.InputTag("generator","","SIM")
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

process.OUT = cms.OutputModule("PoolOutputModule",
    fileName = cms.untracked.string('output.root'),
    outputCommands = cms.untracked.vstring( "keep *_slimmedMETs_*_*",
                                            "keep *_slimmedMETsNoHF_*_*",
                                            #"keep *_patPFMetT1Txy_*_*",
                                            #"keep *_patPFMetT1TxyNoHF_*_*",
                                            #"keep *_patJetCorrFactors_*_*",
                                            #"keep *_patPFMetT1JetResDown_*_*"
                                            "keep *_slimmedMETSignificance_*_*"
                                            ),
    dropMetaData = cms.untracked.string('ALL'),
)
process.endpath+= process.OUT



