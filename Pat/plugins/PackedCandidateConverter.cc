#include "EDM2PXLIO/Pat/plugins/PackedCandidateConverter.h"
#include "EDM2PXLIO/Core/interface/ConverterFactory.h"


//#include "DataFormats/HcalDetId/interface/HcalDetId.h"
#include "DataFormats/DetId/interface/DetId.h"

namespace edm2pxlio
{

PackedCandidateConverter::PackedCandidateConverter(const std::string& name, const edm::ParameterSet& globalConfig, edm::ConsumesCollector& consumesCollector):
    Base(name, globalConfig, consumesCollector)
{
    Base::setDefaultEventView("RecoCandidates");
    if (globalConfig.exists(_name))
    {
        const edm::ParameterSet& iConfig = globalConfig.getParameter<edm::ParameterSet>(_name);   

        if (iConfig.exists("matchToJets"))
        {
            _jetToMatchInputTags = iConfig.getParameter<std::vector<edm::InputTag>>("matchToJets");
            for (unsigned int itag = 0; itag<_jetToMatchInputTags.size();++itag)
            {
                _jetToMatchToken.push_back(consumesCollector.consumes<edm::View<pat::Jet>>(_jetToMatchInputTags[itag]));
            }
        }
        
        if (iConfig.exists("matchToVertex"))
        {
            _vertexToMatchInputTags = iConfig.getParameter<std::vector<edm::InputTag>>("matchToVertex");
            for (unsigned int itag = 0; itag<_vertexToMatchInputTags.size();++itag)
            {
                _vertexToMatchToken.push_back(consumesCollector.consumes<edm::View<reco::Vertex>>(_vertexToMatchInputTags[itag]));
            }
        }
    }
    
    //candPtEta = fs->make<TH2F>( "candPtEta"  , ";pt;eta", 100, 0.0, 30.0,100,-6.0,6.0);
    //candPhiEta = fs->make<TH2F>( "candPhiEta"  , ";phi;eta", 100, -3.5, 3.5,100,-6.0,6.0);
}

void PackedCandidateConverter::addObservables(pxl::Particle* particle, pxl::EventView* pxlEventView, std::vector<pxl::Vertex*>& vertices, const std::vector<const pat::PackedCandidate*>& cands) const
{
    math::XYZTLorentzVector vectorSum(0,0,0,0);
    const pat::PackedCandidate* highPtCand = nullptr;
    double maxPt=-1;
    for (const pat::PackedCandidate* cand: cands)
    {
        vectorSum+=cand->p4();
        if (cand->pt()>maxPt)
        {
            highPtCand = cand;
            maxPt=cand->pt();
        }
    }
    particle->setP4(vectorSum.Px(),vectorSum.Py(),vectorSum.Pz(),vectorSum.E());
    //particle->setUserRecord("vertex",pxl::Basic3Vector(highPtCand->vx(),highPtCand->vy(),highPtCand->vz()));
    
    pxl::Vertex* bestVertex = nullptr;
    double minD = 10000000.0;
    if (highPtCand)
    {
        for (pxl::Vertex* vertex: vertices)
        {
            
            double d = (fabs(vertex->getX()-highPtCand->vx())+fabs(vertex->getY()-highPtCand->vy())+fabs(vertex->getZ()-highPtCand->vz()));
            if (d<minD and d<0.0001)
            {
                bestVertex=vertex;
            }
        }
        if (bestVertex)
        {
            particle->linkMother(bestVertex);
        }
        else
        {
            pxl::Vertex* pxlVertex = pxlEventView->create<pxl::Vertex>();
            pxlVertex->setName("PV");
            pxlVertex->setXYZ(highPtCand->vx(),highPtCand->vy(),highPtCand->vz());
            pxlVertex->setUserRecord("ndof",highPtCand->vertexNdof());
            pxlVertex->setUserRecord("chi2",highPtCand->vertexChi2());
            pxlVertex->setUserRecord("dz",highPtCand->dz());
            pxlVertex->setUserRecord("dxy",highPtCand->dxy());
            particle->linkMother(pxlVertex);
            vertices.push_back(pxlVertex);
        }
    }
    
}

void PackedCandidateConverter::convert(const edm::Event* edmEvent, const edm::EventSetup* iSetup, pxl::Event* pxlEvent) const
{
    if (!_triggerResultFilter.checkPath(*edmEvent))
    {
        return;
    }
    for (unsigned index=0; index<Base::size(); ++index)
    {
        
        pxl::EventView* pxlEventView = Base::findEventView(pxlEvent,Base::getEventViewName(index));

        const edm::Handle<edm::View<pat::PackedCandidate>> collection = Base::getCollection(edmEvent,index);
        
        if (collection.product()) 
        {
            edm::Handle<edm::View<pat::Jet>> jetCollection;
            if (_jetToMatchInputTags.size()==Base::size() && _jetToMatchInputTags[index].label().length()>0)
            {
                edmEvent->getByToken(_jetToMatchToken[index],jetCollection);
            }
            
            std::vector<pxl::Vertex*> vertices;
            edm::Handle<edm::View<reco::Vertex>> vertexCollection;
            if (_vertexToMatchInputTags.size()==Base::size() && _vertexToMatchInputTags[index].label().length()>0)
            {
                edmEvent->getByToken(_vertexToMatchToken[index],vertexCollection);
                /*
                for (unsigned int ivertex = 0; ivertex<vertexCollection->size();++ivertex)
                {
                    const reco::Vertex vertex = (*vertexCollection)[ivertex];
                    pxl::Vertex* pxlVertex = pxlEventView->create<pxl::Vertex>();
                    pxlVertex->setName("PV");
                    pxlVertex->setXYZ(vertex.x(),vertex.y(),vertex.z());
                    pxlVertex->setUserRecord("pt",vertex.p4().pt());
                    pxlVertex->setUserRecord("mass",vertex.p4().mass());
                    pxlVertex->setUserRecord("ndof",vertex.ndof());
                    pxlVertex->setUserRecord("chi2",vertex.chi2());
                    pxlVertex->setUserRecord("error",pxl::Basic3Vector(vertex.xError(),vertex.yError(),vertex.zError()));
                    vertices.push_back(pxlVertex);
                }
                */
            }
            
            std::unordered_map<unsigned int,std::vector<const pat::PackedCandidate*>> chargedCand;
            std::unordered_map<unsigned int,std::vector<const pat::PackedCandidate*>> chargedCandPU;
            std::unordered_map<unsigned int,std::vector<const pat::PackedCandidate*>> photonCand;
            std::unordered_map<unsigned int,std::vector<const pat::PackedCandidate*>> neutralCand;
            
            
            for (unsigned iparticle=0; iparticle< collection->size(); ++iparticle) 
            {
                const pat::PackedCandidate& cand = (*collection)[iparticle];
                //candPtEta->Fill(cand.pt(),cand.eta());
                //candPhiEta->Fill(cand.phi(),cand.eta());
                
                unsigned int Npt = 15;
                for (unsigned int ipt=0; ipt<Npt; ++ipt)
                {
                    
                    double ptmin = TMath::Exp(TMath::Power(2.75*ipt/Npt-0.9,3)+0.2*ipt/Npt);
                    double ptmax = TMath::Exp(TMath::Power(2.75*(ipt+1)/Npt-0.9,3)+0.2*(ipt+1)/Npt);
                    
                    
                    if (cand.pt()>ptmin and cand.pt()<ptmax)
                    {
                        unsigned int Nphi = 32;
                        for (unsigned int iphi=0; iphi<Nphi; ++iphi)
                        {
                            double phimin = -3.2+6.4*iphi/Nphi;
                            double phimax = -3.2+6.4*(iphi+1)/Nphi;
                            
                            if (cand.phi()>phimin and cand.phi()<phimax)
                            {
                                unsigned int Neta = 33;
                                for (unsigned int ieta=0; ieta<Neta; ++ieta)
                                {
                                    double etamin = -5.5+11.0*ieta/Nphi;
                                    double etamax = -5.5+11.0*(ieta+1)/Nphi;

                                    if (cand.eta()>etamin and cand.eta()<etamax)
                                    {
                                        unsigned int key=10000*ipt+100*iphi+ieta;
                                        if (cand.fromPV()>=2 and abs(cand.charge())>0)
                                        {
                                            chargedCand[key].push_back(&cand);
                                        }
                                        else if (cand.fromPV()<=1 && abs(cand.charge()>0))
                                        {
                                            chargedCandPU[key].push_back(&cand);
                                        }
                                        else if (cand.pdgId()==22)
                                        {
                                            photonCand[key].push_back(&cand);
                                        }
                                        else if (cand.pdgId()!=22 and abs(cand.charge())==0)
                                        {
                                            neutralCand[key].push_back(&cand);
                                        }
                                        break;
                                    }
                                }
                            }
                        }
                    }
                }
            }
            
            for (auto it: chargedCand)
            {
                unsigned int key = it.first;
                std::vector<const pat::PackedCandidate*>& cands = it.second;
                pxl::Particle* pxlParticle = pxlEventView->create<pxl::Particle>();
                pxlParticle->setName("CC");
                pxlParticle->setPdgNumber(key);
                pxlParticle->setUserRecord("n",cands.size());
                addObservables(pxlParticle,pxlEventView,vertices,cands);
            }
            
            for (auto it: chargedCandPU)
            {
                unsigned int key = it.first;
                std::vector<const pat::PackedCandidate*>& cands = it.second;
                pxl::Particle* pxlParticle = pxlEventView->create<pxl::Particle>();
                pxlParticle->setName("CPU");
                pxlParticle->setPdgNumber(key);
                pxlParticle->setUserRecord("n",cands.size());
                addObservables(pxlParticle,pxlEventView,vertices,cands);
            }
            
            for (auto it: photonCand)
            {
                unsigned int key = it.first;
                std::vector<const pat::PackedCandidate*>& cands = it.second;
                pxl::Particle* pxlParticle = pxlEventView->create<pxl::Particle>();
                pxlParticle->setName("PC");
                pxlParticle->setPdgNumber(key);
                pxlParticle->setUserRecord("n",cands.size());
                addObservables(pxlParticle,pxlEventView,vertices,cands);
            }
            
            for (auto it: neutralCand)
            {
                unsigned int key = it.first;
                std::vector<const pat::PackedCandidate*>& cands = it.second;
                pxl::Particle* pxlParticle = pxlEventView->create<pxl::Particle>();
                pxlParticle->setName("NC");
                pxlParticle->setPdgNumber(key);
                pxlParticle->setUserRecord("n",cands.size());
                addObservables(pxlParticle,pxlEventView,vertices,cands);
            }
        }
    }
}
     
PackedCandidateConverter::~PackedCandidateConverter()
{
}

}

DEFINE_EDM_PLUGIN(edm2pxlio::ConverterFactory, edm2pxlio::PackedCandidateConverter, "PackedCandidateConverter");




