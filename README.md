edm2pxlio
=========

#General Configuration

##Plugin Configuration
```
pat2pxlio=cms.EDAnalyzer('EDM2PXLIO',
    outFileName=cms.string("out.pxlio"), #output file name
    process=cms.string("Wjets"), #name of the process for pxl::Event
    selectEvents=cms.VPSet(
        cms.PSet(
            process=cms.string(*"..."*), #store all events from this process, eg. PAT
            paths=cms.vstring(*"..."*), #store all events passing this path of the process, eg. p0
        )
    )
```

##Configure the collection to convert
A new converter will be created through an edm.PSet within the plugin configuration:
```
    *...*
    newCollection = cms.PSet(
        type=cms.string("..."), #*(required)* name of the converter, see below for supported types
        *... further configuration ...*
    ),
    *...*
```
General configuration fields for **all** collection converters are:
```
    srcs= edm.VInputTag(*edm.InputTag("..."),...*), #*(required)* the input tags of the collections
    names=cms.vstring(*...*), #*(optional)* names of the objects (if only one is given, same name will be used for all collections in *srcs*.
    targetEventViews=cms.vstring(*...*),  #*(optional)* names of the pxl::EventView (if only one is given, same name will be used for all collections in *srcs*.
```

##Configure the individual class to convert
In addition to the above, pxl::UserRecords can be stored via *StringObjectFunction*, eg.:
```
    userRecords=cms.PSet(
        abseta=cms.string("abs(eta)") #stores |eta| value of all objects as pxl::UserRecord "abseta"
        *...*
    )
```
Also, stored objects can be filtered by specifing a selection string via *StringCutObjectSelector*, eg.:
```
    select=cms.string("pt>10") #selects only objects with *pt > 10 GeV*
```

#Converters
##GenJet2Pxlio
##GenParticle2Pxlio
Additional optional configurations
```
    GenEventInfo=cms.InputTag(*...*), #stores generator information: eg.: Q-scale, PDF,...
    LHEEvent=cms.InputTag(*...*), #accesses new LHEv2 event weights for systematics
    useNameDB=cms.bool(*...*), #(default=true) stores gen particles by names from DB provided through *process.load('SimGeneral.HepPDTESSource.pythiapdt_cfi')*
```
##PatElectron2Pxlio
##PatJet2Pxlio
##PatMET2Pxlio
##PatMuon2Pxlio
##Candidate2Pxlio
##Trigger2Pxlio
Will attach trigger flags directly to the pxl::EventView. Triggers can be filtered by regex, eg.:
```
    regex=cms.vstring("Iso[0-9a-zA-z]*")
```
##ValueList2Pxlio

