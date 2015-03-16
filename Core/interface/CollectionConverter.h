#ifndef __EDM2PXLIO_CORE_COLLECTIONCONVERTER_H__
#define __EDM2PXLIO_CORE_COLLECTIONCONVERTER_H__

// user include files

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/Framework/interface/ConsumesCollector.h"

#include "FWCore/MessageLogger/interface/MessageLogger.h"

#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/Utilities/interface/InputTag.h"

#include "DataFormats/Common/interface/Handle.h"

#include "Pxl/Pxl/interface/pxl/core.hh"
#include "Pxl/Pxl/interface/pxl/hep.hh"

#include "EDM2PXLIO/Core/interface/Converter.h"

#include <vector>
#include <string>

namespace edm2pxlio
{

template<class CollectionClass>
class CollectionConverter: 
    public edm2pxlio::Converter
{
    protected:
        std::vector<edm::EDGetTokenT<CollectionClass> > _tokens;
        std::vector<std::string> _names;
        std::vector<std::string> _eventViewNames;
        
        std::string _defaultName;
        std::string _defaultEventView;
        
    public:    
        CollectionConverter(const std::string& name, const edm::ParameterSet& globalConfig, edm::ConsumesCollector& consumesCollector):
            Converter(name, globalConfig, consumesCollector),
            _defaultName(name),
            _defaultEventView("Reconstructed")
        {
            if (globalConfig.exists(getName()))
            {
                const edm::ParameterSet& iConfig = globalConfig.getParameter<edm::ParameterSet>(getName());

                if (iConfig.exists("srcs"))
                {
                    std::vector<edm::InputTag> tags = iConfig.getParameter<std::vector<edm::InputTag> >("srcs");
                    for (unsigned int itag = 0; itag < tags.size(); ++itag)
                    {
                        _tokens.emplace_back(consumesCollector.consumes<CollectionClass>(tags[itag]));
                    }
                } else {
                    edm::LogWarning(getName()) << "no sources defined";
                }
                
                if (iConfig.exists("names")) 
                {
                    _names = iConfig.getParameter<std::vector<std::string> >("names");
                    if (_names.size()!=_tokens.size() && _names.size()>0)
                    {
                        edm::LogInfo(getName()) << "will use the same name for all sources: "<<_names[0];    
                    }
                } else {
                    _names.push_back(_defaultName);
                    edm::LogInfo(getName()) << "no default name defined - will use default name: "<<_names[0];    
                }
                if (iConfig.exists("targetEventViews"))
                {
                    _eventViewNames = iConfig.getParameter<std::vector<std::string> >("targetEventViews");
                    if (_eventViewNames.size()!=_tokens.size() && _eventViewNames.size()>0)
                    {
                        edm::LogInfo(getName()) << "will use the same eventviewname for all sources: "<<_eventViewNames[0];    
                    }
                } else {
                    _eventViewNames.push_back(_defaultEventView);
                    edm::LogInfo(getName()) << "no default event view defined - will use default: "<<_eventViewNames[0];
                }
            }
            else
            {
            }
        }
        
        inline void setDefaultName(const std::string& defaultName)
        {
            _defaultName=defaultName;
        }
        
        inline void setDefaultEventView(const std::string& defaultEventView)
        {
            _defaultEventView=defaultEventView;
        }

        pxl::EventView* findEventView(pxl::Event* pxlEvent, const std::string& name) const
        {
            pxl::EventView* eventView = pxlEvent->findObject<pxl::EventView>(name);
            if (!eventView) {
                eventView = pxlEvent->createIndexed<pxl::EventView>(name);
                eventView->setName(name);
            }
            return eventView;
        }
        
        virtual unsigned size() const
        {
            return _tokens.size();
        }
        
        virtual const edm::Handle<CollectionClass> getCollection(const edm::Event* edmEvent, unsigned index) const
        {
            if (_tokens.size()>index)
            {
                edm::Handle<CollectionClass> collectionObjects;
                edmEvent->getByToken(_tokens[index],collectionObjects);
                return collectionObjects;
            }
            return edm::Handle<CollectionClass>(nullptr,nullptr);
        }
        
        virtual std::string getCollectionName(unsigned index) const
        {
            std::string collectionName = _names[0];
            if (_names.size()==_tokens.size())
            {
                collectionName=_names[index];
            }
            return std::move(collectionName);
        }
        
        virtual std::string getEventViewName(unsigned index) const
        {
            std::string eventViewName = _eventViewNames[0];
            if (_eventViewNames.size()==_tokens.size())
            {
                eventViewName=_eventViewNames[index];
            }
            return std::move(eventViewName);
        }
        
        
        virtual ~CollectionConverter()
        {
        }

};

}

#endif



