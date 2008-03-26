// -*- C++ -*-
//
// Package:    MuRecal
// Class:      MuRecal
// 
/**\class MuRecal MuRecal.cc Cosmics/MuRecal/src/MuRecal.cc

Description: <one line class summary>

Implementation:
<Notes on implementation>
*/
//
// Original Author:  Federico Ferri
//         Created:  Tue Mar 25 17:46:58 CET 2008
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

#include "DataFormats/EcalRecHit/interface/EcalRecHitCollections.h"
#include "DataFormats/EcalDetId/interface/EBDetId.h"

#include "Cosmics/HafHistogram/interface/HTupleManager.h"
#include "Cosmics/HafHistogram/interface/HTuple.h"

//
// class decleration
//

class MuRecal : public edm::EDAnalyzer {
        public:
                explicit MuRecal(const edm::ParameterSet&);
                ~MuRecal();


        private:
                virtual void beginJob(const edm::EventSetup&) ;
                virtual void analyze(const edm::Event&, const edm::EventSetup&);
                virtual void endJob() ;

                // ----------member data ---------------------------
                edm::InputTag ebRecHitCollection_;
                //edm::InputTag eeRecHitCollection_;
                std::string outputFile_;
                float lowerThreshold_;

                HTupleManager *tplmgr_;
                HTuple *ntu_;
};



MuRecal::MuRecal(const edm::ParameterSet& ps)
{
        ebRecHitCollection_ = ps.getParameter<edm::InputTag>("ebRecHitCollection");
        //eeRecHitCollection_ = ps.getParameter<edm::InputTag>("eeRecHitCollection");
        outputFile_ = ps.getParameter<std::string>("outputFile");
        lowerThreshold_ = ps.getParameter<double>("lowerThreshold");

        tplmgr_ = new HTupleManager(outputFile_.c_str(),"recreate");
        ntu_ = tplmgr_->Ntuple("ntu");
}



MuRecal::~MuRecal()
{
        tplmgr_->Store();
}



void MuRecal::analyze(const edm::Event& ev, const edm::EventSetup& es)
{
        edm::Handle<EcalRecHitCollection> recHitsEB;
        ev.getByLabel( ebRecHitCollection_, recHitsEB );
        for (unsigned int irh = 0; irh < recHitsEB->size(); ++irh) {
                float E1 = (*recHitsEB)[irh].energy();
                // basic recHit selection
                if ( E1 < lowerThreshold_ ) {
                        continue;
                }
                ntu_->Column("detId", (int)(*recHitsEB)[irh].id() );
                EBDetId id( (*recHitsEB)[irh].id() );
                ntu_->Column("iSM", id.ism() );
                ntu_->Column("ixtal", id.ic() );
                ntu_->Column("ieta", id.ieta() );
                ntu_->Column("iphi", id.iphi() );
                ntu_->Column("ietaSM", id.ietaSM() );
                ntu_->Column("iphiSM", id.iphiSM() );
                ntu_->Column("E1", E1 );
                HTValArray<float,8> E8;
                float E2 = 0;
                unsigned int cnt = 0;
                // find the energy of the surrounding crystals
                for (int i = -1; i <= 1; ++i) {
                        for (int j = -1; j <= 1; ++j) {
                                if ( !i && !j ) {
                                        continue;
                                }
                                // continuous boundary conditions...
                                int ie = id.ieta() + i;
                                int ip = id.iphi() + j;
                                if ( ip == 361 ) ip = 1;
                                if ( ip ==   0 ) ip = 360;
                                if ( ie ==   0 ) ie += j;  // eta == 0 does not exist
                                // vector of surrounding energies
                                if ( EBDetId::validDetId( ie, ip ) ) {
                                        EBDetId tmpId( ie, ip );
                                        EcalRecHitCollection::const_iterator it = recHitsEB->find( tmpId );
                                        if ( it != recHitsEB->end() ) {
                                                //std::cout << ".. " << i << " " << j << " " << (*it).energy() << " " << (int)(*it).id() << std::endl;
                                                E8[cnt] = (*it).energy();
                                                // store the maximum energy of the neighbours
                                                if ( (*it).energy() > E2 ) {
                                                        E2 = (*it).energy();
                                                }
                                        }
                                } else {
                                        E8[cnt] = -999.;
                                }
                                ++cnt;
                        }
                }
                ntu_->Column("E8", E8 );
                ntu_->Column("E2", E2 );
                ntu_->DumpData();
        }

        // be serious...
        //edm::Handle<EcalRecHitCollection> recHitsEE;
        //ev.getByLabel( eeRecHitCollection_, recHitsEE );
        //for (unsigned int irh = 0; irh < recHitsEE->size(); ++irh) {
        //}

}



void MuRecal::beginJob(const edm::EventSetup&)
{
}



void MuRecal::endJob() {
}

//define this as a plug-in
DEFINE_FWK_MODULE(MuRecal);
