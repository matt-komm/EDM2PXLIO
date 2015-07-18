#include "PileupSummaryInfoConverter.h"
#include "EDM2PXLIO/Core/interface/ConverterFactory.h"
#include "EDM2PXLIO/Core/interface/ProviderFactory.h"

namespace edm2pxlio
{

PileupSummaryInfoConverter::PileupSummaryInfoConverter(const std::string& name, const edm::ParameterSet& globalConfig, edm::ConsumesCollector& consumesCollector):
    Base(name, globalConfig, consumesCollector),
    _primaryVertexProvider(nullptr)
{
    _primaryVertexProvider=ProviderFactory::get<PrimaryVertexProvider>(globalConfig,consumesCollector);
}

void PileupSummaryInfoConverter::convert(const edm::Event* edmEvent, const edm::EventSetup* iSetup, pxl::Event* pxlEvent) const
{
    //TODO: two producers: 1 for pileup summary, 1 for general reco::Vertex collection
    Base::convert(edmEvent, iSetup, pxlEvent);
    pxl::EventView* pxlEventView = Base::findEventView(pxlEvent,Base::getEventViewName(0));
    pxl::Particle* puObject = pxlEventView->create<pxl::Particle>();
    puObject->setName(Base::getCollectionName(0));

    for (unsigned index=0; index<Base::size(); ++index)
    {
        if (not edmEvent->isRealData())
        {
            const edm::Handle<std::vector<PileupSummaryInfo>> collection = Base::getCollection(edmEvent,index);
            
            if (collection.product())
            {
                pxl::BasicNVector nInteractionVector(collection->size());
                for (unsigned int i = 0; i < collection->size(); ++i)
                {
                    nInteractionVector.setElement(i,(*collection)[i].getPU_NumInteractions());
                    //puObject->setUserRecord(std::string("nInteractions_")+std::to_string(i),(*collection)[i].getPU_NumInteractions());
                }
                puObject->setUserRecord("nInteractions"+std::to_string(index),nInteractionVector);
            }
        }
        
        if (_primaryVertexProvider->getPrimaryVertex())
        {
            puObject->setUserRecord("nVertices",_primaryVertexProvider->getNVertices());
            puObject->setUserRecord("PVx",PRECISION(_primaryVertexProvider->getPrimaryVertex()->x()));
            puObject->setUserRecord("PVy",PRECISION(_primaryVertexProvider->getPrimaryVertex()->y()));
            puObject->setUserRecord("PVz",PRECISION(_primaryVertexProvider->getPrimaryVertex()->z()));
            puObject->setUserRecord("PVxError",PRECISION(_primaryVertexProvider->getPrimaryVertex()->xError()));
            puObject->setUserRecord("PVyError",PRECISION(_primaryVertexProvider->getPrimaryVertex()->yError()));
            puObject->setUserRecord("PVzError",PRECISION(_primaryVertexProvider->getPrimaryVertex()->zError()));
            
            puObject->setUserRecord("PVchi",PRECISION(_primaryVertexProvider->getPrimaryVertex()->chi2()));
            puObject->setUserRecord("PVndof",_primaryVertexProvider->getPrimaryVertex()->ndof());
            puObject->setUserRecord("PVvalid",_primaryVertexProvider->getPrimaryVertex()->isValid());
        }
    }
}

PileupSummaryInfoConverter::~PileupSummaryInfoConverter()
{
}

}

DEFINE_EDM_PLUGIN(edm2pxlio::ConverterFactory, edm2pxlio::PileupSummaryInfoConverter, "PileupSummaryInfoConverter");




