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
// $Id: EcalValidation.cc,v 1.5 2008/06/10 13:45:21 ferriff Exp $
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
                edm::InputTag recHitCollection_EB_;
                edm::InputTag recHitCollection_EE_;
                edm::InputTag basicClusterCollection_EB_;
                edm::InputTag basicClusterCollection_EE_;
                edm::InputTag superClusterCollection_EB_;
                edm::InputTag superClusterCollection_EE_;
                // RecHits ----------------------------------------------
                // ... barrel 
                TH1D *h_recHitsEB_size; 
                TH1D *h_recHitsEB_energy;
                // ... endcap
                TH1D *h_recHitsEE_size;
                TH1D *h_recHitsEE_energy;
                // Basic Clusters ----------------------------------------------
                // ... barrel
                TH1D *h_basicClusters_EB_size;
                TH1D *h_basicClusters_EB_nXtals;
                TH1D *h_basicClusters_EB_energy;
                TH1D *h_basicClusters_EB_eta;
                TH1D *h_basicClusters_EB_phi;
                // ... endcap
                TH1D *h_basicClusters_EE_size;
                TH1D *h_basicClusters_EE_nXtals;
                TH1D *h_basicClusters_EE_energy;
                TH1D *h_basicClusters_EE_eta;
                TH1D *h_basicClusters_EE_phi;
                // Super Clusters ----------------------------------------------
                // ... barrel
                TH1D *h_superClusters_EB_size;
                TH1D *h_superClusters_EB_nXtals;
                TH1D *h_superClusters_EB_energy;
                TH1D *h_superClusters_EB_eta;
                TH1D *h_superClusters_EB_phi;
                // ... endcap
                TH1D *h_superClusters_EE_size;
                TH1D *h_superClusters_EE_nXtals;
                TH1D *h_superClusters_EE_energy;
                TH1D *h_superClusters_EE_eta;
                TH1D *h_superClusters_EE_phi;
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
        recHitCollection_EB_ = ps.getParameter<edm::InputTag>("recHitCollection_EB");
        recHitCollection_EE_ = ps.getParameter<edm::InputTag>("recHitCollection_EE");
        basicClusterCollection_EB_ = ps.getParameter<edm::InputTag>("basicClusterCollection_EB");
        basicClusterCollection_EE_ = ps.getParameter<edm::InputTag>("basicClusterCollection_EE");
        superClusterCollection_EB_ = ps.getParameter<edm::InputTag>("superClusterCollection_EB");
        superClusterCollection_EE_ = ps.getParameter<edm::InputTag>("superClusterCollection_EE");
        edm::Service<TFileService> fs;
        h_recHitsEB_size = fs->make<TH1D>( "h_recHitsEB_size", "h_recHitsEB_size", 1000, 0, 10000 );
        h_recHitsEB_energy = fs->make<TH1D>("h_recHitsEB_energy","h_recHitsEB_energy",2000,-50,350);
        // ... endcap
        h_recHitsEE_size = fs->make<TH1D>("h_recHitsEE_size","h_recHitsEE_size",1000,0,10000);
        h_recHitsEE_energy = fs->make<TH1D>("h_recHitsEE_energy","h_recHitsEE_energy",2000,-50,350);
        // Basic Clusters ----------------------------------------------
        // ... barrel
        h_basicClusters_EB_size = fs->make<TH1D>("h_basicClusters_EB_size","h_basicClusters_EB_size",200,0.,200.);
        h_basicClusters_EB_nXtals = fs->make<TH1D>("h_basicClusters_EB_nXtals","h_basicClusters_EB_nXtals",100,0.,100.);
        h_basicClusters_EB_energy = fs->make<TH1D>("h_basicClusters_EB_energy","h_basicClusters_EB_energy",2000,0.,400.);
        h_basicClusters_EB_eta = fs->make<TH1D>("h_basicClusters_EB_eta","h_basicClusters_EB_eta",250,-2.7,2.7);
        h_basicClusters_EB_phi = fs->make<TH1D>("h_basicClusters_EB_phi","h_basicClusters_EB_phi",250,-3.2,3.2);
        // ... endcap
        h_basicClusters_EE_size = fs->make<TH1D>("h_basicClusters_EE_size","h_basicClusters_EE_size",200,0.,200.);
        h_basicClusters_EE_nXtals = fs->make<TH1D>("h_basicClusters_EE_nXtals","h_basicClusters_EE_nXtals",100,0.,100.);
        h_basicClusters_EE_energy = fs->make<TH1D>("h_basicClusters_EE_energy","h_basicClusters_EE_energy",2000,0.,400.);
        h_basicClusters_EE_eta = fs->make<TH1D>("h_basicClusters_EE_eta","h_basicClusters_EE_eta",250,-2.7,2.7);
        h_basicClusters_EE_phi = fs->make<TH1D>("h_basicClusters_EE_phi","h_basicClusters_EE_phi",250,-3.2,3.2);
        // Super Clusters ----------------------------------------------
        // ... barrel
        h_superClusters_EB_size = fs->make<TH1D>("h_superClusters_EB_size","h_superClusters_EB_size",200,0.,200.);
        h_superClusters_EB_nXtals = fs->make<TH1D>("h_superClusters_EB_nXtals","h_superClusters_EB_nXtals",100,0.,100.);
        h_superClusters_EB_energy = fs->make<TH1D>("h_superClusters_EB_energy","h_superClusters_EB_energy",2000,0.,400.);
        h_superClusters_EB_eta = fs->make<TH1D>("h_superClusters_EB_eta","h_superClusters_EB_eta",250,-2.7,2.7);
        h_superClusters_EB_phi = fs->make<TH1D>("h_superClusters_EB_phi","h_superClusters_EB_phi",250,-3.2,3.2);
        // ... endcap
        h_superClusters_EE_size = fs->make<TH1D>("h_superClusters_EE_size","h_superClusters_EE_size",200,0.,200.);
        h_superClusters_EE_nXtals = fs->make<TH1D>("h_superClusters_EE_nXtals","h_superClusters_EE_nXtals",100,0.,100.);
        h_superClusters_EE_energy = fs->make<TH1D>("h_superClusters_EE_energy","h_superClusters_EE_energy",2000,0.,400.);
        h_superClusters_EE_eta = fs->make<TH1D>("h_superClusters_EE_eta","h_superClusters_EE_eta",250,-2.7,2.7);
        h_superClusters_EE_phi = fs->make<TH1D>("h_superClusters_EE_phi","h_superClusters_EE_phi",250,-3.2,3.2);

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
        ev.getByLabel( recHitCollection_EB_, recHitsEB );
        if ( ! recHitsEB.isValid() ) {
                std::cerr << "EcalValidation::analyze --> recHitsEB not found" << std::endl; 
        }
        h_recHitsEB_size->Fill( recHitsEB->size() );
        for (unsigned int irh = 0; irh < recHitsEB->size(); ++irh) {
                h_recHitsEB_energy->Fill( (*recHitsEB)[irh].energy() );
        }

        edm::Handle<EcalRecHitCollection> recHitsEE;
        ev.getByLabel( recHitCollection_EE_, recHitsEE );
        if ( ! recHitsEE.isValid() ) {
                std::cerr << "EcalValidation::analyze --> recHitsEE not found" << std::endl; 
        }
        h_recHitsEE_size->Fill( recHitsEE->size() );
        for (unsigned int irh = 0; irh < recHitsEE->size(); ++irh) {
                h_recHitsEE_energy->Fill( (*recHitsEE)[irh].energy() );
        }

        // Basic Clusters
        // ... barrel
        edm::Handle<reco::BasicClusterCollection> basicClusters_EB_h;
        ev.getByLabel( basicClusterCollection_EB_, basicClusters_EB_h );
        if ( ! basicClusters_EB_h.isValid() ) {
                std::cerr << "EcalValidation::analyze --> basicClusters_EB_h not found" << std::endl; 
        }
        h_basicClusters_EB_size->Fill( basicClusters_EB_h->size() );
        for (unsigned int icl = 0; icl < basicClusters_EB_h->size(); ++icl) {
                h_basicClusters_EB_energy->Fill( (*basicClusters_EB_h)[icl].energy() );
                h_basicClusters_EB_nXtals->Fill( (*basicClusters_EB_h)[icl].getHitsByDetId().size() );
                h_basicClusters_EB_eta->Fill( (*basicClusters_EB_h)[icl].eta() );
                h_basicClusters_EB_phi->Fill( (*basicClusters_EB_h)[icl].phi() );
        }
        // ... endcap
        edm::Handle<reco::BasicClusterCollection> basicClusters_EE_h;
        ev.getByLabel( basicClusterCollection_EE_, basicClusters_EE_h );
        if ( ! basicClusters_EE_h.isValid() ) {
                std::cerr << "EcalValidation::analyze --> basicClusters_EE_h not found" << std::endl; 
        }
        h_basicClusters_EE_size->Fill( basicClusters_EE_h->size() );
        for (unsigned int icl = 0; icl < basicClusters_EE_h->size(); ++icl) {
                h_basicClusters_EE_energy->Fill( (*basicClusters_EE_h)[icl].energy() );
                h_basicClusters_EE_eta->Fill( (*basicClusters_EE_h)[icl].eta() );
                h_basicClusters_EE_phi->Fill( (*basicClusters_EE_h)[icl].phi() );
        }
        // Super Clusters
        // ... barrel
        edm::Handle<reco::SuperClusterCollection> superClusters_EB_h;
        ev.getByLabel( superClusterCollection_EB_, superClusters_EB_h );
        if ( ! superClusters_EB_h.isValid() ) {
                std::cerr << "EcalValidation::analyze --> superClusters_EB_h not found" << std::endl; 
        }
        h_superClusters_EB_size->Fill( superClusters_EB_h->size() );
        for (unsigned int icl = 0; icl < superClusters_EB_h->size(); ++icl) {
                h_superClusters_EB_energy->Fill( (*superClusters_EB_h)[icl].energy() );
                h_superClusters_EB_nXtals->Fill( (*superClusters_EB_h)[icl].getHitsByDetId().size() );
                h_superClusters_EB_eta->Fill( (*superClusters_EB_h)[icl].eta() );
                h_superClusters_EB_phi->Fill( (*superClusters_EB_h)[icl].phi() );
        }
        // ... endcap
        edm::Handle<reco::SuperClusterCollection> superClusters_EE_h;
        ev.getByLabel( superClusterCollection_EE_, superClusters_EE_h );
        if ( ! superClusters_EE_h.isValid() ) {
                std::cerr << "EcalValidation::analyze --> superClusters_EE_h not found" << std::endl; 
        }
        h_superClusters_EE_size->Fill( superClusters_EE_h->size() );
        for (unsigned int icl = 0; icl < superClusters_EE_h->size(); ++icl) {
                h_superClusters_EE_energy->Fill( (*superClusters_EE_h)[icl].energy() );
                h_superClusters_EE_nXtals->Fill( (*superClusters_EE_h)[icl].getHitsByDetId().size() );
                h_superClusters_EE_eta->Fill( (*superClusters_EE_h)[icl].eta() );
                h_superClusters_EE_phi->Fill( (*superClusters_EE_h)[icl].phi() );
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
