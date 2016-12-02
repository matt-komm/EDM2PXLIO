import FWCore.ParameterSet.Config as cms

def addPltCollection(process,filteredPath,name,leptonCut,neutrinoCut,projectPromptLepton,projectPromptNu,projectAllNu):
    pSeq = cms.Sequence()

    setattr(process,name+"fiducialParticles",cms.EDFilter("PackedGenParticleSelector",
        src = cms.InputTag("packedGenParticles"),
        cut = cms.string("abs(eta)<5.0 && (status==1)")
    ))
    pSeq+=getattr(process,name+"fiducialParticles")

    setattr(process,name+"promptLeptons",cms.EDFilter("PackedGenParticleSelector",
        src = cms.InputTag(name+"fiducialParticles"),
        cut = cms.string(leptonCut)
    ))
    pSeq+=getattr(process,name+"promptLeptons")
    
    setattr(process,name+"dressedLeptons",cms.EDProducer("JetClustering",
        src=cms.InputTag(name+"promptLeptons"),
        jetR=cms.double(0.1),
        minJetPt=cms.double(15.0)
    ))
    pSeq+=getattr(process,name+"dressedLeptons")

    setattr(process,name+"promptNu",cms.EDFilter("PackedGenParticleSelector",
        src = cms.InputTag(name+"fiducialParticles"),
        cut = cms.string(neutrinoCut)
    ))
    pSeq+=getattr(process,name+"promptNu")
    
    setattr(process,name+"allNu",cms.EDFilter("PackedGenParticleSelector",
        src = cms.InputTag(name+"fiducialParticles"),
        cut = cms.string("(abs(pdgId)==12 || abs(pdgId)==14 || abs(pdgId)==16)")
    ))
    pSeq+=getattr(process,name+"allNu")
    

    setattr(process,name+"ghostBHadrons",cms.EDProducer("BHadronSelector",
        src = cms.InputTag("prunedGenParticles"),
        cut = cms.string("pt>5")
    ))
    pSeq+=getattr(process,name+"ghostBHadrons")
    
    setattr(process,name+"ghostBHadronsEta2p4",cms.EDProducer("BHadronSelector",
        src = cms.InputTag("prunedGenParticles"),
        cut = cms.string("(pt>5) && (abs(eta)<2.4)")
    ))
    pSeq+=getattr(process,name+"ghostBHadronsEta2p4")

    setattr(process,name+"stableParticles",cms.EDProducer("PackedGenParticleProjection",
        combine = cms.VInputTag(
            cms.InputTag(name+"fiducialParticles"),
        ),
        exclude = cms.VInputTag()
    ))
    if projectPromptLepton:
        getattr(process,name+"stableParticles").exclude.append(name+"promptLeptons")
    if projectPromptNu:
        getattr(process,name+"stableParticles").exclude.append(name+"promptNu")  
    if projectAllNu:
        getattr(process,name+"stableParticles").exclude.append(name+"allNu")  
    pSeq+=getattr(process,name+"stableParticles")
    
    setattr(process,name+"particleJets",cms.EDProducer("JetClustering",
        src=cms.InputTag(name+"stableParticles"),
        jetR=cms.double(0.4),
        minJetPt=cms.double(20.0),
        ghosts = cms.PSet(
            bHadrons = cms.InputTag(name+"ghostBHadrons"),
            bHadrons2p4 = cms.InputTag(name+"ghostBHadronsEta2p4")
        ),
    ))
    pSeq+=getattr(process,name+"particleJets")
    
    setattr(process,name+"genMetPromptNu",cms.EDProducer("METProducer",
        src=cms.InputTag(name+"promptNu")
    ))
    pSeq+=getattr(process,name+"genMetPromptNu")
    
    setattr(process,name+"genMetAllNu",cms.EDProducer("METProducer",
        src=cms.InputTag(name+"allNu")
    ))
    pSeq+=getattr(process,name+"genMetAllNu")
    
    
    setattr(process.pat2pxlio,name+"dressedLeptons",cms.PSet(
        type=cms.string("PltJetConverter"),
        srcs=cms.VInputTag(cms.InputTag(name+"dressedLeptons")),
        names=cms.vstring("Lepton"),
        targetEventViews=cms.vstring("PL_"+name),
        filterPaths=cms.vstring(filteredPath),
    ))

    setattr(process.pat2pxlio,name+"particleJets",cms.PSet(
        type=cms.string("PltJetConverter"),
        srcs=cms.VInputTag(cms.InputTag(name+"particleJets")),
        names=cms.vstring("Jet"),
        targetEventViews=cms.vstring("PL_"+name),
        filterPaths=cms.vstring(filteredPath),
    ))
    
    setattr(process.pat2pxlio,name+"promptNu",cms.PSet(
        type=cms.string("PackedGenParticleConverter"),
        srcs=cms.VInputTag(cms.InputTag(name+"genMetPromptNu"),cms.InputTag(name+"genMetAllNu")),
        names=cms.vstring("Neutrino","NeutrinoAll"),
        targetEventViews=cms.vstring("PL_"+name),
        filterPaths=cms.vstring(filteredPath),
    ))
    
    
    return pSeq
