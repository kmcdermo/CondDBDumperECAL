// -*- C++ -*-
//
// Package:    Validation
// Class:      Validation
// 
/**\class Validation Validation.cc Diphoton/Validation/src/Validation.cc

 Description: <one line class summary>

 Implementation:
     <Notes on implementation>
*/
//
// Original Author:  "Federico Ferri federi
//         Created:  Mon Apr  7 14:11:00 CEST 2008
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

#include "DataFormats/EgammaCandidates/interface/Photon.h"
#include "DataFormats/EgammaCandidates/interface/PhotonFwd.h"

#include "SimDataFormats/HepMCProduct/interface/HepMCProduct.h" 


class Validation : public edm::EDAnalyzer {
   public:
      explicit Validation(const edm::ParameterSet&);
      ~Validation();


   private:
      virtual void beginJob(const edm::EventSetup&) ;
      virtual void analyze(const edm::Event&, const edm::EventSetup&);
      virtual void endJob() ;

};



Validation::Validation(const edm::ParameterSet& iConfig)
{
}



Validation::~Validation()
{
}



void Validation::analyze(const edm::Event& ev, const edm::EventSetup& es)
{
        edm::Handle< reco::PhotonCollection > pPhotons;
        ev.getByLabel( edm::InputTag("correctedPhotons"), pPhotons );

        const reco::PhotonCollection * photons = pPhotons.product();
        std::cout << "Number of photons: " << photons->size() << std::endl;
        for ( reco::PhotonCollection::const_iterator iph = photons->begin(); iph != photons->end(); ++iph ) {
                std::cout << "... " << iph->energy() << " " << iph->eta() << " " << iph->phi() << std::endl;
        }


        // MC Truth macthing
        edm::Handle< edm::HepMCProduct > pMCTruth;
        ev.getByLabel( edm::InputTag("source"), pMCTruth );
        const HepMC::GenEvent * genEvent = pMCTruth->GetEvent();
        for ( HepMC::GenEvent::particle_const_iterator ipl = genEvent->particles_begin(); ipl != genEvent->particles_end(); ++ipl ) {
                if ( (*ipl)->status() == 1 && abs( (*ipl)->pdg_id() ) == 22 ) {
                        std::cout << "    " << (*ipl)->momentum().e() << " " << (*ipl)->momentum().eta() << " " << (*ipl)->momentum().phi() << std::endl;
                }
        }
        

}



void Validation::beginJob(const edm::EventSetup&)
{
}



void Validation::endJob() {
}

//define this as a plug-in
DEFINE_FWK_MODULE(Validation);
