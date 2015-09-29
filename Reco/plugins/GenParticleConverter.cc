#include "EDM2PXLIO/Reco/plugins/GenParticleConverter.h"
#include "EDM2PXLIO/Core/interface/ConverterFactory.h"

namespace edm2pxlio
{

GenParticleConverter::GenParticleConverter(const std::string& name, const edm::ParameterSet& globalConfig, edm::ConsumesCollector& consumesCollector):
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

void GenParticleConverter::convert(const edm::Event* edmEvent, const edm::EventSetup* iSetup, pxl::Event* pxlEvent) const
{
    //Base::convert(edmEvent, iSetup, pxlEvent);
    if (!_triggerResultFilter.checkPath(*edmEvent))
    {
        return;
    }

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
            
            pxlEventView->setUserRecord("nMEPartons",genEventInfoProduct->nMEPartons());
            pxlEventView->setUserRecord("nMEPartonsFiltered",genEventInfoProduct->nMEPartonsFiltered());
            

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
            if (edmEvent->getByToken(_lheEventProductToken,lheEventProduct))
            {
                const std::vector<LHEEventProduct::WGT>& weights = lheEventProduct->weights();
                for (unsigned int iweight = 0; iweight < weights.size(); ++iweight)
                {
                    pxlEventView->setUserRecord("lheweight_"+weights[iweight].id,weights[iweight].wgt);
                }
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
                    auto daughterIt = addMap.find((size_t)daughterObject);
                    if (daughterIt==addMap.end())
                    {
                        //daughter was not in collection
                        continue;
                    }
                    pxl::Particle* daughter = daughterIt->second.second;
                    mother->linkDaughter(daughter);
                }
            }
        }
    }
}

void GenParticleConverter::convertObject(const reco::GenParticle& genObject, pxl::Particle* pxlParticle) const
{
    pxlParticle->setP4(genObject.px(),genObject.py(),genObject.pz(),genObject.energy());
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

GenParticleConverter::~GenParticleConverter()
{
}

}

DEFINE_EDM_PLUGIN(edm2pxlio::ConverterFactory, edm2pxlio::GenParticleConverter, "GenParticleConverter");

