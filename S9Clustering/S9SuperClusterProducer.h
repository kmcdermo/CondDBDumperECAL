#ifndef RecoEcal_EgammaClusterProducers_S9SuperClusterProducer_h
#define RecoEcal_EgammaClusterProducers_S9SuperClusterProducer_h
/** \class S9SuperClusterProducer
 **   example of producer for BasicCluster from recHits
 **
 **  $Id: S9SuperClusterProducer.h,v 1.1 2006/04/13 14:40:05 rahatlou Exp $
 **  $Date: 2006/04/13 14:40:05 $
 **  $Revision: 1.1 $
 **  \author Shahram Rahatlou, University of Rome & INFN, April 2006
 **
 ***/

// #include "FWCore/Framework/interface/EDProducer.h"
// #include "FWCore/ParameterSet/interface/ParameterSet.h"
// #include "FWCore/Framework/interface/Event.h"
// #include "FWCore/Framework/interface/EventSetup.h"
// #include "RecoEcal/EgammaClusterAlgos/interface/S9ClusterAlgo.h"

#include <memory>

///////////////////////////////////////////////////////////////////
//                                                               //
//                  class S9SuperClusterProducer                      //
//                                                               //
//   Author: Julien DESCAMPS, CEA Saclay DAPNIA/SPP, Sept 2007   //
//                                                               //
///////////////////////////////////////////////////////////////////

#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/EDProducer.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/EventSetup.h"

#include "FWCore/ParameterSet/interface/ParameterSet.h"

#include "RecoEcal/EgammaClusterAlgos/interface/HybridClusterAlgo.h"
#include "RecoEcal/EgammaCoreTools/interface/ClusterShapeAlgo.h"
#include "RecoEcal/EgammaCoreTools/interface/PositionCalc.h"



class S9ClusterAlgo;

// S9SuperClusterProducer inherits from EDProducer, so it can be a module:
class S9SuperClusterProducer : public edm::EDProducer {

public:

  S9SuperClusterProducer (const edm::ParameterSet& ps);
  ~S9SuperClusterProducer();

  virtual void produce(edm::Event& evt, const edm::EventSetup& es);

private:

  S9ClusterAlgo* algo_; // algorithm doing the real work

  std::string hitproducer_;   // name of module/plugin/producer producing hits
  std::string hitcollection_; // secondary name given to collection of hits by hitProducer
  std::string superclusterCollection_;  // secondary name to be given to collection of cluster produced in this module
  std::string basicClusterProducer_;  // secondary name to be given to collection of cluster produced in this module
  std::string basicClusterCollection_;
 
  int nMaxPrintout_; // max # of printouts
  int nEvt_;         // internal counter of events

  bool counterExceeded() const { return ((nEvt_ > nMaxPrintout_) || (nMaxPrintout_ < 0)); }


};
#endif
