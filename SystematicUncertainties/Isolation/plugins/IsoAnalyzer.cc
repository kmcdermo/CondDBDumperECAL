// -*- C++ -*-
//
// Package:    IsoAnalyzer
// Class:      IsoAnalyzer
// 
/**\class IsoAnalyzer IsoAnalyzer.cc SystematicUncertainties/IsoAnalyzer/src/IsoAnalyzer.cc

Description: <one line class summary>

Implementation:
<Notes on implementation>
*/
//
// Original Author:  Federico Ferri
//         Created:  Tue Oct 23 17:27:23 CEST 2007
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
#include "FWCore/ServiceRegistry/interface/Service.h"

#include "PhysicsTools/UtilAlgos/interface/TFileService.h"

#include <DataFormats/Math/interface/Vector3D.h>
#include <DataFormats/TrackReco/interface/Track.h>
#include <DataFormats/EgammaCandidates/interface/PixelMatchGsfElectron.h>
#include <DataFormats/EgammaCandidates/interface/PixelMatchGsfElectronFwd.h>

#include <SystematicUncertainties/Isolation/interface/IsoCone.h>
#include <SystematicUncertainties/Isolation/interface/ConeGenerator.h>
#include <SystematicUncertainties/HafHistogram/interface/HTupleManager.h>
#include <SystematicUncertainties/HafHistogram/interface/HTuple.h>

#include <EgammaAnalysis/ElectronIDAlgos/interface/CutBasedElectronID.h>

#include <CLHEP/Units/SystemOfUnits.h>

class IsoAnalyzer : public edm::EDAnalyzer {
        public:
                explicit IsoAnalyzer(const edm::ParameterSet&);
                ~IsoAnalyzer();

        private:
                virtual void beginJob(const edm::EventSetup&);
                virtual void analyze(const edm::Event&, const edm::EventSetup&);
                virtual void endJob();

                // to get parameters from cfg
                edm::InputTag electronCollection_;
                edm::InputTag isoTrackCollection_;

                Iso::Cone< reco::Track > isoCone_;
                Iso::Results isoRes_;
                ConeGenerator coneGen_;

                HTupleManager *tplmgr_;
                HTuple *ntu_ele_;
                HTuple *ntu_random_;
                HTuple *ntu_kineZ_;
                HTuple *ntu_kineZZ_;

                CutBasedElectronID *cbElId;

                int cnt_ev_;
};



IsoAnalyzer::IsoAnalyzer(const edm::ParameterSet& ps)
{
        electronCollection_ = ps.getParameter<edm::InputTag>("electronCollection");
        isoTrackCollection_ = ps.getParameter<edm::InputTag>("isoTrackCollection");

        double iradius = ps.getParameter<double>("innerConeRadius");
        double oradius = ps.getParameter<double>("outerConeRadius");
        double vertexDz = ps.getParameter<double>("vertexDz");
        double vertexDRt = ps.getParameter<double>("vertexDRt");
        double ptThreshold = ps.getParameter<double>("ptThreshold");
        double pThreshold = ps.getParameter<double>("pThreshold");
        isoCone_.setValues( iradius, oradius, vertexDz, vertexDRt, ptThreshold, pThreshold, Iso::Vector(0,1,0), Iso::Point(0,0,0) );
        
        std::string outputFile = ps.getParameter<std::string>("outputFile");
        tplmgr_ = new HTupleManager(outputFile.c_str(), "recreate");
        ntu_ele_ = tplmgr_->Ntuple("ntu_electrons");
        ntu_random_ = tplmgr_->Ntuple("ntu_random");
        ntu_kineZ_ = tplmgr_->Ntuple("ntu_kineZ");
        ntu_kineZZ_ = tplmgr_->Ntuple("ntu_kineZZ");

        double mX = ps.getParameter<double>("MeanX")*cm;
        double mY = ps.getParameter<double>("MeanY")*cm;
        double mZ = ps.getParameter<double>("MeanZ")*cm;
        double sX = ps.getParameter<double>("SigmaX")*cm;
        double sY = ps.getParameter<double>("SigmaY")*cm;
        double sZ = ps.getParameter<double>("SigmaZ")*cm;
        double mE = ps.getParameter<double>("MinEta");
        double ME = ps.getParameter<double>("MaxEta");
        double mP = ps.getParameter<double>("MinPhi");
        double MP = ps.getParameter<double>("MaxPhi");
        coneGen_.setVertexSigma(sX,sY,sZ);
        coneGen_.setVertexMean(mX,mY,mZ);
        coneGen_.setEtaRange(mE,ME);
        coneGen_.setPhiRange(mP,MP);
        std::string kinematicsZZFile = ps.getParameter<std::string>("kinematicsZZFile");
        std::string kinematicsZFile = ps.getParameter<std::string>("kinematicsZFile");
        coneGen_.loadZZKineCones( kinematicsZZFile.c_str() );
        coneGen_.loadZKineCones( kinematicsZFile.c_str() );
        coneGen_.init();

        cbElId = new CutBasedElectronID();
        cbElId->setup( ps );

        cnt_ev_ = 0;
}



IsoAnalyzer::~IsoAnalyzer()
{
        tplmgr_->Store();
        delete cbElId;
}



void IsoAnalyzer::analyze(const edm::Event& ev, const edm::EventSetup& es)
{
        edm::Handle< edm::View< reco::Track > > pIsoTracks;
        ev.getByLabel( isoTrackCollection_, pIsoTracks );
        
        edm::Handle<reco::PixelMatchGsfElectronCollection> pElectrons;
        ev.getByLabel( electronCollection_, pElectrons );
        const reco::PixelMatchGsfElectronCollection *electrons = pElectrons.product();
        // get isolation for each electron
        int cnt = 0;
        int nel = electrons->size();
        // making the assumption the electron vertex is the event one
        math::XYZPoint vtx;

        // AMBIGUITY RESOLUTION
        std::auto_ptr< reco::PixelMatchGsfElectronCollection > cleanElectrons( new reco::PixelMatchGsfElectronCollection() );
        for (reco::PixelMatchGsfElectronCollection::const_iterator iel = electrons->begin(); iel != electrons->end(); ++iel ) {
                cleanElectrons->push_back( *iel );
                math::XYZVector p = iel->momentum();
                //std::cout << cnt_ev_ << "before " << p.x() << " " << p.y() << " " << p.z() <<  std::endl;
        }

        bool ambiguous = true;
        while ( ambiguous ) {
                ambiguous = false;
                for (reco::PixelMatchGsfElectronCollection::iterator iel = cleanElectrons->begin(); iel != cleanElectrons->end(); ++iel ) {
                        for (reco::PixelMatchGsfElectronCollection::iterator jel = iel + 1; jel != cleanElectrons->end(); ) {
                                // ambiguity definition
                                if( (iel->caloEnergy() == jel->caloEnergy() && iel->caloPosition() == jel->caloPosition()) 
                                                || (iel->gsfTrack() == jel->gsfTrack())) {
                                        // ambiguity resolving
                                        ambiguous = true;
                                        if ( fabs( iel->eSuperClusterOverP() - 1) <= fabs( jel->eSuperClusterOverP() - 1) ) {
                                                cleanElectrons->erase( jel );
                                                iel = cleanElectrons->begin();
                                        } else {
                                                cleanElectrons->erase( iel );
                                                iel = cleanElectrons->begin();
                                        }
                                } else {
                                        ++jel;
                                }
                        }
                }
        }
        

        
        //for (reco::PixelMatchGsfElectronCollection::const_iterator iel = electrons->begin(); iel != electrons->end(); ++iel ) {
        for (reco::PixelMatchGsfElectronCollection::const_iterator iel = cleanElectrons->begin(); iel != cleanElectrons->end(); ++iel ) {
                math::XYZVector p = iel->momentum();
                //std::cout << cnt_ev_ << "after " << p.x() << " " << p.y() << " " << p.z() <<  std::endl;
                isoCone_.setDirection( p );
                if ( iel->p() > 10 ) {
                        vtx = iel->gsfTrack()->referencePoint();
                }
                isoCone_.setPosition( vtx );
                isoRes_ = isoCone_.getIsolation( pIsoTracks );

                std::cout << cnt_ev_ << " " << p.x() << " " << p.y() << " " << p.z() << " -- " << isoRes_.ptSum << " " << isoRes_.nTracks << std::endl;
                
                ntu_ele_->Column( "iev", cnt_ev_ );
                ntu_ele_->Column( "nel", nel );
                ntu_ele_->Column( "iel", cnt );
                ntu_ele_->Column( "iclass", iel->classification() );
                ntu_ele_->Column( "ielId", (int)cbElId->result( &(*iel), ev ) );
                ntu_ele_->Column( "p", iel->p() );
                ntu_ele_->Column( "pt", iel->pt() );
                ntu_ele_->Column( "eta", iel->eta() );
                ntu_ele_->Column( "phi", iel->phi() );
                ntu_ele_->Column( "ptSum", isoRes_.ptSum );
                ntu_ele_->Column( "nTracks", (int)isoRes_.nTracks );
                ntu_ele_->DumpData();
                ++cnt;
        }
        
        // random cones uniformly distributed in eta and phi
        for (int i = 0; i < nel; ++i) {
                std::pair<ConeGenerator::Vector,ConeGenerator::Point> cone = coneGen_.randomCone();
                isoCone_.setDirection( Iso::Vector(cone.first.x(), cone.first.y(), cone.first.z()) );
                isoCone_.setPosition( vtx );
                isoRes_ = isoCone_.getIsolation( pIsoTracks );
                ntu_random_->Column( "iev", cnt_ev_ );
                ntu_random_->Column( "ncone", nel );
                ntu_random_->Column( "icone", i );
                ntu_random_->Column( "eta", cone.first.eta() );
                ntu_random_->Column( "phi", cone.first.phi() );
                ntu_random_->Column( "ptSum", isoRes_.ptSum );
                ntu_random_->Column( "nTracks", (int)isoRes_.nTracks );
                ntu_random_->DumpData();
        }

        // 2 cones kinematically correlated (thrown as Zee events)
        std::vector< std::pair<ConeGenerator::Vector,ConeGenerator::Point> > conesZ = coneGen_.ZKinematicCones();
        for (size_t i = 0; i < conesZ.size(); ++i) {
                isoCone_.setDirection( Iso::Vector(conesZ[i].first.x(), conesZ[i].first.y(), conesZ[i].first.z()) );
                isoCone_.setPosition( vtx );
                isoRes_ = isoCone_.getIsolation( pIsoTracks );
                ntu_kineZ_->Column( "iev", cnt_ev_ );
                ntu_kineZ_->Column( "icone", (int)i );
                ntu_kineZ_->Column( "eta", conesZ[i].first.eta() );
                ntu_kineZ_->Column( "phi", conesZ[i].first.phi() );
                ntu_kineZ_->Column( "ptSum", isoRes_.ptSum );
                ntu_kineZ_->Column( "nTracks", (int)isoRes_.nTracks );
                ntu_kineZ_->DumpData();
        }

        // 4 cones kinematically correlated (thrown as ZZ4e events)
        std::vector< std::pair<ConeGenerator::Vector,ConeGenerator::Point> > conesZZ = coneGen_.ZZKinematicCones();
        for (size_t i = 0; i < conesZZ.size(); ++i) {
                isoCone_.setDirection( Iso::Vector(conesZZ[i].first.x(), conesZZ[i].first.y(), conesZZ[i].first.z()) );
                isoCone_.setPosition( vtx );
                isoRes_ = isoCone_.getIsolation( pIsoTracks );
                ntu_kineZZ_->Column( "iev", cnt_ev_ );
                ntu_kineZZ_->Column( "icone", (int)i );
                ntu_kineZZ_->Column( "eta", conesZZ[i].first.eta() );
                ntu_kineZZ_->Column( "phi", conesZZ[i].first.phi() );
                ntu_kineZZ_->Column( "ptSum", isoRes_.ptSum );
                ntu_kineZZ_->Column( "nTracks", (int)isoRes_.nTracks );
                ntu_kineZZ_->DumpData();
        }

        ++cnt_ev_;
}



void IsoAnalyzer::beginJob(const edm::EventSetup&)
{
}



void IsoAnalyzer::endJob()
{
}


DEFINE_FWK_MODULE(IsoAnalyzer);
