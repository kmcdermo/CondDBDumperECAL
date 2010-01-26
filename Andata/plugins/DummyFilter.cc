// -*- C++ -*-
//
// Package:    DummyFilter
// Class:      DummyFilter
// 
/**\class DummyFilter DummyFilter.cc MyPackage/DummyFilter/src/DummyFilter.cc

 Description: <one line class summary>

 Implementation:
     <Notes on implementation>
*/
//
// Original Author:  Federico FERRI
//         Created:  Wed Jan 20 14:30:45 CET 2010
// $Id$
//
//


// system include files
#include <memory>

// user include files
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/EDFilter.h"

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"

#include "FWCore/ParameterSet/interface/ParameterSet.h"

//
// class declaration
//

class DummyFilter : public edm::EDFilter {
   public:
      explicit DummyFilter(const edm::ParameterSet&);
      ~DummyFilter();

   private:
      virtual void beginJob() ;
      virtual bool filter(edm::Event&, const edm::EventSetup&);
      virtual void endJob() ;
      
      // ----------member data ---------------------------
};

//
// constants, enums and typedefs
//

//
// static data member definitions
//

//
// constructors and destructor
//
DummyFilter::DummyFilter(const edm::ParameterSet& iConfig)
{
   //now do what ever initialization is needed

}


DummyFilter::~DummyFilter()
{
 
   // do anything here that needs to be done at desctruction time
   // (e.g. close files, deallocate resources etc.)

}


//
// member functions
//

// ------------ method called on each new Event  ------------
bool
DummyFilter::filter(edm::Event& iEvent, const edm::EventSetup& iSetup)
{
        int nEventWindow = 2;
        int run = iEvent.run();
        int evt = iEvent.eventAuxiliary().event();
        if ( run != 110916 ) return false;
        if ( evt > 15615116-nEventWindow/2 && evt < 15615116+nEventWindow/2 ) return true;
        return false;
}

// ------------ method called once each job just before starting event loop  ------------
void 
DummyFilter::beginJob()
{
}

// ------------ method called once each job just after ending the event loop  ------------
void 
DummyFilter::endJob() {
}

//define this as a plug-in
DEFINE_FWK_MODULE(DummyFilter);
