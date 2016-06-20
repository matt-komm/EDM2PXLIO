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

options.register(
    'addSys',
    True,
    VarParsing.multiplicity.singleton,
    VarParsing.varType.bool,
    "adds JEC/JER uncertainties"
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
#process.options.allowUnscheduled = cms.untracked.bool(True)
#process.options.numberOfThreads = cms.untracked.uint32(4)
#process.options.numberOfStreams = cms.untracked.uint32(4)

process.load('Configuration.StandardSequences.Services_cff')
process.load('Configuration.EventContent.EventContent_cff')
process.load('Configuration.StandardSequences.GeometryDB_cff')
process.load('Configuration.StandardSequences.MagneticField_38T_cff')
process.load('Configuration.StandardSequences.FrontierConditions_GlobalTag_condDBv2_cff')
#process.load('Configuration.StandardSequences.FrontierConditions_GlobalTag_cff')


if options.isData:
    process.GlobalTag.globaltag = '80X_dataRun2_Prompt_v8'
    
    ### frontier database ###
    from CondCore.DBCommon.CondDBSetup_cfi import *
    process.frontierDB = cms.ESSource("PoolDBESSource",
        CondDBSetup,
        toGet = cms.VPSet(
            cms.PSet(
                record = cms.string('JetCorrectionsRecord'),
                tag    = cms.string('JetCorrectorParametersCollection_Fall15_V2_DATA_AK4PFchs'), #should correspond to V6 (V3 is here an internal name inside the GT)
                label  = cms.untracked.string('AK4PFchs')
            ),
        ),
        connect = cms.string('frontier://FrontierProd/CMS_CONDITIONS'),
    )
else:
    process.GlobalTag.globaltag = '80X_mcRun2_asymptotic_2016_miniAODv2'
    
    ### frontier database ###
    from CondCore.DBCommon.CondDBSetup_cfi import *
    process.frontierDB = cms.ESSource("PoolDBESSource",
        CondDBSetup,
        toGet = cms.VPSet(
            cms.PSet(
                record = cms.string('JetCorrectionsRecord'),
                tag    = cms.string('JetCorrectorParametersCollection_Fall15_25nsV2_MC_AK4PFchs'),
                label  = cms.untracked.string('AK4PFchs')
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
            ' /store/data/Run2015D/SingleMuon/MINIAOD/16Dec2015-v1/10001/E0B88398-4BA9-E511-8FE9-0CC47A78A4A6.root'
        ),
        lumisToProcess = cms.untracked.VLuminosityBlockRange('259811:70-259811:80'), #"259811": [[1, 47], [50, 91]]
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
            'root://cms-xrd-global.cern.ch//store/mc/RunIISpring16MiniAODv2/TT_TuneCUETP8M1_13TeV-powheg-pythia8/MINIAODSIM/PUSpring16_80X_mcRun2_asymptotic_2016_miniAODv2_v0_ext4-v1/00000/004A0552-3929-E611-BD44-0025905A48F0.root'
        ),
        #lumisToProcess = cms.untracked.VLuminosityBlockRange('251244:96-251244:121'),
    )
    
    
if options.isData:
    process.maxEvents = cms.untracked.PSet( input = cms.untracked.int32(400) )
else:
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
addFilter(cms.InputTag("slimmedJets"),"pt>30.0",minN=1)



### gen particle pruner ###

process.load('SimGeneral.HepPDTESSource.pythiapdt_cfi')

if not options.isData:
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
            "keep isLastCopy & abs(pdgId)<7",
            "keep isLastCopyBeforeFSR & abs(pdgId)<7",
            "keep fromHardProcessBeforeFSR & abs(pdgId)<7",
            #"keep isFirstCopy"
            
            #keep prompt leptons
            "keep (isPromptFinalState) & abs(pdgId)>10 & abs(pdgId)<19"
        )
    )
    addModule(process.lessGenParticles)


### electron IDs ###

from PhysicsTools.SelectorUtils.tools.vid_id_tools import *
switchOnVIDElectronIdProducer(process, DataFormat.MiniAOD)
for eleID in [
    'RecoEgamma.ElectronIdentification.Identification.cutBasedElectronID_Spring15_25ns_V1_cff',
]:
    setupAllVIDIdsInModule(process,eleID,setupVIDElectronSelection)

addModule(process.egmGsfElectronIDSequence)



### JEC ###
''''
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
'''

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
    selectEvents=cms.VPSet(filterPSet),
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

#add jets/met
if options.isData:
    setattr(process.pat2pxlio,"patJets",cms.PSet(
        type=cms.string("JetConverter"),
        srcs=cms.VInputTag(
            cms.InputTag("slimmedJets"),
        ),
        names=cms.vstring(
            "Jet",
        ),
        select=cms.string("pt>15.0"),
        valueMaps=cms.PSet(
        ),
        triggerFilter=filterPSet,
    ))
    setattr(process.pat2pxlio,"patMET",cms.PSet(
        type=cms.string("METConverter"),
        srcs=cms.VInputTag(
            cms.InputTag("slimmedMet"),
        ),
        names=cms.vstring(
            "MET",
        )
    ))
else:
    setattr(process.pat2pxlio,"patJets",cms.PSet(
        type=cms.string("JetConverter"),
        srcs=cms.VInputTag(
            cms.InputTag("slimmedJets"),
        ),
        names=cms.vstring(
            "Jet",
        ),
        select=cms.string("pt>15.0"),
        valueMaps=cms.PSet(
        ),
        triggerFilter=filterPSet,
    ))
    setattr(process.pat2pxlio,"patMET",cms.PSet(
        type=cms.string("METConverter"),
        srcs=cms.VInputTag(
            cms.InputTag("slimmedMETs"),
        ),
        names=cms.vstring(
            "MET",
        )
    ))

    ### add sys variations
    if options.addSys:
        pass


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


#add extra trigger info
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


#add gen particles/jets
if not options.isData:
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
    outputCommands = cms.untracked.vstring( "keep *","drop *_*_*_DX", "keep *_QGL*_*_*"),
    dropMetaData = cms.untracked.string('ALL'),
)
process.endpath+= process.OUT
'''
