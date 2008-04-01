// -*- C++ -*-
//
// Package:    TracksForIsolation
// Class:      TracksForIsolation
// 
/**\class TracksForIsolation TracksForIsolation.cc SystematicUncertainties/TracksForIsolation/src/TracksForIsolation.cc

Description: <one line class summary>

Implementation:
<Notes on implementation>
*/
//
// Original Author:  Federico Ferri
//         Created:  Sun Oct 14 21:03:33 CEST 2007
// $Id: TracksForIsolation.cc,v 1.1.1.1 2008/03/28 14:03:25 ferriff Exp $
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

#include <DataFormats/TrackReco/interface/Track.h>
#include <DataFormats/TrackingRecHit/interface/TrackingRecHit.h>

class TracksForIsolation : public edm::EDProducer {
        public:
                explicit TracksForIsolation(const edm::ParameterSet&);
                ~TracksForIsolation();

        private:
                virtual void beginJob(const edm::EventSetup&) ;
                virtual void produce(edm::Event&, const edm::EventSetup&);
                virtual void endJob() ;
                edm::InputTag srcParticles_;
                edm::InputTag srcTracks_;
};


// -------- class implementation ---------


#include <DataFormats/Common/interface/View.h>
#include <DataFormats/EgammaCandidates/interface/PixelMatchGsfElectron.h>
#include <CLHEP/Units/SystemOfUnits.h>
#include <string>

TracksForIsolation::TracksForIsolation(const edm::ParameterSet& iConfig) :
        srcParticles_( iConfig.getParameter<edm::InputTag>( "srcParticles" ) ),
        srcTracks_( iConfig.getParameter<edm::InputTag>( "srcTracks" ) )
{
        // products
        std::string alias;
        //produces< reco::TrackCollection >( alias = "tracksForIsolation" ).setBranchAlias( alias );
        //produces< reco::TrackCollection >( alias = "tracksForIsolation" );
        produces< reco::TrackCollection >().setBranchAlias( "tracksForIsolation" );
}



TracksForIsolation::~TracksForIsolation()
{
}


#include <Math/VectorUtil.h>

void TracksForIsolation::produce(edm::Event& iEvent, const edm::EventSetup& iSetup)
{
        edm::Handle< edm::View< reco::PixelMatchGsfElectron > > particles;
        iEvent.getByLabel( srcParticles_, particles );
        //std::cout << "How many electrons? " << electrons->size() << std::endl;

        edm::Handle< edm::View< reco::Track > > tracks;
        iEvent.getByLabel( srcTracks_, tracks );
        //std::cout << "How many tracks? " << tracks->size() << std::endl;


        std::map<int, bool> to_take;
        for (size_t i = 0; i < tracks->size(); ++i) {
                reco::Track tk = (*tracks)[i];
                int cnt_shared = 0;
                for (size_t j = 0; j < particles->size(); ++j) {
                        reco::GsfTrackRef ptk = (*particles)[j].gsfTrack();
                        //for (size_t i = 0; i < tk->recHitsSize(); ++i ) {
                        //std::cout << "..  tk " <<  tk.momentum().x() << " " <<  tk.momentum().y() << " " <<  tk.momentum().z() << std::endl;
                        //std::cout << ".. ptk " << ptk->innerMomentum().x() << " " << ptk->innerMomentum().y() << " " << ptk->innerMomentum().z() << std::endl;
                        for( trackingRecHit_iterator hit = tk.recHitsBegin(); hit != tk.recHitsEnd(); ++hit ) {
                                if ( ! (*hit)->isValid() ) {
                                        continue;
                                }
                                for( trackingRecHit_iterator hjt = ptk->recHitsBegin(); hjt != ptk->recHitsEnd(); ++hjt ) {
                                        if ( ! (*hjt)->isValid() ) {
                                                continue;
                                        }
                                        //TrackingRecHitRef hit = recHit( i );
                                        //const TrackingRecHit* kt = &(**jt);
                                        if ((*hit)->sharesInput( &(**hjt), TrackingRecHit::some)) {
                                                //std::cout << " " << (*hit)->sharesInput( &(**hjt), TrackingRecHit::some) << "  ";
                                                //std::cout << (*hit)->localPosition().x() << " " << (*hjt)->localPosition().x() << std::endl;
                                                ++cnt_shared;
                                        }
                                }
                        }
                        std::cout << std::endl;
                }
                if ( cnt_shared < 1 ) {
                        to_take[ i ] = true;
                }
        }
        


        

        std::auto_ptr< reco::TrackCollection > cleaned_tracks( new reco::TrackCollection );

        ///////std::cout << "---------------- " << std::endl;
        /////std::map<int, bool> to_take;
        /////for (size_t i = 0; i < tracks->size(); ++i) {
        /////        reco::Track tk = (*tracks)[i];
        /////        //for (size_t j = i+1; j < tracks->size(); ++j) {
        /////        //        reco::Track tk2 = (*tracks)[j];
        /////        //        //std::cout << "invariant (i,j) = " << i << " " << j << " " << 2. * tk.p() * tk2.p() * ( 1. - ROOT::Math::VectorUtil::CosTheta( tk.momentum(), tk2.momentum() )) << std::endl;
        /////        //}
        /////        for (size_t j = 0; j < particles->size(); ++j) {
        /////                reco::GsfTrackRef ptk = (*particles)[j].gsfTrack();
        /////                float dr = ROOT::Math::VectorUtil::DeltaR( (*tracks)[i].momentum(), ptk->momentum() );
        /////                double imass = 2. * tk.p() * ptk->p() * ( 1. - ROOT::Math::VectorUtil::CosTheta( tk.momentum(), ptk->momentum() ));
        /////                if ( imass > 1. && dr < 2) {
        /////                        //cleaned_tracks->push_back( tk );
        /////                        to_take[ i ] = true;
        /////                }
        /////        }
        /////}


        for (std::map<int, bool>::const_iterator it = to_take.begin(); it != to_take.end(); ++it) {
                if ( it->second )cleaned_tracks->push_back( (*tracks)[it->first] );
        }
        //iEvent.put( cleaned_tracks, "tracksForIsolation" );
        iEvent.put( cleaned_tracks );
}



void TracksForIsolation::beginJob(const edm::EventSetup&)
{
}



void TracksForIsolation::endJob()
{
}



DEFINE_FWK_MODULE(TracksForIsolation);
