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

#include <SimDataFormats/GeneratorProducts/interface/GenEventInfoProduct.h>

//#include "DataFormats/HepMCCandidate/interface/GenParticle.h"

#include "PhysicsTools/PatUtils/interface/StringParserTools.h"

#include "Pxl/Pxl/interface/Pxl.h"

#include "EDM2PXLIO/EDM2PXLIO/src/common/Collection2Pxlio.h"

//#include "EDM2PXLIO/EDM2PXLIO/src/common/CollectionClass2Pxlio.h"

#include "EDM2PXLIO/EDM2PXLIO/src/common/ConverterFactory.h"

class GenParticle2Pxlio: public Collection2Pxlio<edm::View<reco::GenParticle>>
{
    protected:
        edm::ESHandle<ParticleDataTable> pdt_;
        std::unordered_map<long,pxl::Particle*> pxlCollectionMap;
        edm::InputTag genEventInfoProductInputTag_;

        bool _useNameDB;

    public:
        GenParticle2Pxlio(std::string name):
            Collection2Pxlio<edm::View<reco::GenParticle>>(name),
            genEventInfoProductInputTag_(),
            _useNameDB(true)
        {
             Collection2Pxlio<edm::View<reco::GenParticle>>::setDefaultEventView("Generated");
        }
        
        static void init()
        {
            ConverterFactory* fac = ConverterFactory::getInstance();
            fac->registerConverter(new ConverterProducerTmpl<GenParticle2Pxlio>("GenParticle2Pxlio"));
        }
        
        virtual void parseParameter(const edm::ParameterSet& globalConfig)
        {
            Collection2Pxlio<edm::View<reco::GenParticle>>::parseParameter(globalConfig);
            if (globalConfig.exists(name_))
        	{
        	    const edm::ParameterSet& iConfig = globalConfig.getParameter<edm::ParameterSet>(name_);   
        	
                if (iConfig.exists("EventInfo"))
                {
                    genEventInfoProductInputTag_ = iConfig.getParameter<edm::InputTag>("EventInfo");
                }
                else
                {
                    edm::LogWarning(name_) << "no EventInfo sources defined";    
                }
                
                if (iConfig.exists("useNameDB"))
                {
                    _useNameDB=iConfig.getParameter<bool>("useNameDB");
                }
                
            }
        }

        virtual void convert(const edm::Event* edmEvent, const edm::EventSetup* iSetup, pxl::Event* pxlEvent)
        {
            Collection2Pxlio<edm::View<reco::GenParticle>>::convert(edmEvent, iSetup, pxlEvent);
        
            iSetup->getData(pdt_);
            for (unsigned index=0; index<Collection2Pxlio<edm::View<reco::GenParticle>>::size(); ++index)
            {
                pxlCollectionMap.clear();
                const edm::Handle<edm::View<reco::GenParticle>> collection = Collection2Pxlio<edm::View<reco::GenParticle>>::getCollection(edmEvent,index);
                pxl::EventView* pxlEventView = Collection2Pxlio<edm::View<reco::GenParticle>>::findEventView(pxlEvent,Collection2Pxlio<edm::View<reco::GenParticle>>::getEventViewName(index));
                
                if (genEventInfoProductInputTag_.label().length()>0)
                {
                    edm::Handle<GenEventInfoProduct> genEventInfoProduct;
                    edmEvent->getByLabel(genEventInfoProductInputTag_,genEventInfoProduct);
                    pxlEventView->setUserRecord<unsigned int>("signalProcessID",genEventInfoProduct->signalProcessID());
                    pxlEventView->setUserRecord<float>("genweight",genEventInfoProduct->weight());
                    pxlEventView->setUserRecord<float>("alphaQCD",genEventInfoProduct->alphaQCD());
                    pxlEventView->setUserRecord<float>("alphaQED",genEventInfoProduct->alphaQED());
                    pxlEventView->setUserRecord<float>("qscale",genEventInfoProduct->qScale());

                    const GenEventInfoProduct::PDF* pdf = genEventInfoProduct->pdf();
                    pxlEventView->setUserRecord<int>("id1",pdf->id.first);
                    pxlEventView->setUserRecord<int>("id2",pdf->id.second);
                    pxlEventView->setUserRecord<float>("x1",pdf->x.first);
                    pxlEventView->setUserRecord<float>("x2",pdf->x.second);
                    pxlEventView->setUserRecord<float>("scalePDF",pdf->scalePDF);


                }


                if (collection.product()) 
                {
                    //std::map<long,std::vector<pxl::Particle*>> collisions;
                
                    for (unsigned iparticle=0; iparticle< collection->size(); ++iparticle) 
                    {
                        const reco::GenParticle genObject = (*collection)[iparticle];
                        if (pxlCollectionMap.find (getHash(&genObject))!=pxlCollectionMap.end())
                        {
                            /*
                            //store colliding particles and substitude them after the rest is connected
                            if (collisions.find(getHash(&genObject))==collisions.end())
                            {
                                //this is the connected object which will be substituted
                                collisions[getHash(&genObject)].push_back(pxlCollectionMap[getHash(&genObject)]);
                            }
                            collisions[getHash(&genObject)].push_back(pxlParticle);
                            */
                        }
                        else
                        {
                           
                            pxl::Particle* pxlParticle = pxlEventView->create<pxl::Particle>();
                            if (_useNameDB)
                            {
                                pxlParticle->setName(getNameFromID(genObject.pdgId()));
                            }
                            else
                            {
                                pxlParticle->setName(Collection2Pxlio<edm::View<reco::GenParticle>>::getCollectionName(index));
                            }
                            convertObject(genObject,pxlParticle);
                            
                            pxlCollectionMap[getHash(&genObject)]=pxlParticle;
                        }
                    }
                    connect(collection);
                    /*
                    stragtegy does not work due to pxl bug in getting the correct mothers
                    //now substitudes
                    for (std::map<long,std::vector<pxl::Particle*>>::iterator it=collisions.begin(); it!=collisions.end(); ++it)
                    {
                        pxl::Particle* daughter = (it->second)[0];
                        std::vector<pxl::Particle*> mothers;
                        //daughter->getMotherRelations().getObjectsOfType<pxl::Particle>(mothers);
                        //daughter->unlinkMothers();
                        const pxl::Relations& motherRel = daughter->getMotherRelations();
                        
                        (it->second).front()->setName("First Daughter");
                        (it->second).back()->setName("Last Daughter");
                        for (unsigned int iparticle = 1; iparticle< (it->second).size(); ++iparticle)
                        {
                            //(it->second)[iparticle]->linkDaughter((it->second)[iparticle-1]);
                        }
                        for (pxl::Relations::iterator mrel = motherRel.begin(); mrel!=motherRel.end(); ++mrel)
                        {
                            pxl::Particle* mother = dynamic_cast<pxl::Particle*>(*mrel);
                            mother->setName("Mother");
                        }
                        
                        for (unsigned int imother = 0; imother < mothers.size(); ++imother)
                        {
                            std::cout<<mothers[imother]<<std::endl;
                            //mothers[imother]->setName("Mother");
                            
                        }
                        
                    }
                    */
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
                    
                    //connect only if the daughter has also been stored
                    if (pxlCollectionMap.find(getHash(daughterObject))!=pxlCollectionMap.end())
                    {
                        pxl::Particle* pxlDaughter = pxlCollectionMap[getHash(daughterObject)];
                        pxlParticle->linkDaughter(pxlDaughter);
                    }
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
            pxlParticle->setP4(genObject.px(),genObject.py(),genObject.pz(),genObject.energy());
            pxlParticle->setParticleId(genObject.pdgId());
            pxlParticle->setUserRecord<int>("status",genObject.status());
        }
        /*
        virtual int getHash(pxl::Particle* particle)
        {
            return int(particle->getPz()*1000000)+int(particle->getPx()*10000)+int(100*particle->getPy());
        }
        */
        virtual long getHash(const reco::Candidate* particle)
        {
            long hash1=long(particle->mass()*100000000000);
            long hash2=long(fabs(particle->energy())*100000000*(boost::math::sign(particle->energy())+2));
            long hash3=long(fabs(particle->pz())*1000000*(boost::math::sign(particle->pz())+2));
            long hash4=long(fabs(particle->px())*10000*(boost::math::sign(particle->px())+2));
            long hash5=long(fabs(particle->py())*100*(boost::math::sign(particle->py())+2));
            long hash6=long(fabs(particle->status())*(boost::math::sign(particle->status())+2));
            
            return hash1+hash2+hash3+hash4+hash5+hash6;
        }
        
        ~GenParticle2Pxlio()
        {
        }
};

#endif

