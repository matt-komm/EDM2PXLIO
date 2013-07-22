#ifndef _GENPARTICLE2PXLIO_H_
#define _GENPARTICLE2PXLIO_H_

// system include files
#include <memory>
#include <unordered_map>

// user include files
#include <boost/math/special_functions/sign.hpp>

#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/EDAnalyzer.h"
#include "FWCore/Utilities/interface/Exception.h"

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/Framework/interface/ESHandle.h"
#include "FWCore/Framework/interface/MakerMacros.h"

#include "FWCore/ParameterSet/interface/ParameterSet.h"

#include "FWCore/Utilities/interface/InputTag.h"

#include "DataFormats/Common/interface/Handle.h"

#include "SimGeneral/HepPDTRecord/interface/ParticleDataTable.h"

//#include "DataFormats/HepMCCandidate/interface/GenParticle.h"

#include "PhysicsTools/PatUtils/interface/StringParserTools.h"

#include "Pxl/Pxl/interface/Pxl.h"

#include "EDM2PXLIO/EDM2PXLIO/src/Collection2Pxlio.h"

#include "EDM2PXLIO/EDM2PXLIO/src/converters/Pat2Pxlio.h"

class GenParticle2Pxlio: public Collection2Pxlio<edm::View<reco::GenParticle>>
{
    protected:
        edm::ESHandle<ParticleDataTable> pdt_;
        std::unordered_map<long,pxl::Particle*> pxlCollectionMap;
    public:
        GenParticle2Pxlio(std::string name):
            Collection2Pxlio<edm::View<reco::GenParticle>>(name)
        {
             Collection2Pxlio<edm::View<reco::GenParticle>>::setDefaultEventView("Generated");
        }
        
        virtual void convert(const edm::Event* edmEvent, const edm::EventSetup* iSetup, pxl::Event* pxlEvent)
        {
            iSetup->getData(pdt_);
            for (unsigned index=0; index<Collection2Pxlio<edm::View<reco::GenParticle>>::size(); ++index)
            {
                pxlCollectionMap.clear();
                const edm::Handle<edm::View<reco::GenParticle>> collection = Collection2Pxlio<edm::View<reco::GenParticle>>::getCollection(edmEvent,index);
                pxl::EventView* pxlEventView = Collection2Pxlio<edm::View<reco::GenParticle>>::findEventView(pxlEvent,Collection2Pxlio<edm::View<reco::GenParticle>>::getEventViewName(index));
                
                if (collection.product()) {
                    for (unsigned iparticle=0; iparticle< collection->size(); ++iparticle) {
                        const reco::GenParticle genObject = (*collection)[iparticle];
                        pxl::Particle* pxlParticle = pxlEventView->create<pxl::Particle>();
                        pxlParticle->setName(getNameFromID(genObject.pdgId()));
                        convertP4(genObject,pxlParticle);
                        convertObject(genObject,pxlParticle);
                        if (pxlCollectionMap.find (getHash(&genObject))!=pxlCollectionMap.end())
                        {
			                /*
                            std::cout<<"collision detected"<<std::endl;
                            std::cout<<getNameFromID(genObject.pdgId())<<"\t"<<genObject.px()<<"\t"<<genObject.py()<<"\t"<<genObject.pz()<<"\t"<<std::endl;
                            std::cout<<pxlCollectionMap[getHash(&genObject)]->getName()<<"\t"<<pxlCollectionMap[getHash(&genObject)]->getPx()<<"\t"<<pxlCollectionMap[getHash(&genObject)]->getPy()<<"\t"<<pxlCollectionMap[getHash(&genObject)]->getPz()<<"\t"<<std::endl;
                            getHash(&genObject);
			                */
			                throw cms::Exception("EDM2PXLIO::GenParticle2Pxlio::convert") << "hash collision detected - report to the developer!";
			            }
                        pxlCollectionMap[getHash(&genObject)]=pxlParticle;
                        
                    }
                    connect(collection);
                }
            }
        }
        
        virtual void connect(const edm::Handle<edm::View<reco::GenParticle>> collection)
        {
            for (unsigned iparticle=0; iparticle< collection->size(); ++iparticle) {
                const reco::GenParticle genObject = (*collection)[iparticle];
                pxl::Particle* pxlParticle = pxlCollectionMap[getHash(&genObject)];
                for (unsigned idaughter=0; idaughter<genObject.numberOfDaughters(); ++idaughter)
                {
                    const reco::Candidate* daughterObject = genObject.daughter(idaughter);
                    pxl::Particle* pxlDaughter = pxlCollectionMap[getHash(daughterObject)];
                    pxlParticle->linkDaughter(pxlDaughter);
                }
            }
        }
        
        virtual std::string getNameFromID(int id)
        {
            const ParticleData* pd = pdt_->particle(id);
            if (!pd)
            {
                std::ostringstream name;
                name << "ID " << id;
                return name.str();
            } else {
                return pd->name();
            }
        }
        
        virtual void convertObject(const reco::GenParticle& genObject, pxl::Particle* pxlParticle)
        {
        }
        /*
        virtual int getHash(pxl::Particle* particle)
        {
            return int(particle->getPz()*1000000)+int(particle->getPx()*10000)+int(100*particle->getPy());
        }
        */
        virtual long getHash(const reco::Candidate* particle)
        {
            long hash1=long(fabs(particle->pz())*1000000*(boost::math::sign(particle->pz())+2));
            long hash2=long(fabs(particle->px())*10000*(boost::math::sign(particle->px())+2));
            long hash3=long(fabs(particle->py())*100*(boost::math::sign(particle->py())+2));
            return hash1+hash2+hash3;
        }
        
        virtual void convertP4(const reco::GenParticle& genObject, pxl::Particle* pxlParticle)
        {
            pxlParticle->setP4(genObject.px(),genObject.py(),genObject.pz(),genObject.energy());
        }
        
        ~GenParticle2Pxlio()
        {
        }
};

#endif

