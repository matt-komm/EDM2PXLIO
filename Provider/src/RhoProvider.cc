#include "EDM2PXLIO/Provider/interface/RhoProvider.h"

#include <iostream>

namespace edm2pxlio
{

RhoProvider::RhoProvider(const edm::ParameterSet& globalConfig, edm::ConsumesCollector& consumesCollector):
    Provider(globalConfig, consumesCollector),
    _rhoInputTag(),
    _rho(-1)
{
    if (globalConfig.exists("rho"))
    {
        _rhoInputTag = globalConfig.getParameter<edm::InputTag>("rho");
        _rhoToken = consumesCollector.consumes<double>(_rhoInputTag);
    }
    else
    {
        edm::LogWarning("RhoProvider") << "Instance requested but input 'rho' missing";
    }
}

void RhoProvider::process(const edm::Event* edmEvent, const edm::EventSetup* iSetup)
{
    if (_rhoInputTag.label().length()>0)
    {
        edm::Handle<double> rho;
        edmEvent->getByToken(_rhoToken,rho);
        _rho=*rho;
    }
}

RhoProvider::~RhoProvider()
{
}

}

