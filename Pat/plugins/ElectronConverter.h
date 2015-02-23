#ifndef __EDM2PXLIO_PAT_ELECTRONCONVERTER_H__
#define __EDM2PXLIO_PAT_ELECTRONCONVERTER_H__

// user include files
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

#include "EDM2PXLIO/Provider/interface/PrimaryVertexProvider.h"

#include "DataFormats/PatCandidates/interface/Electron.h"

#include "DataFormats/VertexReco/interface/Vertex.h"

//#include "EDM2PXLIO/EDM2PXLIO/src/provider/EffectiveAreaIsolationProvider.h"

namespace edm2pxlio
{

class ElectronConverter:
    public CollectionClassConverter<pat::Electron>
{
    private:
        typedef CollectionClassConverter<pat::Electron> Base;
    protected:
        PrimaryVertexProvider* _primaryVertexProvider;

    public:
        ElectronConverter(const std::string& name, const edm::ParameterSet& globalConfig, edm::ConsumesCollector& consumesCollector);
        
        virtual void convertObject(const pat::Electron& patObject, pxl::Particle* pxlParticle) const;
        
        virtual void convertP4(const pat::Electron& patObject, pxl::Particle* pxlParticle) const;
        /*
        virtual void convertCollection(const edm::Handle<edm::View<pat::Electron>> patObjectList, std::vector<pxl::Particle*>& pxlParticleList)
        {
            CollectionClass2Pxlio<pat::Electron>::convertCollection(patObjectList, pxlParticleList);
            
            if (effectiveAreaIsolationProvider_->getEARho())
            {
                //besides reco eletrons, gsf electrons are also present in the map so the numbers do not match...
                
                //if (effectiveAreaIsolationProvider_->getEARho()->size()!=patObjectList->size())
                //{
                //   throw cms::Exception("PatElectron2Pxlio::convertCollection") << "electron ea03 value map differs in size compared to provided pat electrons";
                // }
                
                for (unsigned iel=0; iel<patObjectList->size(); ++iel)
                {
                    const pat::Electron el = (*patObjectList)[iel];
                    if (effectiveAreaIsolationProvider_->getEARho()->contains(el.originalObjectRef().id()))
                    {
                        double ea03 = (*effectiveAreaIsolationProvider_->getEARho())[el.originalObjectRef()];
                        pxlParticleList[iel]->setUserRecord("ea03",ea03);
                    }

                }
            }
        }
        */
        virtual ~ElectronConverter()
        {
        }
};

}

#endif

