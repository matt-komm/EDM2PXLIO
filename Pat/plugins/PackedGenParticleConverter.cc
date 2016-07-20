#include "EDM2PXLIO/Pat/plugins/PackedGenParticleConverter.h"
#include "EDM2PXLIO/Core/interface/ConverterFactory.h"
#include "EDM2PXLIO/Core/interface/ProviderFactory.h"

#include <functional>

namespace edm2pxlio
{


PackedGenParticleConverter::PackedGenParticleConverter(const std::string& name, const edm::ParameterSet& globalConfig, edm::ConsumesCollector& consumesCollector):
    Base(name, globalConfig, consumesCollector),
    _basicsOnly(false),
    _useNameDB(true),
    _pdt(nullptr)
{
    if (globalConfig.exists(_name))
    {
        const edm::ParameterSet& iConfig = globalConfig.getParameter<edm::ParameterSet>(_name);   

        if (iConfig.exists("basicVariablesOnly"))
        {
            _basicsOnly = iConfig.getParameter<bool>("basicVariablesOnly");
        }
        
        if (iConfig.exists("useNameDB"))
        {
            _useNameDB=iConfig.getParameter<bool>("useNameDB");
        }
    }
}

void PackedGenParticleConverter::beginEvent(const edm::Event* edmEvent, const edm::EventSetup* iSetup)
{
    if (_useNameDB)
    {
        edm::ESHandle<ParticleDataTable> pdtHandle;
        iSetup->getData(pdtHandle);
        _pdt=pdtHandle.product();
    }
}

void PackedGenParticleConverter::endEvent(const edm::Event* edmEvent, const edm::EventSetup* iSetup)
{
    _pdt=nullptr;
}

void PackedGenParticleConverter::convertObject(const pat::PackedGenParticle& genObject, pxl::Particle* pxlParticle) const
{
    Base::convertObject(genObject, pxlParticle);
    if (_useNameDB and _pdt)
    {
        const ParticleData* pd = _pdt->particle(genObject.pdgId());
        if (pd)
        {
            pxlParticle->setName(pd->name());
        }
    }
    pxlParticle->setP4(genObject.px(),genObject.py(),genObject.pz(),genObject.energy());
    pxlParticle->setCharge(genObject.charge());
    pxlParticle->setPdgNumber(genObject.pdgId());
    pxlParticle->setUserRecord("status",genObject.status());
    
    unsigned int statusBits = 0;
    const std::bitset<15>& flags = genObject.statusFlags ().flags_;
    int power = 1;
    for (unsigned int ibit = 0; ibit<flags.size();++ibit)
    {
        statusBits+=power*flags[ibit];
        power<<=1;
    }
    pxlParticle->setUserRecord("statusBits",statusBits);
}

PackedGenParticleConverter::~PackedGenParticleConverter()
{
}

}

DEFINE_EDM_PLUGIN(edm2pxlio::ConverterFactory, edm2pxlio::PackedGenParticleConverter, "PackedGenParticleConverter");
