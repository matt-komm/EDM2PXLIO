#ifndef __EDM2PXLIO_RECO_GENPARTICLECONVERTER_H__
#define __EDM2PXLIO_RECO_GENPARTICLECONVERTER_H__

// user include files

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/EventSetup.h"

#include "FWCore/ParameterSet/interface/ParameterSet.h"

#include "FWCore/Utilities/interface/InputTag.h"

#include "DataFormats/Common/interface/Handle.h"
#include "FWCore/Framework/interface/ESHandle.h"
#include "DataFormats/Common/interface/View.h"

#include "SimDataFormats/GeneratorProducts/interface/GenEventInfoProduct.h"
#include "SimDataFormats/GeneratorProducts/interface/LHEEventProduct.h"
#include "SimGeneral/HepPDTRecord/interface/ParticleDataTable.h"
#include "DataFormats/HepMCCandidate/interface/GenParticle.h"

#include "Pxl/Pxl/interface/pxl/core.hh"
#include "Pxl/Pxl/interface/pxl/hep.hh"

#include "boost/math/special_functions/sign.hpp"

#include "EDM2PXLIO/Core/interface/CollectionClassConverter.h"

#include <unordered_map>

namespace edm2pxlio
{

class GenParticleConverter: 
    public CollectionClassConverter<reco::GenParticle>
{
    private:
        typedef CollectionClassConverter<reco::GenParticle> Base;
    protected:
        
        
        edm::InputTag _genEventInfoProductInputTag;
        edm::EDGetTokenT<GenEventInfoProduct>  _genEventInfoProductToken;
        edm::InputTag _lheEventProductInputTag;
        edm::EDGetTokenT<LHEEventProduct>  _lheEventProductToken;
        bool _useNameDB;

    public:
        GenParticleConverter(const std::string& name, const edm::ParameterSet& globalConfig, edm::ConsumesCollector& consumesCollector):
            Base(name,globalConfig,consumesCollector),
            _genEventInfoProductInputTag(),
            _lheEventProductInputTag(),
            _useNameDB(true)
        {
            //TODO: does not work yet
            Base::setDefaultEventView("Generated");

            if (globalConfig.exists(_name))
        	{
        	    const edm::ParameterSet& iConfig = globalConfig.getParameter<edm::ParameterSet>(_name);   
        	
                if (iConfig.exists("GenEventInfo"))
                {
                    _genEventInfoProductInputTag = iConfig.getParameter<edm::InputTag>("GenEventInfo");
                    _genEventInfoProductToken = consumesCollector.consumes<GenEventInfoProduct>(_genEventInfoProductInputTag);
                }
                else
                {
                    edm::LogWarning(_name) << "no EventInfo source defined";    
                }
                
                if (iConfig.exists("LHEEvent"))
                {
                    _lheEventProductInputTag = iConfig.getParameter<edm::InputTag>("LHEEvent");
                    _lheEventProductToken = consumesCollector.consumes<LHEEventProduct>(_lheEventProductInputTag);
                }
                else
                {
                    edm::LogWarning(_name) << "no LHEEvent source defined";    
                }
                
                if (iConfig.exists("useNameDB"))
                {
                    _useNameDB=iConfig.getParameter<bool>("useNameDB");
                }
                
            }
        }

        virtual void convert(const edm::Event* edmEvent, const edm::EventSetup* iSetup, pxl::Event* pxlEvent) const
        {
            //Base::convert(edmEvent, iSetup, pxlEvent);
        
            edm::ESHandle<ParticleDataTable> pdt;
            iSetup->getData(pdt);
            for (unsigned index=0; index<Base::size(); ++index)
            {
                pxl::EventView* pxlEventView = Base::findEventView(pxlEvent,Base::getEventViewName(index));

                if (_genEventInfoProductInputTag.label().length()>0)
                {
                    edm::Handle<GenEventInfoProduct> genEventInfoProduct;
                    edmEvent->getByToken(_genEventInfoProductToken,genEventInfoProduct);
                    pxlEventView->setUserRecord("signalProcessID",genEventInfoProduct->signalProcessID());
                    pxlEventView->setUserRecord("genweight",genEventInfoProduct->weight());
                    pxlEventView->setUserRecord("alphaQCD",genEventInfoProduct->alphaQCD());
                    pxlEventView->setUserRecord("alphaQED",genEventInfoProduct->alphaQED());
                    pxlEventView->setUserRecord("qscale",genEventInfoProduct->qScale());

                    const GenEventInfoProduct::PDF* pdf = genEventInfoProduct->pdf();
                    pxlEventView->setUserRecord("id1",pdf->id.first);
                    pxlEventView->setUserRecord("id2",pdf->id.second);
                    pxlEventView->setUserRecord("x1",pdf->x.first);
                    pxlEventView->setUserRecord("x2",pdf->x.second);
                    pxlEventView->setUserRecord("scalePDF",pdf->scalePDF);

                }
                if (_lheEventProductInputTag.label().length()>0)
                {
                    
                    edm::Handle<LHEEventProduct> lheEventProduct;
                    edmEvent->getByToken(_lheEventProductToken,lheEventProduct);

                    const std::vector<LHEEventProduct::WGT>& weights = lheEventProduct->weights();
                    for (unsigned int iweight = 0; iweight < weights.size(); ++iweight)
                    {
                        pxlEventView->setUserRecord("lheweight_"+weights[iweight].id,weights[iweight].wgt);
                    }
                }
                
                const edm::Handle<edm::View<reco::GenParticle>> collection = Base::getCollection(edmEvent,index);
                std::unordered_map<size_t,std::pair<const reco::GenParticle*,pxl::Particle*>> addMap;

                if (collection.product()) 
                {
                
                    for (unsigned iparticle=0; iparticle< collection->size(); ++iparticle) 
                    {
                        const reco::GenParticle& genObject = (*collection)[iparticle];
                        pxl::Particle* pxlParticle = pxlEventView->create<pxl::Particle>();
                        if (_useNameDB)
                        {
                            const ParticleData* pd = pdt->particle(genObject.pdgId());
                            if (pd)
                            {
                                pxlParticle->setName(pd->name());
                            }
                        }
                        else
                        {
                            pxlParticle->setName(Base::getCollectionName(index));
                        }
                        convertObject(genObject,pxlParticle);
                        addMap[(size_t)&genObject]=std::make_pair(&genObject,pxlParticle);
                    }
                    for (unsigned iparticle=0; iparticle< collection->size(); ++iparticle) 
                    {
                        const reco::GenParticle& genObject = (*collection)[iparticle];
                        pxl::Particle* mother = addMap[(size_t)&genObject].second;
                        for (unsigned idaughter=0; idaughter<genObject.numberOfDaughters(); ++idaughter)
                        {
                            const reco::Candidate* daughterObject = genObject.daughter(idaughter);
                            pxl::Particle* daughter = addMap[(size_t)daughterObject].second;
                            mother->linkDaughter(daughter);
                        }
                        
                    }
                }
            }
        }
        
        virtual void convertObject(const reco::GenParticle& genObject, pxl::Particle* pxlParticle) const
        {
            pxlParticle->setP4(genObject.px(),genObject.py(),genObject.pz(),genObject.energy());
            pxlParticle->setPdgNumber(genObject.pdgId());
            pxlParticle->setUserRecord("status",genObject.status());
        }
        
        virtual ~GenParticleConverter()
        {
        }
};

}

#endif

