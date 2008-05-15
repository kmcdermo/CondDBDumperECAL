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
// $Id: Validation.cc,v 1.1.1.1 2008/04/09 15:56:16 ferriff Exp $
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

#include "Diphoton/HafHistogram/interface/HTupleManager.h"
#include "Diphoton/HafHistogram/interface/HTuple.h"

class Validation : public edm::EDAnalyzer {
   public:
      explicit Validation(const edm::ParameterSet&);
      ~Validation();


   private:
      virtual void beginJob(const edm::EventSetup&) ;
      virtual void analyze(const edm::Event&, const edm::EventSetup&);
      virtual void endJob() ;

      HTupleManager *tplmgr_;
      HTuple *ntu_photons_;
};



Validation::Validation(const edm::ParameterSet& ps)
{
        std::string outputFile = ps.getParameter<std::string>("outputFile");
        tplmgr_ = new HTupleManager(outputFile.c_str(), "recreate");
        ntu_photons_ = tplmgr_->Ntuple("ntu_photons");
}



Validation::~Validation()
{
        tplmgr_->Store();
}



void Validation::analyze(const edm::Event& ev, const edm::EventSetup& es)
{
        //ntu_photons_->Column("evt_id", (int)ev.id());
        ntu_photons_->Column("run", (int)ev.id().run());
        ntu_photons_->Column("event", (int)ev.id().event());

        edm::Handle< reco::PhotonCollection > pPhotons;
        ev.getByLabel( edm::InputTag("correctedPhotons"), pPhotons );

        const reco::PhotonCollection * photons = pPhotons.product();
        std::cout << "Number of photons: " << photons->size() << std::endl;
        ntu_photons_->Column("nph", (int)photons->size());
        HTValVector<float> ph_ene;
        HTValVector<float> ph_phi;
        HTValVector<float> ph_eta;
        HTValVector<float> ph_px;
        HTValVector<float> ph_py;
        HTValVector<float> ph_pz;
        for ( reco::PhotonCollection::const_iterator iph = photons->begin(); iph != photons->end(); ++iph ) {
                //std::cout << "... " << iph->energy() << " " << iph->eta() << " " << iph->phi() << std::endl;
                ph_ene.append( iph->energy() );
                ph_eta.append( iph->eta() );
                ph_phi.append( iph->phi() );
        }
        ntu_photons_->Column("energy", ph_ene, "nph");
        ntu_photons_->Column("eta", ph_ene, "nph");
        ntu_photons_->Column("phi", ph_ene, "nph");
        ntu_photons_->DumpData();

        // MC Truth macthing
        edm::Handle< edm::HepMCProduct > pMCTruth;
        ev.getByLabel( edm::InputTag("source"), pMCTruth );
        const HepMC::GenEvent * genEvent = pMCTruth->GetEvent();
        for ( HepMC::GenEvent::particle_const_iterator ipl = genEvent->particles_begin(); ipl != genEvent->particles_end(); ++ipl ) {
                if ( (*ipl)->status() == 1 && abs( (*ipl)->pdg_id() ) == 22 ) {
                        //std::cout << "    " << (*ipl)->momentum().e() << " " << (*ipl)->momentum().eta() << " " << (*ipl)->momentum().phi() << std::endl;
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
