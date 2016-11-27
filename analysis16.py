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
process.load('Configuration.StandardSequences.FrontierConditions_GlobalTag_condDBv2_cff')
#process.load('Configuration.StandardSequences.FrontierConditions_GlobalTag_cff')


if options.isData:
    process.GlobalTag.globaltag = '80X_dataRun2_Prompt_ICHEP16JEC_v0'
    
    ### frontier database ###
    from CondCore.DBCommon.CondDBSetup_cfi import *
    process.frontierDB = cms.ESSource("PoolDBESSource",
        #CondDBSetup,
        toGet = cms.VPSet(
            cms.PSet(
                record = cms.string('JetCorrectionsRecord'),
                tag    = cms.string('JetCorrectorParametersCollection_Spring16_25nsV6_DATA_AK4PFchs'), #should correspond to V6 (V3 is here an internal name inside the GT)
                label  = cms.untracked.string('AK4PFchs')
            ),
        ),
        connect = cms.string('frontier://FrontierProd/CMS_CONDITIONS'),
        #connect = cms.string('sqlite:Spring16_25nsV3_DATA.db')
    )
else:
    process.GlobalTag.globaltag = '80X_mcRun2_asymptotic_2016_miniAODv2_v1'
    
    ### frontier database ###
    from CondCore.DBCommon.CondDBSetup_cfi import *
    process.frontierDB = cms.ESSource("PoolDBESSource",
        #CondDBSetup,
        toGet = cms.VPSet(
            cms.PSet(
                record = cms.string('JetCorrectionsRecord'),
                tag    = cms.string('JetCorrectorParametersCollection_Spring16_25nsV6_MC_AK4PFchs'),
                label  = cms.untracked.string('AK4PFchs')
            ),
        ),
        connect = cms.string('frontier://FrontierProd/CMS_CONDITIONS'),
        #connect = cms.string('sqlite:Spring16_25nsV3_MC.db')
    )

process.es_prefer_jec = cms.ESPrefer("PoolDBESSource",'frontierDB')

process.load("FWCore.MessageLogger.MessageLogger_cfi")
process.MessageLogger.cerr.FwkReport.reportEvery = 100
process.MessageLogger.suppressWarning = cms.untracked.vstring('ecalLaserCorrFilter','manystripclus53X','toomanystripclus53X')


if options.isData and not options.isReRecoData:
    process.source = cms.Source("PoolSource",
        fileNames = cms.untracked.vstring(
            ' root://cmsxrootd.fnal.gov//store/data/Run2016B/SingleMuon/MINIAOD/PromptReco-v2/000/273/158/00000/18383F36-2E1A-E611-8C57-02163E014186.root'
        ),
        lumisToProcess = cms.untracked.VLuminosityBlockRange('273158:3-273158:13'),
    )
elif options.isData and options.isReRecoData:
    process.source = cms.Source("PoolSource",
        fileNames = cms.untracked.vstring(
            #'root://cmsxrootd.fnal.gov//store/data/Run2015B/SingleMuon/MINIAOD/PromptReco-v1/000/251/244/00000/68275270-7C27-E511-B1F0-02163E011A46.root' #{golden run: 251244:96-251244:121}        ),
        )
        #lumisToProcess = cms.untracked.VLuminosityBlockRange('251244:96-251244:121'),
    )
else:
    process.source = cms.Source("PoolSource",
        inputCommands = cms.untracked.vstring("keep *", "drop LHERunInfoProduct_*_*_*"), #this produces otherwise a memleak
        fileNames = cms.untracked.vstring(
            'root://cmsxrootd.fnal.gov//store/mc/RunIISpring16MiniAODv2/ST_t-channel_top_4f_inclusiveDecays_13TeV-powhegV2-madspin-pythia8_TuneCUETP8M1/MINIAODSIM/PUSpring16_80X_mcRun2_asymptotic_2016_miniAODv2_v0-v1/00000/04615FE5-D844-E611-B5F0-0090FAA58B94.root'
            #'root://cmsxrootd.fnal.gov//store/mc/RunIISpring16MiniAODv2/TT_TuneCUETP8M1_13TeV-powheg-pythia8/MINIAODSIM/PUSpring16_80X_mcRun2_asymptotic_2016_miniAODv2_v0_ext4-v1/00000/004A0552-3929-E611-BD44-0025905A48F0.root'
        ),
        #lumisToProcess = cms.untracked.VLuminosityBlockRange('251244:96-251244:121'),
    )
    
    
if options.isData:
    process.maxEvents = cms.untracked.PSet( input = cms.untracked.int32(400) )
else:
    process.maxEvents = cms.untracked.PSet( input = cms.untracked.int32(1000) )

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
    
if not options.noFilter:
    addFilter(cms.InputTag("slimmedMuons"),"pt>15.0",minN=1)
    addFilter(cms.InputTag("slimmedJets"),"pt>15.0",minN=2)
    addFilter(cms.InputTag("slimmedJets"),"pt>20.0",minN=1)
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
if options.isData:
    process.jec25nsSpring16V3.levels.append('L2L3Residual')
    
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
    isData=options.isData
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
    effAreasConfigFile = cms.FileInPath("RecoEgamma/ElectronIdentification/data/Summer16/effAreaElectrons_cone03_pfNeuHadronsAndPhotons_80X.txt"),
    valueMaps=cms.PSet(
        spring15eleIDVeto25ns = cms.PSet(
            type=cms.string("ValueMapAccessorBool"),
            src=cms.InputTag("egmGsfElectronIDs","cutBasedElectronID-Summer16-80X-V1-veto")
        ),
        spring15eleIDTight25ns = cms.PSet(
            type=cms.string("ValueMapAccessorBool"),
            src=cms.InputTag("egmGsfElectronIDs","cutBasedElectronID-Summer16-80X-V1-tight")
        ),
        
    ),
    select=cms.string("pt>10.0"),
))

#add jets/met
if options.isData:
    setattr(process.pat2pxlio,"patJets",cms.PSet(
        type=cms.string("JetConverter"),
        srcs=cms.VInputTag(
            #cms.InputTag("slimmedJets"),
            cms.InputTag("patJetsReapplyJEC"),
        ),
        names=cms.vstring(
            "Jet",
        ),
        select=cms.string("pt>15.0"),
        valueMaps=cms.PSet(
        ),
        filterPaths=cms.vstring(filteredPath),
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
else:
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
        srcs=cms.VInputTag(cms.InputTag("lessGenParticles") if not options.noGen else ()),
        targetEventViews=cms.vstring("Generated"),
        LHEEvent=cms.InputTag("externalLHEProducer" if not options.noLHE else ""),
        GenEventInfo=cms.InputTag("generator","","SIM"),
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
    process.pseudoTop = pseudoTop.clone()
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
        "keep *_pseudoTop*_*_*"
     ),
    dropMetaData = cms.untracked.string('ALL'),
)
process.endpath+= process.OUT
'''

