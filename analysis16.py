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
    process.GlobalTag.globaltag = '80X_dataRun2_Prompt_v14'
    
    ### frontier database ###
    process.dbJEC = cms.ESSource("PoolDBESSource",
        #CondDBSetup,
        toGet = cms.VPSet(
            cms.PSet(
                record = cms.string('JetCorrectionsRecord'),
                tag    = cms.string('JetCorrectorParametersCollection_Summer16_23Sep2016AllV3_DATA_AK4PFchs'),
                #tag    = cms.string('JetCorrectorParametersCollection_Spring16_23Sep2016AllV2_DATA_AK4PFchs'),
                label  = cms.untracked.string('AK4PFchs')
            ),
        ),
        #connect = cms.string('frontier://FrontierProd/CMS_CONDITIONS'),
        connect = cms.string('sqlite:Summer16_23Sep2016AllV3_DATA.db'),
        #connect = cms.string('sqlite:Spring16_23Sep2016AllV2_DATA.db')
    )
elif options.isData and options.isReRecoData:
    process.GlobalTag.globaltag = '80X_dataRun2_2016SeptRepro_v4'
    
    ### frontier database ###
    process.dbJEC = cms.ESSource("PoolDBESSource",
        #CondDBSetup,
        toGet = cms.VPSet(
            cms.PSet(
                record = cms.string('JetCorrectionsRecord'),
                tag    = cms.string('JetCorrectorParametersCollection_Summer16_23Sep2016AllV3_DATA_AK4PFchs'),
                #tag    = cms.string('JetCorrectorParametersCollection_Spring16_23Sep2016AllV2_DATA_AK4PFchs'),
                label  = cms.untracked.string('AK4PFchs')
            ),
        ),
        #connect = cms.string('frontier://FrontierProd/CMS_CONDITIONS'),
        connect = cms.string('sqlite:Summer16_23Sep2016AllV3_DATA.db'),
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
                tag    = cms.string('JetCorrectorParametersCollection_Summer16_23Sep2016V3_MC_AK4PFchs'),
                #tag    = cms.string('JetCorrectorParametersCollection_Spring16_23Sep2016V2_MC_AK4PFchs'),
                label  = cms.untracked.string('AK4PFchs')
            ),
        ),
        #connect = cms.string('frontier://FrontierProd/CMS_CONDITIONS'),
        connect = cms.string('sqlite:Summer16_23Sep2016V3_MC.db')
        #connect = cms.string('sqlite:Spring16_23Sep2016V2_MC.db')
    )
    
    process.dbJES = cms.ESSource("PoolDBESSource",
        #CondDBSetup,
        toGet = cms.VPSet(
            cms.PSet(
                record = cms.string('JetResolutionRcd'),
                tag    = cms.string('JR_Spring16_25nsV6_MC_PtResolution_AK4PFchs'),
                label  = cms.untracked.string('AK4PFchs_pt')
            ),
            cms.PSet(
                record = cms.string('JetResolutionScaleFactorRcd'),
                tag    = cms.string('JR_Spring16_25nsV6_MC_SF_AK4PFchs'),
                label  = cms.untracked.string('AK4PFchs')
            ),
        ),
        connect = cms.string('frontier://FrontierProd/CMS_CONDITIONS'),
        #connect = cms.string('sqlite:Spring16_25nsV10_MC.db')
    )


process.es_prefer_jec = cms.ESPrefer("PoolDBESSource",'dbJEC')

if not options.isData:
    process.es_prefer_jer = cms.ESPrefer('PoolDBESSource', 'dbJES')



process.load("FWCore.MessageLogger.MessageLogger_cfi")
process.MessageLogger.cerr.FwkReport.reportEvery = 100
process.MessageLogger.suppressWarning = cms.untracked.vstring('ecalLaserCorrFilter','manystripclus53X','toomanystripclus53X')


if options.isData and not options.isReRecoData:
    process.source = cms.Source("PoolSource",
        fileNames = cms.untracked.vstring(
            'root://cmsxrootd.fnal.gov//store/data/Run2016H/SingleMuon/MINIAOD/PromptReco-v2/000/281/382/00000/8A8C3000-F882-E611-80C6-02163E014266.root',
        ),
        #lumisToProcess = cms.untracked.VLuminosityBlockRange('273158:3-273158:13'),
        
    )
elif options.isData and options.isReRecoData:
    process.source = cms.Source("PoolSource",
        fileNames = cms.untracked.vstring(
            #'root://cmsxrootd.fnal.gov//store/data/Run2016G/SingleMuon/MINIAOD/23Sep2016-v1/1110000/A2C0F697-B19C-E611-A4D8-F04DA275BFF2.root'
            
            #sync
            #'/store/data/Run2016H/SingleMuon/MINIAOD/PromptReco-v3/000/284/040/00000/0848D330-889F-E611-8935-02163E01444D.root'
            '/store/data/Run2016B/SingleMuon/MINIAOD/23Sep2016-v3/00000/A2EB30F3-4499-E611-91D6-A0369F7F92E8.root ',
        )
        #lumisToProcess = cms.untracked.VLuminosityBlockRange('251244:96-251244:121'),
        
    )
else:
    process.source = cms.Source("PoolSource",
        inputCommands = cms.untracked.vstring("keep *", "drop LHERunInfoProduct_*_*_*"), #this produces otherwise a memleak
        fileNames = cms.untracked.vstring(
            #'root://cmsxrootd.fnal.gov//store/mc/RunIISpring16MiniAODv2/ST_t-channel_top_4f_inclusiveDecays_13TeV-powhegV2-madspin-pythia8_TuneCUETP8M1/MINIAODSIM/PUSpring16_80X_mcRun2_asymptotic_2016_miniAODv2_v0-v1/00000/04615FE5-D844-E611-B5F0-0090FAA58B94.root'
            #'root://cmsxrootd.fnal.gov//store/mc/RunIISpring16MiniAODv2/TT_TuneCUETP8M1_13TeV-powheg-pythia8/MINIAODSIM/PUSpring16_80X_mcRun2_asymptotic_2016_miniAODv2_v0_ext4-v1/00000/004A0552-3929-E611-BD44-0025905A48F0.root'
            #'root://cmsxrootd.fnal.gov//store/mc/RunIISpring16MiniAODv2/ST_t-channel_4f_leptonDecays_13TeV-amcatnlo-pythia8_TuneCUETP8M1/MINIAODSIM/premix_withHLT_80X_mcRun2_asymptotic_v14_ext1-v1/80000/02236588-E871-E611-BDA6-D8D385AE85C0.root'
            #'root://cmsxrootd.fnal.gov//store/mc/RunIISpring16MiniAODv2/DYJetsToLL_M-50_TuneCUETP8M1_13TeV-madgraphMLM-pythia8/MINIAODSIM/PUSpring16RAWAODSIM_reHLT_80X_mcRun2_asymptotic_v14_ext1-v1/20000/00071E92-6F55-E611-B68C-0025905A6066.root'
            #'root://cmsxrootd.hep.wisc.edu//store/mc/RunIISummer16MiniAODv2/ST_tW_antitop_5f_inclusiveDecays_13TeV-powheg-pythia8_TuneCUETP8M1/MINIAODSIM/PUMoriond17_80X_mcRun2_asymptotic_2016_TrancheIV_v6_ext1-v1/120000/08CB2C65-82BC-E611-8BCE-5065F3810301.root'
            #'root://cmsxrootd.fnal.gov//store/mc/RunIISummer16MiniAODv2/ST_t-channel_antitop_4f_inclusiveDecays_13TeV-powhegV2-madspin-pythia8_TuneCUETP8M1/MINIAODSIM/PUMoriond17_80X_mcRun2_asymptotic_2016_TrancheIV_v6-v1/120000/00688753-BCBD-E611-8B2F-001E67E71DDA.root',
            #'root://sbgse1.in2p3.fr//store/mc/RunIISummer16MiniAODv2/QCD_Pt-1000toInf_MuEnrichedPt5_TuneCUETP8M1_13TeV_pythia8/MINIAODSIM/PUMoriond17_80X_mcRun2_asymptotic_2016_TrancheIV_v6-v1/80000/2ADA07B4-D2B1-E611-A716-0CC47A7C351E.root',
	
	        #sync
	        '/store/mc/RunIISummer16MiniAODv2/ST_t-channel_top_4f_inclusiveDecays_13TeV-powhegV2-madspin-pythia8_TuneCUETP8M1/MINIAODSIM/PUMoriond17_80X_mcRun2_asymptotic_2016_TrancheIV_v6-v1/60000/A059F3BF-79B9-E611-B81C-0CC47A7FC412.root'
	),
        #skipEvents = cms.untracked.uint32(24900)
        #lumisToProcess = cms.untracked.VLuminosityBlockRange('251244:96-251244:121'),
    )
    
    
if options.isData:
    process.maxEvents = cms.untracked.PSet( input = cms.untracked.int32(10) )
else:
    process.maxEvents = cms.untracked.PSet( input = cms.untracked.int32(10) )
    
### bad muon filter ###
process.load("RecoMET.METFilters.badGlobalMuonTaggersMiniAOD_cff")
process.DX_hasBadCloneMuons = cms.Path(process.cloneGlobalMuonTagger)
process.DX_hasBadMuons = cms.Path(process.badGlobalMuonTagger)

process.DX_plain=cms.Path()
process.DX_filtered_mu=cms.Path()
process.DX_filtered_ele=cms.Path()

def addModule(m):
    process.DX_plain+=m
    process.DX_filtered_mu+=m
    process.DX_filtered_ele+=m


### selectors ###
skimMuSequence = cms.Sequence()
process.DX_filtered_mu+=skimMuSequence
skimEleSequence = cms.Sequence()
process.DX_filtered_ele+=skimEleSequence

def addFilter(seq,inputTag,cutString,minN=None):
    name = str(inputTag._InputTag__moduleLabel)
    
    if minN!=None:
        name+="min"+str(minN)
        
    selector = cms.EDFilter("CandViewSelector",
        src = inputTag,
        cut = cms.string(cutString)
    )
    selectorName="select"+name
    setattr(process,selectorName,selector)
    seq+=selector
  
    if minN!=None:
        selectorMinFilter = cms.EDFilter("CandViewCountFilter",
            src = cms.InputTag(selectorName),
            minNumber = cms.uint32(minN)
        )
        selectorMinFilterName="minFilter"+name
        setattr(process,selectorMinFilterName,selectorMinFilter)
        seq+=selectorMinFilter
    
if not options.noFilter:
    addFilter(skimMuSequence,cms.InputTag("slimmedMuons"),"pt>20.0",minN=1)
    #addFilter(skimMuSequence,cms.InputTag("slimmedJetsJEC"),"pt>15.0",minN=2)
    addFilter(skimMuSequence,cms.InputTag("slimmedJetsJEC"),"pt>20.0",minN=1)
    
    addFilter(skimEleSequence,cms.InputTag("slimmedElectrons"),"pt>20.0",minN=1)
    #addFilter(skimEleSequence,cms.InputTag("slimmedJetsJEC"),"pt>15.0",minN=2)
    addFilter(skimEleSequence,cms.InputTag("slimmedJetsJEC"),"pt>20.0",minN=1)
else:
    print "will not apply any filtering!"


### gen particle pruner ###

process.load('SimGeneral.HepPDTESSource.pythiapdt_cfi')

if not options.isData:
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


### electron IDs ###

from PhysicsTools.SelectorUtils.tools.vid_id_tools import *
switchOnVIDElectronIdProducer(process, DataFormat.MiniAOD)
for eleID in [
    'RecoEgamma.ElectronIdentification.Identification.cutBasedElectronID_Summer16_80X_V1_cff',
    'RecoEgamma.ElectronIdentification.Identification.cutBasedElectronHLTPreselecition_Summer16_V1_cff'
]:
    setupAllVIDIdsInModule(process,eleID,setupVIDElectronSelection)

addModule(process.egmGsfElectronIDSequence)


### Jet PU ID ###
'''
from RecoJets.JetProducers.PileupJetID_cfi import pileupJetId
process.pileupJetIdUpdated = pileupJetId.clone(
    jets=cms.InputTag("slimmedJets"),
    inputIsCorrected=True,
    applyJec=True,
    vertexes=cms.InputTag("offlineSlimmedPrimaryVertices")
)
'''
### JEC ###

from PhysicsTools.PatAlgos.producersLayer1.jetUpdater_cff import updatedPatJetCorrFactors
process.newJEC = updatedPatJetCorrFactors.clone(
    src = cms.InputTag("slimmedJets"),
    levels = [
        'L1FastJet', 
        'L2Relative', 
        'L3Absolute'
    ],
    payload = 'AK4PFchs',
)

if options.isData:
    process.newJEC.levels.append('L2L3Residual')
    
from PhysicsTools.PatAlgos.producersLayer1.jetUpdater_cff import updatedPatJets
process.slimmedJetsJEC = updatedPatJets.clone(
    jetSource = cms.InputTag("slimmedJets"),
    jetCorrFactorsSource = cms.VInputTag(
        cms.InputTag("newJEC")
    )
)
#process.slimmedJetsJEC.userData.userFloats.src += ['pileupJetIdUpdated:fullDiscriminant']

process.reappliedJEC = cms.Sequence(process.newJEC*process.slimmedJetsJEC)

### JEC uncertainties

if not options.isData:
    process.slimmedJetsJECEnUp = cms.EDProducer("ShiftedPATJetProducer",
        addResidualJES = cms.bool(True),
        jetCorrLabelUpToL3 = cms.InputTag("ak4PFCHSL1FastL2L3Corrector"),
        jetCorrLabelUpToL3Res = cms.InputTag("ak4PFCHSL1FastL2L3ResidualCorrector"),
        jetCorrPayloadName = cms.string('AK4PFchs'),
        jetCorrUncertaintyTag = cms.string('Uncertainty'),
        shiftBy = cms.double(1.0),
        src = cms.InputTag("slimmedJetsJEC")
    )
    process.reappliedJEC+=process.slimmedJetsJECEnUp
    
    process.slimmedJetsJECEnDown = cms.EDProducer("ShiftedPATJetProducer",
        addResidualJES = cms.bool(True),
        jetCorrLabelUpToL3 = cms.InputTag("ak4PFCHSL1FastL2L3Corrector"),
        jetCorrLabelUpToL3Res = cms.InputTag("ak4PFCHSL1FastL2L3ResidualCorrector"),
        jetCorrPayloadName = cms.string('AK4PFchs'),
        jetCorrUncertaintyTag = cms.string('Uncertainty'),
        shiftBy = cms.double(-1.0),
        src = cms.InputTag("slimmedJetsJEC")
    )
    process.reappliedJEC+=process.slimmedJetsJECEnDown
    
addModule(process.reappliedJEC)


### JES smeared jets ###

process.slimmedJetsJECSmeared = cms.EDProducer('SmearedPATJetProducer',
    src = cms.InputTag('slimmedJetsJEC'),
    enabled = cms.bool(True),
    rho = cms.InputTag("fixedGridRhoFastjetAll"),
    algo = cms.string('AK4PFchs'),
    algopt = cms.string('AK4PFchs_pt'),

    genJets = cms.InputTag('slimmedGenJets'),
    dRMax = cms.double(0.2),
    dPtMaxFactor = cms.double(3),

    debug = cms.untracked.bool(False),
    
    seed = cms.uint32(123),
    
    variation = cms.int32(0) # 0: nominal, 1: up, -1: down
)
if not options.isData:
    process.slimmedJetsJECSmearedEnUp = cms.EDProducer('SmearedPATJetProducer',
        src = cms.InputTag('slimmedJetsJECEnUp'),
        enabled = cms.bool(True),
        rho = cms.InputTag("fixedGridRhoFastjetAll"),
        algo = cms.string('AK4PFchs'),
        algopt = cms.string('AK4PFchs_pt'),

        genJets = cms.InputTag('slimmedGenJets'),
        dRMax = cms.double(0.2),
        dPtMaxFactor = cms.double(3),

        debug = cms.untracked.bool(False),
        
        seed = cms.uint32(123),
        
        variation = cms.int32(0) # 0: nominal, 1: up, -1: down
    )
    process.slimmedJetsJECSmearedEnDown = cms.EDProducer('SmearedPATJetProducer',
        src = cms.InputTag('slimmedJetsJECEnDown'),
        enabled = cms.bool(True),
        rho = cms.InputTag("fixedGridRhoFastjetAll"),
        algo = cms.string('AK4PFchs'),
        algopt = cms.string('AK4PFchs_pt'),

        genJets = cms.InputTag('slimmedGenJets'),
        dRMax = cms.double(0.2),
        dPtMaxFactor = cms.double(3),

        debug = cms.untracked.bool(False),
        
        seed = cms.uint32(123),
        
        variation = cms.int32(0) # 0: nominal, 1: up, -1: down
    )

### JES uncertainties ###

if not options.isData:

    process.slimmedJetsJECSmearedResUp = cms.EDProducer('SmearedPATJetProducer',
        src = cms.InputTag('slimmedJetsJEC'),
        enabled = cms.bool(True),
        rho = cms.InputTag("fixedGridRhoFastjetAll"),
        algo = cms.string('AK4PFchs'),
        algopt = cms.string('AK4PFchs_pt'),

        genJets = cms.InputTag('slimmedGenJets'),
        dRMax = cms.double(0.2),
        dPtMaxFactor = cms.double(3),

        debug = cms.untracked.bool(False),
        
        seed = cms.uint32(123),
        
        variation = cms.int32(1) # 0: nominal, 1: up, -1: down
    )
    process.slimmedJetsJECSmearedResDown = cms.EDProducer('SmearedPATJetProducer',
        src = cms.InputTag('slimmedJetsJEC'),
        enabled = cms.bool(True),
        rho = cms.InputTag("fixedGridRhoFastjetAll"),
        algo = cms.string('AK4PFchs'),
        algopt = cms.string('AK4PFchs_pt'),

        genJets = cms.InputTag('slimmedGenJets'),
        dRMax = cms.double(0.2),
        dPtMaxFactor = cms.double(3),

        debug = cms.untracked.bool(False),
        
        seed = cms.uint32(123),
        
        variation = cms.int32(-1) # 0: nominal, 1: up, -1: down
    )


### recalculate MET & Uncertainties ###

from PhysicsTools.PatUtils.tools.runMETCorrectionsAndUncertainties import runMetCorAndUncFromMiniAOD

runMetCorAndUncFromMiniAOD(
    process,
    isData=options.isData
)
addModule(process.fullPatMetSequence)

### Q/G Likelihood ###

### MET Significance ###


### Match Trigger ###

process.matchMuonTriggers = cms.EDProducer("TriggerMatcherMuons",
    triggerResult = cms.InputTag("TriggerResults","","HLT"),
    triggerObjects = cms.InputTag("selectedPatTrigger"),
    recoObjects = cms.InputTag("slimmedMuons"),
    dR = cms.double(0.1),
    flags = cms.PSet(
        IsoMu22 = cms.string("HLT_IsoMu22_v[0-9]+"),
        IsoTkMu22 = cms.string("HLT_IsoTkMu22_v[0-9]+"),
    
        IsoMu24 = cms.string("HLT_IsoMu24_v[0-9]+"),
        IsoTkMu24 = cms.string("HLT_IsoTkMu24_v[0-9]+"),
        
        IsoMu27 = cms.string("HLT_IsoMu27_v[0-9]+"),
        IsoTkMu27 = cms.string("HLT_IsoTkMu27_v[0-9]+"),
    )
)
addModule(process.matchMuonTriggers)


process.matchElectronTriggers = cms.EDProducer("TriggerMatcherElectrons",
    triggerResult = cms.InputTag("TriggerResults","","HLT"),
    triggerObjects = cms.InputTag("selectedPatTrigger"),
    recoObjects = cms.InputTag("slimmedElectrons"),
    dR = cms.double(0.1),
    flags = cms.PSet(
        Ele27WPLoose = cms.string("HLT_Ele27_eta2p1_WPLoose_Gsf_v[0-9]+"),
        Ele27WPTight = cms.string("HLT_Ele27_eta2p1_WPTight_Gsf_v[0-9]+"),
        
        Ele30WPLoose = cms.string("HLT_Ele30_eta2p1_WPLoose_Gsf_v[0-9]+"),
        Ele30WPTight = cms.string("HLT_Ele30_eta2p1_WPTight_Gsf_v[0-9]+"),
        
        Ele32WPLoose = cms.string("HLT_Ele32_eta2p1_WPLoose_Gsf_v[0-9]+"),
        Ele32WPTight = cms.string("HLT_Ele32_eta2p1_WPTight_Gsf_v[0-9]+"),


    )
)
addModule(process.matchElectronTriggers)

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
    
    


setattr(process.pat2pxlio,"muons",cms.PSet(
    type=cms.string("MuonConverter"),
    srcs=cms.VInputTag(cms.InputTag("slimmedMuons")),
    names=cms.vstring("Muon"),
    valueMaps = cms.PSet(
        IsoMu22 = cms.PSet(type=cms.string("ValueMapAccessorBool"), src=cms.InputTag("matchMuonTriggers","IsoMu22")),
        IsoTkMu22 = cms.PSet(type=cms.string("ValueMapAccessorBool"), src=cms.InputTag("matchMuonTriggers","IsoTkMu22")),

        IsoMu24 = cms.PSet(type=cms.string("ValueMapAccessorBool"), src=cms.InputTag("matchMuonTriggers","IsoMu24")),
        IsoTkMu24 = cms.PSet(type=cms.string("ValueMapAccessorBool"), src=cms.InputTag("matchMuonTriggers","IsoTkMu24")),
    
        IsoMu27 = cms.PSet(type=cms.string("ValueMapAccessorBool"), src=cms.InputTag("matchMuonTriggers","IsoMu27")),
        IsoTkMu27 = cms.PSet(type=cms.string("ValueMapAccessorBool"), src=cms.InputTag("matchMuonTriggers","IsoTkMu27")),
    ),
    select=cms.string("pt>10.0"), #keep at 10 for veto
    
))

setattr(process.pat2pxlio,"electrons",cms.PSet(
    type=cms.string("ElectronConverter"),
    srcs=cms.VInputTag(cms.InputTag("slimmedElectrons")),
    names=cms.vstring("Electron"),
    effAreasConfigFile = cms.FileInPath("RecoEgamma/ElectronIdentification/data/Summer16/effAreaElectrons_cone03_pfNeuHadronsAndPhotons_80X.txt"),
    valueMaps=cms.PSet(
        Ele27WPLoose = cms.PSet(type=cms.string("ValueMapAccessorBool"), src=cms.InputTag("matchElectronTriggers","Ele27WPLoose")),
        Ele27WPTight = cms.PSet(type=cms.string("ValueMapAccessorBool"), src=cms.InputTag("matchElectronTriggers","Ele27WPTight")),
        Ele30WPLoose = cms.PSet(type=cms.string("ValueMapAccessorBool"), src=cms.InputTag("matchElectronTriggers","Ele30WPLoose")),
        Ele30WPTight = cms.PSet(type=cms.string("ValueMapAccessorBool"), src=cms.InputTag("matchElectronTriggers","Ele30WPTight")),
        Ele32WPLoose = cms.PSet(type=cms.string("ValueMapAccessorBool"), src=cms.InputTag("matchElectronTriggers","Ele32WPLoose")),
        Ele32WPTight = cms.PSet(type=cms.string("ValueMapAccessorBool"), src=cms.InputTag("matchElectronTriggers","Ele32WPTight")),
    
        summer16eleIDVeto25ns = cms.PSet(
            type=cms.string("ValueMapAccessorBool"),
            src=cms.InputTag("egmGsfElectronIDs","cutBasedElectronID-Summer16-80X-V1-veto")
        ),
        summer16eleIDTight25ns = cms.PSet(
            type=cms.string("ValueMapAccessorBool"),
            src=cms.InputTag("egmGsfElectronIDs","cutBasedElectronID-Summer16-80X-V1-tight")
        ),
        summer16eleHTLPreselection = cms.PSet(
            type=cms.string("ValueMapAccessorBool"),
            src=cms.InputTag("egmGsfElectronIDs","cutBasedElectronHLTPreselection-Summer16-V1")
        )
        
    ),
    select=cms.string("pt>15.0"), #keep at 15 for veto
))

#add jets/met
if options.isData:
    setattr(process.pat2pxlio,"patJets",cms.PSet(
        type=cms.string("JetConverter"),
        srcs=cms.VInputTag(
            #cms.InputTag("slimmedJets","","RECO"),
            cms.InputTag("slimmedJetsJEC"),
        ),
        names=cms.vstring(
            #"Jet_reco",
            "Jet",
        ),
        select=cms.string("pt>20.0"),
        valueMaps=cms.PSet(
        ),
        filterPaths=cms.vstring(filteredPath),
    ))
    setattr(process.pat2pxlio,"patMET",cms.PSet(
        type=cms.string("METConverter"),
        srcs=cms.VInputTag(
            cms.InputTag("slimmedMETs","","RECO"),
            #cms.InputTag("patPFMetT1","","DX"),
            cms.InputTag("slimmedMETs","","DX"),
        ),
        names=cms.vstring(
            "MET_reco",
            #"MET",
            "MET"
        )
    ))
else:
    setattr(process.pat2pxlio,"patJets",cms.PSet(
        type=cms.string("JetConverter"),
        srcs=cms.VInputTag(
            cms.InputTag("slimmedJetsJEC"),
            cms.InputTag("slimmedJetsJECSmeared"),
            
            cms.InputTag("slimmedJetsJECSmearedEnUp"),
            cms.InputTag("slimmedJetsJECSmearedEnDown"),
            cms.InputTag("slimmedJetsJECSmearedResUp"),
            cms.InputTag("slimmedJetsJECSmearedResDown"),
            
            
            #cms.InputTag("shiftedPatJetEnDown"), # this cannot be used - jets are already cleaned against leptons for T1
            #cms.InputTag("shiftedPatJetEnUp"), # this cannot be used - jets are already cleaned against leptons for T1
        ),
        names=cms.vstring(
            "Jet",
            "Jetsmeared",
            
            "JetEnUp",
            "JetEnDown",
            "JetResUp",
            "JetResDown",
        ),
        select=cms.string("pt>20.0"),
        valueMaps=cms.PSet(
        ),
        filterPaths=cms.vstring(filteredPath),
    ))
    setattr(process.pat2pxlio,"patMET",cms.PSet(
        type=cms.string("METConverter"),
        srcs=cms.VInputTag(
            #cms.InputTag("slimmedMETs","","RECO"),
            
            cms.InputTag("patPFMetT1","","DX"),
            
            cms.InputTag("patPFMetT1JetEnUp"),
            cms.InputTag("patPFMetT1JetEnDown"),
            
            #cms.InputTag("patPFMetT1MuonEnUp"),
            #cms.InputTag("patPFMetT1MuonEnDown"),
            #cms.InputTag("patPFMetT1ElectronEnUp"),
            #cms.InputTag("patPFMetT1ElectronEnDown"),
            #cms.InputTag("patPFMetT1PhotonEnUp"),
            #cms.InputTag("patPFMetT1PhotonEnDown"),
            #cms.InputTag("patPFMetT1TauEnUp"),
            #cms.InputTag("patPFMetT1TauEnDown"),
            
            cms.InputTag("patPFMetT1UnclusteredEnUp"),
            cms.InputTag("patPFMetT1UnclusteredEnDown"),
            
        ),
        names=cms.vstring(
            #"MET_reco",
            
            "MET",
            "METEnUp",
            "METEnDown",
            "METUncUp",
            "METUncDown",
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

if options.reHLT:
    setattr(process.pat2pxlio,"triggersHLT",cms.PSet(
        type=cms.string("TriggerResultConverter"),
        srcs=cms.VInputTag(cms.InputTag("TriggerResults","","HLT2")),
        regex=cms.vstring("HLT_Iso[0-9a-zA-z_]*","HLT_Ele[0-9a-zA-z_]*")
    ))
else:
    setattr(process.pat2pxlio,"triggersHLT",cms.PSet(
        type=cms.string("TriggerResultConverter"),
        srcs=cms.VInputTag(cms.InputTag("TriggerResults","","HLT")),
        regex=cms.vstring("HLT_IsoMu[2-9][0-9a-zA-z_]*","HLT_IsoTkMu[2-9][0-9a-zA-z_]*","HLT_Ele[2-9][0-9a-zA-z_]*")
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
        srcs=cms.VInputTag(cms.InputTag("lessGenParticles") if not options.noGen else ()),
        targetEventViews=cms.vstring("Generated"),
        LHEEvent=cms.InputTag("externalLHEProducer"),
        skipLHEweights = cms.bool(options.noLHE),
        GenEventInfo=cms.InputTag("generator"),
        filterPaths=cms.vstring(filteredPath),
    ))

'''      
    setattr(process.pat2pxlio,"genjets",cms.PSet(
        type=cms.string("GenJetConverter"),
        srcs=cms.VInputTag(cms.InputTag("slimmedGenJets") if not options.noGen else ()),
        names=cms.vstring("GenJet"),
        targetEventViews=cms.vstring("GenJets"),
        filterPaths=cms.vstring(filteredPath),
    ))
'''
    
    
#add particle level reco

if (not options.isData) and options.addPL:
    print "Adding particle level objects"
    
    from TopQuarkAnalysis.TopEventProducers.producers.pseudoTop_cfi import pseudoTop
    process.pseudoTop = pseudoTop.clone(jetMaxEta = cms.double(10.0))
    addModule(process.pseudoTop)
    setattr(process.pat2pxlio,"ptLeptons",cms.PSet(
        type=cms.string("GenJetConverter"),
        srcs=cms.VInputTag(cms.InputTag("pseudoTop","leptons") if not options.noGen else ()),
        names=cms.vstring("Lepton"),
        targetEventViews=cms.vstring("PT"),
        filterPaths=cms.vstring(filteredPath),
    ))
    setattr(process.pat2pxlio,"ptJets",cms.PSet(
        type=cms.string("GenJetConverter"),
        srcs=cms.VInputTag(cms.InputTag("pseudoTop","jets") if not options.noGen else ()),
        names=cms.vstring("Jet"),
        targetEventViews=cms.vstring("PT"),
        filterPaths=cms.vstring(filteredPath),
    ))
    setattr(process.pat2pxlio,"ptNeutrino",cms.PSet(
        type=cms.string("GenParticleConverter"),
        srcs=cms.VInputTag(cms.InputTag("pseudoTop","neutrinos") if not options.noGen else ()),
        names=cms.vstring("Neutrino"),
        targetEventViews=cms.vstring("PT"),
        filterPaths=cms.vstring(filteredPath),
    ))
    

    ### new RIVET-based producer ###
    
    process.genMerger = cms.EDProducer("MergedGenParticleProducer",
        inputPruned = cms.InputTag("prunedGenParticles"),
        inputPacked = cms.InputTag("packedGenParticles")
    )
    process.hepmcProducer = cms.EDProducer("GenParticles2HepMCConverter",
        genParticles = cms.InputTag("genMerger"),
        genEventInfo = cms.InputTag("generator"),
    )
    
    process.rivetPseudoTop = cms.EDProducer("PseudoTopRivetProducer",
        src = cms.InputTag("hepmcProducer","unsmeared"),
        minLeptonPt = cms.double(15),
        maxLeptonEta = cms.double(2.5),
        minJetPt = cms.double(20),
        maxJetEta = cms.double(5.0),
        leptonConeSize = cms.double(0.1),
        jetConeSize = cms.double(0.4),
        wMass = cms.double(80.4),
        tMass = cms.double(172.5),

        minLeptonPtDilepton = cms.double(20),
        maxLeptonEtaDilepton = cms.double(2.4),
        minDileptonMassDilepton = cms.double(20),
        minLeptonPtSemilepton = cms.double(20),
        maxLeptonEtaSemilepton = cms.double(2.4),
        minVetoLeptonPtSemilepton = cms.double(15),
        maxVetoLeptonEtaSemilepton = cms.double(2.5),
        minMETSemiLepton = cms.double(30),
        minMtWSemiLepton = cms.double(30),
    )
    addModule(process.rivetPseudoTop)
    setattr(process.pat2pxlio,"rivetLeptons",cms.PSet(
        type=cms.string("GenParticleConverter"),
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

    
    import EDM2PXLIO.Plt.pltReco as pltReco
    
    
    
    for postfix,projectLepton,projectNu,projectAllNu in [
        ["",False,False,False],
        ["ExL",True,False,False],
        ["ExLN",True,True,False],
        ["ExLNN",True,True,True],
    ]:
        addModule(pltReco.addPltCollection(process,filteredPath,"exTauPromptNu"+postfix,
            "(abs(pdgId)==11 || abs(pdgId)==13 || abs(pdgId)==22)&&(isPromptFinalState)",
            "(abs(pdgId)==12 || abs(pdgId)==14 || abs(pdgId)==16)&&(isPromptFinalState)",
            projectLepton,projectNu,projectAllNu
        ))
        addModule(pltReco.addPltCollection(process,filteredPath,"incTauPromptNu"+postfix,
            "(abs(pdgId)==11 || abs(pdgId)==13 || abs(pdgId)==22)&&(isPromptFinalState||isDirectPromptTauDecayProductFinalState)",
            "(abs(pdgId)==12 || abs(pdgId)==14 || abs(pdgId)==16)&&(isPromptFinalState||isDirectPromptTauDecayProductFinalState)",
            projectLepton,projectNu,projectAllNu
        ))
        
    '''
    for postfix,projectLepton,projectNu,projectAllNu in [
        ["",False,False,False],
        ["ExL",True,False,False],
        ["ExLN",True,True,False],
    ]:
        addModule(pltReco.addPltCollection(process,filteredPath,"exTauAllNu"+postfix,
            "(abs(pdgId)==11 || abs(pdgId)==13 || abs(pdgId)==22)&&(isPromptFinalState)",
            "(abs(pdgId)==12 || abs(pdgId)==14 || abs(pdgId)==16)",
            projectLepton,projectNu,projectAllNu
        ))
        addModule(pltReco.addPltCollection(process,filteredPath,"incTauAllNu"+postfix,
            "(abs(pdgId)==11 || abs(pdgId)==13 || abs(pdgId)==22)&&(isPromptFinalState||isDirectPromptTauDecayProductFinalState)",
            "(abs(pdgId)==12 || abs(pdgId)==14 || abs(pdgId)==16)",
            projectLepton,projectNu,projectAllNu
        ))
    '''

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
        "keep *_matchMuonTriggers_*_*"
     ),
    dropMetaData = cms.untracked.string('ALL'),
)
process.endpath+= process.OUT
'''
