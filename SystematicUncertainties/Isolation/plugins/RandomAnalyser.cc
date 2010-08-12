
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

#include <DataFormats/Math/interface/Vector3D.h>
#include <DataFormats/Math/interface/Point3D.h>
#include <vector>
#include <cmath>

class RandomAnalyser : public edm::EDAnalyzer 
{
  public : 
    explicit RandomAnalyser(const edm::ParameterSet&);
    ~RandomAnalyser();
	
    typedef math::RhoEtaPhiVector Vector;
    typedef math::XYZPoint Point;
    
  private :
    virtual void beginJob();
    virtual void analyze(const edm::Event&, const edm::EventSetup&);
    virtual void endJob();
    
  protected :
    // parameter set
    const edm::ParameterSet& _ps;
    
    // tuple management
    TreeManager _tm;
    std::string _outputFile;
	
	// cone generator
    ConeGenerator _cGen;
    std::pair<Vector, Point> genPair;
	
	// variables
    int event;
    int run;
    double xVtx;
    double yVtx;
    double zVtx;
    double coneEta;
    double conePhi;
    double ecalIso;
    double trackIso;
    double hcalIso;
    double sumIso;
	
};

RandomAnalyser::RandomAnalyser(const edm::ParameterSet& ps) : _ps(ps)
{
  // Input parameters
  _outputFile = _ps.getParameter<string>("outputFile");
  // trackCollection
  // recHitCollection
  // hcalTowerCollection
    
  // Ntuple Manager
  _tm.open( _outputFile.c_str(), TreeManager::kWrite );
  _tm.addTree("RandomConeIso");
  
  // cone generator
  _cGen.init() ;
  
}

RandomAnalyser::~RandomAnalyser()
{
}

void 
    RandomAnalyser::beginJob()
{
}

void 
    RandomAnalyser::analyze(const edm::Event& iEvent, const edm::EventSetup& iSetup)
{
  // generate a cone
  genPair = _cGen.randomCone();
  genDir = genPair.first();
  genVtx = genPair.second();
 
  // compute the three isolations in that cone
  
  
  event     = iEvent.id().run() ;
  run       = iEvent.id().event() ;
  xVtx      = genVtx.x();
  yVtx      = genVtx.y();
  zVtx      = genVtx.z();
  coneEta   = genDir.eta();
  conePhi   = genDir.phi();
  ecalIso   = 1.;
  trackIso  = 1.;
  hcalIso   = 1.;
  sumIso    = ecalIso + trackIso + hcalIso;
  
  // save the data in a tree
  _tm.branchio<Int_t>( "RandomConeIso", "event",  event);
  _tm.branchio<Int_t>( "RandomConeIso", "run",  run);
  
  _tm.branchio<Double_t>( "RandomConeIso", "xVtx",  xVtx);
  _tm.branchio<Double_t>( "RandomConeIso", "yVtx",  yVtx);
  _tm.branchio<Double_t>( "RandomConeIso", "zVtx",  zVtx);
  
  _tm.branchio<Double_t>( "RandomConeIso", "coneEta",  coneEta);
  _tm.branchio<Double_t>( "RandomConeIso", "conePhi",  conePhi);
  
  _tm.branchio<Double_t>( "RandomConeIso", "ecalIso",  ecalIso);
  _tm.branchio<Double_t>( "RandomConeIso", "trackIso", trackIso);
  _tm.branchio<Double_t>( "RandomConeIso", "hcalIso",  hcalIso);
  _tm.branchio<Double_t>( "RandomConeIso", "sumIso",   sumIso);
  
  _tm.sync( "RandomConeIso" );
}

void 
    RandomAnalyser::endJob()
{
}


DEFINE_FWK_MODULE( RandomAnalyser );




/*
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
    virtual void beginJob();
    virtual void analyze(const edm::Event&, const edm::EventSetup&);
    virtual void endJob();
    
  protected :
    // parameter set
    const edm::ParameterSet& _ps;
    
    // tuple management
    TreeManager _tm;
    std::string _outputFile;
};

RandomAnalyser::RandomAnalyser(const edm::ParameterSet& ps) : _ps(ps)
{
  // Input parameters
  _outputFile = _ps.getParameter<string>("outputFile");
  
  // Ntuple Manager
  _tm.open( _outputFile.c_str(), TreeManager::kWrite );
  _tm.addTree("Photons");
}

RandomAnalyser::~RandomAnalyser()
{
}

void 
RandomAnalyser::beginJob()
{
}

void 
    RandomAnalyser::analyze(const edm::Event&, const edm::EventSetup&)
{
  
  _tm.branchio<Int_t>( "Photons", "test",    1);
  _tm.sync( "Photons" );
}

void 
RandomAnalyser::endJob()
{
}


DEFINE_FWK_MODULE( RandomAnalyser );

*/
