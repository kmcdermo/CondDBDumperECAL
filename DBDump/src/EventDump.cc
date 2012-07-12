// -*- mode: c++; c-basic-offset:8; -*-
//
// Package:    EventDump
// Class:      EventDump
// 
/**\class EventDump EventDump.cc MyPackage/EventDump/src/EventDump.cc

Description: <one line class summary>

Implementation:
<Notes on implementation>
 */
//
// Original Author:  Federico FERRI
//         Created:  Fri May 11 11:06:00 CEST 2012
// $Id:  $
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

#include "CondFormats/EcalObjects/interface/EcalADCToGeVConstant.h"
#include "CondFormats/EcalObjects/interface/EcalChannelStatus.h"
#include "CondFormats/EcalObjects/interface/EcalGainRatios.h"
#include "CondFormats/EcalObjects/interface/EcalIntercalibConstants.h"
#include "CondFormats/EcalObjects/interface/EcalIntercalibConstantsMC.h"
#include "CondFormats/EcalObjects/interface/EcalLaserAPDPNRatios.h"
#include "CondFormats/EcalObjects/interface/EcalLaserAPDPNRatiosRef.h"
#include "CondFormats/EcalObjects/interface/EcalLaserAlphas.h"
#include "CondFormats/EcalObjects/interface/EcalPedestals.h"
#include "CondFormats/EcalObjects/interface/EcalTimeCalibConstants.h"

#include "CondFormats/DataRecord/interface/EcalADCToGeVConstantRcd.h"
#include "CondFormats/DataRecord/interface/EcalChannelStatusRcd.h"
#include "CondFormats/DataRecord/interface/EcalGainRatiosRcd.h"
#include "CondFormats/DataRecord/interface/EcalIntercalibConstantsMCRcd.h"
#include "CondFormats/DataRecord/interface/EcalIntercalibConstantsRcd.h"
#include "CondFormats/DataRecord/interface/EcalLaserAPDPNRatiosRcd.h"
#include "CondFormats/DataRecord/interface/EcalLaserAPDPNRatiosRefRcd.h"
#include "CondFormats/DataRecord/interface/EcalLaserAlphasRcd.h"
#include "CondFormats/DataRecord/interface/EcalPedestalsRcd.h"
#include "CondFormats/DataRecord/interface/EcalTimeCalibConstantsRcd.h"

#include "CalibCalorimetry/EcalLaserCorrection/interface/EcalLaserDbService.h"
#include "CalibCalorimetry/EcalLaserCorrection/interface/EcalLaserDbRecord.h"

#include "DataFormats/EcalRecHit/interface/EcalRecHitCollections.h"

#include "Geometry/CaloEventSetup/interface/CaloTopologyRecord.h"
#include "Geometry/CaloGeometry/interface/CaloGeometry.h"

#include "CalibCalorimetry/EcalLaserAnalyzer/interface/MEEBGeom.h"
#include "CalibCalorimetry/EcalLaserAnalyzer/interface/MEEEGeom.h"

#include <Geometry/EcalMapping/interface/EcalElectronicsMapping.h>
#include <Geometry/EcalMapping/interface/EcalMappingRcd.h>

#include "Geometry/CaloTopology/interface/CaloTopology.h"
#include "Geometry/CaloGeometry/interface/CaloCellGeometry.h"
#include "Geometry/CaloGeometry/interface/CaloSubdetectorGeometry.h"

#include "FWCore/ParameterSet/interface/ParameterSet.h"

#include "HistoManager.h"

#include "TProfile.h"
#include "TProfile2D.h"
#include "TH1D.h"
#include "TH2D.h"
#include "TGraph.h"
#include "TGraphAsymmErrors.h"

class EventDump : public edm::EDAnalyzer {
   public:
      explicit EventDump(const edm::ParameterSet&);
      ~EventDump();
      void setDumpFalse();

   private:
      virtual void beginJob() ;
      virtual void analyze(const edm::Event&, const edm::EventSetup&);
      //virtual void beginRun(const edm::Event&, const edm::EventSetup&);
      //virtual void endRun(const edm::Event&, const edm::EventSetup&);
      virtual void endJob() ;

      void dumpRecHits(const edm::Event& ev, const edm::EventSetup& es);
      void dumpRecHits(const edm::Event& ev, const edm::EventSetup& es, const edm::Handle<EcalRecHitCollection> & rechits);

      typedef struct Coord {
              int ix_;
              int iy_;
              int iz_;
      } Coord;
      void coord(DetId id, struct Coord * c);

      void printSummary();

      // output files
      bool outDump_;
      std::string outDumpFile_;
      std::ofstream ofile_;

      FILE * fd_dump_;

      // geometry
      bool dumpGeometry_;

      // inter-calibration constants
      bool dumpIC_;
      edm::ESHandle<EcalIntercalibConstants> ic_;
      edm::ESHandle<EcalIntercalibConstantsMC> icMC_;

      // time-calibration constants
      bool dumpTC_;
      edm::ESHandle<EcalTimeCalibConstants> tc_;

      // ADCToGeV constant
      bool dumpADCToGeV_;
      edm::ESHandle<EcalADCToGeVConstant> adcToGeV_;

      // laser transparency measurements
      bool dumpTransp_;
      edm::ESHandle<EcalLaserAPDPNRatios> apdpn_;
      edm::ESHandle<EcalLaserAPDPNRatiosRef> apdpnref_;

      // laser alpha
      bool dumpAlpha_;
      edm::ESHandle<EcalLaserAlphas> alpha_;

      // laser transparency corrections
      bool dumpTranspCorr_;
      edm::ESHandle<EcalLaserDbService> laser_;

      // channel status map
      bool dumpChStatus_;
      edm::ESHandle<EcalChannelStatus> chStatus_;

      // pedestals
      bool dumpPedestals_;
      edm::ESHandle<EcalPedestals> ped_;

      // gain ratios
      bool dumpGainRatios_;
      edm::ESHandle<EcalGainRatios> gr_;

      // recHits
      bool dumpRecHits_;

      // Input tags
      // EcalRecHits
      edm::InputTag ieb_rechits_;
      edm::InputTag iee_rechits_;
      edm::Handle<EcalRecHitCollection> heb_rechits_;
      edm::Handle<EcalRecHitCollection> hee_rechits_;

      // conditions: iterators
      EcalIntercalibConstantMap::const_iterator it_ic_;
      EcalChannelStatusMap::const_iterator it_chstatus_;
      EcalPedestalsMap::const_iterator it_ped_;
      EcalGainRatioMap::const_iterator it_gr_;
      EcalLaserAPDPNRatios::EcalLaserAPDPNRatiosMap::const_iterator it_apdpn_;
      EcalLaserAPDPNRatiosRef::const_iterator it_apdpnref_;
      EcalLaserAlphas::const_iterator it_alpha_;
      EcalLaserAPDPNRatios::EcalLaserTimeStamp ts_;

      HistoManager histos;

      //edm::ESHandle<CaloGeometry> caloGeometry_;
      const CaloGeometry * geometry;

      std::vector<DetId> ecalDetIds_;

      // number of IOVs, first and last
      int niov_;
      time_t iov_first_;
      time_t iov_last_;

      // job start and stop time
      time_t jstart_;
      time_t jstop_;

      time_t il_;

      int run_, event_, time_, ls_;

      bool first_;

      FILE * fgeom;
};



EventDump::EventDump(const edm::ParameterSet& ps) :
        niov_(0),
        iov_first_(-1),
        iov_last_(0),
        jstart_(time(NULL)),
        jstop_(0),
        il_(0),
        first_(false) // do not pass if not needed
{
	outDump_ = ps.getParameter<bool>("outDump");
	outDumpFile_ = ps.getParameter<std::string>("outDumpFile");
        // geometry
        dumpGeometry_ = ps.getParameter<bool>("dumpGeometry");
	// inter-calibration constants
	dumpIC_ = ps.getParameter<bool>("dumpIC");
	// time-calibration constants
	dumpTC_ = ps.getParameter<bool>("dumpTC");
	// ADCToGeV constant
	dumpADCToGeV_ = ps.getParameter<bool>("dumpADCToGeV");
	// laser transparency measurements
	dumpTransp_ = ps.getParameter<bool>("dumpTransp");
	// laser alpha
	dumpAlpha_ = ps.getParameter<bool>("dumpAlpha");
	// channel status map
	dumpChStatus_ = ps.getParameter<bool>("dumpChStatus");
	// pedestals
	dumpPedestals_ = ps.getParameter<bool>("dumpPedestals");
	// gain ratios
	dumpGainRatios_ = ps.getParameter<bool>("dumpGainRatios");
	// laser transparency corrections
	dumpTranspCorr_ = ps.getParameter<bool>("dumpTranspCorr");

	// laser transparency corrections
	dumpRecHits_ = ps.getParameter<bool>("dumpRecHits");

        ieb_rechits_ = ps.getParameter<edm::InputTag>("ecalRecHitsEB");
        iee_rechits_ = ps.getParameter<edm::InputTag>("ecalRecHitsEE");

	if (outDumpFile_ != "") {
		ofile_.open(outDumpFile_.c_str(), std::ios::out);
		if (!ofile_.is_open()) {
			edm::LogError("EventDump") << "EventDump::EventDump cannot open output file " << outDumpFile_;
			setDumpFalse();
		}
	} else if (outDump_) {
		edm::LogError("EventDump") << "EventDump::EventDump mismatch in your config file, setting outDump to false.";
		setDumpFalse();
	} else {
		setDumpFalse();
	}

        fd_dump_ = fopen(outDumpFile_.c_str(), "w");

	// initialise ECAL DetId vector
	for (int hi = EBDetId::MIN_HASH; hi <= EBDetId::MAX_HASH; ++hi ) {
		EBDetId ebId = EBDetId::unhashIndex(hi);
		if (ebId != EBDetId()) {
			ecalDetIds_.push_back(ebId);
		}
	}
	for ( int hi = 0; hi < EEDetId::kSizeForDenseIndexing; ++hi ) {
		EEDetId eeId = EEDetId::unhashIndex(hi);
		if (eeId != EEDetId()) {
			ecalDetIds_.push_back(eeId);
		}
	}
	assert(ecalDetIds_.size() == 75848);
}

void EventDump::setDumpFalse()
{
        dumpRecHits_ = false;

        dumpGeometry_ = false;
	dumpIC_ = false;
	dumpTC_ = false;
	dumpADCToGeV_ = false;
	dumpTransp_ = false;
        dumpAlpha_ = false;
	dumpTranspCorr_ = false;
	dumpChStatus_ = false;
	dumpPedestals_ = false;
}

EventDump::~EventDump()
{
        if (ofile_.is_open()) {
                ofile_.close();
        }
        jstop_ = time(NULL);
        printSummary();
}

void EventDump::printSummary()
{
	//char buf[128];
	//strftime(buf, sizeof(buf), "%F %R:%S", gmtime(&iov_first_));
	//printf("First IOV: %ld (%s UTC)\n", iov_first_, buf);
	//strftime(buf, sizeof(buf), "%F %R:%S", gmtime(&iov_last_));
	//printf(" Last IOV: %ld (%s UTC)\n", iov_last_, buf);
}


void EventDump::coord(DetId id, struct Coord * c)
{
        if (id.subdetid() == EcalBarrel) {
                EBDetId eid(id);
                c->ix_ = eid.ieta();
                c->iy_ = eid.iphi();
                c->iz_ = 0;
        } else if (id.subdetid() == EcalEndcap) {
                EEDetId eid(id);
                c->ix_ = eid.ix();
                c->iy_ = eid.iy();
                c->iz_ = eid.zside();
        } else {
                fprinft(stderr, "[EventDump::coord] ERROR: invalid DetId %d", id.rawId());
                assert(0);
        }
}


void EventDump::dumpRecHits(const edm::Event& ev, const edm::EventSetup& es, const edm::Handle<EcalRecHitCollection> & rechits)
{
        // dump header
        fprintf(fd_dump_, "#erh run event ls time\trawid ieta/ix iphi/iy 0/zside\tenergy time flag");
        fprintf(fd_dump_, "\tic lc");
        fprintf(fd_dump_, "\n");
        Coord c;
        for(size_t irh =0; irh < rechits->size(); ++irh) {
                EcalRecHitRef rhref(rechits, irh);
                DetId id(rhref->id());
                coord(id, &c);
                fprintf(fd_dump_, "erh %d %d %d %d\t%d %d %d %d\t%f %f %d", 
                        run_, event_, ls_, time_,
                        id.rawId(), c.ix_, c.iy_, c.iz_,
                        rhref->energy(), rhref->time(), rhref->recoFlag()
                        );
                it_ic_ = ic_->find(id);
                float lc = laser_->getLaserCorrection(id, ev.time());
                if (it_ic_ == ic_->end()) {
                        fprintf(stderr, "problem with IC for detId %d", id.rawId());
                        assert(0);
                }
                fprintf(fd_dump_, "\t%f %f", *it_ic_, lc);
                fprintf(fd_dump_, "\t%f  %d %f  %d %f  %d %f", );
                fprintf(fd_dump_, "\n");
        }
}


void EventDump::dumpRecHits(const edm::Event& ev, const edm::EventSetup& es)
{
        ev.getByLabel(ieb_rechits_, heb_rechits_);
        dumpRecHits(ev, es, heb_rechits_);

        ev.getByLabel(iee_rechits_, hee_rechits_);
        dumpRecHits(ev, es, hee_rechits_);
        // Preshower
        //ev.getByLabel( ecalRecHitESCollection_, esRecHits_h_ );
}


// ------------ method called to for each event  ------------
void
EventDump::analyze(const edm::Event& ev, const edm::EventSetup& es)
{
        run_   = ev.id().run();
        event_ = ev.id().event();
        time_  = ev.time().unixTime();
        ls_    = ev.id().luminosityBlock();

        //es.get<EcalTimeCalibConstantsRcd>().get( tc_ );
        //es.get<CaloGeometryRecord>().get(caloGeometry_);
        //geo_ = caloGeometry_.product();
        edm::ESHandle< EcalElectronicsMapping > ecalmapping;
        es.get<EcalMappingRcd>().get(ecalmapping);

        edm::ESHandle<CaloGeometry> pGeometry;
        es.get<CaloGeometryRecord>().get(pGeometry);
        geometry = pGeometry.product();

        // get conditions
        es.get<EcalIntercalibConstantsRcd>().get(ic_);
        es.get<EcalIntercalibConstantsRcd>().get(icMC_);
        es.get<EcalChannelStatusRcd>().get(chStatus_);
        es.get<EcalPedestalsRcd>().get(ped_);
        es.get<EcalGainRatiosRcd>().get(gr_);
        es.get<EcalLaserAPDPNRatiosRcd>().get(apdpn_);
        es.get<EcalLaserAPDPNRatiosRefRcd>().get(apdpnref_);
        es.get<EcalLaserAlphasRcd>().get(alpha_);
        es.get<EcalLaserDbRecord>().get(laser_);

        if (dumpRecHits_) dumpRecHits(ev, es);

        /*
        iov_last_ = ev.time().unixTime();
        if (iov_first_ == -1) iov_first_ = iov_last_;

        if (iov_last_ - il_ > 3600 * 24 * 7) {
                il_ = iov_last_;
        }

        bool atLeastOneDump = dumpIC_ || dumpChStatus_ || dumpPedestals_ || dumpTransp_ || dumpTranspCorr_ || dumpAlpha_ || dumpGeometry_;
        if (atLeastOneDump) {

                if (dumpIC_) {
                        es.get<EcalIntercalibConstantsRcd>().get(ic_);
                        es.get<EcalIntercalibConstantsRcd>().get(icMC_);
                }
                if (dumpChStatus_ || dumpTransp_)   es.get<EcalChannelStatusRcd>().get(chStatus_);
                if (dumpPedestals_)                 es.get<EcalPedestalsRcd>().get(ped_);
                if (dumpGainRatios_)                es.get<EcalGainRatiosRcd>().get(gr_);
                if (dumpTransp_)     {
                        es.get<EcalLaserAPDPNRatiosRcd>().get(apdpn_);
                        es.get<EcalLaserAPDPNRatiosRefRcd>().get(apdpnref_);
                }
                if (dumpAlpha_)      es.get<EcalLaserAlphasRcd>().get(alpha_);
                if (dumpTranspCorr_) es.get<EcalLaserDbRecord>().get(laser_);

                // if you need to average and use the average later on...
                if (first_) {
                        fgeom = fopen("detid_geom.dat", "w");
                }
                if (dumpTranspCorr_) {
                }
                if (first_) {
                        fclose(fgeom);
                        first_ = false;
                }
       
                EcalIntercalibConstantMap::const_iterator itIC;
                EcalIntercalibConstantMap::const_iterator itICMC;
                EcalChannelStatusMap::const_iterator itChStatus;
                EcalPedestalsMap::const_iterator itPed;
                EcalGainRatioMap::const_iterator itGR;
                EcalLaserAPDPNRatios::EcalLaserAPDPNRatiosMap::const_iterator itAPDPN;
                EcalLaserAPDPNRatiosRef::const_iterator itAPDPNRef;
                EcalLaserAlphas::const_iterator itAlpha;
                EcalLaserAPDPNRatios::EcalLaserTimeStamp ts;

                char ecalPart[3] = "E*";
                //char eename[6] = "*Z:d*";
                char eename[3] = "*Z";

                char str[128];

                for (size_t iid = 0; iid < ecalDetIds_.size(); ++iid) {
                        DetId id(ecalDetIds_[iid]);

                        //  ix = ieta() for EB, ix() for EE
                        //  iy = iphi() for EB, iy() for EE
                        int ix = -1, iy = -1, iz = -1, r = -1;
                        int isEB = 0;

                        if (id.subdetId() == EcalBarrel) {
                                ecalPart[1] = 'B';
                                ix = EBDetId(id).ieta();
                                iy = EBDetId(id).iphi();
                                iz = 0;
                                isEB = 1;
                        } else if (id.subdetId() == EcalEndcap) {
                                ecalPart[1] = 'E';
                                ix = EEDetId(id).ix();
                                iy = EEDetId(id).iy();
                                iz = EEDetId(id).zside();
                                r = sqrt((ix - 50) * (ix - 50) + (iy - 50) * (iy - 50));
                                if (iz > 0) {
                                        eename[0] = 'p';
                                        //eename[4] = ix < 50 ? '1' : '2';
                                } else {
                                        eename[0] = 'n';
                                        //eename[4] = ix < 50 ? '3' : '4';
                                }

                        }

                        if (dumpIC_) {
                                itIC = ic_->find(id);
                                itICMC = icMC_->find(id);
                        }
                        if (dumpChStatus_)     itChStatus = chStatus_->find(id);
                        if (dumpPedestals_)   itPed = ped_->find(id);
                        if (dumpGainRatios_) itGR = gr_->find(id);
                        if (dumpTransp_) {
                                size_t iLM = 0;
                                itAPDPN = apdpn_->getLaserMap().find(id);
                                if (id.subdetId()==EcalBarrel) {
                                        EBDetId ebid( id.rawId() );
                                        iLM = MEEBGeom::lmr(ebid.ieta(), ebid.iphi());
                                } else if (id.subdetId()==EcalEndcap) {
                                        EEDetId eeid( id.rawId() );
                                        // SuperCrystal coordinates
                                        MEEEGeom::SuperCrysCoord iX = (eeid.ix()-1)/5 + 1;
                                        MEEEGeom::SuperCrysCoord iY = (eeid.iy()-1)/5 + 1;    
                                        iLM = MEEEGeom::lmr(iX, iY, eeid.zside());    
                                }
                                if ( iLM-1 < apdpn_->getTimeMap().size() ) {
                                        ts = apdpn_->getTimeMap()[iLM];
                                }
                                itAPDPNRef = apdpnref_->find(id);
                        }
                        if (dumpAlpha_) itAlpha = alpha_->find(id);
                        float transpCorr = -1234567890.;
                        if (dumpTranspCorr_) {
                                transpCorr = laser_->getLaserCorrection(id, ev.time());
                        }

                        if (atLeastOneDump) {
                                ofile_ << ecalPart << "  rawId= " << id.rawId()
                                        << "  ieta/ix= " << ix
                                        << "  iphi/iy= " << iy
                                        << "  0/zside= " << iz
                                        ;
                        }

                        if (dumpGeometry_) {
                                const CaloCellGeometry * this_cell = geometry->getSubdetectorGeometry(id)->getGeometry(id);
                                const GlobalPoint & p = this_cell->getPosition();
                                ofile_
                                        << "  eta= " << p.eta()
                                        << "  phi= " << p.phi()
                                        << "  deltaEta= " << geometry->getSubdetectorGeometry(id)->deltaEta(id)
                                        << "  deltaPhi= " << geometry->getSubdetectorGeometry(id)->deltaPhi(id)
                                        ;
                        }
                        if (dumpIC_) {
                                if(itIC != ic_->end()) ofile_ << "  ic= " << *itIC;
                                else                   ofile_ << "  ic= " << "NA";
                                if(itICMC != icMC_->end()) ofile_ << "  icMC= " << *itIC;
                                else                       ofile_ << "  icMC= " << "NA";
                        }
                        if (dumpChStatus_) {
                                if(itChStatus != chStatus_->end()) ofile_ << "  chSt= " << (*itChStatus).getStatusCode();
                                else                               ofile_ << "  chSt= " << "NA";
                        }
                        if (dumpPedestals_) {
                                if(itPed != ped_->end()) {
                                        ofile_ << "  ped_g12_g6_g1= " << (*itPed).mean(1) << "_" << (*itPed).mean(2) << "_" << (*itPed).mean(3);
                                        ofile_ << "  ped_rms_g12_g6_g1= " << (*itPed).rms(1) << "_" << (*itPed).rms(2) << "_" << (*itPed).rms(3);
                                } else  ofile_ << "  ped= " << "NA";
                        }
                        if (dumpGainRatios_) {
                                if (itGR != gr_->end()) {
                                        ofile_ << "  gr_12/6_6/1= " << (*itGR).gain12Over6() << "_" << (*itGR).gain6Over1();
                                }
                        }
                        if (dumpTransp_) {
                                if ( itAPDPN != apdpn_->getLaserMap().end() ) {
                                        ofile_ << "  evtTS_t1_t2_t3_p1_p2_p3_pRef= " 
                                                << ev.time().value() 
                                                << "_" << ts.t1.value() << "_" << ts.t2.value() 
                                                << "_" << ts.t3.value() 
                                                << "_" << (*itAPDPN).p1 << "_" << (*itAPDPN).p2 
                                                << "_" << (*itAPDPN).p3
                                                << "_" << *itAPDPNRef
                                                ;
                                }
                        }
                        if (dumpAlpha_) {
                                if ( itAlpha != alpha_->end() ) {
                                        ofile_ << "  alpha= " 
                                                << *itAlpha
                                                ;
                                }
                        }
                        if (dumpTranspCorr_) {
                                ofile_ << "  transpCorr= " 
                                        << transpCorr;
                        }
                        if (atLeastOneDump) ofile_ << "\n";
                }
        }
        if ( dumpADCToGeV_ ) {
                es.get<EcalADCToGeVConstantRcd>().get(adcToGeV_);
                ofile_ << "ADCToGeV  EB= " << adcToGeV_->getEBValue() << "  EE= " << adcToGeV_->getEEValue() << "\n";
        }
        */
        ++niov_;
}


// ------------ method called once each job just before starting event loop  ------------
	void 
EventDump::beginJob()
{
}

// ------------ method called once each job just after ending the event loop  ------------
void 
EventDump::endJob() {
}

//define this as a plug-in
DEFINE_FWK_MODULE(EventDump);
