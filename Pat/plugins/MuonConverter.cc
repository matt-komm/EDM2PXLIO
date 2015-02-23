#include "EDM2PXLIO/Pat/plugins/MuonConverter.h"
#include "EDM2PXLIO/Core/interface/ConverterFactory.h"
#include "EDM2PXLIO/Core/interface/ProviderFactory.h"

namespace edm2pxlio
{

MuonConverter::MuonConverter(const std::string& name, const edm::ParameterSet& globalConfig, edm::ConsumesCollector& consumesCollector):
    Base(name, globalConfig, consumesCollector),
    _primaryVertexProvider(nullptr)
{
    _primaryVertexProvider=ProviderFactory::get<PrimaryVertexProvider>(globalConfig,consumesCollector);
}

void MuonConverter::convertObject(const pat::Muon& patObject, pxl::Particle* pxlParticle) const
{
    Base::convertObject(patObject, pxlParticle);
    pxlParticle->setP4(patObject.px(),patObject.py(),patObject.pz(),patObject.energy());
    pxlParticle->setCharge(patObject.charge());


    pxlParticle->setUserRecord("R04PFsumChargedHadronPt",patObject.pfIsolationR04().sumChargedHadronPt);
    pxlParticle->setUserRecord("R04PFsumNeutralHadronPt",patObject.pfIsolationR04().sumNeutralHadronEt);
    pxlParticle->setUserRecord("R04PFsumPhotonIso",patObject.pfIsolationR04().sumPhotonEt);
    pxlParticle->setUserRecord("R04PFsumPUPt",patObject.pfIsolationR04().sumPUPt);

    pxlParticle->setUserRecord("PATchargedHadronIso",patObject.chargedHadronIso());
    pxlParticle->setUserRecord("PATneutralHadronIso",patObject.neutralHadronIso());
    pxlParticle->setUserRecord("PATpuChargedHadronIso",patObject.puChargedHadronIso());

    pxlParticle->setUserRecord("isGlobalMuon",patObject.isGlobalMuon());
    pxlParticle->setUserRecord("isTrackerMuon",patObject.isTrackerMuon());
    pxlParticle->setUserRecord("isPFMuon",patObject.isPFMuon());

    pxlParticle->setUserRecord("numberOfMatchedStations",patObject.numberOfMatchedStations());

    //ref to embedded Track (tracker+muon chambers)
    if (patObject.combinedMuon().get()!=nullptr)
    {
        const reco::Track* combinedTrack = patObject.combinedMuon().get();

        pxlParticle->setUserRecord("numberOfValidMuonHits",combinedTrack->hitPattern().numberOfValidMuonHits());
        pxlParticle->setUserRecord("numberOfLostMuonHits",combinedTrack->hitPattern().numberOfLostMuonHits());
        pxlParticle->setUserRecord("chi2",combinedTrack->normalizedChi2());
        pxlParticle->setUserRecord("ndof",combinedTrack->ndof());
    }

    //ref to embedded Track (tracker only)
    if (patObject.track().get()!=nullptr)
    {
        const reco::Track* trackerTrack = patObject.track().get();
        pxlParticle->setUserRecord("trackerLayersWithMeasurement",trackerTrack->hitPattern().trackerLayersWithMeasurement());
        pxlParticle->setUserRecord("numberOfValidPixelHits",trackerTrack->hitPattern().numberOfValidPixelHits());

        if (_primaryVertexProvider->getPrimaryVertex()!=nullptr)
        {
            double dz = trackerTrack->dz(_primaryVertexProvider->getPrimaryVertex()->position());
            double dxy = trackerTrack->dxy(_primaryVertexProvider->getPrimaryVertex()->position());
            pxlParticle->setUserRecord("dz",dz);
            pxlParticle->setUserRecord("dxy",dxy);
            pxlParticle->setUserRecord("isTightMuon",patObject.isTightMuon(*_primaryVertexProvider->getPrimaryVertex()));
        }
    }
}

MuonConverter::~MuonConverter()
{
}

}

DEFINE_EDM_PLUGIN(edm2pxlio::ConverterFactory, edm2pxlio::MuonConverter, "MuonConverter");

