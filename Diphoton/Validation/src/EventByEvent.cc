// -*- C++ -*-
//
// Package:    EventByEvent
// Class:      EventByEvent
// 
/**\class EventByEvent EventByEvent.cc Diphoton/EventByEvent/src/EventByEvent.cc

Description: <one line class summary>

Implementation:
<Notes on implementation>
*/
//
// Original Author:  "Federico Ferri federi
//         Created:  Mon Apr  7 14:11:00 CEST 2008
// $Id: EventByEvent.cc,v 1.1 2008/05/05 15:20:10 ferriff Exp $
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

#include "DataFormats/EgammaCandidates/interface/Photon.h"
#include "DataFormats/EgammaCandidates/interface/PhotonFwd.h"

#include "SimDataFormats/HepMCProduct/interface/HepMCProduct.h" 

class HTupleManager;
class HTuple;

class EventByEvent : public edm::EDAnalyzer {
        public:
                explicit EventByEvent(const edm::ParameterSet&);
                ~EventByEvent();


        private:
                virtual void beginJob(const edm::EventSetup&) ;
                virtual void analyze(const edm::Event&, const edm::EventSetup&);
                virtual void endJob() ;

                edm::InputTag photonsFull_;
                edm::InputTag photonsFast_;

                HTupleManager *tplmgr_;
                HTuple *ntu_photons_full_;
                HTuple *ntu_photons_fast_;
                void comparePhotonCollections( const reco::PhotonCollection *photons1, const reco::PhotonCollection *photons2, const HepMC::GenEvent *genEvent, HTuple *ntu );

};


#include "Diphoton/HafHistogram/interface/HTupleManager.h"
#include "Diphoton/HafHistogram/interface/HTuple.h"
#include "PhysicsTools/Utilities/interface/deltaR.h"


EventByEvent::EventByEvent(const edm::ParameterSet & ps)
{
        photonsFull_ = ps.getParameter<edm::InputTag>( "photonCollectionFull" );
        photonsFast_ = ps.getParameter<edm::InputTag>( "photonCollectionFast" );

        std::string outputFile = ps.getParameter<std::string>("outputFile");
        tplmgr_ = new HTupleManager(outputFile.c_str(), "recreate");
        ntu_photons_full_ = tplmgr_->Ntuple("ntu_photons_full");
        ntu_photons_fast_ = tplmgr_->Ntuple("ntu_photons_fast");
}



EventByEvent::~EventByEvent()
{
        tplmgr_->Store();
}



void EventByEvent::comparePhotonCollections( const reco::PhotonCollection *photons1, const reco::PhotonCollection *photons2, const HepMC::GenEvent *genEvent, HTuple *ntu )
{
        //std::cout << "Number of photons: " << photons1->size() << std::endl;
        ntu->Column("nPhotons", (int)photons1->size());
        ntu->Column("dN", (int)(photons1->size() - photons2->size()));
        // build map of Dr (i.e. matching criterion)
        HTValVector<float> venergy;
        HTValVector<float> veta;
        HTValVector<float> vphi;
        HTValVector<float> vmatch;
        HTValVector<float> vmcMatch;
        HTValVector<float> vdEnergy;
        HTValVector<float> vdEta;
        HTValVector<float> vdPhi;
        HTValVector<float> vdMCEnergy;
        HTValVector<float> vdMCEta;
        HTValVector<float> vdMCPhi;
        for ( reco::PhotonCollection::const_iterator iph = photons1->begin(); iph != photons1->end(); ++iph ) {

                venergy.append( iph->energy() );
                veta.append( iph->eta() );
                vphi.append( iph->phi() );
                // --- match with Fast Simulation
                //std::map<float, reco::Photon> phAss;
                float match = 1000.;
                const reco::Photon * matchedPhoton = 0;
                for ( reco::PhotonCollection::const_iterator jph = photons2->begin(); jph != photons2->end(); ++jph ) {
                        //phAss[ reco::deltaR( iph, jph ) ] = &(*jph);
                        float tmpMatch = reco::deltaR( *iph, *jph );
                        if ( tmpMatch < match ) {
                                match = tmpMatch;
                                matchedPhoton = &(*jph);
                        }
                }
                if ( matchedPhoton != 0 ) {
                        vmatch.append( match );
                        vdEnergy.append( iph->energy() - matchedPhoton->energy() );
                        vdEta.append( iph->eta() - matchedPhoton->eta() );
                        vdPhi.append( iph->phi() - matchedPhoton->phi() );
                }

                // --- match with Monte Carlo
                float mcMatch = 1000.;
                HepMC::FourVector mcPhoton = 0;
                for ( HepMC::GenEvent::particle_const_iterator ipl = genEvent->particles_begin(); ipl != genEvent->particles_end(); ++ipl ) {
                        if ( (*ipl)->status() == 1 && abs( (*ipl)->pdg_id() ) == 22 ) {
                                //std::cout << "    " << (*ipl)->momentum().e() << " " << (*ipl)->momentum().eta() << " " << (*ipl)->momentum().phi() << std::endl;
                                float tmpMatch = reco::deltaR( *iph, (*ipl)->momentum() );
                                if ( tmpMatch < mcMatch ) {
                                        mcMatch = tmpMatch;
                                        mcPhoton = (*ipl)->momentum();
                                }
                        }
                }
                if ( mcPhoton != 0 ) {
                        vmcMatch.append( mcMatch );
                        vdMCEnergy.append( iph->energy() - mcPhoton.e() );
                        vdMCEta.append( iph->eta() - mcPhoton.eta() );
                        vdMCPhi.append( iph->phi() - mcPhoton.phi() );
                }
        }

        ntu->Column("energy",    venergy,    "nPhotons");
        ntu->Column("eta",       veta,       "nPhotons");
        ntu->Column("phi",       vphi,       "nPhotons");
        ntu->Column("match",     vmatch,     "nPhotons");
        ntu->Column("dEnergy",   vdEnergy,   "nPhotons");
        ntu->Column("dEta",      vdEta,      "nPhotons");
        ntu->Column("dPhi",      vdPhi,      "nPhotons");
        ntu->Column("mcMatch",   vmcMatch,   "nPhotons");
        ntu->Column("dMCEnergy", vdMCEnergy, "nPhotons");
        ntu->Column("dMCEta",    vdMCEta,    "nPhotons");
        ntu->Column("dMCPhi",    vdMCPhi,    "nPhotons");
        ntu->DumpData();
}



void EventByEvent::analyze(const edm::Event& ev, const edm::EventSetup& es)
{
        // Full Sim Collections
        edm::Handle< reco::PhotonCollection > pPhotonsFull;
        ev.getByLabel( photonsFull_, pPhotonsFull );
        const reco::PhotonCollection * photonsFull = pPhotonsFull.product();
        
        // Fast Sim Collections
        edm::Handle< reco::PhotonCollection > pPhotonsFast;
        ev.getByLabel( photonsFast_, pPhotonsFast );
        const reco::PhotonCollection * photonsFast = pPhotonsFast.product();

        // MC Truth
        edm::Handle< edm::HepMCProduct > pMCTruth;
        ev.getByLabel( edm::InputTag("source"), pMCTruth );
        const HepMC::GenEvent * genEvent = pMCTruth->GetEvent();

        comparePhotonCollections( photonsFull, photonsFast, genEvent, ntu_photons_full_ );
        comparePhotonCollections( photonsFast, photonsFull, genEvent, ntu_photons_fast_ );
}



void EventByEvent::beginJob(const edm::EventSetup&)
{
}



void EventByEvent::endJob() {
}

//define this as a plug-in
DEFINE_FWK_MODULE(EventByEvent);
