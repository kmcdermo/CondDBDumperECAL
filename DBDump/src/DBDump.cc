// -*- C++ -*-
//
// Package:    DBDump
// Class:      DBDump
// 
/**\class DBDump DBDump.cc MyPackage/DBDump/src/DBDump.cc

 Description: <one line class summary>

 Implementation:
     <Notes on implementation>
*/
//
// Original Author:  Federico FERRI
//         Created:  Thu Jun 25 15:39:48 CEST 2009
// $Id: DBDump.cc,v 1.7 2011/04/20 00:11:50 ferriff Exp $
//
//


// system include files
#include <memory>
#include <fstream>
#include <string>

// user include files
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/EDAnalyzer.h"

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/Framework/interface/ESHandle.h"
#include "FWCore/Framework/interface/MakerMacros.h"

#include "FWCore/ServiceRegistry/interface/Service.h"
//#include "PhysicsTools/UtilAlgos/interface/TFileService.h"

#include "CondFormats/EcalObjects/interface/EcalADCToGeVConstant.h"
#include "CondFormats/EcalObjects/interface/EcalChannelStatus.h"
#include "CondFormats/EcalObjects/interface/EcalGainRatios.h"
#include "CondFormats/EcalObjects/interface/EcalIntercalibConstants.h"
#include "CondFormats/EcalObjects/interface/EcalIntercalibConstantsMC.h"
#include "CondFormats/EcalObjects/interface/EcalLaserAPDPNRatios.h"
#include "CondFormats/EcalObjects/interface/EcalPedestals.h"
#include "CondFormats/EcalObjects/interface/EcalTimeCalibConstants.h"

#include "CondFormats/DataRecord/interface/EcalADCToGeVConstantRcd.h"
#include "CondFormats/DataRecord/interface/EcalChannelStatusRcd.h"
#include "CondFormats/DataRecord/interface/EcalGainRatiosRcd.h"
#include "CondFormats/DataRecord/interface/EcalIntercalibConstantsMCRcd.h"
#include "CondFormats/DataRecord/interface/EcalIntercalibConstantsRcd.h"
#include "CondFormats/DataRecord/interface/EcalLaserAPDPNRatiosRcd.h"
#include "CondFormats/DataRecord/interface/EcalPedestalsRcd.h"
#include "CondFormats/DataRecord/interface/EcalTimeCalibConstantsRcd.h"

#include "CalibCalorimetry/EcalLaserCorrection/interface/EcalLaserDbService.h"
#include "CalibCalorimetry/EcalLaserCorrection/interface/EcalLaserDbRecord.h"

#include "Geometry/CaloEventSetup/interface/CaloTopologyRecord.h"
//#include "Geometry/Records/interface/IdealGeometryRecord.h"
//#include "Geometry/EcalMapping/interface/EcalMappingRcd.h"
#include "Geometry/CaloGeometry/interface/CaloGeometry.h"


#include "FWCore/ParameterSet/interface/ParameterSet.h"

#include "HistoManager.h"

#include "TProfile.h"
#include "TH1D.h"
#include "TH2D.h"
#include "TGraph.h"


class DBDump : public edm::EDAnalyzer {
   public:
      explicit DBDump(const edm::ParameterSet&);
      ~DBDump();
      void setDumpFalse();
      void setPlotFalse();

   private:
      virtual void beginJob() ;
      virtual void analyze(const edm::Event&, const edm::EventSetup&);
      //virtual void beginRun(const edm::Event&, const edm::EventSetup&);
      //virtual void endRun(const edm::Event&, const edm::EventSetup&);
      virtual void endJob() ;

      int getLMNumber(DetId const & xid) const;

      // output files
      bool outPlot_;
      bool outDump_;
      std::string outDumpFile_;
      std::string outPlotFile_;
      std::ofstream ofile_;

      // inter-calibration constants
      bool dumpIC_;
      bool plotIC_;
      edm::ESHandle<EcalIntercalibConstants> ic_;
      edm::ESHandle<EcalIntercalibConstantsMC> icMC_;

      // time-calibration constants
      bool dumpTC_;
      bool plotTC_;
      edm::ESHandle<EcalTimeCalibConstants> tc_;

      // ADCToGeV constant
      bool dumpADCToGeV_;
      edm::ESHandle<EcalADCToGeVConstant> adcToGeV_;

      // laser transparency measurements
      bool dumpTransp_;
      bool plotTransp_;
      edm::ESHandle<EcalLaserAPDPNRatios> apdpn_;

      // laser transparency corrections
      bool dumpTranspCorr_;
      bool plotTranspCorr_;
      edm::ESHandle<EcalLaserDbService> laser_;

      // channel status map
      bool dumpChStatus_;
      bool plotChStatus_;
      edm::ESHandle<EcalChannelStatus> chStatus_;

      // pedestals
      bool dumpPedestals_;
      bool plotPedestals_;
      edm::ESHandle<EcalPedestals> ped_;

      // gain ratios
      bool dumpGainRatios_;
      bool plotGainRatios_;
      edm::ESHandle<EcalGainRatios> gr_;

      HistoManager histos;

      edm::ESHandle<CaloGeometry> caloGeometry_;
      const CaloGeometry * geo_;

};



DBDump::DBDump(const edm::ParameterSet& ps)
{
        outPlot_ = ps.getParameter<bool>("outPlot");
        outDump_ = ps.getParameter<bool>("outDump");
        outDumpFile_ = ps.getParameter<std::string>("outDumpFile");
        outPlotFile_ = ps.getParameter<std::string>("outPlotFile");
        // inter-calibration constants
        dumpIC_ = ps.getParameter<bool>("dumpIC");
        plotIC_ = ps.getParameter<bool>("plotIC");
        // time-calibration constants
        dumpTC_ = ps.getParameter<bool>("dumpTC");
        plotTC_ = ps.getParameter<bool>("plotTC");
        // ADCToGeV constant
        dumpADCToGeV_ = ps.getParameter<bool>("dumpADCToGeV");
        // laser transparency measurements
        dumpTransp_ = ps.getParameter<bool>("dumpTransp");
        plotTransp_ = ps.getParameter<bool>("plotTransp");
        // channel status map
        dumpChStatus_ = ps.getParameter<bool>("dumpChStatus");
        plotChStatus_ = ps.getParameter<bool>("plotChStatus");
        // pedestals
        dumpPedestals_ = ps.getParameter<bool>("dumpPedestals");
        plotPedestals_ = ps.getParameter<bool>("plotPedestals");
        // gain ratios
        dumpGainRatios_ = ps.getParameter<bool>("dumpGainRatios");
        plotGainRatios_ = ps.getParameter<bool>("plotGainRatios");
        // laser transparency corrections
        dumpTranspCorr_ = ps.getParameter<bool>("dumpTranspCorr");
        plotTranspCorr_ = ps.getParameter<bool>("plotTranspCorr");

        if ( outDumpFile_ != "" ) {
                ofile_.open( outDumpFile_.c_str(), std::ios::out );
                if ( ! ofile_.is_open() ) {
                        edm::LogError("DBDump") << "DBDump::DBDump cannot open output file " << outDumpFile_;
                        setDumpFalse();
                }
        } else if ( outDump_ ) {
                edm::LogError("DBDump") << "DBDump::DBDump mismatch in your config file, setting outDump to false.";
                setDumpFalse();
        } else {
                setDumpFalse();
        }

        if ( outPlot_ ) {
                histos.addTemplate<TH2D>( "EBh2", new TH2D( "EB", "EB", 360, 0.5, 360.5, 171, -85.5, 85.5 ) );
                histos.addTemplate<TH2D>( "EEh2", new TH2D( "EE", "EE", 100, 0., 100., 100, 0., 100. ) );
                histos.addTemplate<TProfile>( "EEprof", new TProfile( "EE", "EE", 55, 0., 55.) );
                histos.addTemplate<TProfile>( "EBprof", new TProfile( "EB", "EB", 171, -85.5, 85.5) );
                histos.addTemplate<TGraph>("history", new TGraph());
                histos.addTemplate<TH1D>("distr", new TH1D("distribution", "distribution", 2000, 0., 2.));
                histos.addTemplate<TProfile>("etaProf", new TProfile("etaProf", "etaProf", 250, -2.75, 2.75));
        } else {
                setPlotFalse();
        }
}

void DBDump::setDumpFalse()
{
        dumpIC_ = false;
        dumpTC_ = false;
        dumpADCToGeV_ = false;
        dumpTransp_ = false;
        dumpTranspCorr_ = false;
        dumpChStatus_ = false;
        dumpPedestals_ = false;
}

void DBDump::setPlotFalse()
{
        plotIC_ = false;
        plotTC_ = false;
        plotTransp_ = false;
        plotTranspCorr_ = false;
        plotChStatus_ = false;
        plotPedestals_ = false;
}


DBDump::~DBDump()
{
        if ( plotIC_ ) {
                histos.h<TProfile>("EBprof", "IC")->SetMarkerColor(4);
                histos.h<TProfile>("EBprof", "IC")->SetMarkerStyle(20);
                histos.h<TProfile>("EEprof", "posZ:d1:IC")->SetMarkerColor(4);                                   
                histos.h<TProfile>("EEprof", "posZ:d1:IC")->SetMarkerStyle(20);                                  
                histos.h<TProfile>("EEprof", "posZ:d2:IC")->SetMarkerColor(2);                                   
                histos.h<TProfile>("EEprof", "posZ:d2:IC")->SetMarkerStyle(20);                                  
                histos.h<TProfile>("EEprof", "negZ:d3:IC")->SetMarkerColor(7);                                   
                histos.h<TProfile>("EEprof", "negZ:d3:IC")->SetMarkerStyle(20);                                  
                histos.h<TProfile>("EEprof", "negZ:d4:IC")->SetMarkerColor(6);
                histos.h<TProfile>("EEprof", "negZ:d4:IC")->SetMarkerStyle(20);
        }
        if ( outPlot_ ) {
                histos.save( outPlotFile_.c_str() );
        }
        if ( ofile_.is_open() ) {
                ofile_.close();
        }
}


// ------------ method called to for each event  ------------
void
DBDump::analyze(const edm::Event& ev, const edm::EventSetup& es)
{
        //es.get<EcalTimeCalibConstantsRcd>().get( tc_ );
        es.get<CaloGeometryRecord>().get(caloGeometry_);                                                
        geo_ = caloGeometry_.product();                                                                 

        bool atLeastOneDump = dumpIC_ || dumpChStatus_ || dumpPedestals_ || dumpTransp_ || dumpTranspCorr_;
        bool atLeastOnePlot = plotIC_ || plotChStatus_ || plotPedestals_ || plotTransp_ || plotTranspCorr_;
        if ( atLeastOneDump || atLeastOnePlot ) {

                if ( dumpIC_ || plotIC_ ) {
                        es.get<EcalIntercalibConstantsRcd>().get(ic_);
                        es.get<EcalIntercalibConstantsRcd>().get(icMC_);
                }
                if ( dumpChStatus_ || plotChStatus_ ) es.get<EcalChannelStatusRcd>().get(chStatus_);
                if ( dumpPedestals_ || plotPedestals_ ) es.get<EcalPedestalsRcd>().get(ped_);
                if ( dumpGainRatios_ || plotGainRatios_ ) es.get<EcalGainRatiosRcd>().get(gr_);
                if ( dumpTransp_ || plotTransp_ ) es.get<EcalLaserAPDPNRatiosRcd>().get(apdpn_);
                if ( dumpTranspCorr_ || plotTranspCorr_ ) es.get<EcalLaserDbRecord>().get(laser_);

                // if you need to average and use the average later on...
                //histos.h<TProfile>("etaProf", "p2")->Reset();
                for ( int hi = EBDetId::MIN_HASH; hi <= EBDetId::MAX_HASH; ++hi ) {
                        EBDetId ebId = EBDetId::unhashIndex( hi );
                        if ( ebId != EBDetId() ) {
                                EcalLaserAPDPNRatios::EcalLaserAPDPNRatiosMap::const_iterator itAPDPN;
                                EcalLaserAPDPNRatios::EcalLaserTimeStamp ts;
                                size_t iLM = 0;
                                if ( dumpTransp_ || plotTransp_ ) {
                                        itAPDPN = apdpn_->getLaserMap().find( ebId );
                                        iLM = getLMNumber( ebId );
                                        if ( iLM-1 < apdpn_->getTimeMap().size() ) {
                                                ts = apdpn_->getTimeMap()[iLM];
                                        }
                                }
                                //float transpCorr = -1234567890.;
                                float p2         = -1234567890.;
                                float eta = 99999;
                                char name[64];
                                sprintf(name, "%d_p2", ev.time().unixTime());
                                if (dumpTranspCorr_ || plotTranspCorr_) {
                                        //transpCorr = laser_->getLaserCorrection( ebId, ev.time());
                                        p2 = (*itAPDPN).p2;
                                        eta = geo_->getPosition(ebId).eta();
                                        histos.h<TProfile>("etaProf", name)->Fill(eta, p2);
                                }
                        }
                }
                for ( int hi = 0; hi < EEDetId::kSizeForDenseIndexing; ++hi ) {
                        EEDetId eeId = EEDetId::unhashIndex( hi );
                        if ( eeId != EEDetId() ) {
                                EcalLaserAPDPNRatios::EcalLaserAPDPNRatiosMap::const_iterator itAPDPN;
                                EcalLaserAPDPNRatios::EcalLaserTimeStamp ts;
                                size_t iLM = 0;
                                if ( dumpTransp_ || plotTransp_ ) {
                                        itAPDPN = apdpn_->getLaserMap().find( eeId );
                                        iLM = getLMNumber( eeId );
                                        if ( iLM-1 < apdpn_->getTimeMap().size() ) {
                                                ts = apdpn_->getTimeMap()[iLM];
                                        }
                                }
                                //float transpCorr = -1234567890.;
                                float p2         = -1234567890.;
                                float eta = 99999;
                                char name[64];
                                sprintf(name, "%d_p2", ev.time().unixTime());
                                if (dumpTranspCorr_ || plotTranspCorr_) {
                                        //transpCorr = laser_->getLaserCorrection( ebId, ev.time());
                                        p2 = (*itAPDPN).p2;
                                        eta = geo_->getPosition(eeId).eta();
                                        histos.h<TProfile>("etaProf", name)->Fill(eta, p2);
                                }
                        }
                }

                for ( int hi = EBDetId::MIN_HASH; hi <= EBDetId::MAX_HASH; ++hi ) {
                        EBDetId ebId = EBDetId::unhashIndex( hi );
                        if ( ebId != EBDetId() ) {
                                EcalIntercalibConstantMap::const_iterator itIC;
                                EcalIntercalibConstantMap::const_iterator itICMC;
                                if ( dumpIC_ || plotIC_ ) {
                                        itIC = ic_->find( ebId );
                                        itICMC = icMC_->find( ebId );
                                }
                                EcalChannelStatusMap::const_iterator itChStatus;
                                if ( dumpChStatus_ || plotChStatus_ ) itChStatus = chStatus_->find( ebId );
                                EcalPedestalsMap::const_iterator itPed;
                                if ( dumpPedestals_ || plotPedestals_ ) itPed = ped_->find( ebId );
                                EcalGainRatioMap::const_iterator itGR;
                                if ( dumpGainRatios_ || plotGainRatios_ ) itGR = gr_->find( ebId );
                                EcalLaserAPDPNRatios::EcalLaserAPDPNRatiosMap::const_iterator itAPDPN;
                                EcalLaserAPDPNRatios::EcalLaserTimeStamp ts;
                                size_t iLM = 0;
                                if ( dumpTransp_ || plotTransp_ ) {
                                        itAPDPN = apdpn_->getLaserMap().find( ebId );
                                        iLM = getLMNumber( ebId );
                                        if ( iLM-1 < apdpn_->getTimeMap().size() ) {
                                                ts = apdpn_->getTimeMap()[iLM];
                                        }
                                }
                                float transpCorr = -1234567890.;
                                if (dumpTranspCorr_ || plotTranspCorr_) {
                                        transpCorr = laser_->getLaserCorrection( ebId, ev.time());
                                }

                                ofile_ << "EB  rawId= " << ebId.rawId() 
                                        << "  ieta= " << ebId.ieta() 
                                        << "  iphi= " << ebId.iphi();
                                if ( dumpIC_ ) {
                                        if( itIC != ic_->end() ) ofile_ << "  ic= " << *itIC;
                                        else                     ofile_ << "  ic= " << "NA";
                                        if( itICMC != icMC_->end() ) ofile_ << "  icMC= " << *itIC;
                                        else                         ofile_ << "  icMC= " << "NA";
                                }
                                if ( dumpChStatus_ ) {
                                        if( itChStatus != chStatus_->end() ) ofile_ << "  chSt= " << (*itChStatus).getStatusCode();
                                        else                                 ofile_ << "  chSt= " << "NA";
                                }
                                if ( dumpPedestals_ ) {
                                        if( itPed != ped_->end() ) {
                                                ofile_ << "  ped_g12_g6_g1= " << (*itPed).mean(1) << "_" << (*itPed).mean(2) << "_" << (*itPed).mean(3);
                                                ofile_ << "  ped_rms_g12_g6_g1= " << (*itPed).rms(1) << "_" << (*itPed).rms(2) << "_" << (*itPed).rms(3);
                                        } else  ofile_ << "  ped= " << "NA";
                                }
                                if ( dumpGainRatios_ ) {
                                        if ( itGR != gr_->end() ) {
                                                ofile_ << "  gr_12/6_6/1= " << (*itGR).gain12Over6() << "_" << (*itGR).gain6Over1();
                                        }
                                }
                                if ( dumpTransp_ ) {
                                        if ( itAPDPN != apdpn_->getLaserMap().end() ) {
                                                ofile_ << "  evtTS_t1_t2_t3_p1_p2_p3= " 
                                                        << ev.time().value() 
                                                        << "_" << ts.t1.value() << "_" << ts.t2.value() 
                                                        << "_" << ts.t3.value() 
                                                        << "_" << (*itAPDPN).p1 << "_" << (*itAPDPN).p2 
                                                        << "_" << (*itAPDPN).p3
                                                        ;
                                        }
                                }
                                if (dumpTranspCorr_) {
                                        ofile_ << "  transpCorr= " 
                                                << transpCorr;
                                }
                                ofile_ << "\n";
                                // for plotting
                                int ieta = ebId.ieta();
                                int iphi = ebId.iphi();
                                if ( plotIC_ ) {
                                        //h_EB_ic->Fill( ieta, *itIC);
                                        if ( itIC != ic_->end() ) histos.h<TProfile>("EBprof", "IC")->Fill( ieta, *itIC);
                                        if ( itICMC != icMC_->end() ) histos.h<TProfile>("EBprof", "ICMC")->Fill( ieta, *itICMC);
                                }
                                if ( plotChStatus_ ) {
                                        //h_EB_chStatus->Fill( iphi, ieta, (*itChStatus).getStatusCode() );
                                        if ( itChStatus != chStatus_->end() ) histos.h<TH2D>( "EBh2", "chStatus" )->Fill( iphi, ieta, (*itChStatus).getStatusCode() );
                                }
                                if ( plotPedestals_ ) {
                                        if ( itPed != ped_->end() ) {
                                                histos.h<TH2D>( "EBh2", "ped_g12" )->Fill( iphi, ieta, (*itPed).mean(0) );
                                                histos.h<TH2D>( "EBh2", "ped_g6"  )->Fill( iphi, ieta, (*itPed).mean(1) );
                                                histos.h<TH2D>( "EBh2", "ped_g1"  )->Fill( iphi, ieta, (*itPed).mean(2) );
                                                histos.h<TH2D>( "EBh2", "ped_rms_g12" )->Fill( iphi, ieta, (*itPed).rms(0) );
                                                histos.h<TH2D>( "EBh2", "ped_rms_g6"  )->Fill( iphi, ieta, (*itPed).rms(1) );
                                                histos.h<TH2D>( "EBh2", "ped_rms_g1"  )->Fill( iphi, ieta, (*itPed).rms(2) );
                                        }
                                }
                                if ( plotGainRatios_ ) {
                                        if ( itGR != gr_->end() ) {
                                                histos.h<TH2D>( "EBh2", "gr_g12/6" )->Fill( iphi, ieta, (*itGR).gain12Over6() );
                                                histos.h<TH2D>( "EBh2", "gr_g6/1"  )->Fill( iphi, ieta, (*itGR).gain6Over1() );
                                        }
                                }
                                if (plotTranspCorr_) {
                                        histos.h<TH2D>( "EBh2", "transpCorr" )->Fill( iphi, ieta, transpCorr );
                                        float eta = geo_->getPosition(ebId).eta();
                                        char name[64];
                                        sprintf(name, "%d_p2", ev.time().unixTime());
                                        TProfile * p = histos.h<TProfile>("etaProf", name);
                                        float p2_mean = p->GetBinContent(p->FindBin(eta));
                                        histos.h<TH1D>("distr", "eta_normalised_p2")->Fill((*itAPDPN).p2 / p2_mean);
                                }
                        }
                }
                
                for ( int hi = 0; hi < EEDetId::kSizeForDenseIndexing; ++hi ) {
                        EEDetId eeId = EEDetId::unhashIndex( hi );
                        if ( eeId != EEDetId() ) {
                                EcalIntercalibConstantMap::const_iterator itIC;
                                EcalIntercalibConstantMap::const_iterator itICMC;
                                if ( dumpIC_ || plotIC_ ) {
                                        itIC = ic_->find( eeId );
                                        itICMC = icMC_->find( eeId );
                                }
                                EcalChannelStatusMap::const_iterator itChStatus;
                                if ( dumpChStatus_ || plotChStatus_ ) itChStatus = chStatus_->find( eeId );
                                EcalPedestalsMap::const_iterator itPed;
                                if ( dumpPedestals_ || plotPedestals_ ) itPed = ped_->find( eeId );
                                EcalGainRatioMap::const_iterator itGR;
                                if ( dumpGainRatios_ || plotGainRatios_ ) itGR = gr_->find( eeId );
                                EcalLaserAPDPNRatios::EcalLaserAPDPNRatiosMap::const_iterator itAPDPN;
                                EcalLaserAPDPNRatios::EcalLaserTimeStamp ts;
                                size_t iLM = 0;
                                if ( dumpTransp_ || plotTransp_ ) {
                                        itAPDPN = apdpn_->getLaserMap().find( eeId );
                                        iLM = getLMNumber( eeId );
                                        if ( iLM-1 < apdpn_->getTimeMap().size() ) {
                                                ts = apdpn_->getTimeMap()[iLM];
                                        }
                                }
                                float transpCorr = -1234567890.;
                                if (dumpTranspCorr_ || plotTranspCorr_) {
                                        transpCorr = laser_->getLaserCorrection( eeId, ev.time());
                                }

                                ofile_ << "EE  rawId= " << eeId.rawId() 
                                        << "  ix= " << eeId.ix() 
                                        << "  iy= " << eeId.ix()
                                        << "  iz= " << eeId.zside();

                                if ( dumpIC_ ) {
                                        if( itIC != ic_->end() ) ofile_ << "  ic= " << *itIC;
                                        else                     ofile_ << "  ic= " << "NA";
                                        if( itICMC != icMC_->end() ) ofile_ << "  icMC= " << *itICMC;
                                        else                         ofile_ << "  icMC= " << "NA";
                                }
                                if ( dumpChStatus_ ) {
                                        if( itChStatus != chStatus_->end() ) ofile_ << "  chSt= " << (*itChStatus).getStatusCode();
                                        else                                 ofile_ << "  chSt= " << "NA";
                                }
                                if ( dumpPedestals_ ) {
                                        if( itPed != ped_->end() ) {
                                                ofile_ << "  ped_g12_g6_g1= " << (*itPed).mean(1) << "_" << (*itPed).mean(2) << "_" << (*itPed).mean(3);
                                                ofile_ << "  ped_rms_g12_g6_g1= " << (*itPed).rms(1) << "_" << (*itPed).rms(2) << "_" << (*itPed).rms(3);
                                        } else  ofile_ << "  ped= " << "NA";
                                }
                                if ( dumpGainRatios_ ) {
                                        if ( itGR != gr_->end() ) {
                                                ofile_ << "  gr_12/6_6/1= " << (*itGR).gain12Over6() << "_" << (*itGR).gain6Over1();
                                        }
                                }
                                if ( dumpTransp_ ) {
                                        if ( itAPDPN != apdpn_->getLaserMap().end() ) {
                                                ofile_ << "  evtTS_t1_t2_t3_p1_p2_p3= " 
                                                        << ev.time().value() 
                                                        << "_" << ts.t1.value() << "_" << ts.t2.value() 
                                                        << "_" << ts.t3.value() 
                                                        << "_" << (*itAPDPN).p1 << "_" << (*itAPDPN).p2 
                                                        << "_" << (*itAPDPN).p3
                                                        ;
                                        }
                                }
                                if (dumpTranspCorr_) {
                                        ofile_ << "  transpCorr= " 
                                                << transpCorr;
                                }
                                ofile_ << "\n";
                                int ix = eeId.ix();
                                int iy = eeId.iy();
                                int id = eeId.ix() - 50;
                                float r = sqrt( (ix-50)*(ix-50) + (iy-50)*(iy-50) );
                                if ( eeId.zside() > 0 ) {
                                        if ( plotIC_ ) {
                                                if ( itIC != ic_->end() ) {
                                                        if ( id < 0 ) histos.h<TProfile>("EEprof", "posZ:d1:IC")->Fill( r, *itIC );
                                                        else          histos.h<TProfile>("EEprof", "posZ:d2:IC")->Fill( r, *itIC );
                                                }
                                                if ( itICMC != icMC_->end() ) {
                                                        if ( id < 0 ) histos.h<TProfile>("EEprof", "posZ:d1:ICMC")->Fill( r, *itICMC );
                                                        else          histos.h<TProfile>("EEprof", "posZ:d2:ICMC")->Fill( r, *itICMC );
                                                }
                                        }
                                        if ( plotChStatus_ ) {
                                                if ( itChStatus != chStatus_->end() ) {
                                                        if ( id < 0 ) histos.h<TH2D>("EEh2","posZ:d1:chStatus")->Fill( ix, iy, (*itChStatus).getStatusCode() );
                                                        else          histos.h<TH2D>("EEh2","posZ:d2:chStatus")->Fill( ix, iy, (*itChStatus).getStatusCode() );
                                                }
                                        }
                                        if ( plotPedestals_ ) {
                                                if ( itPed != ped_->end() ) {
                                                        if ( id < 0 ) {
                                                                histos.h<TH2D>("EEh2", "posZ:d1:ped_g12")->Fill( ix, iy, (*itPed).mean(0) );
                                                                histos.h<TH2D>("EEh2", "posZ:d1:ped_g6" )->Fill( ix, iy, (*itPed).mean(1) );
                                                                histos.h<TH2D>("EEh2", "posZ:d1:ped_g1" )->Fill( ix, iy, (*itPed).mean(2) );
                                                                histos.h<TH2D>("EEh2", "posZ:d1:ped_rms_g12")->Fill( ix, iy, (*itPed).rms(0) );
                                                                histos.h<TH2D>("EEh2", "posZ:d1:ped_rms_g6" )->Fill( ix, iy, (*itPed).rms(1) );
                                                                histos.h<TH2D>("EEh2", "posZ:d1:ped_rms_g1" )->Fill( ix, iy, (*itPed).rms(2) );
                                                        } else {
                                                                histos.h<TH2D>("EEh2", "posZ:d2:ped_g12")->Fill( ix, iy, (*itPed).mean(0) );
                                                                histos.h<TH2D>("EEh2", "posZ:d2:ped_g6" )->Fill( ix, iy, (*itPed).mean(1) );
                                                                histos.h<TH2D>("EEh2", "posZ:d2:ped_g1" )->Fill( ix, iy, (*itPed).mean(2) );
                                                                histos.h<TH2D>("EEh2", "posZ:d2:ped_rms_g12")->Fill( ix, iy, (*itPed).rms(0) );
                                                                histos.h<TH2D>("EEh2", "posZ:d2:ped_rms_g6" )->Fill( ix, iy, (*itPed).rms(1) );
                                                                histos.h<TH2D>("EEh2", "posZ:d2:ped_rms_g1" )->Fill( ix, iy, (*itPed).rms(2) );
                                                        }
                                                }
                                        }
                                        if ( plotGainRatios_ ) {
                                                if ( itGR != gr_->end() ) {
                                                        if ( id > 0 ) {
                                                                histos.h<TH2D>("EEh2", "negZ:d1:gr12/6")->Fill( ix, iy, (*itGR).gain12Over6() );
                                                                histos.h<TH2D>("EEh2", "negZ:d1:gr6/1" )->Fill( ix, iy, (*itGR).gain6Over1() );
                                                        } else {
                                                                histos.h<TH2D>("EEh2", "negZ:d2:gr12/6")->Fill( ix, iy, (*itGR).gain12Over6() );
                                                                histos.h<TH2D>("EEh2", "negZ:d2:gr6/1" )->Fill( ix, iy, (*itGR).gain6Over1() );
                                                        }
                                                }
                                        }
                                        if (plotTranspCorr_) {
                                                char name[64];
                                                sprintf(name, "%d_p2", ev.time().unixTime());
                                                float eta = geo_->getPosition(eeId).eta();
                                                TProfile * p = histos.h<TProfile>("etaProf", name);
                                                float p2_mean = p->GetBinContent(p->FindBin(eta));
                                                histos.h<TH1D>("distr", "eta_normalised_p2")->Fill((*itAPDPN).p2 / p2_mean);
                                                if ( id > 0 ) {
                                                        histos.h<TH2D>("EEh2", "negZ:d1:transpCorr")->Fill( ix, iy, transpCorr );
                                                } else {
                                                        histos.h<TH2D>("EEh2", "negZ:d2:transpCorr")->Fill( ix, iy, transpCorr );
                                                }
                                        }
                                } else {
                                        if ( plotIC_ ) {
                                                if ( itIC != ic_->end() ) {
                                                        if ( id > 0 ) histos.h<TProfile>("EEprof", "negZ:d3:IC")->Fill( r, *itIC );
                                                        else          histos.h<TProfile>("EEprof", "negZ:d4:IC")->Fill( r, *itIC );
                                                }
                                                if ( itICMC != icMC_->end() ) {
                                                        if ( id > 0 ) histos.h<TProfile>("EEprof", "negZ:d3:ICMC")->Fill( r, *itICMC );
                                                        else          histos.h<TProfile>("EEprof", "negZ:d4:ICMC")->Fill( r, *itICMC );
                                                }
                                        }
                                        if ( plotChStatus_ ) {
                                                if ( id < 0 ) histos.h<TH2D>("EEh2","negZ:d3:chStatus")->Fill( ix, iy, (*itChStatus).getStatusCode() );
                                                else          histos.h<TH2D>("EEh2","negZ:d4:chStatus")->Fill( ix, iy, (*itChStatus).getStatusCode() );
                                        }
                                        if ( plotPedestals_ ) {
                                                if ( id > 0 ) {
                                                        histos.h<TH2D>("EEh2", "negZ:d3:ped_g12")->Fill( ix, iy, (*itPed).mean(0) );
                                                        histos.h<TH2D>("EEh2", "negZ:d3:ped_g6" )->Fill( ix, iy, (*itPed).mean(1) );
                                                        histos.h<TH2D>("EEh2", "negZ:d3:ped_g1" )->Fill( ix, iy, (*itPed).mean(2) );
                                                        histos.h<TH2D>("EEh2", "negZ:d3:ped_rms_g12")->Fill( ix, iy, (*itPed).rms(0) );
                                                        histos.h<TH2D>("EEh2", "negZ:d3:ped_rms_g6" )->Fill( ix, iy, (*itPed).rms(1) );
                                                        histos.h<TH2D>("EEh2", "negZ:d3:ped_rms_g1" )->Fill( ix, iy, (*itPed).rms(2) );
                                                } else {
                                                        histos.h<TH2D>("EEh2", "negZ:d4:ped_g12")->Fill( ix, iy, (*itPed).mean(0) );
                                                        histos.h<TH2D>("EEh2", "negZ:d4:ped_g6" )->Fill( ix, iy, (*itPed).mean(1) );
                                                        histos.h<TH2D>("EEh2", "negZ:d4:ped_g1" )->Fill( ix, iy, (*itPed).mean(2) );
                                                        histos.h<TH2D>("EEh2", "negZ:d4:ped_rms_g12")->Fill( ix, iy, (*itPed).rms(0) );
                                                        histos.h<TH2D>("EEh2", "negZ:d4:ped_rms_g6" )->Fill( ix, iy, (*itPed).rms(1) );
                                                        histos.h<TH2D>("EEh2", "negZ:d4:ped_rms_g1" )->Fill( ix, iy, (*itPed).rms(2) );
                                                }
                                        }
                                        if ( plotGainRatios_ ) {
                                                if ( id > 0 ) {
                                                        histos.h<TH2D>("EEh2", "negZ:d3:gr12/6")->Fill( ix, iy, (*itGR).gain12Over6() );
                                                        histos.h<TH2D>("EEh2", "negZ:d3:gr6/1" )->Fill( ix, iy, (*itGR).gain6Over1() );
                                                } else {
                                                        histos.h<TH2D>("EEh2", "negZ:d4:gr12/6")->Fill( ix, iy, (*itGR).gain12Over6() );
                                                        histos.h<TH2D>("EEh2", "negZ:d4:gr6/1" )->Fill( ix, iy, (*itGR).gain6Over1() );
                                                }
                                        }
                                        if (plotTranspCorr_) {
                                                float eta = geo_->getPosition(eeId).eta();
                                                char name[64];
                                                sprintf(name, "%d_p2", ev.time().unixTime());
                                                TProfile * p = histos.h<TProfile>("etaProf", name);
                                                float p2_mean = p->GetBinContent(p->FindBin(eta));
                                                histos.h<TH1D>("distr", "eta_normalised_p2")->Fill((*itAPDPN).p2 / p2_mean);
                                                if ( id > 0 ) {
                                                        histos.h<TH2D>("EEh2", "negZ:d3:transpCorr")->Fill( ix, iy, transpCorr );
                                                } else {
                                                        histos.h<TH2D>("EEh2", "negZ:d4:transpCorr")->Fill( ix, iy, transpCorr );
                                                }
                                        }
                                }
                        }
                }
        }
        if ( dumpADCToGeV_ ) {
                es.get<EcalADCToGeVConstantRcd>().get(adcToGeV_);
                ofile_ << "ADCToGeV  EB= " << adcToGeV_->getEBValue() << "  EE= " << adcToGeV_->getEEValue() << "\n";
        }
}


// ------------ method called once each job just before starting event loop  ------------
void 
DBDump::beginJob()
{
}

// ------------ method called once each job just after ending the event loop  ------------
void 
DBDump::endJob() {
}

int DBDump::getLMNumber(DetId const & xid) const {

        int iLM = 0;

        if (xid.subdetId()==EcalBarrel) {

                EBDetId tempid(xid.rawId());

                int iSM  = tempid.ism();
                int iETA = tempid.ietaSM();
                int iPHI = tempid.iphiSM();

                const int nSM = 36;     
                int numLM[nSM] = {37, 39, 41, 43, 45, 47, 49, 51, 53, 55, 57, 59, 61, 63, 65, 67, 69, 71, 1, 3, 5, 7, 9, 11, 13, 15, 17, 19, 21, 23, 25, 27, 29, 31, 33, 35};

                if (iSM<=nSM) {
                        iLM = numLM[iSM-1];
                } 
                // now assign light module within SM
                if (iPHI>10&&iETA>5) { iLM++; }   

                //    std::cout << " SM , LM ---> " << iSM << " " << iLM << std::endl;

        } else if (xid.subdetId()==EcalEndcap) {

                EEDetId tempid(xid.rawId());

                int iSC = tempid.isc();
                int iX  = tempid.ix();
                //    int iY  = tempid.iy();
                int iZ  = tempid.zside();

                const int nSC = 312; 
                const int nEELM = 18;

                // Z+ side

                int indexSCpos[nSC] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 65, 66, 67, 68, 69, 71, 72, 73, 74, 75, 76, 77, 78, 79, 80, 81, 82, 83, 84, 85, 86, 87, 88, 89, 90, 91, 92, 93, 94, 95, 96, 97, 98, 99, 100, 101, 102, 103, 104, 105, 106, 107, 108, 109, 110, 111, 112, 113, 114, 115, 116, 117, 118, 119, 120, 121, 122, 123, 124, 125, 126, 127, 128, 129, 130, 131, 132, 133, 134, 135, 136, 137, 138, 139, 140, 141, 142, 143, 144, 145, 146, 147, 148, 150, 151, 152, 153, 154, 155, 156, 157, 158, 159, 160, 161, 162, 163,164, 165, 166, 167, 168, 169, 170, 171, 172, 173, 174, 175, 176, 177, 178, 179, 180, 181, 182, 183, 184, 185, 186, 187, 188, 189, 190, 191, 192, 193, 194, 195, 196, 197, 198, 199, 200, 201, 202, 203, 204, 205, 206, 207, 208, 209, 210, 211, 212, 213, 214, 215, 216, 217, 218, 219, 220, 221, 222, 223, 224, 225, 226, 227, 229, 230, 231, 232, 233, 234, 235, 236, 237, 238, 239, 240, 241, 242, 243, 244, 245, 246, 247, 248, 249, 250, 251, 252, 253, 254, 255, 256, 257, 258, 259, 260, 261, 262, 263, 264, 265, 266, 267, 268, 269, 270, 271, 272, 273, 274, 275, 276, 277, 278, 279, 280, 281, 282, 283, 284, 285, 286, 287, 288, 289, 290, 291, 292, 293, 294, 295, 296, 297, 298, 299, 300, 301, 302, 303, 304, 305, 306, 308, 309, 310, 311, 312, 313, 314, 315, 316 };

                int indexDCCpos[nSC] = { 48, 48, 48, 48, 48, 48, 48, 48, 47, 48, 48, 48, 48, 48, 48, 48, 48, 47, 47, 47, 48, 48, 48, 48, 48, 48, 48, 46, 47, 47, 47, 48, 48, 48, 48,48, 46, 47, 47, 47, 47, 48, 48, 48, 48, 46, 47, 47, 47, 47, 47, 47, 48, 48, 46, 47, 47, 47, 47, 47, 47, 47, 46, 47, 47, 47, 47, 47, 47, 46, 46, 47, 47, 47, 47, 46, 46, 47, 49, 49, 49, 49, 49, 49, 49, 49, 50, 49, 49, 49, 49, 49, 49, 49, 49, 50, 50, 50, 49, 49, 49, 49, 49, 49, 49, 51, 50, 50, 50, 49, 49, 49, 49, 49, 51, 50, 50, 50, 50, 49, 49, 49, 49, 51, 50, 50, 50, 50, 50, 50, 49, 49, 51, 50, 50, 50, 50, 50, 50, 50, 51, 50, 50, 50, 50, 50, 50, 51, 51, 50, 50, 50, 50, 51, 51, 50, 53, 53, 53, 53, 53, 53, 53, 53, 52, 52, 52, 53, 53, 53, 53, 53, 53, 51, 52, 52, 52, 52, 52, 52, 53, 53, 53, 51, 51, 52, 52, 52, 52, 52, 52, 52, 51, 51, 51, 52, 52, 52, 52, 52, 52, 51, 51, 51, 52, 52, 52, 52, 52, 52,51, 51, 51, 51, 52, 52, 52, 52, 51, 51, 51, 51, 52, 52, 52, 51, 51, 51, 51, 51, 52, 51, 51, 51, 53, 53, 53, 53, 53, 53, 53, 53, 54, 54, 54,53, 53, 53, 53, 53, 53, 46, 54, 54, 54, 54, 54, 54, 53, 53, 53, 46, 46, 54, 54, 54, 54, 54, 54, 54, 46, 46, 46, 54, 54, 54, 54, 54, 54, 46, 46, 46, 54, 54, 54, 54, 54, 54, 46, 46, 46, 46, 54, 54, 54, 54, 46, 46, 46, 46, 54, 54, 54, 46, 46, 46, 46, 46, 54, 46, 46, 46 };

                // Z- side

                int indexSCneg[nSC] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37,38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 65, 66, 67, 68, 69, 71, 72, 73,74, 75, 76, 77, 78, 79, 80, 81, 82, 83, 84, 85, 86, 87, 88, 89, 90, 91, 92, 93, 94, 95, 96, 97, 98, 99, 100, 101, 102, 103, 104, 105, 106, 107, 108, 109, 110, 111, 112, 113, 114, 115, 116, 117, 118, 119, 120, 121, 122, 123, 124, 125, 126, 127, 128, 129, 130, 131, 132, 133, 134, 135, 136, 137, 138, 139, 140, 141, 142, 143, 144, 145, 146, 147, 148, 150, 151, 152, 153, 154, 155, 156, 157, 158, 159, 160, 161, 162, 163, 164, 165, 166, 167, 168, 169, 170, 171, 172, 173, 174, 175, 176, 177, 178, 179, 180, 181, 182, 183, 184, 185, 186, 187, 188, 189, 190, 191, 192, 193, 194, 195, 196, 197, 198, 199, 200, 201, 202, 203, 204, 205, 206, 207, 208, 209, 210, 211, 212, 213, 214, 215, 216, 217, 218, 219, 220, 221, 222, 223, 224, 225, 226, 227, 229, 230, 231, 232, 233, 234, 235, 236, 237, 238, 239, 240, 241, 242, 243, 244, 245, 246, 247, 248, 249, 250, 251, 252, 253, 254, 255, 256, 257, 258, 259, 260, 261, 262, 263, 264, 265, 266, 267, 268, 269, 270, 271, 272, 273, 274, 275, 276, 277, 278, 279, 280, 281, 282, 283, 284, 285, 286, 287, 288, 289, 290, 291, 292, 293, 294, 295, 296, 297, 298, 299, 300, 301, 302, 303, 304, 305, 306, 308, 309, 310, 311, 312, 313, 314, 315, 316 };

                int indexDCCneg[nSC] = { 3, 3, 3, 3, 3, 3, 3, 3, 2, 3, 3, 3, 3, 3, 3, 3, 3, 2, 2, 2, 3, 3, 3, 3, 3, 3, 3, 1, 2, 2, 2, 3, 3, 3, 3, 3, 1, 2, 2, 2, 2, 3, 3, 3, 3, 1, 2, 2, 2, 2, 2, 2, 3, 3, 1, 2, 2, 2, 2, 2, 2, 2, 1, 2, 2, 2, 2, 2, 2, 1, 1, 2, 2, 2, 2, 1, 1, 2, 4, 4, 4, 4, 4, 4, 4, 4, 5, 4, 4, 4, 4, 4, 4, 4, 4, 5, 5, 5, 4, 4, 4, 4, 4, 4, 4, 6, 5, 5, 5, 4, 4, 4, 4, 4, 6, 5, 5, 5, 5, 4, 4, 4, 4, 6, 5, 5, 5, 5, 5, 5, 4, 4, 6, 5, 5, 5, 5, 5, 5, 5, 6, 5, 5, 5, 5, 5, 5, 6, 6, 5, 5, 5, 5, 6, 6, 5, 8, 8, 8, 8, 8, 8, 8, 8, 7, 7, 7, 8, 8, 8, 8, 8, 8, 6, 7, 7, 7, 7, 7, 7, 8, 8, 8, 6, 6, 7, 7, 7, 7, 7, 7, 7, 6, 6, 6, 7, 7, 7, 7, 7, 7, 6, 6, 6, 7, 7, 7, 7, 7, 7, 6, 6, 6, 6, 7, 7, 7, 7, 6, 6, 6, 6, 7, 7, 7, 6, 6, 6, 6, 6,7, 6, 6, 6, 8, 8, 8, 8, 8, 8, 8, 8, 9, 9, 9, 8, 8, 8, 8, 8, 8, 1, 9, 9, 9, 9, 9, 9, 8, 8, 8, 1, 1, 9, 9, 9, 9, 9, 9, 9, 1, 1, 1, 9, 9, 9, 9, 9, 9, 1, 1, 1, 9, 9, 9, 9, 9, 9, 1, 1, 1, 1, 9, 9, 9, 9, 1, 1, 1, 1, 9, 9, 9, 1, 1, 1, 1, 1, 9, 1, 1, 1 };

                int numDCC[nEELM] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 46, 47, 48, 49, 50, 51, 52, 53, 54 };
                int numLM[nEELM] = { 73, 74, 75, 76, 77, 78, 79, 80, 82, 83, 84, 85, 86, 87, 88, 89, 90, 92 };

                int tempdcc = 0;

                // assign SC to DCC
                if (iZ>0) {
                        for (int i=0; i<nSC; i++) {
                                if (indexSCpos[i]==iSC) {
                                        tempdcc = indexDCCpos[i];
                                        break;
                                }      
                        }
                } else {
                        for (int i=0; i<nSC; i++) {
                                if (indexSCneg[i]==iSC) {
                                        tempdcc = indexDCCneg[i];
                                        break;
                                }      
                        }
                }

                // now assign LM number based on DCC
                for (int j=0; j<nEELM; j++) {
                        if (tempdcc==numDCC[j]) {
                                iLM = numLM[j];
                                break;
                        } 
                }

                // now assign LM readout number for two exceptional cases:
                if (tempdcc==53&&iX>50) {
                        iLM++;
                } else if (tempdcc==8&&iX>50) {
                        iLM++;
                }

                //    std::cout << "DCC , LM ---> " << tempdcc << " " << iLM << std::endl;
                //    std::cout << "SC DCC : LM : XYZ ---> " << iSC << " \t" << tempdcc << ": \t" << iLM << ": \t" << iX << " \t" << iY << " \t" << iZ << std::endl;

                //    iLM = tempid.isc();
        } else {
                edm::LogError("EcalLaserDbService") << " getLMNumber: DetId is not in ECAL." << std::endl;

        }

        return iLM;

}

//define this as a plug-in
DEFINE_FWK_MODULE(DBDump);
