#ifndef __CONVERTERFACTORY_H__
#define __CONVERTERFACTORY_H__

#include <string>
#include <map>

#include "EDM2PXLIO/EDM2PXLIO/src/common/Converter.h"

class ConverterProducer
{
    protected:
        std::string _productName;
    public:
        ConverterProducer(std::string productName):
            _productName(productName)
        {
            
        }
        
        virtual Converter* create(std::string)
        {
            return nullptr;
        }
        
        virtual std::string getProductName()
        {
            return _productName;
        }    
};

template<class Product>
class ConverterProducerTmpl:
    public ConverterProducer
{
    public:
        ConverterProducerTmpl(std::string productName):
            ConverterProducer(productName)
        {
        }
        
        virtual Converter* create(std::string confName)
        {
            return (Converter*)(new Product(confName));
        }
};

class ConverterFactory
{
    protected:
        ConverterFactory()
        {
        }
        std::map<std::string,ConverterProducer*> _producerDict;
    public:
        
        static ConverterFactory* getInstance()
        {
            static ConverterFactory factory;
            return &factory;
        }        
        
        void registerConverter(ConverterProducer* producer)
        {
            if (_producerDict.find(producer->getProductName())==_producerDict.end())
            {
                _producerDict[producer->getProductName()]=producer;
            }
            else
            {
                throw "product already present in factory";
            }
        }
        
        bool hasConverter(std::string typeName)
        {
            return _producerDict.find(typeName)!=_producerDict.end();
        }
        
        Converter* createConverter(std::string typeName, std::string objectName)
        {
            return _producerDict[typeName]->create(objectName);
        }
        
        ~ConverterFactory()
        {
        }
};

#endif

