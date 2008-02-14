
///////////////////////////////////////////////////////////////////
//                                                               //
//                  class S9SuperClusterProducer                      //
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
#include "RecoEcal/EgammaClusterProducers/interface/S9SuperClusterProducer.h"
#include "RecoEcal/EgammaClusterAlgos/interface/S9ClusterAlgo.h"
#include "RecoEcal/EgammaCoreTools/interface/ClusterShapeAlgo.h"
#include "DataFormats/EgammaReco/interface/ClusterShape.h"
#include "DataFormats/EgammaReco/interface/ClusterShapeFwd.h"
#include "RecoEcal/EgammaCoreTools/interface/PositionCalc.h"


S9SuperClusterProducer::S9SuperClusterProducer(const edm::ParameterSet & ps)
{
	// use configuration file to setup input/output collection names
	nMaxPrintout_ = ps.getUntrackedParameter < int >("nMaxPrintout", 1);

	hitproducer_ = ps.getParameter < std::string > ("ecalhitproducer");
	hitcollection_ = ps.getParameter < std::string > ("ecalhitcollection");
	basicClusterCollection_ = ps.getParameter < std::string > ("basicClusterCollection");
	basicClusterProducer_ = ps.getParameter < std::string > ("basicClusterProducer");
	superclusterCollection_ = ps.getParameter < std::string > ("superClusterCollection");

	// S9 algorithm parameters
	double ES1 = ps.getParameter < double >("ES1");
	double ES2 = ps.getParameter < double >("ES2");
	double ETSG = ps.getParameter < double >("ETSG");
	int PhiStepsMax = ps.getParameter < int >("PhiStepsMax");
	int EtaStepsMax = ps.getParameter < int >("EtaStepsMax");

	// S9 algorithm logposition measuremement parameters
	double X0 = ps.getParameter < double >("X0");
	double W0 = ps.getParameter < double >("W0");
	double T0 = ps.getParameter < double >("T0");

	int ParticleType = ps.getParameter < int >("ParticleType");

	algo_ = new S9ClusterAlgo(ES1, ES2, ETSG, PhiStepsMax, EtaStepsMax, X0, W0, T0, ParticleType);
	produces < reco::SuperClusterCollection > (superclusterCollection_);
}

S9SuperClusterProducer::~S9SuperClusterProducer()
{
	delete algo_;
}


void
 S9SuperClusterProducer::produce(edm::Event & evt, const edm::EventSetup & es)
{
	// get the hit collection from the event:
	edm::Handle < EcalRecHitCollection > rhcHandle;
	evt.getByLabel(hitproducer_, hitcollection_, rhcHandle);
	const EcalRecHitCollection *hit_collection = rhcHandle.product();

	// get the collection geometry:
	edm::ESHandle < CaloGeometry > geoHandle;
	es.get < IdealGeometryRecord > ().get(geoHandle);
	const CaloGeometry & geometry = *geoHandle;
	const CaloSubdetectorGeometry *geometry_p;
	std::auto_ptr < const CaloSubdetectorTopology > topology;

	if (hitcollection_ == "EcalRecHitsEB") {
		geometry_p = geometry.getSubdetectorGeometry(DetId::Ecal, EcalBarrel);
		topology.reset(new EcalBarrelTopology(geoHandle));
	} else if (hitcollection_ == "EcalRecHitsEE") {
		geometry_p = geometry.getSubdetectorGeometry(DetId::Ecal, EcalEndcap);
		topology.reset(new EcalEndcapTopology(geoHandle));
	} else if (hitcollection_ == "EcalRecHitsPS") {
		geometry_p = geometry.getSubdetectorGeometry(DetId::Ecal, EcalPreshower);
		topology.reset(new EcalPreshowerTopology(geoHandle));
	} else
		throw(std::runtime_error("\n\nHybrid Cluster Producer encountered invalied ecalhitcollection type.\n\n"));

        std::cout << "Trying to get the basic cluster collection freshly produced" << std::endl;
        edm::Handle<reco::BasicClusterCollection> pBasicClusters;
        evt.getByLabel(basicClusterProducer_, basicClusterCollection_, pBasicClusters);

        const reco::BasicClusterCollection *clusterCollection_p = pBasicClusters.product();
        reco::BasicClusterRefVector clusterRefVector_p;
        for (unsigned int i = 0; i < clusterCollection_p->size(); i++)
        {
                clusterRefVector_p.push_back(reco::BasicClusterRef(pBasicClusters, i));
        }
        std::cout << "Done" << std::endl;

	std::auto_ptr < reco::SuperClusterCollection > clusters(new reco::SuperClusterCollection(algo_->makeSuperClusters(hit_collection, geometry_p, clusterRefVector_p)));
	//std::auto_ptr < reco::SuperClusterCollection > clusters(new reco::SuperClusterCollection());
	// put the product in the event
	evt.put(clusters, superclusterCollection_);
}
