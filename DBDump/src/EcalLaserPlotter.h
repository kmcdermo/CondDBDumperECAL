
#ifndef __ECAL_LASER_PLOTTER
#define __ECAL_LASER_PLOTTER

#include "CondFormats/EcalObjects/interface/EcalLaserAPDPNRatios.h"
#include "CondFormats/EcalObjects/interface/EcalChannelStatus.h"
#include "CalibCalorimetry/EcalLaserCorrection/interface/EcalLaserDbService.h"
#include "CalibCalorimetry/EcalLaserAnalyzer/interface/MEEBGeom.h"
#include "CalibCalorimetry/EcalLaserAnalyzer/interface/MEEEGeom.h"

#include "HistoManager.h"
#include "Quantile.h"

#include "TProfile.h"
#include "TProfile2D.h"
#include "TGraphAsymmErrors.h"
#include <iostream>
#include <fstream>

#include <vector>
#include <set>

#define NSUBDET 3

class EcalLaserPlotter {
        public:
                EcalLaserPlotter(const char * geom_filename = "detid_geom.dat");
                ~EcalLaserPlotter() { saveMichaelPlots(); printSummary(); };

                void fill(const EcalLaserAPDPNRatios &, time_t);
                void printSummary();
                void save(const char * filename = "ecallaserplotter.root", const char * opt = "RECREATE");
                void setEcalChannelStatus(const EcalChannelStatus & chStatus, int onceForAll = 0);
                void setEcalGeometry(const char * geom_filename = "detid_geom.dat");
		void saveMichaelPlots();
		void printText();
        private:
		time_t il_;
                char weekly_[128];

                void reset_quantile();
                int etabin(float eta);
                float geom_eta(DetId id);
                void compute_averages(const EcalLaserAPDPNRatios & apdpn, time_t t);
                void fill_histories(time_t t);
                void fill_slope_histories(time_t t);
		void max_min(DetId id, float val);
                void max_min_plots();

                //number of bins for plots of corrections in an eta ring
                const static int netabins_ = 20;

                // all, EE-, EB-, EB+, EE+, 92 LMR, netabins eta ring
                const static int nq_ = 97 + netabins_;
                const static int qetaoffs_ = 97;
                Quantile<int> q_[nq_];  //p2 values
                Quantile<int> qslope3_[nq_]; //p3 values
		Quantile<int> qslope1_[nq_]; //p1 values
		
		int t1, t2, t3;
		
		char qname_[nq_][32];

                HistoManager hm_;

                // number of IOVs, first and last
                int niov_;
                int nSlope1_[nq_];
		int nSlope3_[nq_];
		time_t iov_first_;
                time_t iov_last_;

                std::vector<DetId> ecalDetIds_;
                std::vector<float> max_val_;
                std::vector<float> min_val_;
                std::vector<float> geom_eta_;
                std::vector<uint16_t> ch_status_;
		std::vector< std::vector<int> > bad_channels_;
		std::vector< std::vector<int> > previous_bad_channels_;
		std::vector< std::vector<int> > current_bad_channels_;
                std::set<int> inf_; // +/- DetId according to inf sign
                std::set<int> nan_; // DetId

                char ecalPart[3];
                char eename[3];

                char str[128];

                int set_geom_;
                int set_ch_status_;
};

EcalLaserPlotter::EcalLaserPlotter(const char * geom_filename) :
        il_(0), niov_(0), iov_first_(-1), iov_last_(0), set_geom_(0), set_ch_status_(0)
{
        sprintf(ecalPart, "E*");
        sprintf(eename, "*Z");
        setEcalGeometry(geom_filename);
        hm_.addTemplate<TH2D>( "EBh2", new TH2D( "EBh2", "EBh2", 360, 0.5, 360.5, 171, -85.5, 85.5 ) );
        hm_.addTemplate<TH2D>( "EEh2", new TH2D( "EEh2", "EEh2", 100, 0., 100., 100, 0., 100. ) );
        hm_.addTemplate<TProfile2D>( "EBprof2", new TProfile2D( "EBp2", "EBp2", 360, 0.5, 360.5, 171, -85.5, 85.5 ) );
        hm_.addTemplate<TProfile2D>( "EEprof2", new TProfile2D( "EEp2", "EEp2", 100, 0., 100., 100, 0., 100. ) );
        hm_.addTemplate<TProfile>( "EEprof", new TProfile( "EEp", "EEp", 55, 0., 55.) );
        hm_.addTemplate<TProfile>( "EBprof", new TProfile( "EBp", "EBp", 171, -85.5, 85.5) );
        hm_.addTemplate<TGraphAsymmErrors>("history", new TGraphAsymmErrors());
        hm_.addTemplate<TH1D>("distr", new TH1D("distribution", "distribution", 2000, 0., 2.));
        hm_.addTemplate<TProfile>("etaProf", new TProfile("etaProf", "etaProf", 250, -2.75, 2.75));
	hm_.addTemplate<TProfile2D>("channel_eta", new TProfile2D("channel_eta","channel_eta", 250., -2.75, 2.75, 2000., 0., 2000.));
	for(int i=0;i<nq_;i++) //initialize counters for TGRaph of slopes.
	  {
	    nSlope1_[i]=0;
	    nSlope3_[i]=0;
	  }
        
	// initialise ECAL DetId, max_val, min_val vectors
        ecalDetIds_.resize(EBDetId::MAX_HASH + 1 + EEDetId::kSizeForDenseIndexing);
        max_val_.resize(EBDetId::MAX_HASH + 1 + EEDetId::kSizeForDenseIndexing);
        min_val_.resize(EBDetId::MAX_HASH + 1 + EEDetId::kSizeForDenseIndexing);
        int idx = -1;
        for (int hi = EBDetId::MIN_HASH; hi <= EBDetId::MAX_HASH; ++hi ) {
                EBDetId ebId = EBDetId::unhashIndex(hi);
                if (ebId != EBDetId()) {
                        ecalDetIds_[hi] = ebId;
                        max_val_[hi] = -FLT_MAX;
                        min_val_[hi] = FLT_MAX;
                }
        }
        for ( int hi = 0; hi < EEDetId::kSizeForDenseIndexing; ++hi ) {
                EEDetId eeId = EEDetId::unhashIndex(hi);
                if (eeId != EEDetId()) {
                        idx = EBDetId::MAX_HASH + 1 + hi;
                        ecalDetIds_[idx] = eeId;
                        max_val_[idx] = -FLT_MAX;
                        min_val_[idx] = FLT_MAX;
                }
        }
        assert(ecalDetIds_.size() == 75848);
        assert(ecalDetIds_.size() == EBDetId::MAX_HASH + 1 + EEDetId::kSizeForDenseIndexing);

        // initialise quantile names
	int i = 0;
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

void EcalLaserPlotter::reset_quantile()
{
        for (int i = 0; i < nq_; ) 
	  {
	    q_[i++].reset();
	    qslope3_[i++].reset();
	    qslope1_[i++].reset();
	  }
}

void EcalLaserPlotter::save(const char * filename, const char * opt)
{
        max_min_plots();
        hm_.save(filename, opt);
}

int EcalLaserPlotter::etabin(float eta)
{
        const float etamin  =  -2.964;
        const float etamax  =  2.964;
        //		if(!(etamin < eta && eta < etamax)){ printf("---> %f\n", eta); }
        assert(etamin < eta && eta < etamax);
        return int((eta - etamin) / (etamax - etamin) * netabins_);
}

float EcalLaserPlotter::geom_eta(DetId id)
{
        if (id.subdetId() == EcalBarrel) {
                return geom_eta_[EBDetId(id).hashedIndex()];
        } else if (id.subdetId() == EcalEndcap) {
                return geom_eta_[EBDetId::MAX_HASH + 1 + EEDetId(id).denseIndex()];
        } else {
                assert(0);
        }
}

void EcalLaserPlotter::max_min(DetId id, float val)
{
        int idx = -1;
        if (id.subdetId() == EcalBarrel) {
                idx = EBDetId(id).hashedIndex();
        } else if (id.subdetId() == EcalEndcap) {
                idx = EBDetId::MAX_HASH + 1 + EEDetId(id).denseIndex();
        } else {
                assert(0);
        }
        if (max_val_[idx] < val)  max_val_[idx] = val;
        if (min_val_[idx] > val)  min_val_[idx] = val;
}

void EcalLaserPlotter::max_min_plots()
{
        int ix = -1, iy = -1, iz = -1;
        int isEB = 0;
        const char * subdet[NSUBDET] =  { "nZ_", "" , "pZ_" };
        static TH2D * h2d_max[3]; // FIXME - hope static arrays are zeroed by default
        static TH2D * h2d_min[3]; // FIXME - hope static arrays are zeroed by default
        const char * temhl[] = { "EEh2", "EBh2"};
        for (size_t i = 0; i < ecalDetIds_.size(); ++i) {
                DetId id(ecalDetIds_[i]);
                if (id.subdetId() == EcalBarrel) {
                        ix = EBDetId(id).iphi();
                        iy = EBDetId(id).ieta();
                        iz = 0;
                        isEB = 1;
                } else if (id.subdetId() == EcalEndcap) {
                        ix = EEDetId(id).ix();
                        iy = EEDetId(id).iy();
                        iz = EEDetId(id).zside();
                        isEB = 0;
                }
                sprintf(str, "%smax", subdet[iz+1]);
                hm_.h<TH2D>(temhl[isEB], str, &h2d_max[iz + 1])->Fill(ix, iy, max_val_[i]);
                sprintf(str, "%smin", subdet[iz+1]);
                hm_.h<TH2D>(temhl[isEB], str, &h2d_min[iz + 1])->Fill(ix, iy, min_val_[i]);
        }
}

void EcalLaserPlotter::setEcalGeometry(const char * geom_filename)
{
        if (set_geom_) return;
        FILE * fg = fopen(geom_filename, "r");
        if (fg == NULL) {
                fprintf(stderr, "Cannot open file `%s'.\n", geom_filename);
                fprintf(stderr, "Please specify the correct location of the geometry\n");
                fprintf(stderr, "ascii file (format: DetId eta phi R).\n");
        } else {
                int n, id;
                float eta;
                size_t offset = EBDetId::MAX_HASH - EBDetId::MIN_HASH + 1;
                geom_eta_.resize(offset + EEDetId::kSizeForDenseIndexing);
                while ((n = fscanf(fg, "%d %f %*f %*f", &id, &eta)) == 2) {
                        if (DetId(id).subdetId() == EcalBarrel) {
                                geom_eta_[EBDetId(id).hashedIndex()] = eta;
                        } else {
                                geom_eta_[offset + EEDetId(id).denseIndex()] = eta;
                        }
                }
        }
        fclose(fg);
        set_geom_ = 1;
}

void EcalLaserPlotter::setEcalChannelStatus(const EcalChannelStatus & chStatus, int onceForAll)
{
        if (onceForAll && set_ch_status_) return;
        if (!set_ch_status_) ch_status_.resize(ecalDetIds_.size());
        FILE * ftmp;
        if (!set_ch_status_) ftmp = fopen("channelStatus.dump", "w");
        for (size_t i = 0; i < ecalDetIds_.size(); ++i) {
                ch_status_[i] = chStatus.find(ecalDetIds_[i])->getStatusCode();
                if (!set_ch_status_) {
                        DetId id(ecalDetIds_[i]);
                        int ix = -1, iy = -1, iz = -1;
                        if (id.subdetId() == EcalBarrel) {
                                ix = EBDetId(id).iphi();
                                iy = EBDetId(id).ieta();
                                iz = 0;
                        } else if (id.subdetId() == EcalEndcap) {
                                ix = EEDetId(id).ix();
                                iy = EEDetId(id).iy();
                                iz = EEDetId(id).zside();
                        }
                        //fprintf(ftmp, "%d %d %d  %d\n", ix, iy, iz, ch_status_[i]);
                }
        }
        if (!set_ch_status_) fclose(ftmp);
        set_ch_status_ = 1;
}

void EcalLaserPlotter::printSummary()
{
        printf("%d IOV(s) analysed.\n", niov_);
        char buf[256];
        strftime(buf, sizeof(buf), "%F %R:%S", gmtime(&iov_first_));
        printf("First IOV: %ld (%s UTC)\n", iov_first_, buf);
        strftime(buf, sizeof(buf), "%F %R:%S", gmtime(&iov_last_));
        printf(" Last IOV: %ld (%s UTC)\n", iov_last_, buf);
        printf("\n");
        printf("List of problematic channels:\n");
        printf("%d nan:", (int)nan_.size());
        for (std::set<int>::const_iterator it = nan_.begin(); it != nan_.end(); ++it) {
                printf(" %d", *it);
        }
        printf("\n");
        printf("%d inf:", (int)inf_.size());
        for (std::set<int>::const_iterator it = inf_.begin(); it != inf_.end(); ++it) {
                printf(" %d", *it);
        }
        printf("\n");
}

void EcalLaserPlotter::compute_averages(const EcalLaserAPDPNRatios & apdpn, time_t t)
{
        static TProfile * p = 0;
        char name[64];
        //sprintf(name, "p2_%ld", t);
        sprintf(name, "p2");
        hm_.h<TProfile>("etaProf", name, &p)->Reset();
	previous_bad_channels_.clear();
	previous_bad_channels_=current_bad_channels_;
	current_bad_channels_.clear();
        for (size_t iid = 0; iid < ecalDetIds_.size(); ++iid) {
	        if (set_ch_status_ && ch_status_[iid] != 0) continue;
	        DetId id(ecalDetIds_[iid]);
                EcalLaserAPDPNRatios::EcalLaserAPDPNRatiosMap::const_iterator itAPDPN;
                EcalLaserAPDPNRatios::EcalLaserTimeStamp ts;
		EcalLaserAPDPNRatios::EcalLaserTimeStampMap::const_iterator itLtime;
		size_t iLM = 0;
		
		itAPDPN = apdpn.getLaserMap().find(id);
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
                if ( iLM-1 < apdpn.getTimeMap().size() ) {
		           ts = apdpn.getTimeMap()[iLM];
                }

                float p2  = -1234567890.;
                float p3  = -1234567890.;
		float p1  = -1234567890.;
		float eta = 99999;
                p2 = (*itAPDPN).p2;
		p1 = (*itAPDPN).p1;
		p3 = (*itAPDPN).p3;
	
		t1 = ts.t1.value()>>32;
		t2 = ts.t2.value()>>32;
		t3 = ts.t3.value()>>32;
		
		//if(t3==t2||t2==t1)
		// std::cout << "t1 " << t1 << "  t2 " << t2 << "  t3  " << t3 << std::endl; 
		
		
		

                if (isinf(p2) > 0) {
                        p2 =  FLT_MAX;
                } else if (isinf(p2) < 0) {
                        p2 = -FLT_MAX;
                }
                if (isnan(p2)) {
                        p2 =  FLT_MAX;
                }

                eta = geom_eta(id);
			if(iid==0)
		  {
		    std::cout << "previous size : " << std::endl;
		    std::cout << previous_bad_channels_.size() << std::endl;
		    std::cout << "current size : " << std::endl;
		    std::cout << current_bad_channels_.size() << std::endl;
		  }
		
		if(previous_bad_channels_.size()==0)
		  {
		    std::vector <int> channel;
		    channel.push_back(t1);
		    channel.push_back(t2);
		    channel.push_back(id.rawId());
		    //std::cout << id.rawId() << std::endl;
		    if(p2==1.0)
		      channel.push_back(-1);
		    else
		      if(p2==0)
			channel.push_back(0);
		      else
			channel.push_back(1);
		    current_bad_channels_.push_back(channel);
		    // std::cout << current_bad_channels_[iid][3] << std::endl;
		  }
		for(int j=0;j<(int)previous_bad_channels_.size();++j)
		  {
		    //		    std::cout << "rawid "  <<  id.rawId() << " === " << previous_bad_channels_[j][2]<< std::endl;
		    
		    if(previous_bad_channels_[j][2]==(int)id.rawId()) //finds last value for p2
		      {
			std::vector <int> channel;
		    if(p2==1.0) //if p2 is bad this time
			  {
			    if(previous_bad_channels_[j][3]!=-1) //if p2 wasn't bad last time
			      {
				//std::cout << previous_bad_channels_[j][3] << "!=" << -1 << std::endl;
				channel.push_back(t1);
				channel.push_back(t2);
				channel.push_back(id.rawId());
				channel.push_back(-1);
				current_bad_channels_.push_back(channel);
				bad_channels_.push_back(channel);  //fill changing channel list
			      }
			    else //if no change, just fill current channel list
			      {
				channel.push_back(t1);
				channel.push_back(t2);
				channel.push_back(id.rawId());
				channel.push_back(-1);
				current_bad_channels_.push_back(channel);
			    }
			  }
			if(p2==0) //if p2 is unkown this time
			  {
			    if(previous_bad_channels_[j][3]!=0) //if p2 wasn't unkown last time
			      {
				//std::cout << previous_bad_channels_[j][3] << "!=" << 0 << std::endl;
				channel.push_back(t1);
				channel.push_back(t2);
				channel.push_back(id.rawId());
				channel.push_back(0);
				current_bad_channels_.push_back(channel); 
				bad_channels_.push_back(channel); //fill changing channel list
			      }
			    else //if no change, just fill current channel list
			      {
				channel.push_back(t1);
				channel.push_back(t2);
				channel.push_back(id.rawId());
				channel.push_back(0);
				current_bad_channels_.push_back(channel);
			      }		 
			  } 
			else //if p2 is a good value this time
			  {
			    if(previous_bad_channels_[j][3]!=1) //if p2 was not good last time 
			      {
				//std::cout << previous_bad_channels_[j][3] << "!=" << 1 << std::endl;
				channel.push_back(t1);
				channel.push_back(t2);
				channel.push_back(id.rawId());
				channel.push_back(1);
				current_bad_channels_.push_back(channel);
				bad_channels_.push_back(channel); //fill changing channel list
			      }
			    else //if no change, just fill current channel list 
			      {
				channel.push_back(t1);
				channel.push_back(t2);
				channel.push_back(id.rawId());
			      channel.push_back(1);
			      current_bad_channels_.push_back(channel);
			      }
			  }
		      }
		  }
		/*		if(p2==1.0) //fills log file with list of bad channels and IOV
		  {
		  std::vector <int> channel;
		  channel.push_back(t1);
		  channel.push_back(t2);
		  channel.push_back(id);
		  channel.push_back(-1);
		  current_bad_channels_.push_back(channel);
		  //   std::cout << bad_channels_[0][0] << "  " << bad_channels_[0][1] << " " << bad_channels_[0][2] << std::endl;
		  }
		  else
		  {
		  std::vector <int> channel;
		  channel.push_back(t1);
		  channel.push_back(t2);
		  channel.push_back(id);
		  channel.push_back(1);
		  current_bad_channels_.push_back(channel);
		  //   std::cout << bad_channels_[0][0] << "  " << bad_channels_[0][1] << " " << bad_channels_[0][2] << std::endl;
		  }
		*/
		
		
		hm_.h<TProfile>("etaProf_Mean", name, &p)->Fill(eta, p2);
		q_[0].fill(p2, iid);
		//	std::cout << "p3-p2 " << p3-p2 << std::endl;
		//std::cout << "p2-p1 " << p2-p1 << std::endl;
		//std::cout << "t3-t2 " << t3-t2 << std::endl;
		//std::cout << "t2-t1 " << t2-t1 << std::endl;
		float slope3;
		float slope1;
		if(!isinf((p2-p1)/(t2-t1))&&!isnan((p2-p1)/(t2-t1)))
		  slope1 = (p2-p1)/(t2-t1);
		else
		  slope1=-100.;
		if(!isinf((p3-p2)/(t3-t2))&&!isnan((p3-p2)/(t3-t2)))
		  slope3 =(p2-p1)/(t2-t1);
		else
		  slope3=-100.;
		
		if(slope1!=-100.)
		  qslope1_[0].fill(slope1, iid);
		if(slope3!=-100.)
		  qslope3_[0].fill(slope3, iid);
		if (id.subdetId() == EcalBarrel) {
		  if (EBDetId(id).ieta() < 0) 
		    {
		      q_[2].fill(p2, iid);
		      if(slope1!=-100.)
			qslope1_[2].fill(slope1, iid);
		      if(slope3!=-100.)
			qslope3_[2].fill(slope3, iid);
		    }
		  else                       
		    {
		      q_[3].fill(p2, iid);
		      if(slope1!=-100.)			  
			qslope1_[3].fill(slope1, iid);
		      if(slope3!=-100.)
			qslope3_[3].fill(slope3, iid);
			}
		} else {
		  if (EEDetId(id).zside() < 0) 
		    {
		      q_[1].fill(p2, iid);
		      if(slope1!=-100.)
			qslope1_[1].fill(slope1, iid);
		      if(slope3!=-100.)
			qslope3_[1].fill(slope3, iid);
		    }
		  else
		    {
		      q_[4].fill(p2, iid);
		      if(slope1!=-100.)
			qslope1_[4].fill(slope1, iid);
		      if(slope3!=-100.)
			qslope3_[4].fill(slope3, iid);
		    } 
		}
                q_[5 + iLM - 1].fill(p2, iid);
                q_[qetaoffs_ + etabin(eta)].fill(p2, iid);
		if(slope1!=-100.)
		  qslope1_[5 + iLM - 1].fill(slope1, iid);
		if(slope1!=-100.)
		  qslope1_[qetaoffs_ + etabin(eta)].fill(slope1, iid);
		if(slope3!=-100.)
		  qslope3_[5 + iLM - 1].fill(slope3, iid);
		if(slope3!=-100.)
		  qslope3_[qetaoffs_ + etabin(eta)].fill(slope3, iid);
        }
}

void EcalLaserPlotter::fill_histories(time_t t)
{
        float fracs[] = { 0.5 * (1 - 0.997), 0.5 * (1 - 0.954), 0.5 * (1 - 0.682), 0 };
	const char * nfrac[] = { "3S", "2S", "1S", "E" };
        char str[128];
        for (int i = 0; i < nq_; ++i) {
	        float xm = q_[i].xlow(0.5);//median of p2
                for (size_t j = 0; j < sizeof(fracs)/sizeof(float); ++j) {
		  
		        sprintf(str, "p2_%s_%s", qname_[i], nfrac[j]);
                        TGraphAsymmErrors * g = hm_.h<TGraphAsymmErrors>("history", str);
                        g->SetPoint(niov_, t, xm);
                        g->SetPointEYlow(niov_, xm - q_[i].xlow(fracs[j]));
                        g->SetPointEYhigh(niov_, q_[i].xhigh(fracs[j]) - xm);
                }
        } 
        ++niov_;
}

void EcalLaserPlotter::fill_slope_histories(time_t t)
{
          char str[128];  
	  float fracs[] = { 0.5 * (1 - 0.997), 0.5 * (1 - 0.954), 0.5 * (1 - 0.682), 0 };
	  const char * nfrac[] = {"3S", "2S", "1S", "E" };
          for (int i = 0; i < nq_; ++i) 
	    {
	      float xm1 = qslope1_[i].xlow(0.5);//median of slope1*10^15
	      float xm3 = qslope3_[i].xlow(0.5);//median of slope3*10^15
	      for(size_t j = 0; j < sizeof(fracs)/sizeof(float); ++j)
		{
		  //  if(!isnan(xm3)&&!isinf(xm3))
		  // ifa(!isnan(xm1)&&!isinf(xm1))
		  //   std::cout << "slope 3 " << xm3 << "   slope 1 " << xm1 << std::endl;
		  if(!isnan(xm3)&&!isinf(xm3)) 
		    // slope3 propagates with infs for [0]
		    // plots other than [0] have only 1 time value for each point because t set to 0 for some points
		    // need individual iterators for each plot to prevent above problem. 
		    // If point is not set, default sets to zero
		    {
		      
		      sprintf(str, "p3_p2_%s_%s", qname_[i],nfrac[j]);
		      TGraphAsymmErrors * g3 = hm_.h<TGraphAsymmErrors>("history", str);
		      g3->SetPoint(nSlope3_[i], t, xm3);
		      g3->SetPointEYlow(nSlope3_[i], xm3 - qslope3_[i].xlow(fracs[j]));
		      g3->SetPointEYhigh(nSlope3_[i], qslope3_[i].xhigh(fracs[j]) - xm3);
		      if(j==0)
			{
			  ++nSlope3_[i];
			  //	  g3->Print();
			}
		    } 
		  if(!isnan(xm1)&&!isinf(xm1))
		    { 
		      sprintf(str, "p2_p1_%s_%s", qname_[i],nfrac[j]);
		      TGraphAsymmErrors * g1 = hm_.h<TGraphAsymmErrors>("history", str);
		      g1->SetPoint(nSlope1_[i], t, xm1);
		      g1->SetPointEYlow(nSlope1_[i], xm1 - qslope1_[i].xlow(fracs[j]));
		      g1->SetPointEYhigh(nSlope1_[i], qslope1_[i].xhigh(fracs[j]) - xm1);
		      if(j==0)
			++nSlope1_[i];
		    } 
		}
	    }
}

void EcalLaserPlotter::fill(const EcalLaserAPDPNRatios & apdpn, time_t t)
{
 reset_quantile();
        iov_last_ = t;
        static TProfile2D * p2d_weekly_norm_mean[3]; // FIXME - hope static arrays are zeroed by default
        static TProfile2D * p2d_weekly_norm_median[3]; // FIXME - hope static arrays are zeroed by default
        static TProfile2D * p2d_weekly[3]; // FIXME - hope static arrays are zeroed by default
		

	bzero(p2d_weekly, sizeof(p2d_weekly));
        bzero(p2d_weekly_norm_mean, sizeof(p2d_weekly_norm_mean));
        bzero(p2d_weekly_norm_median, sizeof(p2d_weekly_norm_median));
        if (iov_first_ == -1) iov_first_ = iov_last_;
        if (iov_last_ - il_ > 3600 * 24 * 7) {
                il_ = iov_last_;
                sprintf(weekly_, "week_%ld", il_);
                for (int i = 0; i < NSUBDET; ++i) {
                        p2d_weekly[i] = 0;
                        p2d_weekly_norm_mean[i] = 0;
			p2d_weekly_norm_median[i] = 0;
		}
        }
        compute_averages(apdpn, t);
        float p2;
        TProfile * p_eta_norm_median = 0;
	TProfile * p_eta_norm_mean = 0;
        char name[64];
        //sprintf(name, "p2_%ld", t);
        sprintf(name, "p2");
        for (size_t iid = 0; iid < ecalDetIds_.size(); ++iid) {
	       if (set_ch_status_ && ch_status_[iid] != 0) {
		  max_val_[iid] = 1.;
		  min_val_[iid] = 1.;
		  continue;
		}
                DetId id(ecalDetIds_[iid]);

                int ix = -1, iy = -1, iz = -1, r = -1;
                int isEB = 0;

                if (id.subdetId() == EcalBarrel) { //setting up plots names
                        ecalPart[1] = 'B';
                        ix = EBDetId(id).iphi();
                        iy = EBDetId(id).ieta();
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
                EcalLaserAPDPNRatios::EcalLaserAPDPNRatiosMap::const_iterator itAPDPN;
                itAPDPN = apdpn.getLaserMap().find(id);
                p2 = (*itAPDPN).p2;

                const char * subdet[NSUBDET] =  { "nZ_", "" , "pZ_" };

                static TH2D * h2d_infp[3]; // FIXME - hope static arrays are zeroed by default
                static TH2D * h2d_infm[3]; // FIXME - hope static arrays are zeroed by default
                static TH2D * h2d_nan[3]; // FIXME - hope static arrays are zeroed by default
                const char * temhl[] = { "EEh2", "EBh2"};
		
                if (isinf(p2) > 0) {
                        p2 =  FLT_MAX;
                        sprintf(str, "%sinfp", subdet[iz+1]);
                        hm_.h<TH2D>(temhl[isEB], str, &h2d_infp[iz + 1])->Fill(ix, iy);
                        inf_.insert((int)id);
                        continue;
                } else if (isinf(p2) < 0) {
                        p2 = -FLT_MAX;
                        sprintf(str, "%sinfm", subdet[iz+1]);
                        hm_.h<TH2D>(temhl[isEB], str, &h2d_infm[iz + 1])->Fill(ix, iy);
                        inf_.insert(-(int)id);
                        continue;
                }
                if (isnan(p2)) {
                        p2 =  FLT_MAX;
                        sprintf(str, "%snan", subdet[iz+1]);
                        hm_.h<TH2D>(temhl[isEB], str, &h2d_nan[iz + 1])->Fill(ix, iy);
                        nan_.insert((int)id);
                        continue;
                }

                max_min(id, p2);

                float eta = geom_eta(id);
                static TH1D * h_eta_norm_mean = 0;
		static TH1D * h_eta_norm_median = 0;
                hm_.h<TProfile>("etaProf", "Mean_p2", &p_eta_norm_mean);
		hm_.h<TProfile>("etaProf", "Median_p2", &p_eta_norm_median);
		float p2_mean = p_eta_norm_mean->GetBinContent(p_eta_norm_mean->FindBin(eta));
                float p2_median = q_[qetaoffs_+etabin(eta)].xlow(.5);
		hm_.h<TH1D>("distr", "eta_normalised_median_p2", &h_eta_norm_median)->Fill(p2 / p2_median);
		hm_.h<TH1D>("distr", "eta_normalised_mean_p2", &h_eta_norm_mean)->Fill(p2 / p2_mean);
		
		
                const char* templ[] = { "EEprof2", "EBprof2"};

                static TProfile2D * p2d_all[3]; // FIXME - hope static arrays are zeroed by default
                static TProfile2D * p2d_all_norm_mean[3]; // FIXME - hope static arrays are zeroed by default
		static TProfile2D * p2d_all_norm_median[3]; // FIXME - hope static arrays are zeroed by default

                sprintf(str, "%sp2", subdet[iz+1]);
                hm_.h<TProfile2D>(templ[isEB], str, &p2d_all[iz + 1])->Fill(ix, iy, p2);
                sprintf(str, "%sp2%s", subdet[iz+1], weekly_);
                hm_.h<TProfile2D>(templ[isEB], str, &p2d_weekly[iz + 1])->Fill(ix, iy, p2);
		

                sprintf(str, "%sp2Norm_Median", subdet[iz+1]);
                hm_.h<TProfile2D>(templ[isEB], str, &p2d_all_norm_median[iz + 1])->Fill(ix, iy, p2 / p2_median);
                if(iid==2) hm_.h<TProfile2D>(templ[iz+1], str, &p2d_all_norm_median[iz + 1])->SetErrorOption("s");
                sprintf(str, "%sp2Norm_Median_%s", subdet[iz+1], weekly_);
                hm_.h<TProfile2D>(templ[isEB], str, &p2d_weekly_norm_median[iz + 1])->Fill(ix, iy, p2 / p2_median);
                if(iid==2) hm_.h<TProfile2D>(templ[isEB], str, &p2d_weekly_norm_median[iz + 1])->SetErrorOption("s");

		 sprintf(str, "%sp2Norm_Mean", subdet[iz+1]);
                hm_.h<TProfile2D>(templ[isEB], str, &p2d_all_norm_mean[iz + 1])->Fill(ix, iy, p2 / p2_mean);
                if(iid==2) hm_.h<TProfile2D>(templ[iz+1], str, &p2d_all_norm_mean[iz + 1])->SetErrorOption("s");
                sprintf(str, "%sp2Norm_Mean_%s", subdet[iz+1], weekly_);
                hm_.h<TProfile2D>(templ[isEB], str, &p2d_weekly_norm_mean[iz + 1])->Fill(ix, iy, p2 / p2_mean);
                if(iid==2) hm_.h<TProfile2D>(templ[isEB], str, &p2d_weekly_norm_mean[iz + 1])->SetErrorOption("s");
				
		/************Added By Michael Planer**************/
		static TProfile2D * bad_channel_map[3][2000];
		static TProfile2D * bad_channel_map_eta;
		
		if(niov_>=1000)
		  sprintf(str, "%sBadChannel_%i", subdet[iz+1], niov_);  //(if else)s to make sure the plots are displayed in the correct order 
		else if(niov_>=100)
		  sprintf(str, "%sBadChannel_0%i", subdet[iz+1], niov_);
		else if(niov_>=10)
                  sprintf(str, "%sBadChannel_00%i", subdet[iz+1], niov_);
		else
		  sprintf(str, "%sBadChannel_000%i", subdet[iz+1], niov_);

		if(p2==1.0) //bad channel
		  {
		    hm_.h<TProfile2D>(templ[isEB], str, &bad_channel_map[iz + 1][niov_])->Fill(ix, iy, -p2);
		    hm_.h<TProfile2D>("channel_eta",str, &bad_channel_map_eta)->AddBinContent( hm_.h<TProfile2D>("channel_eta",str, &bad_channel_map_eta)->GetBin(eta, niov_),p2);
		  }
		else
		  {
		    hm_.h<TProfile2D>(templ[isEB], str, &bad_channel_map[iz + 1][niov_])->Fill(ix, iy, 1.0);
		  }
		  // if(p2==1.0)
		  // std::cout <<"ix " << ix << "  iy  " << iy  << "  niov  " << niov_ << std::endl; 
		  // if(ix==20&&iy==30)
		  //  std::cout << "value : " << p2 << std::endl;
		  /********************************/
	}
	//	fill_slope_histories(t);
	//fill_histories(t);
}



void EcalLaserPlotter::printText()
{
  for(int i=0;i<((int) bad_channels_.size());++i) 
    std::cout << bad_channels_[i][0] << " " << bad_channels_[i][1] << " " << bad_channels_[i][2] << " " << bad_channels_[i][3] << std::endl;
}

void EcalLaserPlotter::saveMichaelPlots()
{
  std::ifstream readfile;
  char temp[256];
  readfile.open("bad_channels.txt");
  if(readfile.is_open())
    std::cout << "readfile open" << std::endl;
  readfile.getline(temp,256);
  std::cout << temp << std::endl;
  readfile.close();
  
  ofstream writefile;
  writefile.open("bad_channels.txt");
  if(writefile.is_open())
    {
      //std::cout << "bad channels log file opened succesfully" << std::endl;
      //std::cout << "size of bad_channels " << (int) bad_channels_.size() << std::endl;
      writefile << "nIOV ix  iy" << std::endl;
      for(int i=0;i<((int) bad_channels_.size())/3;++i)
	{
	  writefile << bad_channels_[i][0]  << " " << bad_channels_[i][1] << " " << bad_channels_[i][2] << std::endl;
	  //	  std::cout << bad_channels_[i][0]  << " " << bad_channels_[i][1] << " " << bad_channels_[i][2] << std::endl;
	}
	  writefile.close();
    }
  else
    std::cout << "Bad Channel log file not opened!" << std::endl;
}

#endif
