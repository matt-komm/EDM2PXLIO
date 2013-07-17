#ifndef _COLLECTION2PXLIO_H_
#define _COLLECTION2PXLIO_H_

// system include files
#include <memory>

// user include files
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/EDAnalyzer.h"

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/Framework/interface/MakerMacros.h"

#include "FWCore/ParameterSet/interface/ParameterSet.h"

#include "FWCore/Utilities/interface/InputTag.h"

#include "DataFormats/Common/interface/Handle.h"

#include "DataFormats/PatCandidates/interface/Muon.h"

#include "PhysicsTools/PatUtils/interface/StringParserTools.h"

#include "Pxl/Pxl/interface/Pxl.h"


template<class CollectionClass>
class Collection2Pxlio
{
    protected:
        std::string name_;
        std::vector<edm::InputTag> srcs_;
        std::vector<std::string> names_;
        std::vector<std::string> eventViewNames_;
        
        std::string default_name_;
        std::string default_eventView_;
        
    public:    
        Collection2Pxlio(std::string name):
            name_(name),
            default_name_(name),
            default_eventView_("Reconstructed")
        {
        }
        
        ~Collection2Pxlio()
        {
        }

        void setDefaultName(std::string name)
        {
            default_name_=name;
        }
        
        void setDefaultEventView(std::string name)
        {
            default_eventView_=name;
        }

        void parseParameter(const edm::ParameterSet& iConfig)
        {
            if (iConfig.exists(name_+"Srcs")) 
            {
                srcs_ = iConfig.getParameter<std::vector<edm::InputTag> >(name_+"Srcs");
            } else {
                edm::LogWarning(name_+"Srcs") << "no sources defined";    
            }
            if (iConfig.exists(name_+"Names")) 
            {
                names_ = iConfig.getParameter<std::vector<std::string> >(name_+"Names");
                if (names_.size()!=srcs_.size() && names_.size()>0)
                {
                    edm::LogInfo(name_+"Names") << "will use the same name for all sources:"<<name_[0];    
                }
            } else {
                names_.push_back(default_name_);
                edm::LogInfo(name_+"Names") << "no default name defined - will use default name:"<<names_[0];    
            }
            if (iConfig.exists(name_+"TargetEventViews"))
            {
                eventViewNames_= iConfig.getParameter<std::vector<std::string> >(name_+"TargetEventViews");
                if (eventViewNames_.size()!=srcs_.size() && eventViewNames_.size()>0)
                {
                    edm::LogInfo(name_+"TargetEventViews") << "will use the same eventviewname for all sources:"<<eventViewNames_[0];    
                }
            } else {
                eventViewNames_.push_back(default_eventView_);
                edm::LogInfo(name_+"TargetEventViews") << "no default event view defined - will use default:"<<eventViewNames_[0];
            }
        }

        pxl::EventView* findEventView(pxl::Event* pxlEvent, std::string name)
        {
            pxl::EventView* eventView = pxlEvent->findObject<pxl::EventView>(name);
            if (!eventView) {
                eventView = pxlEvent->createIndexed<pxl::EventView>(name);
                eventView->setName(name);
            }
            return eventView;
        }
        
        virtual unsigned size()
        {
            return srcs_.size();
        }
        
        const virtual CollectionClass* getCollection(const edm::Event* edmEvent, unsigned index)
        {
            if (srcs_.size()>index)
            {
                edm::Handle<CollectionClass> collectionObjects;
                edmEvent->getByLabel(srcs_[index],collectionObjects);
                return &(*collectionObjects);
            }
            return nullptr;
        }
        
        virtual std::string getCollectionName(unsigned index)
        {
            std::string collectionName = names_[0];
            if (names_.size()==srcs_.size())
            {
                collectionName=names_[index];
            }
            return collectionName;
        }
        
        virtual std::string getEventViewName(unsigned index)
        {
            std::string eventViewName = eventViewNames_[0];
            if (eventViewNames_.size()==srcs_.size())
            {
                eventViewName=eventViewNames_[index];
            }
            return eventViewName;
        }


        virtual void convert(const edm::Event* edmEvent, const edm::EventSetup* iSetup, pxl::Event* pxlEvent)
        {
        }
};

#endif



