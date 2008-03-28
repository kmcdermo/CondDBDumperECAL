// -*- C++ -*-
//
// Package:    IsoTrackDumper
// Class:      IsoTrackDumper
// 
/**\class IsoTrackDumper IsoTrackDumper.cc SystematicUncertainties/IsoTrackDumper/src/IsoTrackDumper.cc

Description: <one line class summary>

Implementation:
<Notes on implementation>
*/
//
// Original Author:  Federico Ferri
//         Created:  Sun Oct 14 21:03:33 CEST 2007
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
#include <SystematicUncertainties/Isolation/interface/ConeGenerator.h>

#include <SystematicUncertainties/Isolation/interface/IsoCone.h>

#include <DataFormats/TrackReco/interface/Track.h>

class IsoTrackDumper : public edm::EDProducer {
        public:
                explicit IsoTrackDumper(const edm::ParameterSet&);
                ~IsoTrackDumper();

        private:
                virtual void beginJob(const edm::EventSetup&) ;
                virtual void produce(edm::Event&, const edm::EventSetup&);
                virtual void endJob() ;
                edm::InputTag srcElectrons_;
                edm::InputTag srcCleanedTracks_;
                edm::InputTag srcTracks_;
};


// -------- class implementation ---------


#include <DataFormats/Common/interface/View.h>
#include <DataFormats/EgammaCandidates/interface/PixelMatchGsfElectron.h>
#include <CLHEP/Units/SystemOfUnits.h>
#include <TBranch.h>
#include <TFile.h>
#include <TTree.h>
#include <string>

IsoTrackDumper::IsoTrackDumper(const edm::ParameterSet& iConfig) :
        srcElectrons_( iConfig.getParameter<edm::InputTag>( "srcParticles" ) ),
        srcCleanedTracks_( iConfig.getParameter<edm::InputTag>( "srcCleanedTracks" ) ),
        srcTracks_( iConfig.getParameter<edm::InputTag>( "srcTracks" ) )
{
        // products
        std::string alias;
        produces< std::vector<math::RhoEtaPhiVector> >( alias = "isoElectrons" ).setBranchAlias( alias );
        produces< std::vector<math::RhoEtaPhiVector> >( alias = "isoRandomCones" ).setBranchAlias( alias );
        produces< std::vector<math::RhoEtaPhiVector> >( alias = "isoKinematicCones" ).setBranchAlias( alias );
        produces< std::vector<math::RhoEtaPhiVector> >( alias = "isoRandomConesVertices" ).setBranchAlias( alias );
        produces< std::vector<math::RhoEtaPhiVector> >( alias = "nTracksRandomCones" ).setBranchAlias( alias );
        produces< std::vector<math::RhoEtaPhiVector> >( alias = "nTracksElectrons" ).setBranchAlias( alias );
        produces< std::vector<math::RhoEtaPhiVector> >( alias = "nTracksKinematicCones" ).setBranchAlias( alias );
        produces< std::vector<math::RhoEtaPhiVector> >( alias = "nTracksRandomConesVertices" ).setBranchAlias( alias );
        //produces< std::vector<math::RhoEtaPhiVector> >( alias = "DeltaRTracks" ).setBranchAlias( alias );
        produces< std::vector<int> >( alias = "electronClass" ).setBranchAlias( alias );
        produces< std::vector<float> >( alias = "electronEoverP" ).setBranchAlias( alias );
        produces< std::vector<float> >( alias = "electronEnergy" ).setBranchAlias( alias );
        produces< std::vector< math::RhoEtaPhiVector > >( alias = "electronPCalo" ).setBranchAlias( alias );
        produces< std::vector< math::RhoEtaPhiVector > >( alias = "electronPVertex" ).setBranchAlias( alias );


        // parameters
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
        double iradius = iConfig.getParameter<double>("innerConeRadius");
        double oradius = iConfig.getParameter<double>("outerConeRadius");
        double vertexDz = iConfig.getParameter<double>("vertexDz");
        double vertexDRt = iConfig.getParameter<double>("vertexDRt");
        double ptThreshold = iConfig.getParameter<double>("ptThreshold");
        double pThreshold = iConfig.getParameter<double>("pThreshold");
        std::string kinematicsFile = iConfig.getParameter<std::string>("kinematicsFile");

        coneGen_.setVertexSigma(sX,sY,sZ);
        coneGen_.setVertexMean(mX,mY,mZ);
        coneGen_.setEtaRange(mE,ME);
        coneGen_.setPhiRange(mP,MP);

        iso_.setValues( iradius, oradius, vertexDz, vertexDRt, ptThreshold, pThreshold, Iso::Vector(0,1,0), Iso::Point(0,0,0) );

        coneGen_.loadKineCones( kinematicsFile.c_str() );
        coneGen_.init();
}



IsoTrackDumper::~IsoTrackDumper()
{
}



void IsoTrackDumper::produce(edm::Event& iEvent, const edm::EventSetup& iSetup)
{
        edm::Handle< edm::View< reco::PixelMatchGsfElectron > > electrons;
        iEvent.getByLabel( srcElectrons_, electrons );
        //std::cout << "How many electrons? " << electrons->size() << std::endl;

        edm::Handle< edm::View< reco::Track > > cleanedTracks;
        iEvent.getByLabel( srcCleanedTracks_, cleanedTracks );
        //std::cout << "How many tracks? " << tracks->size() << std::endl;

        edm::Handle< edm::View< reco::Track > > tracks;
        iEvent.getByLabel( srcTracks_, tracks );
        //std::cout << "How many tracks? " << tracks->size() << std::endl;

        //Iso::Cone< reco::Track > iso_;
        Iso::Results res;

        std::auto_ptr< std::vector<math::RhoEtaPhiVector> > isoElectrons              ( new std::vector< math::RhoEtaPhiVector > );
        std::auto_ptr< std::vector<math::RhoEtaPhiVector> > isoRandomCones            ( new std::vector< math::RhoEtaPhiVector > );
        std::auto_ptr< std::vector<math::RhoEtaPhiVector> > isoKinematicCones         ( new std::vector< math::RhoEtaPhiVector > );
        std::auto_ptr< std::vector<math::RhoEtaPhiVector> > isoRandomConesVertices    ( new std::vector< math::RhoEtaPhiVector > );
        std::auto_ptr< std::vector<math::RhoEtaPhiVector> > nTracksElectrons          ( new std::vector< math::RhoEtaPhiVector > );
        std::auto_ptr< std::vector<math::RhoEtaPhiVector> > nTracksRandomCones        ( new std::vector< math::RhoEtaPhiVector > );
        std::auto_ptr< std::vector<math::RhoEtaPhiVector> > nTracksKinematicCones     ( new std::vector< math::RhoEtaPhiVector > );
        std::auto_ptr< std::vector<math::RhoEtaPhiVector> > nTracksRandomConesVertices( new std::vector< math::RhoEtaPhiVector > );
        std::auto_ptr< std::vector<int> > electronClass( new std::vector<int> );
        std::auto_ptr< std::vector<float> > electronEoverP( new std::vector<float> );
        std::auto_ptr< std::vector<float> > electronEnergy( new std::vector<float> );
        std::auto_ptr< std::vector< math::RhoEtaPhiVector > > electronPCalo( new std::vector< math::RhoEtaPhiVector > );
        std::auto_ptr< std::vector< math::RhoEtaPhiVector > > electronPVertex ( new std::vector< math::RhoEtaPhiVector > );

        // save event vertex from one electron
        // to be used for random cones
        // and kinematic cones
        math::XYZPoint vtx;
        // real electrons
        //std::cout << "REAL PARTICLES" << std::endl;
        reco::PixelMatchGsfElectronCollection eColl;
        for (size_t i = 0; i < electrons->size(); ++i) {
                eColl.push_back( (*electrons)[i] );
        }

        //// random cones
        //// 1 - 2 - 4
        ////std::cout << "RANDOM CONES" << std::endl;
        //for (int i = 0; i < 7; ++i) {
        //        //std::cout << "   random cone:" << std::endl;
        //        std::pair<ConeGenerator::Vector,ConeGenerator::Point> cone = coneGen_.randomCone();
        //        iso_.setDirection( Iso::Vector(cone.first.x(), cone.first.y(), cone.first.z()) );
        //        //std::cout << "---- random cone " << i << "  d = " << Iso::Vector(cone.first.x(), cone.first.y(), cone.first.z()) << std::endl;
        //        //std::cout << "----             " << cone.first.eta() << " " << cone.first.phi() << std::endl;
        //        iso_.setPosition( vtx );
        //        res = iso_.getIsolation( tracks );
        //        isoRandomCones->push_back( math::RhoEtaPhiVector( res.ptSum, cone.first.eta(), cone.first.phi()  ) );
        //        nTracksRandomCones->push_back( math::RhoEtaPhiVector( res.nTracks, cone.first.eta(), cone.first.phi()  ) );
        //}

        //// 4 cones kinematically correlated (thrown as ZZ4e events)
        //std::vector< std::pair<ConeGenerator::Vector,ConeGenerator::Point> > cones = coneGen_.fourKinematicCones();
        //for (size_t i = 0; i < cones.size(); ++i) {
        //        iso_.setDirection( Iso::Vector(cones[i].first.x(), cones[i].first.y(), cones[i].first.z()) );
        //        iso_.setPosition( vtx );
        //        res = iso_.getIsolation( tracks );
        //        isoKinematicCones->push_back( math::RhoEtaPhiVector( res.ptSum, cones[i].first.eta(), cones[i].first.phi()  ) );
        //        nTracksKinematicCones->push_back( math::RhoEtaPhiVector( res.nTracks, cones[i].first.eta(), cones[i].first.phi()  ) );
        //}

        //// random cones
        //// 1 - 2 - 4
        //for (int i = 0; i < 7; ++i) {
        //        //std::cout << "   random cone:" << std::endl;
        //        std::pair<ConeGenerator::Vector,ConeGenerator::Point> cone = coneGen_.randomCone();
        //        iso_.setDirection( Iso::Vector(cone.first.x(), cone.first.y(), cone.first.z()) );
        //        iso_.setPosition( cone.second );
        //        res = iso_.getIsolation( tracks );
        //        isoRandomConesVertices->push_back( math::RhoEtaPhiVector( res.ptSum, cone.first.eta(), cone.first.phi()  ) );
        //        nTracksRandomConesVertices->push_back( math::RhoEtaPhiVector( res.nTracks, cone.first.eta(), cone.first.phi()  ) );
        //}

        iEvent.put( isoElectrons, "isoElectrons" );
        iEvent.put( nTracksElectrons, "nTracksElectrons" );
        iEvent.put( isoRandomCones, "isoRandomCones" );
        iEvent.put( nTracksRandomCones, "nTracksRandomCones" );
        iEvent.put( isoKinematicCones, "isoKinematicCones" );
        iEvent.put( nTracksKinematicCones, "nTracksKinematicCones" );
        iEvent.put( isoRandomConesVertices, "isoRandomConesVertices" );
        iEvent.put( nTracksRandomConesVertices, "nTracksRandomConesVertices" );
        iEvent.put( electronClass, "electronClass" );
        iEvent.put( electronEoverP, "electronEoverP" );
        iEvent.put( electronEnergy, "electronEnergy" );
        iEvent.put( electronPCalo, "electronPCalo" );
        iEvent.put( electronPVertex, "electronPVertex" );
}



void IsoTrackDumper::beginJob(const edm::EventSetup&)
{
}



void IsoTrackDumper::endJob()
{
}



DEFINE_FWK_MODULE(IsoTrackDumper);

