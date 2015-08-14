#include "EDM2PXLIO/Pat/plugins/JetConverter.h"
#include "EDM2PXLIO/Core/interface/ConverterFactory.h"
#include "EDM2PXLIO/Core/interface/ProviderFactory.h"

#include "PhysicsTools/CandUtils/interface/EventShapeVariables.h"

#include <functional>

namespace edm2pxlio
{

const std::function<bool(const reco::Candidate*)> JetChargeCalculator::SELECT_CHARGED=[](const reco::Candidate* candidate)
{
    return candidate->charge()!=0;
};

const std::function<bool(const reco::Candidate*)> JetChargeCalculator::SELECT_VERTEX=[](const reco::Candidate* candidate)
{
    const pat::PackedCandidate* pkCandidate = dynamic_cast<const pat::PackedCandidate*>(candidate);
    return (fabs(pkCandidate->dxy())<0.001) and (fabs(pkCandidate->dz())<0.001);
};

const std::function<bool(const reco::Candidate*)> JetChargeCalculator::SELECT_NOTVERTEX=[](const reco::Candidate* candidate)
{
    const pat::PackedCandidate* pkCandidate = dynamic_cast<const pat::PackedCandidate*>(candidate);
    return (fabs(pkCandidate->dxy())>0.001) or (fabs(pkCandidate->dz())>0.001);
};

const std::function<bool(const reco::Candidate*)> JetChargeCalculator::SELECT_ALL=[](const reco::Candidate*)
{
    return true;
};

double JetChargeCalculator::calculate(const pat::Jet& jet, MEASURE measure, double exp, std::function<bool(const reco::Candidate*)> selector)
{
    double nominator = 0.0;
    double denominator = 0.0;
    
    
    for (unsigned int idaughter = 0; idaughter < jet.numberOfDaughters(); ++idaughter)
    {
        const reco::Candidate* daughter = jet.daughter(idaughter);
        if (not selector(daughter))
        {
            continue;
        }
        double weight = 0.0;
        
        switch (measure)
        {
            case DR:
            {
                const double dY = daughter->rapidity()-jet.rapidity();
                const double dPhi = reco::deltaPhi(daughter->phi(),jet.phi());
                const double dR = std::sqrt(dY*dY+dPhi*dPhi);   
                weight = std::pow(dR,exp);
                break;
            }
            case PT:
            {
                const double pt = daughter->pt();
                weight = std::pow(pt,exp);
                break;
            }
            case E:
            {
                const double energy = daughter->energy();
                weight = std::pow(energy,exp);
                break;
            }
            case ONE:
            {
                weight =1.0;
                break;
            }
            case MASSDROP:
            {
                math::XYZTLorentzVector jetDrop = jet.p4()-daughter->p4();
                const double massDrop = std::max(0.0,jet.mass()-jetDrop.mass());
                weight = std::pow(massDrop,exp);
                break;
            }
        }
        nominator+=daughter->charge()*weight;
        denominator+=weight;
    }
    return nominator/denominator;
}

JetConverter::JetConverter(const std::string& name, const edm::ParameterSet& globalConfig, edm::ConsumesCollector& consumesCollector):
    Base(name, globalConfig, consumesCollector)
{
}

void JetConverter::calculateJetShapes(const pat::Jet& patObject, pxl::Particle* pxlParticle) const
{
    //pull: http://arxiv.org/abs/1001.5027
    //rms: https://cds.cern.ch/record/1581583
    //jet charge: http://arxiv.org/abs/1209.2421
    
    double pullY = 0;
    double pullPhi = 0;
    double weightedPtSum2 = 0;
    double weightedSum2 = 0;
    
   
    
    //use here dY/dPhi space
    std::vector< math::XYZVector > eventShapeVectorYPhi(patObject.numberOfDaughters());
    
    for (unsigned int idaughter = 0; idaughter < patObject.numberOfDaughters(); ++idaughter)
    {
        const reco::Candidate* daughter = patObject.daughter(idaughter);
        const double dY = daughter->rapidity()-patObject.rapidity();
        const double dPhi = reco::deltaPhi(daughter->phi(),patObject.phi());
        const double dR = std::sqrt(dY*dY+dPhi*dPhi);
        pullY+=daughter->pt()*dR*dY;
        pullPhi+=daughter->pt()*dR*dPhi;
        
        weightedPtSum2+=dR*dR*daughter->pt()*daughter->pt();
        weightedSum2+=daughter->pt()*daughter->pt();
        
        eventShapeVectorYPhi[idaughter].SetXYZ(dY,dPhi,0);
    }
    
    pullY/=patObject.pt();
    pullPhi/=patObject.pt();
    
    pxlParticle->setUserRecord("pullY",PRECISION(pullY));
    pxlParticle->setUserRecord("pullPhi",PRECISION(pullPhi));
    pxlParticle->setUserRecord("rms",PRECISION(weightedPtSum2/weightedSum2));
    
    EventShapeVariables eventShapeYPhi(eventShapeVectorYPhi);
    pxlParticle->setUserRecord("circularityYPhi",PRECISION(eventShapeYPhi.circularity()));
    

    std::vector<unsigned int> exps{{25,50,75,100}};
    for (unsigned int exp: exps)
    {
        pxlParticle->setUserRecord("charge_dr_all_"+std::to_string(exp),PRECISION(JetChargeCalculator::calculate(patObject,JetChargeCalculator::DR,exp*0.01,JetChargeCalculator::SELECT_ALL)));
        pxlParticle->setUserRecord("charge_pt_all_"+std::to_string(exp),PRECISION(JetChargeCalculator::calculate(patObject,JetChargeCalculator::PT,exp*0.01,JetChargeCalculator::SELECT_ALL)));
        pxlParticle->setUserRecord("charge_e_all_"+std::to_string(exp),PRECISION(JetChargeCalculator::calculate(patObject,JetChargeCalculator::E,exp*0.01,JetChargeCalculator::SELECT_ALL)));
        pxlParticle->setUserRecord("charge_md_all_"+std::to_string(exp),PRECISION(JetChargeCalculator::calculate(patObject,JetChargeCalculator::MASSDROP,exp*0.01,JetChargeCalculator::SELECT_ALL)));
    
        pxlParticle->setUserRecord("charge_dr_charged_"+std::to_string(exp),PRECISION(JetChargeCalculator::calculate(patObject,JetChargeCalculator::DR,exp*0.01,JetChargeCalculator::SELECT_CHARGED)));
        pxlParticle->setUserRecord("charge_pt_charged_"+std::to_string(exp),PRECISION(JetChargeCalculator::calculate(patObject,JetChargeCalculator::PT,exp*0.01,JetChargeCalculator::SELECT_CHARGED)));
        pxlParticle->setUserRecord("charge_e_charged_"+std::to_string(exp),PRECISION(JetChargeCalculator::calculate(patObject,JetChargeCalculator::E,exp*0.01,JetChargeCalculator::SELECT_CHARGED)));
        pxlParticle->setUserRecord("charge_md_charged_"+std::to_string(exp),PRECISION(JetChargeCalculator::calculate(patObject,JetChargeCalculator::MASSDROP,exp*0.01,JetChargeCalculator::SELECT_CHARGED)));

        pxlParticle->setUserRecord("charge_dr_vertex_"+std::to_string(exp),PRECISION(JetChargeCalculator::calculate(patObject,JetChargeCalculator::DR,exp*0.01,JetChargeCalculator::SELECT_VERTEX)));
        pxlParticle->setUserRecord("charge_pt_vertex_"+std::to_string(exp),PRECISION(JetChargeCalculator::calculate(patObject,JetChargeCalculator::PT,exp*0.01,JetChargeCalculator::SELECT_VERTEX)));
        pxlParticle->setUserRecord("charge_e_vertex_"+std::to_string(exp),PRECISION(JetChargeCalculator::calculate(patObject,JetChargeCalculator::E,exp*0.01,JetChargeCalculator::SELECT_VERTEX)));
        pxlParticle->setUserRecord("charge_md_vertex_"+std::to_string(exp),PRECISION(JetChargeCalculator::calculate(patObject,JetChargeCalculator::MASSDROP,exp*0.01,JetChargeCalculator::SELECT_VERTEX)));

        pxlParticle->setUserRecord("charge_dr_nonvertex_"+std::to_string(exp),PRECISION(JetChargeCalculator::calculate(patObject,JetChargeCalculator::DR,exp*0.01,JetChargeCalculator::SELECT_NOTVERTEX)));
        pxlParticle->setUserRecord("charge_pt_nonvertex_"+std::to_string(exp),PRECISION(JetChargeCalculator::calculate(patObject,JetChargeCalculator::PT,exp*0.01,JetChargeCalculator::SELECT_NOTVERTEX)));
        pxlParticle->setUserRecord("charge_e_nonvertex_"+std::to_string(exp),PRECISION(JetChargeCalculator::calculate(patObject,JetChargeCalculator::E,exp*0.01,JetChargeCalculator::SELECT_NOTVERTEX)));
        pxlParticle->setUserRecord("charge_md_nonvertex_"+std::to_string(exp),PRECISION(JetChargeCalculator::calculate(patObject,JetChargeCalculator::MASSDROP,exp*0.01,JetChargeCalculator::SELECT_NOTVERTEX)));
    }
}


void JetConverter::convertObject(const pat::Jet& patObject, pxl::Particle* pxlParticle) const
{
    Base::convertObject(patObject, pxlParticle);
    pxlParticle->setP4(patObject.px(),patObject.py(),patObject.pz(),patObject.energy());
    
    const std::vector<std::pair<std::string, float>>& bDiscriminators = patObject.getPairDiscri();
    for (unsigned int i = 0; i<bDiscriminators.size(); ++i)
    {
        pxlParticle->setUserRecord(bDiscriminators[i].first,PRECISION(bDiscriminators[i].second));
    }

    pxlParticle->setUserRecord("nConstituents",patObject.nConstituents());
    pxlParticle->setUserRecord("n60",patObject.n60());
    pxlParticle->setUserRecord("n90",patObject.n90());
    
    pxlParticle->setUserRecord("chargedMultiplicity",patObject.chargedMultiplicity());
    
    pxlParticle->setUserRecord("chargedHadronMultiplicity",patObject.chargedHadronMultiplicity ());
    if (patObject.chargedHadronMultiplicity ()>0)
    {
        pxlParticle->setUserRecord("chargedHadronEnergy",PRECISION(patObject.chargedHadronEnergy()));
        pxlParticle->setUserRecord("chargedHadronEnergyFraction",PRECISION(patObject.chargedHadronEnergyFraction()));
    }
    
    pxlParticle->setUserRecord("neutralHadronMultiplicity",patObject.neutralHadronMultiplicity());
    if (patObject.neutralHadronMultiplicity()>0)
    {
        pxlParticle->setUserRecord("neutralHadronEnergy",PRECISION(patObject.neutralHadronEnergy()));
        pxlParticle->setUserRecord("neutralHadronEnergyFraction",PRECISION(patObject.neutralHadronEnergyFraction()));
    } 
    
    pxlParticle->setUserRecord("neutralMultiplicity",patObject.neutralMultiplicity());
    if (patObject.neutralMultiplicity()>0)
    {
        pxlParticle->setUserRecord("neutralEmEnergy",PRECISION(patObject.neutralEmEnergy()));
        pxlParticle->setUserRecord("neutralEmEnergyFraction",PRECISION(patObject.neutralEmEnergyFraction()));
    }
    
    pxlParticle->setUserRecord("photonMultiplicity",patObject.photonMultiplicity());
    if (patObject.photonMultiplicity()>0)
    {
        pxlParticle->setUserRecord("photonEnergy",PRECISION(patObject.photonEnergy()));
        pxlParticle->setUserRecord("photonEnergyFraction",PRECISION(patObject.photonEnergyFraction()));
    }
    
    pxlParticle->setUserRecord("HFHadronMultiplicity",patObject.HFHadronMultiplicity());
    if (patObject.HFHadronMultiplicity()>0)
    {
        pxlParticle->setUserRecord("HFHadronEnergy",PRECISION(patObject.HFHadronEnergy()));
        pxlParticle->setUserRecord("HFHadronEnergyFraction",PRECISION(patObject.HFHadronEnergyFraction()));
    }
    
    pxlParticle->setUserRecord("muonMultiplicity",patObject.muonMultiplicity());
    if (patObject.muonMultiplicity()>0)
    {
        //same as 'chargedMuEnergy'
        pxlParticle->setUserRecord("muonEnergy",PRECISION(patObject.muonEnergy()));
        pxlParticle->setUserRecord("muonEnergyFraction",PRECISION(patObject.muonEnergyFraction()));
    }
    
    pxlParticle->setUserRecord("electronMultiplicity",patObject.electronMultiplicity());
    if (patObject.electronMultiplicity()>0)
    {
        //same as 'chargedEmEnergy'
        pxlParticle->setUserRecord("electronEnergy",PRECISION(patObject.electronEnergy()));
        pxlParticle->setUserRecord("electronEnergyFraction",PRECISION(patObject.electronEnergyFraction()));
    }
    
    pxlParticle->setUserRecord("HFEMMultiplicity",patObject.HFEMMultiplicity());
    if (patObject.HFEMMultiplicity()>0)
    {
        pxlParticle->setUserRecord("HFEMEnergy",PRECISION(patObject.HFEMEnergy()));
        pxlParticle->setUserRecord("HFEMEnergyFraction",PRECISION(patObject.HFEMEnergyFraction()));
    }
    
    const reco::JetFlavourInfo& flavorInfo = patObject.jetFlavourInfo();
    pxlParticle->setUserRecord("partonFlavour",flavorInfo.getPartonFlavour());
    
    pxlParticle->setUserRecord("hadronFlavour",flavorInfo.getHadronFlavour());
    
    pxlParticle->setUserRecord("jetArea",PRECISION(patObject.jetArea()));
    pxlParticle->setUserRecord("jetCharge",PRECISION(patObject.jetCharge()));
    
    const reco::Candidate::LorentzVector& rawP4 = patObject.correctedP4("Uncorrected");
    pxlParticle->setUserRecord("rawP4",pxl::LorentzVector(rawP4.px(),rawP4.py(),rawP4.pz(),rawP4.energy()));
    
    for (unsigned int iname = 0; iname < patObject.userFloatNames ().size(); ++ iname)
    {
        float value = patObject.userFloat(patObject.userFloatNames()[iname]);
        if (fabs(value)>0.0)
        {
            pxlParticle->setUserRecord("user_"+patObject.userFloatNames()[iname],PRECISION(value));
        }
    }
    
    if (patObject.genParton())
    {
        pxlParticle->setUserRecord("partonFlavour",patObject.partonFlavour());
        
    }
    if (patObject.genJet())
    {
        const pat::Jet::LorentzVector& vec = patObject.genJet()->p4();
        pxlParticle->setUserRecord("genJet",pxl::LorentzVector(vec.px(),vec.py(),vec.pz(),vec.energy()));
    }
    
    calculateJetShapes(patObject,pxlParticle);
}

JetConverter::~JetConverter()
{
}

}

DEFINE_EDM_PLUGIN(edm2pxlio::ConverterFactory, edm2pxlio::JetConverter, "JetConverter");
