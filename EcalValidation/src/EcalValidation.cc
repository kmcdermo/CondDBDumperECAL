// -*- C++ -*-
//
// Package:    EcalValidation
// Class:      EcalValidation
// 
/**\class EcalValidation EcalValidation.cc Validation/EcalValidation/src/EcalValidation.cc

Description: <one line class summary>

Implementation:
<Notes on implementation>
*/
//
// Original Author:  Federico Ferri
//         Created:  Fri Mar 21 18:06:59 CET 2008
// $Id: EcalValidation.cc,v 1.1.1.1 2008/04/29 07:33:57 ferriff Exp $
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
#include "FWCore/ServiceRegistry/interface/Service.h"
#include "PhysicsTools/UtilAlgos/interface/TFileService.h"

#include "DataFormats/EcalRecHit/interface/EcalRecHitCollections.h"
#include "DataFormats/EgammaReco/interface/SuperCluster.h"
#include "DataFormats/EgammaReco/interface/SuperClusterFwd.h"
#include "DataFormats/EgammaReco/interface/BasicCluster.h"
#include "DataFormats/EgammaReco/interface/BasicClusterFwd.h"

#include "TH1.h"
//
// class decleration
//

class EcalValidation : public edm::EDAnalyzer {
        public:
                explicit EcalValidation(const edm::ParameterSet&);
                ~EcalValidation();


        private:
                virtual void beginJob(const edm::EventSetup&) ;
                virtual void analyze(const edm::Event&, const edm::EventSetup&);
                virtual void endJob() ;

                // ----------member data ---------------------------
                edm::InputTag ebRecHitCollection_;
                edm::InputTag eeRecHitCollection_;
                // RecHits ----------------------------------------------
                // ... barrel 
                TH1D *h_recHitsEB_size; 
                TH1D *h_recHitsEB_energy;
                // ... endcap
                TH1D *h_recHitsEE_size;
                TH1D *h_recHitsEE_energy;
                // Clusters ----------------------------------------------
                // ... hybrid
                TH1D *h_clustersHybrid_size;
                TH1D *h_clustersHybrid_xtals;
                TH1D *h_clustersHybrid_energy;
                TH1D *h_clustersHybrid_eta;
                TH1D *h_clustersHybrid_phi;
                // ... island barrel
                TH1D *h_clustersIslandEB_size;
                TH1D *h_clustersIslandEB_energy;
                TH1D *h_clustersIslandEB_eta;
                TH1D *h_clustersIslandEB_phi;
                // ... island endcap
                TH1D *h_clustersIslandEE_size;
                TH1D *h_clustersIslandEE_energy;
                TH1D *h_clustersIslandEE_eta;
                TH1D *h_clustersIslandEE_phi;
                // ... island supercluster barrel
                TH1D *h_clustersIslandSCEB_size;
                TH1D *h_clustersIslandSCEB_xtals;
                TH1D *h_clustersIslandSCEB_energy;
                TH1D *h_clustersIslandSCEB_eta;
                TH1D *h_clustersIslandSCEB_phi;
                // ... island supercluster endcap
                TH1D *h_clustersIslandSCEE_size;
                TH1D *h_clustersIslandSCEE_xtals;
                TH1D *h_clustersIslandSCEE_energy;
                TH1D *h_clustersIslandSCEE_eta;
                TH1D *h_clustersIslandSCEE_phi;
};

//
// constants, enums and typedefs
//

//
// static data member definitions
//

//
// constructors and destructor
//
EcalValidation::EcalValidation(const edm::ParameterSet& ps)
{
        //now do what ever initialization is needed
        ebRecHitCollection_ = ps.getParameter<edm::InputTag>("ebRecHitCollection");
        eeRecHitCollection_ = ps.getParameter<edm::InputTag>("eeRecHitCollection");
        edm::Service<TFileService> fs;
        h_recHitsEB_size = fs->make<TH1D>( "h_recHitsEB_size", "h_recHitsEB_size", 1000, 0, 10000 );
        h_recHitsEB_energy = fs->make<TH1D>("h_recHitsEB_energy","h_recHitsEB_energy",1000,-100,100);
        // ... endcap
        h_recHitsEE_size = fs->make<TH1D>("h_recHitsEE_size","h_recHitsEE_size",1000,0,10000);
        h_recHitsEE_energy = fs->make<TH1D>("h_recHitsEE_energy","h_recHitsEE_energy",1000,-100,100);
        // Clusters ----------------------------------------------
        // ... hybrid
        h_clustersHybrid_size = fs->make<TH1D>("h_clustersHybrid_size","h_clustersHybrid_size",200,0.,200.);
        h_clustersHybrid_xtals = fs->make<TH1D>("h_clustersHybrid_xtals","h_clustersHybrid_xtals",100,0.,100.);
        h_clustersHybrid_energy = fs->make<TH1D>("h_clustersHybrid_energy","h_clustersHybrid_energy",1000,0.,200.);
        h_clustersHybrid_eta = fs->make<TH1D>("h_clustersHybrid_eta","h_clustersHybrid_eta",250,-2.7,2.7);
        h_clustersHybrid_phi = fs->make<TH1D>("h_clustersHybrid_phi","h_clustersHybrid_phi",250,-3.2,3.2);
        // ... island barrel
        h_clustersIslandEB_size = fs->make<TH1D>("h_clustersIslandEB_size","h_clustersIslandEB_size",200,0.,200.);
        h_clustersIslandEB_energy = fs->make<TH1D>("h_clustersIslandEB_energy","h_clustersIslandEB_energy",1000,0.,200.);
        h_clustersIslandEB_eta = fs->make<TH1D>("h_clustersIslandEB_eta","h_clustersIslandEB_eta",250,-2.7,2.7);
        h_clustersIslandEB_phi = fs->make<TH1D>("h_clustersIslandEB_phi","h_clustersIslandEB_phi",250,-3.2,3.2);
        // ... island endcap
        h_clustersIslandEE_size = fs->make<TH1D>("h_clustersIslandEE_size","h_clustersIslandEE_size",200,0.,200.);
        h_clustersIslandEE_energy = fs->make<TH1D>("h_clustersIslandEE_energy","h_clustersIslandEE_energy",1000,0.,200.);
        h_clustersIslandEE_eta = fs->make<TH1D>("h_clustersIslandEE_eta","h_clustersIslandEE_eta",250,-2.7,2.7);
        h_clustersIslandEE_phi = fs->make<TH1D>("h_clustersIslandEE_phi","h_clustersIslandEE_phi",250,-3.2,3.2);
        // ... island supercluster barrel
        h_clustersIslandSCEB_size = fs->make<TH1D>("h_clustersIslandSCEB_size","h_clustersIslandSCEB_size",200,0.,200.);
        h_clustersIslandSCEB_xtals = fs->make<TH1D>("h_clustersIslandSCEB_xtals","h_clustersIslandSCEB_xtals",100,0.,100.);
        h_clustersIslandSCEB_energy = fs->make<TH1D>("h_clustersIslandSCEB_energy","h_clustersIslandSCEB_energy",1000,0.,200.);
        h_clustersIslandSCEB_eta = fs->make<TH1D>("h_clustersIslandSCEB_eta","h_clustersIslandSCEB_eta",250,-2.7,2.7);
        h_clustersIslandSCEB_phi = fs->make<TH1D>("h_clustersIslandSCEB_phi","h_clustersIslandSCEB_phi",250,-3.2,3.2);
        // ... island supercluster endcap
        h_clustersIslandSCEE_size = fs->make<TH1D>("h_clustersIslandSCEE_size","h_clustersIslandSCEE_size",200,0.,200.);
        h_clustersIslandSCEE_xtals = fs->make<TH1D>("h_clustersIslandSCEE_xtals","h_clustersIslandSCEE_xtals",100,0.,100.);
        h_clustersIslandSCEE_energy = fs->make<TH1D>("h_clustersIslandSCEE_energy","h_clustersIslandSCEE_energy",1000,0.,200.);
        h_clustersIslandSCEE_eta = fs->make<TH1D>("h_clustersIslandSCEE_eta","h_clustersIslandSCEE_eta",250,-2.7,2.7);
        h_clustersIslandSCEE_phi = fs->make<TH1D>("h_clustersIslandSCEE_phi","h_clustersIslandSCEE_phi",250,-3.2,3.2);

}


EcalValidation::~EcalValidation()
{

        // do anything here that needs to be done at desctruction time
        // (e.g. close files, deallocate resources etc.)

}


//
// member functions
//

// ------------ method called to for each event  ------------
void EcalValidation::analyze(const edm::Event& ev, const edm::EventSetup& iSetup)
{
        edm::Handle<EcalRecHitCollection> recHitsEB;
        ev.getByLabel( ebRecHitCollection_, recHitsEB );
        if ( ! recHitsEB.isValid() ) {
                std::cerr << "EcalValidation::analyze --> recHitsEB not found" << std::endl; 
        }
        h_recHitsEB_size->Fill( recHitsEB->size() );
        for (unsigned int irh = 0; irh < recHitsEB->size(); ++irh) {
                h_recHitsEB_energy->Fill( (*recHitsEB)[irh].energy() );
        }

        edm::Handle<EcalRecHitCollection> recHitsEE;
        ev.getByLabel( eeRecHitCollection_, recHitsEE );
        if ( ! recHitsEE.isValid() ) {
                std::cerr << "EcalValidation::analyze --> recHitsEE not found" << std::endl; 
        }
        h_recHitsEE_size->Fill( recHitsEE->size() );
        for (unsigned int irh = 0; irh < recHitsEE->size(); ++irh) {
                h_recHitsEE_energy->Fill( (*recHitsEE)[irh].energy() );
        }

        // Clusters / SuperClusters
        // ... hybrid
        edm::Handle<reco::SuperClusterCollection> clHybrid;
        ev.getByLabel( edm::InputTag( "hybridSuperClusters", "", "" ), clHybrid );
        if ( ! clHybrid.isValid() ) {
                std::cerr << "EcalValidation::analyze --> clHybrid not found" << std::endl; 
        }
        h_clustersHybrid_size->Fill( clHybrid->size() );
        for (unsigned int icl = 0; icl < clHybrid->size(); ++icl) {
                h_clustersHybrid_energy->Fill( (*clHybrid)[icl].energy() );
                h_clustersHybrid_xtals->Fill( (*clHybrid)[icl].getHitsByDetId().size() );
                h_clustersHybrid_eta->Fill( (*clHybrid)[icl].eta() );
                h_clustersHybrid_phi->Fill( (*clHybrid)[icl].phi() );
        }
        // ... island barrel
        edm::Handle<reco::BasicClusterCollection> clIslandEB;
        ev.getByLabel( edm::InputTag( "islandBasicClusters","islandBarrelBasicClusters",""), clIslandEB);
        if ( ! clIslandEB.isValid() ) {
                std::cerr << "EcalValidation::analyze --> clIslandEB not found" << std::endl; 
        }
        h_clustersIslandEB_size->Fill( clIslandEB->size() );
        for (unsigned int icl = 0; icl < clIslandEB->size(); ++icl) {
                h_clustersIslandEB_energy->Fill( (*clIslandEB)[icl].energy() );
                h_clustersIslandEB_eta->Fill( (*clIslandEB)[icl].eta() );
                h_clustersIslandEB_phi->Fill( (*clIslandEB)[icl].phi() );
        }
        // ... island endcap
        edm::Handle<reco::BasicClusterCollection> clIslandEE;
        ev.getByLabel( edm::InputTag( "islandBasicClusters","islandEndcapBasicClusters",""), clIslandEE );
        if ( ! clIslandEE.isValid() ) {
                std::cerr << "EcalValidation::analyze --> clIslandEE not found" << std::endl; 
        }
        h_clustersIslandEE_size->Fill( clIslandEE->size() );
        for (unsigned int icl = 0; icl < clIslandEE->size(); ++icl) {
                h_clustersIslandEE_energy->Fill( (*clIslandEE)[icl].energy() );
                h_clustersIslandEE_eta->Fill( (*clIslandEE)[icl].eta() );
                h_clustersIslandEE_phi->Fill( (*clIslandEE)[icl].phi() );
        }
        // ... island superclusters barrel
        edm::Handle<reco::SuperClusterCollection> clIslandSCEB;
        ev.getByLabel( edm::InputTag( "islandSuperClusters","islandBarrelSuperClusters",""), clIslandSCEB );
        if ( ! clIslandSCEB.isValid() ) {
                std::cerr << "EcalValidation::analyze --> clIslandSCEB not found" << std::endl; 
        }
        h_clustersIslandSCEB_size->Fill( clIslandSCEB->size() );
        for (unsigned int icl = 0; icl < clIslandSCEB->size(); ++icl) {
                h_clustersIslandSCEB_energy->Fill( (*clIslandSCEB)[icl].energy() );
                h_clustersIslandSCEB_xtals->Fill( (*clIslandSCEB)[icl].getHitsByDetId().size() );
                h_clustersIslandSCEB_eta->Fill( (*clIslandSCEB)[icl].eta() );
                h_clustersIslandSCEB_phi->Fill( (*clIslandSCEB)[icl].phi() );
        }
        // ... island superclusters endcap
        edm::Handle<reco::SuperClusterCollection> clIslandSCEE;
        ev.getByLabel( edm::InputTag( "islandSuperClusters","islandEndcapSuperClusters",""), clIslandSCEE );
        if ( ! clIslandSCEE.isValid() ) {
                std::cerr << "EcalValidation::analyze --> clIslandSCEE not found" << std::endl; 
        }
        h_clustersIslandSCEE_size->Fill( clIslandSCEE->size() );
        for (unsigned int icl = 0; icl < clIslandSCEE->size(); ++icl) {
                h_clustersIslandSCEE_energy->Fill( (*clIslandSCEE)[icl].energy() );
                h_clustersIslandSCEE_xtals->Fill( (*clIslandSCEE)[icl].getHitsByDetId().size() );
                h_clustersIslandSCEE_eta->Fill( (*clIslandSCEE)[icl].eta() );
                h_clustersIslandSCEE_phi->Fill( (*clIslandSCEE)[icl].phi() );
        }
}


// ------------ method called once each job just before starting event loop  ------------
        void 
EcalValidation::beginJob(const edm::EventSetup&)
{
}

// ------------ method called once each job just after ending the event loop  ------------
void 
EcalValidation::endJob() {
}

//define this as a plug-in
DEFINE_FWK_MODULE(EcalValidation);
