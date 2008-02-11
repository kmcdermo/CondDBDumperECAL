
///////////////////////////////////////////////////////////////////
//                                                               //
//                  class S9ClusterProducer                      //
//                                                               //
//   Author: Julien DESCAMPS, CEA Saclay DAPNIA/SPP, Sept 2007   //
//                                                               //
///////////////////////////////////////////////////////////////////



// C/C++ headers
#include <iostream>
#include <vector>
#include <memory>

// Framework
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/EventSetup.h"
#include "DataFormats/Common/interface/Handle.h"
#include "FWCore/MessageLogger/interface/MessageLogger.h"
#include "FWCore/Framework/interface/ESHandle.h"

// Reconstruction Classes
#include "DataFormats/EcalRecHit/interface/EcalRecHit.h"
#include "DataFormats/EcalRecHit/interface/EcalRecHitCollections.h"
#include "DataFormats/EcalDetId/interface/EBDetId.h"
#include "DataFormats/EgammaReco/interface/BasicCluster.h"
#include "DataFormats/EgammaReco/interface/SuperCluster.h"
#include "DataFormats/EgammaReco/interface/BasicClusterFwd.h"
#include "DataFormats/EgammaReco/interface/BasicClusterShapeAssociation.h"

// Geometry
#include "Geometry/Records/interface/IdealGeometryRecord.h"
#include "Geometry/CaloGeometry/interface/CaloSubdetectorGeometry.h"
#include "Geometry/CaloGeometry/interface/CaloCellGeometry.h"
#include "Geometry/CaloGeometry/interface/CaloGeometry.h"
#include "Geometry/CaloTopology/interface/EcalBarrelTopology.h"
#include "Geometry/CaloTopology/interface/EcalEndcapTopology.h"
#include "Geometry/CaloTopology/interface/EcalPreshowerTopology.h"


// Class header file
#include "RecoEcal/EgammaClusterProducers/interface/S9ClusterProducer.h"
#include "RecoEcal/EgammaClusterAlgos/interface/S9ClusterAlgo.h"
#include "RecoEcal/EgammaCoreTools/interface/ClusterShapeAlgo.h"
#include "DataFormats/EgammaReco/interface/ClusterShape.h"
#include "DataFormats/EgammaReco/interface/ClusterShapeFwd.h"
#include "RecoEcal/EgammaCoreTools/interface/PositionCalc.h"


S9ClusterProducer::S9ClusterProducer(const edm::ParameterSet& ps) {


        // use configuration file to setup input/output collection names
        nMaxPrintout_ = ps.getUntrackedParameter<int>("nMaxPrintout",1);

        hitproducer_   = ps.getParameter<std::string>("ecalhitproducer");
        hitcollection_ = ps.getParameter<std::string>("ecalhitcollection");
        superclusterCollection_ = ps.getParameter<std::string>("superclusterCollection");

        // S9 algorithm parameters
        double ES1       = ps.getParameter<double>("ES1");
        double ES2       = ps.getParameter<double>("ES2");
        double ETSG      = ps.getParameter<double>("ETSG");
        int PhiStepsMax  = ps.getParameter<int>("PhiStepsMax");
        int EtaStepsMax  = ps.getParameter<int>("EtaStepsMax");

        // S9 algorithm logposition measuremement parameters
        double X0       = ps.getParameter<double>("X0");
        double W0       = ps.getParameter<double>("W0");
        double T0       = ps.getParameter<double>("T0");

        int ParticleType  = ps.getParameter<int>("ParticleType");

        algo_ = new S9ClusterAlgo(ES1,ES2,ETSG,PhiStepsMax,EtaStepsMax,X0,W0,T0,ParticleType);


        produces< reco::SuperClusterCollection >(superclusterCollection_);


}

S9ClusterProducer::~S9ClusterProducer() {
        delete algo_;
}


void
S9ClusterProducer::produce(edm::Event& evt, const edm::EventSetup& es) {


        // get the hit collection from the event:
        edm::Handle<EcalRecHitCollection> rhcHandle;
        //  evt.getByType(rhcHandle);
        evt.getByLabel(hitproducer_, hitcollection_, rhcHandle);
        //  if (!(rhcHandle.isValid())) 
        //     {
        //       if (debugL <= HybridClusterAlgo::pINFO)
        // 	std::cout << "could not get a handle on the EcalRecHitCollection!" << std::endl;
        //       return;
        //     }
        const EcalRecHitCollection *hit_collection = rhcHandle.product();

        // get the collection geometry:
        edm::ESHandle<CaloGeometry> geoHandle;
        es.get<IdealGeometryRecord>().get(geoHandle);
        const CaloGeometry& geometry = *geoHandle;
        const CaloSubdetectorGeometry *geometry_p;
        std::auto_ptr<const CaloSubdetectorTopology> topology;

        //  if (debugL == HybridClusterAlgo::pDEBUG)
        //     std::cout << "\n\n\n" << hitcollection_ << "\n\n" << std::endl;

        if(hitcollection_ == "EcalRecHitsEB") {
                geometry_p = geometry.getSubdetectorGeometry(DetId::Ecal, EcalBarrel);
                topology.reset(new EcalBarrelTopology(geoHandle));
        } else if(hitcollection_ == "EcalRecHitsEE") {
                geometry_p = geometry.getSubdetectorGeometry(DetId::Ecal, EcalEndcap);
                topology.reset(new EcalEndcapTopology(geoHandle));
        } else if(hitcollection_ == "EcalRecHitsPS") {
                geometry_p = geometry.getSubdetectorGeometry(DetId::Ecal, EcalPreshower);
                topology.reset(new EcalPreshowerTopology (geoHandle));
        } else throw(std::runtime_error("\n\nHybrid Cluster Producer encountered invalied ecalhitcollection type.\n\n"));




        // fe -- it was working before 
        //algo_->makeSuperClusters(hit_collection,geometry_p);



        // fe -- all this was uncommented -- try to see if it works
        // output collection of basic clusters
        // reco::BasicClusterCollection defined in BasicClusterFwd.h

        // make the clusters by passing rechits to the agorithm
        std::auto_ptr< reco::SuperClusterCollection >  clusters(  new reco::SuperClusterCollection(algo_->makeSuperClusters( hit_collection, geometry_p )) );

        //   // put the product in the event
        evt.put( clusters, superclusterCollection_ );
}
