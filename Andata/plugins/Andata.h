#ifndef MyPackageAndata_h
#define MyPackageAndata_h

// -*- C++ -*-
//
// Package:    Andata
// Class:      Andata
// 
/**\class Andata Andata.cc MyPackage/Andata/plugins/Andata.cc

Description: <one line class summary>

Implementation:
<Notes on implementation>
*/
//
// Original Author:  Federico Ferri
//         Created:  Mon Jan 25 17:32:42 CET 2010
// $Id$
//
//


#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/EDAnalyzer.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"

#include "DataFormats/EcalDigi/interface/EcalDigiCollections.h"

#include "MyPackage/Andata/interface/HistoManager.h"

#include <memory>

class Andata : public edm::EDAnalyzer {
    public:

        explicit Andata(const edm::ParameterSet&);
        ~Andata();

    private:
        virtual void beginJob() ;
        virtual void analyze(const edm::Event &, const edm::EventSetup & );
        virtual void endJob() ;

        void getCollections( const edm::Event & );

        // ----------member data ---------------------------
        edm::InputTag ebDigiCollection_;
        edm::InputTag eeDigiCollection_;
        std::string outputFile_;

        const EBDigiCollection *ebDigis_;
        const EEDigiCollection *eeDigis_;

        bool verbosity_;

        HistoManager histos_;
};

#endif
