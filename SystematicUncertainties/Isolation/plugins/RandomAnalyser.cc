
// Random Cone Analyser

#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/EDAnalyzer.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/ServiceRegistry/interface/Service.h"

#include "GhmAnalysis/GhmNtupleMaker/interface/TreeManager.h"

#include <SystematicUncertainties/Isolation/interface/IsoCone.h>
#include <SystematicUncertainties/Isolation/interface/ConeGenerator.h>

class RandomAnalyser : public edm::EDAnalyzer 
{
  public : 
    explicit RandomAnalyser(const edm::ParameterSet&);
    ~RandomAnalyser();
    
  private :
    virtual void beginJob(const edm::EventSetup&);
    virtual void analyze(const edm::Event&, const edm::EventSetup&);
    virtual void endJob();
    
};

RandomAnalyser::RandomAnalyser(const edm::ParameterSet&)
{
}

RandomAnalyser::~RandomAnalyser()
{
}

void 
    RandomAnalyser::beginJob(const edm::EventSetup&)
{
}

void 
    RandomAnalyser::analyze(const edm::Event&, const edm::EventSetup&)
{
}

void 
RandomAnalyser::endJob()
{
}


DEFINE_FWK_MODULE( RandomAnalyser );


