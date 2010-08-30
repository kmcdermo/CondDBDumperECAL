// -*- C++ -*-
//
// Package:    NtupleMaker
// Class:      NtupleMaker
// 
/**\class NtupleMaker NtupleMaker.cc Validation/NtupleMaker/src/NtupleMaker.cc

Description: <one line class summary>

Implementation:
<Notes on implementation>
*/
//
// Original Author:  Federico Ferri
//         Created:  Fri Mar 21 18:06:59 CET 2008
// $Id: NtupleMaker.cc,v 1.1 2010/03/03 13:51:15 ferriff Exp $
//
//


// system include files
#include <memory>
#include <string>

// user include files
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/EDAnalyzer.h"

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/ESHandle.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/Common/interface/TriggerNames.h"

#include "FWCore/ParameterSet/interface/ParameterSet.h"
//#include "FWCore/ServiceRegistry/interface/Service.h"
//#include "PhysicsTools/UtilAlgos/interface/TFileService.h"

#include "DataFormats/EcalRecHit/interface/EcalRecHitCollections.h"
#include "DataFormats/EcalDigi/interface/EcalDigiCollections.h"
#include "DataFormats/EcalDigi/interface/EcalMGPASample.h"
#include "DataFormats/L1GlobalTrigger/interface/L1GlobalTriggerReadoutRecord.h"
//#include "DataFormats/EgammaReco/interface/SuperCluster.h"
//#include "DataFormats/EgammaReco/interface/SuperClusterFwd.h"
//#include "DataFormats/EgammaReco/interface/BasicCluster.h"
//#include "DataFormats/EgammaReco/interface/BasicClusterFwd.h"

#include "DataFormats/Common/interface/TriggerResults.h"

#include "CondFormats/EcalObjects/interface/EcalChannelStatus.h"
#include "CondFormats/DataRecord/interface/EcalChannelStatusRcd.h"

#include "CondFormats/L1TObjects/interface/L1GtTriggerMenu.h"
#include "CondFormats/DataRecord/interface/L1GtTriggerMenuRcd.h"


#include "RecoLocalCalo/EcalRecAlgos/interface/EcalSeverityLevelAlgo.h"

#include "TBits.h"

#include "TreeManager.h"
//
// class decleration
//

class NtupleMaker : public edm::EDAnalyzer {
        public:
                explicit NtupleMaker(const edm::ParameterSet&);
                ~NtupleMaker();


        private:
                virtual void beginJob();
                virtual void analyze(const edm::Event&, const edm::EventSetup&);
                virtual void endJob() ;

                // ----------member data ---------------------------
                edm::InputTag recHitCollection_EB_;
                edm::InputTag recHitCollection_EE_;
                edm::InputTag uncalibRecHitCollection_EB_;
                edm::InputTag uncalibRecHitCollection_EE_;
                edm::InputTag digiCollection_EB_;
                edm::InputTag digiCollection_EE_;

                edm::InputTag l1InputTag_;

                TreeManager tm_;
                std::string outputFile_;

                bool first_;
                double energyThreshold_;
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
NtupleMaker::NtupleMaker(const edm::ParameterSet& ps) :
        first_(true)
{
        //now do what ever initialization is needed
        recHitCollection_EB_ = ps.getParameter<edm::InputTag>("recHitCollection_EB");
        recHitCollection_EE_ = ps.getParameter<edm::InputTag>("recHitCollection_EE");
        uncalibRecHitCollection_EB_ = ps.getParameter<edm::InputTag>("uncalibRecHitCollection_EB");
        uncalibRecHitCollection_EE_ = ps.getParameter<edm::InputTag>("uncalibRecHitCollection_EE");
        digiCollection_EB_ = ps.getParameter<edm::InputTag>("digiCollection_EB");
        digiCollection_EE_ = ps.getParameter<edm::InputTag>("digiCollection_EE");
        l1InputTag_ = ps.getParameter<edm::InputTag>("l1InputTag");

        energyThreshold_ = ps.getParameter<double>("energyThreshold");

        outputFile_ = ps.getParameter<std::string>("outputFile");

        tm_.open( outputFile_.c_str(), TreeManager::kWrite );
        tm_.addTree( "ntu" );
        tm_.addTree( "runSummary" );
}


NtupleMaker::~NtupleMaker()
{
        // do anything here that needs to be done at desctruction time
        // (e.g. close files, deallocate resources etc.)
        //tm_.save();
}


//
// member functions
//

// ------------ method called to for each event  ------------
void NtupleMaker::analyze(const edm::Event& ev, const edm::EventSetup& es)
{
        edm::ESHandle<EcalChannelStatus> chStatus;
        es.get<EcalChannelStatusRcd>().get(chStatus);

        /** UncalibratedRecHits
         */
        edm::Handle<EcalUncalibratedRecHitCollection> uncalibRecHitsEB_h;
        ev.getByLabel( uncalibRecHitCollection_EB_, uncalibRecHitsEB_h );
        const EcalUncalibratedRecHitCollection * uncalibRecHitsEB = 0;
        if ( ! uncalibRecHitsEB_h.isValid() ) {
                std::cerr << "NtupleMaker::analyze --> uncalibRecHitsEB not found" << std::endl; 
        } else {
                uncalibRecHitsEB = uncalibRecHitsEB_h.product();
        }

        edm::Handle<EcalRecHitCollection> recHitsEB_h;
        ev.getByLabel( recHitCollection_EB_, recHitsEB_h );
        const EcalRecHitCollection * recHitsEB = 0;
        if ( ! recHitsEB_h.isValid() ) {
                std::cerr << "NtupleMaker::analyze --> recHitsEB not found" << std::endl; 
        } else {
                recHitsEB = recHitsEB_h.product();
        }
        edm::Handle<EBDigiCollection> digisEB_h;
        ev.getByLabel( digiCollection_EB_, digisEB_h );
        const EBDigiCollection * digisEB = 0;
        if ( ! digisEB_h.isValid() ) {
                std::cerr << "NtupleMaker::analyze --> digisEB not found" << std::endl; 
        } else {
                digisEB = digisEB_h.product();
        }

        edm::InputTag TrigResLabel("TriggerResults","","HLT");
        edm::Handle< edm::TriggerResults > hltResults_h;
        edm::TriggerNames hltTriggerNames_;
        ev.getByLabel(TrigResLabel, hltResults_h);
        const edm::TriggerResults * hltResults = 0;
        if ( ! hltResults_h.isValid() ) {
                std::cerr << "NtupleMaker::analyze --> TriggerResults not found" << std::endl; 
        } else {
                hltResults = hltResults_h.product();
                hltTriggerNames_ = ev.triggerNames( *(hltResults_h.product()) );
        }
        unsigned npaths = hltResults->size();
        TBits * hltBits = new TBits( npaths );
        for( unsigned ipath=0; ipath<npaths; ipath++ )
        {
                std::string pathName = hltTriggerNames_.triggerName(ipath);
                bool accept = hltResults->accept(ipath);

                //const edm::HLTPathStatus & status =  hltResults->at(ipath);
                //int state = status.state();
                hltBits->SetBitNumber( ipath, accept );
        }

        // Algo and Technical L1 bits
        edm::ESHandle<L1GtTriggerMenu> menuRcd;
        es.get<L1GtTriggerMenuRcd>().get(menuRcd) ;
        const L1GtTriggerMenu * l1Menu = 0;
        if ( ! menuRcd.isValid() ) {
                std::cerr << "NtupleMaker::analyze --> L1 menu not found" << std::endl; 
        } else {
                l1Menu = menuRcd.product();
        }

        assert( l1Menu != 0 );

        edm::Handle<L1GlobalTriggerReadoutRecord> gtRecord_h;
        ev.getByLabel(l1InputTag_, gtRecord_h);

        assert( gtRecord_h.isValid() );

        DecisionWord algoWord = gtRecord_h->decisionWord();
        TechnicalTriggerWord techWord = gtRecord_h->technicalTriggerWord();

        TBits * l1TechBits = new TBits( techWord.size() );
        // Loop over the technical bits
        for (size_t ibit = 0; ibit < techWord.size(); ++ibit)
        {
                l1TechBits->SetBitNumber( ibit, techWord[ibit] );
        }

        TBits * l1AlgoBits = new TBits( algoWord.size() );
        // Loop over the algo bits
        for (size_t ibit = 0; ibit < algoWord.size(); ++ibit)
        {
                l1AlgoBits->SetBitNumber( ibit, algoWord[ibit] );
        }

        // Trigger names 
        if ( first_ ) {
                std::string hltNames;
                for( unsigned ipath=0; ipath<hltTriggerNames_.size(); ipath++ )
                {
                        std::string pathName = hltTriggerNames_.triggerName(ipath);
                        hltNames += pathName + " ";
                }
                tm_.branchio<std::string >( "runSummary", "hltTriggerNames", hltNames ); 

                std::string l1AlgoNames;
                for ( CItAlgo itAlgo = l1Menu->gtAlgorithmMap().begin(); itAlgo != l1Menu->gtAlgorithmMap().end(); ++itAlgo ) {
                        std::string pathName = (itAlgo->second).algoName();
                        l1AlgoNames += pathName + " ";
                }
                tm_.branchio<std::string >( "runSummary", "l1AlgorithmNames", l1AlgoNames ); 

                std::string l1TechNames;
                for ( CItAlgo itTech = l1Menu->gtTechnicalTriggerMap().begin(); itTech != l1Menu->gtTechnicalTriggerMap().end(); ++itTech ) {
                        std::string pathName = (itTech->second).algoName();
                        l1TechNames += pathName + " ";
                }
                tm_.branchio<std::string >( "runSummary", "l1TechnicalNames", l1TechNames ); 

                tm_.sync( "runSummary" );
                first_ = false;
        }

        Int_t orbit = (Int_t)ev.eventAuxiliary().orbitNumber();
        Int_t bx    = (Int_t)ev.eventAuxiliary().bunchCrossing();  
        Int_t run   = ev.run();
        Int_t event = ev.id().event();

        std::vector<Int_t> * damp = new std::vector<Int_t>;
        std::vector<Int_t> * dgid = new std::vector<Int_t>;
        for (unsigned int irh = 0; irh < recHitsEB->size(); ++irh) {

                // recHit
                EcalRecHit rh( (*recHitsEB)[irh] );
                EBDetId id( rh.id() );

                if ( rh.energy() < energyThreshold_ ) continue;
                
                // get corresponding uncalib
                EcalUncalibratedRecHitCollection::const_iterator urhit = uncalibRecHitsEB->find( id );
                assert( urhit != uncalibRecHitsEB->end() );
                EcalUncalibratedRecHit urh( *urhit );

                // get corresponding digi
                EBDigiCollection::const_iterator dit = digisEB->find( id );
                assert( dit != digisEB->end() );
                EBDataFrame df( *dit );

                // event information
                tm_.branchio<Int_t>( "ntu", "evt", event );
                tm_.branchio<Int_t>( "ntu", "run", run );
                tm_.branchio<Int_t>( "ntu", "orbit", orbit );    
                tm_.branchio<Int_t>( "ntu", "BX", bx );
                tm_.branchio<TBits*>( "ntu", "hltBits", &hltBits );
                tm_.branchio<TBits*>( "ntu", "l1TechBits", &l1TechBits );
                tm_.branchio<TBits*>( "ntu", "l1AlgoBits", &l1AlgoBits );

                // recHit information
                tm_.branchio<Int_t>( "ntu", "ii", id.ieta() );
                tm_.branchio<Int_t>( "ntu", "ij", id.iphi() );
                tm_.branchio<Int_t>( "ntu", "ik", 0 );
                tm_.branchio<Float_t>( "ntu", "rh_ene", rh.energy() );
                tm_.branchio<Float_t>( "ntu", "rh_time", rh.time() );
                tm_.branchio<Float_t>( "ntu", "rh_ootEne", rh.outOfTimeEnergy() );
                tm_.branchio<Float_t>( "ntu", "rh_recoFlag", rh.recoFlag() );
                tm_.branchio<Float_t>( "ntu", "rh_severity", EcalSeverityLevelAlgo::severityLevel( rh.id(), (*recHitsEB), *chStatus ));
                tm_.branchio<Float_t>( "ntu", "rh_chStatus", chStatus->find( id )->getStatusCode() );

                // uncalibrated recHit information
                tm_.branchio<Float_t>( "ntu", "urh_amp", urh.amplitude() );
                tm_.branchio<Float_t>( "ntu", "urh_phase", urh.jitter()*25. );
                tm_.branchio<Float_t>( "ntu", "urh_ped", urh.pedestal() );
                tm_.branchio<Float_t>( "ntu", "urh_recoFlag", urh.recoFlag() );

                // digi information
                for ( int is=0; is<df.size(); ++is ) {
                        EcalMGPASample s = df[is];
                        damp->push_back( s.adc() );
                        dgid->push_back( s.gainId() );
                }
                tm_.branchio< std::vector<Int_t> * >( "ntu", "dg_amp", &damp );
                tm_.branchio< std::vector<Int_t> * >( "ntu", "dg_gainId", &dgid );

                tm_.sync( "ntu" );

                damp->clear();
                dgid->clear();
        }
        delete damp;
        delete dgid;
        delete hltBits;
        delete l1AlgoBits;
        delete l1TechBits;

        /*
        edm::Handle<EcalUncalibratedRecHitCollection> uncalibRecHitsEE;
        ev.getByLabel( recHitCollection_EE_, recHitsEE );
        if ( ! recHitsEE.isValid() ) {
                std::cerr << "NtupleMaker::analyze --> recHitsEE not found" << std::endl; 
        }
        h_recHitsEE_size->Fill( recHitsEE->size() );
        for (unsigned int iurh = 0; iurh < recHitsEE->size(); ++iurh) {
                h_recHitsEE_energy->Fill( (*recHitsEE)[iurh].energy() );
        }
        */

        /** RecHits
         */
        /*
        edm::Handle<EcalRecHitCollection> recHitsEB;
        ev.getByLabel( recHitCollection_EB_, recHitsEB );
        if ( ! recHitsEB.isValid() ) {
                std::cerr << "NtupleMaker::analyze --> recHitsEB not found" << std::endl; 
        }
        h_recHitsEB_size->Fill( recHitsEB->size() );
        for (unsigned int irh = 0; irh < recHitsEB->size(); ++irh) {
                h_recHitsEB_energy->Fill( (*recHitsEB)[irh].energy() );
        }

        edm::Handle<EcalRecHitCollection> recHitsEE;
        ev.getByLabel( recHitCollection_EE_, recHitsEE );
        if ( ! recHitsEE.isValid() ) {
                std::cerr << "NtupleMaker::analyze --> recHitsEE not found" << std::endl; 
        }
        h_recHitsEE_size->Fill( recHitsEE->size() );
        for (unsigned int irh = 0; irh < recHitsEE->size(); ++irh) {
                h_recHitsEE_energy->Fill( (*recHitsEE)[irh].energy() );
        }
        */
}


// ------------ method called once each job just before starting event loop  ------------
        void 
NtupleMaker::beginJob()
{
}

// ------------ method called once each job just after ending the event loop  ------------
void 
NtupleMaker::endJob() {
}

//define this as a plug-in
DEFINE_FWK_MODULE(NtupleMaker);
