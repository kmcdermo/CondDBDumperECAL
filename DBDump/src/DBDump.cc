// -*- mode: c++; c-basic-offset:8; -*-
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
// $Id: DBDump.cc,v 1.12 2011/08/03 18:21:39 ecalmon Exp $
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
#include "Quantile.h"

#include "TProfile.h"
#include "TProfile2D.h"
#include "TH1D.h"
#include "TH2D.h"
#include "TGraph.h"
#include "TGraphAsymmErrors.h"


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
		void printSummary();

		int etabin(float eta){
			const float etamin  =  -2.964;
			const float etamax  =  2.964;
			//		if(!(etamin < eta && eta < etamax)){ printf("---> %f\n", eta); }
			assert(etamin < eta && eta < etamax);
			return int((eta - etamin) / (etamax - etamin) * netabins_);
		}

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

		std::vector<DetId> ecalDetIds_;

		// number of IOVs, first and last
		int niov_;
		time_t iov_first_;
		time_t iov_last_;

		// job start and stop time
		time_t jstart_;
		time_t jstop_;

		time_t il_;
		char weekly_[128];

		//number of bins for plots of corrections in an eta ring
		const static int netabins_ = 20;

		// all, EE-, EB-, EB+, EE+, 92 LMR, netabins eta ring
		const static int nq_ = 97 + netabins_;
		const static int qetaoffs_ = 97;
		Quantile<int> q_[nq_];
		char qname_[nq_][32];

};



DBDump::DBDump(const edm::ParameterSet& ps) :
	niov_(0),
	iov_first_(-1),
	iov_last_(0),
	jstart_(time(NULL)),
	jstop_(0),
	il_(0)
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

	if (outDumpFile_ != "") {
		ofile_.open(outDumpFile_.c_str(), std::ios::out);
		if (!ofile_.is_open()) {
			edm::LogError("DBDump") << "DBDump::DBDump cannot open output file " << outDumpFile_;
			setDumpFalse();
		}
	} else if (outDump_) {
		edm::LogError("DBDump") << "DBDump::DBDump mismatch in your config file, setting outDump to false.";
		setDumpFalse();
	} else {
		setDumpFalse();
	}

	if (outPlot_) {
		histos.addTemplate<TH2D>( "EBh2", new TH2D( "EB", "EB", 360, 0.5, 360.5, 171, -85.5, 85.5 ) );
		histos.addTemplate<TH2D>( "EEh2", new TH2D( "EE", "EE", 100, 0., 100., 100, 0., 100. ) );
		histos.addTemplate<TProfile2D>( "EBprof2", new TProfile2D( "EB", "EB", 360, 0.5, 360.5, 171, -85.5, 85.5 ) );
		histos.addTemplate<TProfile2D>( "EEprof2", new TProfile2D( "EE", "EE", 100, 0., 100., 100, 0., 100. ) );
		histos.addTemplate<TProfile>( "EEprof", new TProfile( "EE", "EE", 55, 0., 55.) );
		histos.addTemplate<TProfile>( "EBprof", new TProfile( "EB", "EB", 171, -85.5, 85.5) );
		histos.addTemplate<TGraphAsymmErrors>("history", new TGraphAsymmErrors());
		histos.addTemplate<TH1D>("distr", new TH1D("distribution", "distribution", 2000, 0., 2.));
		histos.addTemplate<TProfile>("etaProf", new TProfile("etaProf", "etaProf", 250, -2.75, 2.75));
	} else {
		setPlotFalse();
	}

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

	// initialise quantile names
	int i =0;
	sprintf(qname_[i++], "All");
	sprintf(qname_[i++], "EE-");
	sprintf(qname_[i++], "EB-");
	sprintf(qname_[i++], "EB+");
	sprintf(qname_[i++], "EE+");
	for (int j = 1; j <= 92; ++j) {
		sprintf(qname_[i++], "LM%02d", j);
	}
	for(int j = 1; j <= netabins_; ++j){
		sprintf(qname_[i++], "eta%02d", j);
	}
	assert(i == sizeof(qname_)/sizeof(qname_[0]));
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
	if (outPlot_) {
		histos.save(outPlotFile_.c_str());
	}
	if (ofile_.is_open()) {
		ofile_.close();
	}
	jstop_ = time(NULL);
	printSummary();
}

void DBDump::printSummary()
{
	printf("%d IOV(s) analysed in %ld s (%.2f h).\n", niov_, jstop_ - jstart_, (float)(jstop_ - jstart_) / 3600.);
	char buf[128];
	strftime(buf, sizeof(buf), "%F %R:%S", gmtime(&iov_first_));
	printf("First IOV: %ld (%s UTC)\n", iov_first_, buf);
	strftime(buf, sizeof(buf), "%F %R:%S", gmtime(&iov_last_));
	printf(" Last IOV: %ld (%s UTC)\n", iov_last_, buf);
}

// ------------ method called to for each event  ------------
	void
DBDump::analyze(const edm::Event& ev, const edm::EventSetup& es)
{
	//es.get<EcalTimeCalibConstantsRcd>().get( tc_ );
	es.get<CaloGeometryRecord>().get(caloGeometry_);                                                
	geo_ = caloGeometry_.product();                                                                 

	iov_last_ = ev.time().unixTime();
	if (iov_first_ == -1) iov_first_ = iov_last_;

	if (iov_last_ - il_ > 3600 * 24 * 7) {
		il_ = iov_last_;
		sprintf(weekly_, "week_%ld", il_);
	}

	bool atLeastOneDump = dumpIC_ || dumpChStatus_ || dumpPedestals_ || dumpTransp_ || dumpTranspCorr_;
	bool atLeastOnePlot = plotIC_ || plotChStatus_ || plotPedestals_ || plotTransp_ || plotTranspCorr_;
	if (atLeastOneDump || atLeastOnePlot) {

		if (dumpIC_ || plotIC_) {
			es.get<EcalIntercalibConstantsRcd>().get(ic_);
			es.get<EcalIntercalibConstantsRcd>().get(icMC_);
		}
		if (dumpChStatus_   || plotChStatus_)   es.get<EcalChannelStatusRcd>().get(chStatus_);
		if (dumpPedestals_  || plotPedestals_)  es.get<EcalPedestalsRcd>().get(ped_);
		if (dumpGainRatios_ || plotGainRatios_) es.get<EcalGainRatiosRcd>().get(gr_);
		if (dumpTransp_     || plotTransp_)     es.get<EcalLaserAPDPNRatiosRcd>().get(apdpn_);
		if (dumpTranspCorr_ || plotTranspCorr_) es.get<EcalLaserDbRecord>().get(laser_);

		// if you need to average and use the average later on...
		if (dumpTranspCorr_ || plotTranspCorr_) {
			for (int i = 0; i < nq_; ) q_[i++].reset();
			for (size_t iid = 0; iid < ecalDetIds_.size(); ++iid) {
				DetId id(ecalDetIds_[iid]);
				EcalLaserAPDPNRatios::EcalLaserAPDPNRatiosMap::const_iterator itAPDPN;
				EcalLaserAPDPNRatios::EcalLaserTimeStamp ts;
				size_t iLM = 0;
				itAPDPN = apdpn_->getLaserMap().find(id);
				iLM = getLMNumber(id);
				if ( iLM-1 < apdpn_->getTimeMap().size() ) {
					ts = apdpn_->getTimeMap()[iLM];
				}
				//float transpCorr = -1234567890.;
				float p2  = -1234567890.;
				float eta = 99999;
				char name[64];
				sprintf(name, "p2_%d", ev.time().unixTime());
				p2 = (*itAPDPN).p2;
				eta = geo_->getPosition(id).eta();

				// 				static float ebmax = 0;
				// 				static float eemax = 0;
				// 				static float ebmin = 0;
				// 				static float eemin = 0;
				// 				if(iid< 61200 && eta > ebmax) ebmax = eta;
				// 				if(iid>= 61200 && eta > eemax /*&& eta < -1.48118*/) eemax = eta;
				// 				if(iid< 61200 && eta  < ebmin) ebmin = eta;
				// 				if(iid>= 61200 && eta < eemin /*&& eta > 1.48118*/) eemin = eta;
				// 				printf("%f %d %f %f %f %f\n", eta, (int)iid, ebmin, ebmax, eemin, eemax);

				histos.h<TProfile>("etaProf", name)->Fill(eta, p2);
				q_[0].fill(p2, iid);
				if (id.subdetId() == EcalBarrel) {
					if (EBDetId(id).ieta() < 0) q_[2].fill(p2, iid);
					else                        q_[3].fill(p2, iid);
				} else {
					if (EEDetId(id).zside() < 0) q_[1].fill(p2, iid);
					else                         q_[4].fill(p2, iid);
				}
				q_[5 + iLM - 1].fill(p2, iid);
				q_[qetaoffs_ + etabin(eta) ].fill(p2, iid);
			}
		}

		EcalIntercalibConstantMap::const_iterator itIC;
		EcalIntercalibConstantMap::const_iterator itICMC;
		EcalChannelStatusMap::const_iterator itChStatus;
		EcalPedestalsMap::const_iterator itPed;
		EcalGainRatioMap::const_iterator itGR;
		EcalLaserAPDPNRatios::EcalLaserAPDPNRatiosMap::const_iterator itAPDPN;
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

			if (dumpIC_ || plotIC_) {
				itIC = ic_->find(id);
				itICMC = icMC_->find(id);
			}
			if (dumpChStatus_ || plotChStatus_)     itChStatus = chStatus_->find(id);
			if (dumpPedestals_ || plotPedestals_)   itPed = ped_->find(id);
			if (dumpGainRatios_ || plotGainRatios_) itGR = gr_->find(id);
			if (dumpTransp_ || plotTransp_) {
				size_t iLM = 0;
				itAPDPN = apdpn_->getLaserMap().find(id);
				iLM = getLMNumber(id);
				if ( iLM-1 < apdpn_->getTimeMap().size() ) {
					ts = apdpn_->getTimeMap()[iLM];
				}
			}
			float transpCorr = -1234567890.;
			if (dumpTranspCorr_ || plotTranspCorr_) {
				transpCorr = laser_->getLaserCorrection(id, ev.time());
			}

			if (atLeastOneDump) {
				ofile_ << ecalPart << "  rawId= " << id.rawId()
					<< "  ieta/ix= " << ix
					<< "  iphi/iy= " << iy
					<< "  0/iz= " << iz;
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
			if (atLeastOneDump) ofile_ << "\n";

			// plotting:
			if (plotIC_) {
				if (itIC != ic_->end()) {
					if (isEB) {
						histos.h<TProfile>("EBprof", "IC")->Fill(ix, *itIC);
					} else {
						//assert(eename[5] == '\0');
						sprintf(str, "%s:IC", eename);
						histos.h<TProfile>("EEprof", str)->Fill(r, *itIC);
					}
				}
				if (itICMC != icMC_->end()) {
					if (isEB) {
						histos.h<TProfile>("EBprof", "ICMC")->Fill(ix, *itICMC);
					} else {
						//assert(eename[5] == '\0');
						sprintf(str, "%s:ICMC", eename);
						histos.h<TProfile>("EEprof", str)->Fill(r, *itIC);
					}
				}
			}
			if (plotChStatus_) {
				if (itChStatus != chStatus_->end()) {
					if (isEB) {
						histos.h<TH2D>("EBh2", "chStatus")->Fill(iy, ix, (*itChStatus).getStatusCode());
					} else {
						//assert(eename[5] == '\0');
						sprintf(str, "%s:chStatus", eename);
						histos.h<TH2D>("EEh2", str)->Fill(ix, iy, (*itChStatus).getStatusCode());
					}
				}
			}
			if (plotPedestals_) {
				if (itPed != ped_->end()) {
					if (isEB) {
						histos.h<TH2D>("EBh2", "ped_g12" )->Fill(iy, ix, (*itPed).mean(0));
						histos.h<TH2D>("EBh2", "ped_g6"  )->Fill(iy, ix, (*itPed).mean(1));
						histos.h<TH2D>("EBh2", "ped_g1"  )->Fill(iy, ix, (*itPed).mean(2));
						histos.h<TH2D>("EBh2", "ped_rms_g12" )->Fill(iy, ix, (*itPed).rms(0));
						histos.h<TH2D>("EBh2", "ped_rms_g6"  )->Fill(iy, ix, (*itPed).rms(1));
						histos.h<TH2D>("EBh2", "ped_rms_g1"  )->Fill(iy, ix, (*itPed).rms(2));
					} else {
						sprintf(str, "%s:ped_g12", eename);
						histos.h<TH2D>("EEh2", str)->Fill( ix, iy, (*itPed).mean(0) );
						sprintf(str, "%s:ped_g6", eename);
						histos.h<TH2D>("EEh2", str)->Fill( ix, iy, (*itPed).mean(1) );
						sprintf(str, "%s:ped_g1", eename);
						histos.h<TH2D>("EEh2", str)->Fill( ix, iy, (*itPed).mean(2) );
						sprintf(str, "%s:ped_rms_g12", eename);
						histos.h<TH2D>("EEh2", str)->Fill( ix, iy, (*itPed).rms(0) );
						sprintf(str, "%s:ped_rms_g6", eename);
						histos.h<TH2D>("EEh2", str)->Fill( ix, iy, (*itPed).rms(1) );
						sprintf(str, "%s:ped_rms_g1", eename);
						histos.h<TH2D>("EEh2", str)->Fill( ix, iy, (*itPed).rms(2) );
					}
				}
			}
			if (plotGainRatios_) {
				if (itGR != gr_->end()) {
					if (isEB) {
						histos.h<TH2D>("EBh2", "gr_g12/6")->Fill(iy, ix, (*itGR).gain12Over6());
						histos.h<TH2D>("EBh2", "gr_g6/1" )->Fill(iy, ix, (*itGR).gain6Over1());
					} else {
						sprintf(str, "%s:gr12/6", eename);
						histos.h<TH2D>("EEh2", str)->Fill( ix, iy, (*itGR).gain12Over6() );
						sprintf(str, "%s:gr6/1", eename);
						histos.h<TH2D>("EEh2", str)->Fill( ix, iy, (*itGR).gain6Over1() );
					}
				}
			}
			if (plotTranspCorr_) {
				float eta = geo_->getPosition(id).eta();
				char name[64];
				sprintf(name, "p2_%d", ev.time().unixTime());
				TProfile * p = histos.h<TProfile>("etaProf", name);
				float p2_mean = p->GetBinContent(p->FindBin(eta));
				histos.h<TH1D>("distr", "eta_normalised_p2")->Fill((*itAPDPN).p2 / p2_mean);

				const char* templ[] = { "EEprof2", "EBprof2"};
				const char* subdet[] =  { "nZ_", "" , "pZ_" };

				sprintf(str, "%stranspCorr", subdet[iz+1]);
				histos.h<TProfile2D>( templ[isEB], str)->Fill(iy, ix, transpCorr);
				sprintf(str, "%stranspCorr_%s", subdet[iz+1], weekly_);
				histos.h<TProfile2D>( templ[isEB], str )->Fill(iy, ix, transpCorr);

				sprintf(str, "%stranspCorrNorm", subdet[iz+1]);
				histos.h<TProfile2D>(templ[isEB], str )->Fill(iy, ix, (*itAPDPN).p2 / p2_mean);
				if(iid==2) histos.h<TProfile2D>(templ[iz+1], str)->SetErrorOption("s");
				sprintf(str, "%stranspCorrNorm_%s", subdet[iz+1], weekly_);
				histos.h<TProfile2D>(templ[isEB], str )->Fill(iy, ix, (*itAPDPN).p2 / p2_mean);
				if(iid==2) histos.h<TProfile2D>(templ[isEB], str)->SetErrorOption("s");


				// if (isEB) {
				//     	//histos.h<TH2D>("EBh2", "transpCorr" )->Fill(iy, ix, transpCorr);
				//     	histos.h<TProfile2D>( "EBprof2", "transpCorr" )->Fill(iy, ix, transpCorr);
				//     	sprintf(str, "transpCorr_%s", weekly_);
				//     	histos.h<TProfile2D>("EBprof2", str )->Fill(iy, ix, transpCorr);

				// 	sprintf(str, "transpCorrNorm_%s", ebname);
				// 	histos.h<TProfile2D>("EBprof2", str)->Fill(iy, ix, (*itAPDPN).p2 / p2_mean);
				// 	if(iid==2) histos.h<TProfile2D>("EBprof2", str)->SetErrorOption("s");

				// 	sprintf(str, "transpCorrNorm_%s", weekly_);
				// 	histos.h<TProfile2D>("EBprof2", str )->Fill(iy, ix, (*itAPDPN).p2 / p2_mean);
				// 	if(iid==2) histos.h<TProfile2D>("EBprof2", str)->SetErrorOption("s");

				// } else {
				//         //sprintf(str, "%s_transpCorr", eename);
				//         //histos.h<TH2D>("EEh2", str)->Fill( ix, iy, transpCorr );
				//         sprintf(str, "%s_transpCorr", eename);
				//         histos.h<TProfile2D>( "EEprof2", str)->Fill(iy, ix, transpCorr);
				//         sprintf(str, "%s_transpCorr_%s", eename, weekly_);
				//         histos.h<TProfile2D>( "EEprof2", str )->Fill(iy, ix, transpCorr);

				// 	sprintf(str, "transpCorrNorm_%s", eename);
				// 	histos.h<TProfile2D>("EEprof2", str )->Fill(iy, ix, (*itAPDPN).p2 / p2_mean);
				// 	if(iid==2) histos.h<TProfile2D>("EEprof2", str)->SetErrorOption("s");
				// 	sprintf(str, "transpCorrNorm_%s", weekly_);
				// 	histos.h<TProfile2D>("EEprof2", str )->Fill(iy, ix, (*itAPDPN).p2 / p2_mean);
				// 	if(iid==2) histos.h<TProfile2D>("EEprof2", str)->SetErrorOption("s");
				// 	}
				//}
		}
	}
	//// history plots
	float fracs[] = { 0.5 * (1 - 0.997), 0.5 * (1 - 0.954), 0.5 * (1 - 0.682), 0 };
	const char * nfrac[] = { "3S", "2S", "1S", "E" };
	for (int i = 0; i < nq_; ++i) {
		float xm = q_[i].xlow(0.5);
		for (size_t j = 0; j < sizeof(fracs)/sizeof(float); ++j) {
			sprintf(str, "p2_%s_%s", qname_[i], nfrac[j]);
			TGraphAsymmErrors * g = histos.h<TGraphAsymmErrors>("history", str);
			g->SetPoint(niov_, ev.time().unixTime(), xm);
			g->SetPointEYlow(niov_, xm - q_[i].xlow(fracs[j]));
			g->SetPointEYhigh(niov_, q_[i].xhigh(fracs[j]) - xm);
			//sprintf(str, "p2_%s_%s_H", qname_[i], nfrac[j]);
			//histos.h<TGraphAsymmErrors>("history", str)->SetPoint(niov_, ev.time().unixTime(), q_[i].xhigh(fracs[j]));
		}
	}
}
if ( dumpADCToGeV_ ) {
	es.get<EcalADCToGeVConstantRcd>().get(adcToGeV_);
	ofile_ << "ADCToGeV  EB= " << adcToGeV_->getEBValue() << "  EE= " << adcToGeV_->getEEValue() << "\n";
}
++niov_;
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
