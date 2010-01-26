#include "MyPackage/Andata/plugins/Andata.h"

#include "DataFormats/EcalDigi/interface/EcalMGPASample.h"

#include "TH1F.h"


Andata::Andata(const edm::ParameterSet& ps) :
    ebDigis_(0),
    eeDigis_(0)
{
    ebDigiCollection_ = ps.getParameter<edm::InputTag>("EBDigiCollection");
    eeDigiCollection_ = ps.getParameter<edm::InputTag>("EEDigiCollection");

    outputFile_ = ps.getParameter<std::string>("outputFile");
    verbosity_  = ps.getParameter<bool>("verbosity");

    histos_.addTemplate<TH1F>( "samples", new TH1F( "samples", "samples", 10, -0.5, 9.5 ) );
}


Andata::~Andata()
{
}


//
// member functions
//

// ------------ method called to for each event  ------------
void
Andata::analyze(edm::Event const & ev, edm::EventSetup const & es)
{
    getCollections( ev );
    // if they exist, loop on EB digis
    if ( ebDigis_ ) {
        for (size_t idigi=0; idigi<ebDigis_->size(); ++idigi ) {
            EBDataFrame samples = (*ebDigis_)[idigi];
            for ( int isample=0; isample<samples.size(); ++isample ) {
                EcalMGPASample sam = samples[isample];

                if ( verbosity_ ) std::cout << "EB_digis sample= " << isample
                        << " ADC= " << sam.adc()
                        << " gainId= " << sam.gainId()
                        << "\n";

                histos_.h<TH1F>( "samples", "EB" )->Fill( isample, sam.adc() );
            }
        }
    }
    // if they exist, loop on EE digis
    if ( eeDigis_ ) {
        for (size_t idigi=0; idigi<eeDigis_->size(); ++idigi ) {
            EEDataFrame samples = (*eeDigis_)[idigi];
            for ( int isample=0; isample<samples.size(); ++isample ) {
                EcalMGPASample sam = samples[isample];

                if ( verbosity_ ) std::cout << "EE_digis sample= " << isample
                        << " ADC= " << sam.adc()
                        << " gainId= " << sam.gainId()
                        << "\n";

                histos_.h<TH1F>( "samples", "EE" )->Fill( isample, sam.adc() );
            }
        }
    }
}


// ------------ method called once each job just before starting event loop  ------------
void 
Andata::beginJob()
{
}

// ------------ method called once each job just after ending the event loop  ------------
void 
Andata::endJob()
{
    TFile *fout = new TFile( outputFile_.c_str(), "RECREATE" );
    assert( fout->IsOpen() );
    histos_.setFile( fout );
    histos_.save();
}


void
Andata::getCollections( edm::Event const & ev )
{
    if ( ebDigiCollection_.label() != "" ) {
        edm::Handle<EBDigiCollection> ebDigis_h;
        ev.getByLabel( ebDigiCollection_, ebDigis_h );
        ebDigis_ = ebDigis_h.product();
        if ( ebDigis_h.isValid() ) {
            ebDigis_ = ebDigis_h.product();
        } else {
            std::cout << "Invalid collection with label `" << ebDigiCollection_.label() 
                << " and instance `" << ebDigiCollection_.instance() << "'"
                << "\n";
        }
    }
    if ( eeDigiCollection_.label() != "" ) {
        edm::Handle<EEDigiCollection> eeDigis_h;
        ev.getByLabel( eeDigiCollection_, eeDigis_h );
        if ( eeDigis_h.isValid() ) {
            eeDigis_ = eeDigis_h.product();
        } else {
            std::cout << "Invalid collection with label `" << eeDigiCollection_.label() 
                << " and instance `" << eeDigiCollection_.instance() << "'"
                << "\n";
        }
    }
}

//define this as a plug-in
DEFINE_FWK_MODULE(Andata);
