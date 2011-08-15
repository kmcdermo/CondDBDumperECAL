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

#define NSUBDET 3

class EcalLaserPlotter {
        public:
                EcalLaserPlotter(const char * geom_filename = "detid_geom.dat");
                ~EcalLaserPlotter() { printSummary(); };

                void fill(const EcalLaserAPDPNRatios &, time_t);
                void printSummary();
                void save(const char * filename = "ecallaserplotter.root", const char * opt = "RECREATE");
                void setEcalChannelStatus(const EcalChannelStatus & chStatus, int onceForAll = 0);
                void setEcalGeometry(const char * geom_filename = "detid_geom.dat");

        private:
                time_t il_;
                char weekly_[128];

                void reset_quantile();
                int etabin(float eta);
                float geom_eta(DetId id);
                void compute_averages(const EcalLaserAPDPNRatios & apdpn, time_t t);
                void fill_histories(time_t t);

                //number of bins for plots of corrections in an eta ring
                const static int netabins_ = 20;

                // all, EE-, EB-, EB+, EE+, 92 LMR, netabins eta ring
                const static int nq_ = 97 + netabins_;
                const static int qetaoffs_ = 97;
                Quantile<int> q_[nq_];
                char qname_[nq_][32];

                HistoManager hm_;

                // number of IOVs, first and last
                int niov_;
                time_t iov_first_;
                time_t iov_last_;

                std::vector<DetId> ecalDetIds_;
                std::vector<float> geom_eta_;
                std::vector<uint16_t> ch_status_;

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
        for (int i = 0; i < nq_; ) q_[i++].reset();
}

void EcalLaserPlotter::save(const char * filename, const char * opt)
{
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
        //if (!set_ch_status_) ftmp = fopen("channelStatus.dump", "w");
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
        //if (!set_ch_status_) fclose(ftmp);
        set_ch_status_ = 1;
}

void EcalLaserPlotter::printSummary()
{
        printf("%d IOV(s) analysed.\n", niov_);
        char buf[128];
        strftime(buf, sizeof(buf), "%F %R:%S", gmtime(&iov_first_));
        printf("First IOV: %ld (%s UTC)\n", iov_first_, buf);
        strftime(buf, sizeof(buf), "%F %R:%S", gmtime(&iov_last_));
        printf(" Last IOV: %ld (%s UTC)\n", iov_last_, buf);
}

void EcalLaserPlotter::compute_averages(const EcalLaserAPDPNRatios & apdpn, time_t t)
{
        static TProfile * p;
        p = 0;
        for (size_t iid = 0; iid < ecalDetIds_.size(); ++iid) {
                if (set_ch_status_ && ch_status_[iid] != 0) continue;
                DetId id(ecalDetIds_[iid]);
                EcalLaserAPDPNRatios::EcalLaserAPDPNRatiosMap::const_iterator itAPDPN;
                EcalLaserAPDPNRatios::EcalLaserTimeStamp ts;
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
                float eta = 99999;
                char name[64];
                sprintf(name, "p2_%ld", t);
                p2 = (*itAPDPN).p2;

                if (isinf(p2) > 0)      p2 =  FLT_MAX;
                else if (isinf(p2) < 0) p2 = -FLT_MAX;

                eta = geom_eta(id);

                hm_.h<TProfile>("etaProf", name, &p)->Fill(eta, p2);
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

void EcalLaserPlotter::fill_histories(time_t t)
{
        float fracs[] = { 0.5 * (1 - 0.997), 0.5 * (1 - 0.954), 0.5 * (1 - 0.682), 0 };
        const char * nfrac[] = { "3S", "2S", "1S", "E" };
        char str[128];
        for (int i = 0; i < nq_; ++i) {
                float xm = q_[i].xlow(0.5);
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

void EcalLaserPlotter::fill(const EcalLaserAPDPNRatios & apdpn, time_t t)
{
        reset_quantile();
        iov_last_ = t;
        static TProfile2D * p2d_weekly_norm[3]; // FIXME - hope static arrays are zeroed by default
        static TProfile2D * p2d_weekly[3]; // FIXME - hope static arrays are zeroed by default
        bzero(p2d_weekly, sizeof(p2d_weekly));
        bzero(p2d_weekly_norm, sizeof(p2d_weekly_norm));
        if (iov_first_ == -1) iov_first_ = iov_last_;
        if (iov_last_ - il_ > 3600 * 24 * 7) {
                il_ = iov_last_;
                sprintf(weekly_, "week_%ld", il_);
                for (int i = 0; i < NSUBDET; ++i) {
                        p2d_weekly[i] = 0;
                        p2d_weekly_norm[i] = 0;
                }
        }
        compute_averages(apdpn, t);
        float p2;
        TProfile * p_eta_norm = 0;
        for (size_t iid = 0; iid < ecalDetIds_.size(); ++iid) {
                if (set_ch_status_ && ch_status_[iid] != 0) continue;
                DetId id(ecalDetIds_[iid]);

                int ix = -1, iy = -1, iz = -1, r = -1;
                int isEB = 0;

                if (id.subdetId() == EcalBarrel) {
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

                if (isinf(p2) > 0)      p2 =  FLT_MAX;
                else if (isinf(p2) < 0) p2 = -FLT_MAX;

                float eta = geom_eta(id);
                char name[64];
                sprintf(name, "p2_%ld", t);
                static TH1D * h_eta_norm = 0;
                hm_.h<TProfile>("etaProf", name, &p_eta_norm);
                float p2_mean = p_eta_norm->GetBinContent(p_eta_norm->FindBin(eta));
                hm_.h<TH1D>("distr", "eta_normalised_p2", &h_eta_norm)->Fill(p2 / p2_mean);

                const char* templ[] = { "EEprof2", "EBprof2"};
                const char* subdet[NSUBDET] =  { "nZ_", "" , "pZ_" };

                static TProfile2D * p2d_all[3]; // FIXME - hope static arrays are zeroed by default
                static TProfile2D * p2d_all_norm[3]; // FIXME - hope static arrays are zeroed by default

                sprintf(str, "%sp2", subdet[iz+1]);
                hm_.h<TProfile2D>(templ[isEB], str, &p2d_all[iz + 1])->Fill(ix, iy, p2);
                sprintf(str, "%sp2%s", subdet[iz+1], weekly_);
                hm_.h<TProfile2D>(templ[isEB], str, &p2d_weekly[iz + 1])->Fill(ix, iy, p2);

                sprintf(str, "%sp2Norm", subdet[iz+1]);
                hm_.h<TProfile2D>(templ[isEB], str, &p2d_all_norm[iz + 1])->Fill(ix, iy, p2 / p2_mean);
                if(iid==2) hm_.h<TProfile2D>(templ[iz+1], str, &p2d_all_norm[iz + 1])->SetErrorOption("s");
                sprintf(str, "%sp2Norm_%s", subdet[iz+1], weekly_);
                hm_.h<TProfile2D>(templ[isEB], str, &p2d_weekly_norm[iz + 1])->Fill(ix, iy, p2 / p2_mean);
                if(iid==2) hm_.h<TProfile2D>(templ[isEB], str, &p2d_weekly_norm[iz + 1])->SetErrorOption("s");
        }
        fill_histories(t);
}

#endif
