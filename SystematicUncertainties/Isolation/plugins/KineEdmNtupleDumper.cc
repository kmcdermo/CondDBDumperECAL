// -*- C++ -*-
//
// Package:    KineEdmNtupleDumper
// Class:      KineEdmNtupleDumper
// 
/**\class KineEdmNtupleDumper KineEdmNtupleDumper.cc SystematicUncertainties/KineEdmNtupleDumper/src/KineEdmNtupleDumper.cc

Description: <one line class summary>

Implementation:
<Notes on implementation>
*/
//
// Original Author:  Federico Ferri
//         Created:  Mon Oct 15 15:10:29 CEST 2007
// $Id$
//
//


// system include files
#include <memory>

// user include files
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/EDProducer.h"

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"

#include "FWCore/ParameterSet/interface/ParameterSet.h"


class KineEdmNtupleDumper : public edm::EDProducer {
        public:
                explicit KineEdmNtupleDumper(const edm::ParameterSet&);
                ~KineEdmNtupleDumper();

        private:
                virtual void beginJob(const edm::EventSetup&) ;
                virtual void produce(edm::Event&, const edm::EventSetup&);
                virtual void endJob() ;
                edm::InputTag srcParticles_;

};


// -------- class implementation ---------

#include <DataFormats/Common/interface/View.h>
#include <DataFormats/Candidate/interface/Candidate.h>
#include <DataFormats/Math/interface/Vector3D.h>


KineEdmNtupleDumper::KineEdmNtupleDumper(const edm::ParameterSet& iConfig) :
        srcParticles_( iConfig.getParameter<edm::InputTag>( "srcParticles" ) )
{
        // products
        std::string alias;
        produces< std::vector< math::RhoEtaPhiVector > >( alias = "particleMomenta" ).setBranchAlias( alias );
        produces< std::vector< math::XYZPoint > >( alias = "particleVertices" ).setBranchAlias( alias );
}



KineEdmNtupleDumper::~KineEdmNtupleDumper()
{
}



void KineEdmNtupleDumper::produce(edm::Event& iEvent, const edm::EventSetup& iSetup)
{
        edm::Handle< edm::View< reco::Candidate > > particles;
        iEvent.getByLabel( srcParticles_, particles );

        // skip events non ZZ->4e nor Z->ee
        if ( !( particles->size() == 4 || particles->size() == 2 ) ) return;

        std::auto_ptr< std::vector< math::RhoEtaPhiVector > > particleMomenta( new std::vector< math::RhoEtaPhiVector > );
        std::auto_ptr< std::vector< math::XYZPoint > > particleVertices( new std::vector< math::XYZPoint > );

        // real electrons
        for (size_t i = 0; i < particles->size(); ++i) {
                const reco::Candidate *c = &(*particles)[i];
                ////if (c->mother()->pdgId() != 23 ) continue;
                //std::cout << c->pdgId() << " " << c->mother()->pdgId() << " " << c->status() << " " << c->momentum() << " " << c->eta() << " " << c->phi() << std::endl;
                particleMomenta->push_back( math::RhoEtaPhiVector( c->p(), c->eta(), c->phi() ) );
                particleVertices->push_back( math::XYZPoint( c->vertex() ) );
        }
        iEvent.put( particleMomenta, "particleMomenta" );
        iEvent.put( particleVertices, "particleVertices" );
}



void KineEdmNtupleDumper::beginJob(const edm::EventSetup&)
{
}



void KineEdmNtupleDumper::endJob()
{
}

//define this as a plug-in
DEFINE_FWK_MODULE(KineEdmNtupleDumper);
