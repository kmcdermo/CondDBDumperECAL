// -*- C++ -*-
//
// Package:    TestConeGenerator
// Class:      TestConeGenerator
// 
/**\class TestConeGenerator TestConeGenerator.cc SystematicUncertainties/Isolation/test/TestConeGenerator.cc

 Description: <one line class summary>

 Implementation:
     <Notes on implementation>
*/
//
// Original Author:  Federico Ferri
//         Created:  Fri Oct 12 19:45:33 CEST 2007
// $Id$
//
//


// system include files
#include <memory>

// user include files
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/EDAnalyzer.h"

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"

#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include <SystematicUncertainties/Isolation/interface/ConeGenerator.h>

#include <TH1F.h>
#include <TFile.h>

//
// class decleration
//

class TestConeGenerator : public edm::EDAnalyzer {
   public:
      explicit TestConeGenerator(const edm::ParameterSet&);
      ~TestConeGenerator();


   private:
      virtual void beginJob(const edm::EventSetup&) ;
      virtual void analyze(const edm::Event&, const edm::EventSetup&);
      virtual void endJob() ;
      ConeGenerator coneGen_;
      TH1F *h_x, *h_y, *h_z;
      TH1F *h_eta, *h_phi;
      TFile *fout;

      // ----------member data ---------------------------
};

//
// constants, enums and typedefs
//

//
// static data member definitions
//

#include <DataFormats/Common/interface/View.h>
#include <DataFormats/EgammaCandidates/interface/PixelMatchGsfElectron.h>
#include <DataFormats/TrackReco/interface/Track.h>
#include <SystematicUncertainties/Isolation/interface/IsoCone.h>
#include <CLHEP/Units/SystemOfUnits.h>

//
// constructors and destructor
//
TestConeGenerator::TestConeGenerator(const edm::ParameterSet& iConfig)
{
        //now do what ever initialization is needed
        double mX = iConfig.getParameter<double>("MeanX")*cm;
        double mY = iConfig.getParameter<double>("MeanY")*cm;
        double mZ = iConfig.getParameter<double>("MeanZ")*cm;
        double sX = iConfig.getParameter<double>("SigmaX")*cm;
        double sY = iConfig.getParameter<double>("SigmaY")*cm;
        double sZ = iConfig.getParameter<double>("SigmaZ")*cm;
        double mE = iConfig.getParameter<double>("MinEta");
        double ME = iConfig.getParameter<double>("MaxEta");
        double mP = iConfig.getParameter<double>("MinPhi");
        double MP = iConfig.getParameter<double>("MaxPhi");
        coneGen_.setVertexSigma(sX,sY,sZ);
        coneGen_.setVertexMean(mX,mY,mZ);
        coneGen_.setEtaRange(mE,ME);
        coneGen_.setPhiRange(mP,MP);
        coneGen_.init();
}


TestConeGenerator::~TestConeGenerator()
{
      delete h_x;
      delete h_y;
      delete h_z;
      delete h_eta;
      delete h_phi;
}


//
// member functions
//

// ------------ method called to for each event  ------------
void
TestConeGenerator::analyze(const edm::Event& iEvent, const edm::EventSetup& iSetup)
{
        Iso::Cone< reco::Track > iso;
        std::pair<ConeGenerator::Vector,ConeGenerator::Point> cone = coneGen_.randomCone();
        h_x->Fill( cone.second.x() );
        h_y->Fill( cone.second.y() );
        h_z->Fill( cone.second.z() );
        h_eta->Fill( cone.first.eta() );
        h_phi->Fill( cone.first.phi() );
}


// ------------ method called once each job just before starting event loop  ------------
void 
TestConeGenerator::beginJob(const edm::EventSetup&)
{
        fout = new TFile("test_cone_generator.root","RECREATE");
        h_x = new TH1F("x","x",1000,-0.5,0.5);
        h_y = new TH1F("y","y",1000,-0.5,0.5);
        h_z = new TH1F("z","z",1000,-250.,250.);
        h_eta = new TH1F("eta","eta",100,-2.7,2.7);
        h_phi = new TH1F("phi","phi",100,-0.5,3.6415);
}

// ------------ method called once each job just after ending the event loop  ------------
void 
TestConeGenerator::endJob() {
        fout->Write();
}

//define this as a plug-in
DEFINE_FWK_MODULE(TestConeGenerator);
