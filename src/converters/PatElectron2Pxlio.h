#ifndef _PATELECTRON2PXLIO_H_
#define _PATELECTRON2PXLIO_H_

// system include files
#include <memory>

// user include files
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/EDAnalyzer.h"

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"

#include "FWCore/ParameterSet/interface/ParameterSet.h"

#include "FWCore/Utilities/interface/InputTag.h"

#include "DataFormats/Common/interface/Handle.h"

#include "DataFormats/PatCandidates/interface/Electron.h"

#include "DataFormats/VertexReco/interface/Vertex.h"

#include "PhysicsTools/PatUtils/interface/StringParserTools.h"

#include "Pxl/Pxl/interface/Pxl.h"

#include "EDM2PXLIO/EDM2PXLIO/src/Collection2Pxlio.h"

#include "EDM2PXLIO/EDM2PXLIO/src/converters/Pat2Pxlio.h"

#include <iostream>

class PatElectron2Pxlio: public Pat2Pxlio<pat::Electron>
{
    protected:
        const reco::Vertex* primaryVertex_;
        std::string vertexInputTag_;
    public:
        PatElectron2Pxlio(std::string name,std::string vertexInputTag=std::string()):
            Pat2Pxlio<pat::Electron>(name),
            vertexInputTag_(vertexInputTag)
        {
        }
        
        virtual void convert(const edm::Event* edmEvent, pxl::Event* pxlEvent)
        {
            /*
            if (vertexInputTag_.length()>0) {
                edm::Handle<std::vector<reco::Vertex>> vertexList;
                edmEvent->getByLabel(vertexInputTag_,vertexList);
                primaryVertex_ = &(*vertexList)[0];
            }
            */
            Pat2Pxlio<pat::Electron>::convert(edmEvent,pxlEvent);
            
        }
                
        virtual void convertObject(const pat::Electron& patObject, pxl::Particle* pxlParticle)
        {
            pxlParticle->setCharge(patObject.charge());
            pxlParticle->setUserRecord<float>("dB",patObject.dB());
            pxlParticle->setUserRecord<float>("mva",patObject.electronID("mvaTrigV0"));
            pxlParticle->setUserRecord<bool>("ecalDrivenSeed",patObject.ecalDrivenSeed());
            pxlParticle->setUserRecord<bool>("trackerDrivenSeed",patObject.trackerDrivenSeed());
            pxlParticle->setUserRecord<bool>("isInEB-EE",(1.4442 < fabs(patObject.eta())) && (fabs(patObject.eta()) < 1.5660));
            
            pxlParticle->setUserRecord<bool>("passConversionVeto",patObject.passConversionVeto());
            pxlParticle->setUserRecord<int>("numberOfHits",patObject.gsfTrack()->trackerExpectedHitsInner().numberOfHits());
            
            if (primaryVertex_)
            {
                double dz =fabs(patObject.gsfTrack()->dxy(primaryVertex_->position()));
	            pxlParticle->setUserRecord<float>("dz",dz);
            }

        }
        
        virtual void convertP4(const pat::Electron& patObject, pxl::Particle* pxlParticle)
        {
            pxlParticle->setP4(patObject.ecalDrivenMomentum().px(),patObject.ecalDrivenMomentum().py(),patObject.ecalDrivenMomentum().pz(),patObject.ecalDrivenMomentum().energy());
        }
        
        virtual void convertCollection(const std::vector<pat::Electron>* patObjectList, std::vector<pxl::Particle*> pxlParticleList)
        {
        }
        
        ~PatElectron2Pxlio()
        {
        }
};

#endif

