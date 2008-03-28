// -*- C++ -*-
//
// Package:    IsoHistogrammer
// Class:      IsoHistogrammer
// 
/**\class IsoHistogrammer IsoHistogrammer.cc SystematicUncertainties/IsoHistogrammer/src/IsoHistogrammer.cc

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
#include <TH1F.h>
#include <TH2F.h>
#include <TNtuple.h>


typedef std::vector< math::RhoEtaPhiVector > repV;


class IsoHistogrammer : public edm::EDAnalyzer {
        public:
                explicit IsoHistogrammer(const edm::ParameterSet&);
                ~IsoHistogrammer();

        private:
                virtual void beginJob(const edm::EventSetup&) ;
                virtual void analyze(const edm::Event&, const edm::EventSetup&);
                virtual void endJob();

                TH1F *h_IC_, *h_RC_, *h_RCK_;
                TH1F *h_eClass_;
                TH2F *h_IC_vs_eClass_;
                TNtuple *ntu_;
                edm::InputTag srcClass_, srcEnergy_, srcIsoC_, srcIsoRC_, srcIsoKC_;
};



IsoHistogrammer::IsoHistogrammer(const edm::ParameterSet& iConfig) :
        srcClass_(iConfig.getParameter<edm::InputTag>("srcClass")),
        srcEnergy_(iConfig.getParameter<edm::InputTag>("srcEnergy")),
        srcIsoC_(iConfig.getParameter<edm::InputTag>("srcIsoC")),
        srcIsoRC_(iConfig.getParameter<edm::InputTag>("srcIsoRC")),
        srcIsoKC_(iConfig.getParameter<edm::InputTag>("srcIsoKC"))
{
        edm::Service<TFileService> fs;
        h_IC_           = fs->make<TH1F>( "h_IC_", "Electron Isolation Cone", 200, 0., 100. );
        h_eClass_       = fs->make<TH1F>( "h_eClass_", "Electron Class", 200, 0., 200. );
        h_IC_vs_eClass_ = fs->make<TH2F>( "h_IC_vs_eClass_", "Electron Isolation vs Electron Class", 150, 0., 150., 200, 0., 200. );
        ntu_ = fs->make<TNtuple>( "ntu", "ntu", "energy:eta:phi:eclass:iso:isoRC:isoKC" );
}



IsoHistogrammer::~IsoHistogrammer()
{
}



void IsoHistogrammer::analyze(const edm::Event& iEvent, const edm::EventSetup& iSetup)
{
        edm::Handle< std::vector<int> > eClass;
        iEvent.getByLabel( srcClass_, eClass );

        edm::Handle< std::vector<float> > eEnergy;
        iEvent.getByLabel( srcEnergy_, eEnergy );

        edm::Handle< repV > isoC;
        iEvent.getByLabel( srcIsoC_, isoC );

        edm::Handle< repV > isoRC;
        iEvent.getByLabel( srcIsoRC_, isoRC );

        edm::Handle< repV > isoKC;
        iEvent.getByLabel( srcIsoKC_, isoKC );

        for (size_t i = 0; i < eClass->size(); ++i) {
                h_eClass_->Fill( (*eClass)[i] );
                ntu_->Fill( (*eEnergy)[i], (*isoC)[i].eta(), (*isoC)[i].phi(), (*eClass)[i], (*isoC)[i].Rho(), (*isoRC)[i].Rho(), (*isoKC)[i].Rho());
        }
}



void IsoHistogrammer::beginJob(const edm::EventSetup&)
{
}



void IsoHistogrammer::endJob()
{
}



DEFINE_FWK_MODULE(IsoHistogrammer);
