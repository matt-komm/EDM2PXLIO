#ifndef __EDM2PXLIO_PAT_JETCONVERTER_H__
#define __EDM2PXLIO_PAT_JETCONVERTER_H__

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/EventSetup.h"

#include "FWCore/ParameterSet/interface/ParameterSet.h"

#include "FWCore/Utilities/interface/InputTag.h"

#include "DataFormats/Common/interface/Handle.h"
#include "DataFormats/Common/interface/View.h"

#include "DataFormats/Candidate/interface/Candidate.h"

#include "Pxl/Pxl/interface/pxl/core.hh"
#include "Pxl/Pxl/interface/pxl/hep.hh"

#include "EDM2PXLIO/Core/interface/CollectionClassConverter.h"

#include "DataFormats/PatCandidates/interface/MET.h"

#include <vector>
#include <utility>

namespace edm2pxlio
{

class METConverter:
    public CollectionClassConverter<pat::MET>
{
    private:
        typedef CollectionClassConverter<pat::MET> Base;
        
        static const std::vector<std::pair<std::string,pat::MET::METUncertainty>> SYSVariations;
        
        std::vector<edm::InputTag> _metSignificanceInputTags;
        std::vector<edm::EDGetTokenT<double>> _metSignificancesToken;
        
    public:
        METConverter(const std::string& name, const edm::ParameterSet& globalConfig, edm::ConsumesCollector& consumesCollector):
            Base(name, globalConfig, consumesCollector)
        {
        
            if (globalConfig.exists(_name))
	        {
	            const edm::ParameterSet& iConfig = globalConfig.getParameter<edm::ParameterSet>(_name);   
	
                if (iConfig.exists("metSignificances"))
                {
                    _metSignificanceInputTags = iConfig.getParameter<std::vector<edm::InputTag>>("metSignificances");
                    for (unsigned int itag = 0; itag<_metSignificanceInputTags.size();++itag)
                    {
                        _metSignificancesToken.push_back(consumesCollector.consumes<double>(_metSignificanceInputTags[itag]));
                    }
                }
            }
        }
        
        void addSystematicVariation(
            const pat::MET& met, 
            pxl::Particle* particle, 
            const std::string& name,
            pat::MET::METUncertainty shift, 
            pat::MET::METUncertaintyLevel level
        ) const
        {
            pat::MET::Vector2 shiftedVec = met.shiftedP2(shift,level);
            pxl::BasicNVector vec(2);
            vec.setElement(0,shiftedVec.px);
            vec.setElement(1,shiftedVec.py);
            particle->setUserRecord(name,vec);
        }
        
        void convert(const edm::Event* edmEvent, const edm::EventSetup* iSetup, pxl::Event* pxlEvent) const
        {
            for (unsigned index=0; index<Base::size(); ++index)
            {
                
                pxl::EventView* pxlEventView = Base::findEventView(pxlEvent,Base::getEventViewName(index));

                const edm::Handle<edm::View<pat::MET>> collection = Base::getCollection(edmEvent,index);
                std::unordered_map<size_t,std::pair<const pat::MET*,pxl::Particle*>> addMap;
                
                if (collection.product()) 
                {
                    
                    edm::Handle<double> metSigCollection;
                    
                    if (_metSignificanceInputTags.size()==Base::size() && _metSignificanceInputTags[index].label().length()>0)
                    {
                        edmEvent->getByToken(_metSignificancesToken[index],metSigCollection);
                    }
                    
                    for (unsigned iparticle=0; iparticle< collection->size(); ++iparticle) 
                    {
                        const pat::MET& met = (*collection)[iparticle];
                        pxl::Particle* pxlParticle = pxlEventView->create<pxl::Particle>();
                        pxlParticle->setName(Base::getCollectionName(index));
                        convertObject(met,pxlParticle);
                        
                        if (_metSignificanceInputTags.size()==Base::size() && _metSignificanceInputTags[index].label().length()>0)
                        {
                            pxlParticle->setUserRecord("metSignificance",*metSigCollection);
                        }
                    }
                }
            }
        }
        
        
        virtual void convertObject(const pat::MET& patObject, pxl::Particle* pxlParticle) const
        {
            Base::convertObject(patObject,pxlParticle);
            pxlParticle->setP4(patObject.px(),patObject.py(),patObject.pz(),patObject.energy());
            
            pxlParticle->setUserRecord("uncorrectedPhi",PRECISION(patObject.uncorrectedPhi()));
            pxlParticle->setUserRecord("uncorrectedPt",PRECISION(patObject.uncorrectedPt()));
            
            
            for (unsigned int isys = 0; isys < SYSVariations.size(); ++isys)
            {
                try
                {
                    addSystematicVariation(patObject,pxlParticle,SYSVariations[isys].first,SYSVariations[isys].second,pat::MET::Type1p2);
                }
                catch (...)
                {
                }
            }
            
            //pxlParticle->setUserRecord("metSignificance",patObject.metSignificance());
        }
        
        ~METConverter()
        {
        }
};

const std::vector<std::pair<std::string,pat::MET::METUncertainty>> METConverter::SYSVariations = {
    {"JetEnUp",pat::MET::JetEnUp},
    {"JetEnDown",pat::MET::JetEnDown},
    {"JetResUp",pat::MET::JetResUp},
    {"JetResDown",pat::MET::JetResDown},
    {"MuonEnUp",pat::MET::MuonEnUp},
    {"MuonEnDown",pat::MET::MuonEnDown},
    {"ElectronEnUp",pat::MET::ElectronEnUp},
    {"ElectronEnDown",pat::MET::ElectronEnDown},
    {"TauEnUp ",pat::MET::TauEnUp },
    {"TauEnDown",pat::MET::TauEnDown},
    {"UnclusteredEnUp",pat::MET::UnclusteredEnUp},
    {"UnclusteredEnDown",pat::MET::UnclusteredEnDown}
};

}

#endif

