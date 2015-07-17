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

process.load('Configuration.StandardSequences.Services_cff')
process.load('Configuration.EventContent.EventContent_cff')
process.load('Configuration.Geometry.GeometryIdeal_cff')
process.load('Configuration.StandardSequences.MagneticField_38T_cff')
process.load('Configuration.StandardSequences.FrontierConditions_GlobalTag_cff')
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

process.source = cms.Source("PoolSource",
    fileNames = cms.untracked.vstring(
        'root://xrootd.unl.edu//store/mc/RunIISpring15DR74/TT_TuneCUETP8M1_13TeV-powheg-pythia8/MINIAODSIM/Asympt50ns_MCRUN2_74_V9A-v1/50000/02CF0510-4CFF-E411-A715-0025905A6090.root'
    )
)
process.maxEvents = cms.untracked.PSet( input = cms.untracked.int32(1000) )


### gen particle pruner ###

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




### selectors ###

process.selectedMuons = cms.EDFilter("CandViewSelector",
    src = cms.InputTag("slimmedMuons"),
    cut = cms.string("pt > 10.0")
)
process.numMuonsFilter = cms.EDFilter("CandViewCountFilter",
    src = cms.InputTag("selectedMuons"),
    minNumber = cms.uint32(1)
)
'''
process.selectedJets = cms.EDFilter("CandViewSelector",
    src = cms.InputTag("slimmedJets"),
    cut = cms.string("pt > 20.0")
)
process.numJetsFilter = cms.EDFilter("CandViewCountFilter",
    src = cms.InputTag("selectedJets"),
    minNumber = cms.uint32(2)
)

process.selectedPUPPIJets = cms.EDFilter("CandViewSelector",
    src = cms.InputTag("slimmedPUPPIJets"),
    cut = cms.string("pt > 20.0")
)
process.numPUPPIJetsFilter = cms.EDFilter("CandViewCountFilter",
    src = cms.InputTag("selectedPUPPIJets"),
    minNumber = cms.uint32(2)
)
'''
process.skimSequence = cms.Sequence(
    process.selectedMuons
    *process.numMuonsFilter
    #*process.selectedJets
    #*process.numJetsFilter
    #*process.selectedPUPPIJets
    #*process.numPUPPIJetsFilter
)
    


### electron IDs ###

from PhysicsTools.SelectorUtils.tools.vid_id_tools import *
switchOnVIDElectronIdProducer(process, DataFormat.MiniAOD)
for eleID in [
    'RecoEgamma.ElectronIdentification.Identification.cutBasedElectronID_PHYS14_PU20bx25_V2_cff',
    'RecoEgamma.ElectronIdentification.Identification.heepElectronID_HEEPV51_cff'
]:
    setupAllVIDIdsInModule(process,eleID,setupVIDElectronSelection)




'''
#PF-Weighted Candidates
## PF ChargedParticles
process.pfAllChargedParticles = cms.EDFilter("CandPtrSelector",
    src = cms.InputTag("packedPFCandidates"),
    cut = cms.string("charge!=0 && fromPV")
)
## PF Pileup ChargedParticles
process.pfPileUpAllChargedParticles = cms.EDFilter("CandPtrSelector",
    src = cms.InputTag("packedPFCandidates"),
    cut = cms.string("charge!=0 && !fromPV")
)

## PF Photons
process.pfAllPhotons = cms.EDFilter("CandPtrSelector", 
    src = cms.InputTag("slimmedPhotons"), 
    cut = cms.string("pt>0.5 && pdgId==22"),
    filter = cms.bool(False)
)

## PF NeutralHadrons
process.pfAllNeutralHadrons = cms.EDFilter("CandPtrSelector",
    src = cms.InputTag("packedPFCandidates"),
    cut = cms.string("pt>0.5 && charge==0 && !pdgId==22")
)

process.PFSequence = cms.Sequence(process.pfAllChargedParticles+process.pfPileUpAllChargedParticles+process.pfAllPhotons+process.pfAllNeutralHadrons)

## PF weights
from CommonTools.ParticleFlow.deltaBetaWeights_cfi import *
process.pfWeightedPhotons = pfWeightedPhotons.clone()
process.pfWeightedPhotons.src  =  cms.InputTag('pfAllPhotons')
process.pfWeightedPhotons.chargedFromPV  = cms.InputTag('pfAllChargedParticles')
process.pfWeightedPhotons.chargedFromPU  = cms.InputTag("pfPileUpAllChargedParticles")

process.pfWeightedNeutralHadrons = pfWeightedNeutralHadrons.clone()
process.pfWeightedNeutralHadrons.src  = cms.InputTag("pfAllNeutralHadrons")
process.pfWeightedNeutralHadrons.chargedFromPV  = cms.InputTag("pfAllChargedParticles")
process.pfWeightedNeutralHadrons.chargedFromPU  = cms.InputTag("pfPileUpAllChargedParticles")

process.pfDeltaBetaWeightingSequence = cms.Sequence(process.pfWeightedPhotons*process.pfWeightedNeutralHadrons)

# PUPPI weights
from CommonTools.PileupAlgos.Puppi_cff import puppi
process.puppi = puppi.clone()
process.puppi.candName=cms.InputTag("packedPFCandidates")
process.puppi.vertexName=cms.InputTag("offlineSlimmedPrimaryVertices")

process.puppiSequence = cms.Sequence(process.puppi)

puppiIsoMuonList=[]
for coneSize in [0.05,0.1,0.15,0.2,0.25,0.3,0.35,0.4,0.45,0.5]:
    puppiIsoMuon = cms.EDProducer('PUPPILeptonIsoProducer',
        leptons = cms.InputTag("slimmedMuons"),
        pfCands = cms.InputTag("packedPFCandidates"),
        puppi = cms.InputTag("puppi"),
        dRConeSize = cms.untracked.double(coneSize)
    )
    producerName = "puppiIsoMuonR%02i" % int(coneSize*100)
    puppiIsoMuonList.append(producerName)
    setattr(process,producerName,puppiIsoMuon)
    process.puppiSequence+=getattr(process,producerName)
    
puppiIsoElectronList=[]
for coneSize in [0.05,0.1,0.15,0.2,0.25,0.3,0.35,0.4,0.45,0.5]:
    puppiIsoElectron = cms.EDProducer('PUPPILeptonIsoProducer',
        leptons = cms.InputTag("slimmedElectrons"),
        pfCands = cms.InputTag("packedPFCandidates"),
        puppi = cms.InputTag("puppi"),
        dRConeSize = cms.untracked.double(coneSize)
    )
    producerName = "puppiIsoElectronR%02i" % int(coneSize*100)
    puppiIsoElectronList.append(producerName)
    setattr(process,producerName,puppiIsoElectron)
    process.puppiSequence+=getattr(process,producerName)
    
'''
'''
process.pfWeightedLeptonIso = cms.EDProducer('PFWeightedLeptonIsoProducer',
    electrons = cms.InputTag("slimmedElectrons"),
    muons = cms.InputTag("slimmedMuons"),
    pfCands = cms.InputTag("packedPFCandidates"),
    pfWeightedHadrons = cms.InputTag("pfWeightedNeutralHadrons"),
    pfWeightedPhotons =cms.InputTag("pfWeightedPhotons"),
    dRConeSize = cms.untracked.double(0.4)
)
'''
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
                "STEA_plain","STEA_filtered"
            ),
        )
    ),
    primaryVertex = cms.InputTag("offlineSlimmedPrimaryVertices"),

    muons = cms.PSet(
        type=cms.string("MuonConverter"),
        srcs=cms.VInputTag(cms.InputTag("slimmedMuons")),
        names=cms.vstring("Muon"),
        valueMaps = cms.PSet()
    ),
    
    electrons = cms.PSet(
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
        )
    ),
                                 
    jets = cms.PSet(
        type=cms.string("JetConverter"),
        srcs=cms.VInputTag(cms.InputTag("slimmedJets")),
        names=cms.vstring("Jet"),
        select=cms.string("pt>20.0"),
        valueMaps=cms.PSet(
            #jesUp = cms.PSet(
            #    type=cms.string("ValueMapAccessorLorentzVector"),
            #    src=cms.InputTag("jesUp","jets")
            #),
            #jesDown = cms.PSet(
            #    type=cms.string("ValueMapAccessorLorentzVector"),
            #    src=cms.InputTag("jesDown","jets")
            #),
            #btagSFB = cms.PSet(
            #    type=cms.string("ValueMapAccessorFloat"),
            #    src=cms.InputTag("btaggingSF","SFB")
            #),
            #btagSFBerr = cms.PSet(
            #   type=cms.string("ValueMapAccessorFloat"),
            #    src=cms.InputTag("btaggingSF","SFBerr")
            #)
        ),
        triggerFilter=filterPSet
    ),
    
    puppiJets = cms.PSet(
        type=cms.string("JetConverter"),
        srcs=cms.VInputTag(cms.InputTag("slimmedJetsPuppi")),
        names=cms.vstring("PuppiJets"),
        select=cms.string("pt>20.0"),
        triggerFilter=filterPSet
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
        names=cms.vstring("MET"),
        valueMaps=cms.PSet(
            #jesUp = cms.PSet(
            #    type=cms.string("ValueMapAccessorLorentzVector"),
            #    src=cms.InputTag("jesUp","mets")
            #),
            #jesDown = cms.PSet(
            #    type=cms.string("ValueMapAccessorLorentzVector"),
            #    src=cms.InputTag("jesDown","mets")
            #)
        )
        
    ),
    puppiMets = cms.PSet(
        type=cms.string("METConverter"),
        srcs=cms.VInputTag(cms.InputTag("slimmedMETsPuppi")),
        names=cms.vstring("PuppiMET"),
        
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
    
    puInfo = cms.PSet(
        type=cms.string("PileupSummaryInfoConverter"),
        srcs=cms.VInputTag(cms.InputTag("addPileupInfo","","HLT")),
        names=cms.vstring("PU")
    ),
)

'''
for puppiIsoMuon in puppiIsoMuonList:
    pSet = cms.PSet(
        type=cms.string("ValueMapAccessorDouble"),
        src=cms.InputTag(puppiIsoMuon)
    )
    setattr(process.pat2pxlio.muons.valueMaps,puppiIsoMuon,pSet)

for puppiIsoElectron in puppiIsoElectronList:
    pSet = cms.PSet(
        type=cms.string("ValueMapAccessorDouble"),
        src=cms.InputTag(puppiIsoElectron)
    )
    setattr(process.pat2pxlio.electrons.valueMaps,puppiIsoElectron,pSet)
'''
    
    
process.STEA_plain=cms.Path(
    process.lessGenParticles
    *process.egmGsfElectronIDSequence
    #*process.jesUp
    #*process.jesDown
    #*process.btaggingSF
    #*process.PFSequence
    #*process.pfDeltaBetaWeightingSequence
    #*process.puppiSequence
    #*process.pfWeightedLeptonIso
)

process.STEA_filtered=cms.Path(
    process.skimSequence
    *process.lessGenParticles
    *process.egmGsfElectronIDSequence
    #*process.jesUp
    #*process.jesDown
    #*process.btaggingSF
    #*process.PFSequence
    #*process.pfDeltaBetaWeightingSequence
    #*process.puppiSequence
    #*process.pfWeightedLeptonIso
)

process.endpath= cms.EndPath()

process.endpath+=process.pat2pxlio
'''
process.OUT = cms.OutputModule("PoolOutputModule",
    fileName = cms.untracked.string('output.root'),
    outputCommands = cms.untracked.vstring('keep *','keep patJets_patJetsAK4PF_*_*','keep patJets_patJetsAK4PFCHS_*_*','keep *_*_*_PAT','keep *_*_*_S2')
)
process.endpath+= process.OUT
'''
