#ifndef __ECAL_LASER_PLOTTER
#define __ECAL_LASER_PLOTTER

#include "CondFormats/EcalObjects/interface/EcalLaserAPDPNRatios.h"
#include "CalibCalorimetry/EcalLaserCorrection/interface/EcalLaserDbService.h"
#include "HistoManager.h"
#include "Quantile.h"

#include "TProfile.h"
#include "TProfile2D.h"
#include "TGraphAsymmErrors.h"

class EcalLaserPlotter {
        public:
                EcalLaserPlotter(const char * geom_filename = "detid_geom.dat");
                ~EcalLaserPlotter() {};

                void fill(const EcalLaserAPDPNRatios &, time_t);
                void printSummary();
                void save(const char * filename = "ecallaserplotter.root");

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

                char ecalPart[3];
                char eename[3];

                char str[128];
};

EcalLaserPlotter::EcalLaserPlotter(const char * geom_filename) :
        il_(0), niov_(0), iov_first_(-1), iov_last_(0)
{
        sprintf(ecalPart, "E*");
        sprintf(eename, "*Z");
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
        hm_.addTemplate<TH2D>( "EBh2", new TH2D( "EB", "EB", 360, 0.5, 360.5, 171, -85.5, 85.5 ) );
        hm_.addTemplate<TH2D>( "EEh2", new TH2D( "EE", "EE", 100, 0., 100., 100, 0., 100. ) );
        hm_.addTemplate<TProfile2D>( "EBprof2", new TProfile2D( "EB", "EB", 360, 0.5, 360.5, 171, -85.5, 85.5 ) );
        hm_.addTemplate<TProfile2D>( "EEprof2", new TProfile2D( "EE", "EE", 100, 0., 100., 100, 0., 100. ) );
        hm_.addTemplate<TProfile>( "EEprof", new TProfile( "EE", "EE", 55, 0., 55.) );
        hm_.addTemplate<TProfile>( "EBprof", new TProfile( "EB", "EB", 171, -85.5, 85.5) );
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

void EcalLaserPlotter::save(const char * filename)
{
        hm_.save(filename);
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
        for (size_t iid = 0; iid < ecalDetIds_.size(); ++iid) {
                DetId id(ecalDetIds_[iid]);
                EcalLaserAPDPNRatios::EcalLaserAPDPNRatiosMap::const_iterator itAPDPN;
                EcalLaserAPDPNRatios::EcalLaserTimeStamp ts;
                size_t iLM = 0;
                itAPDPN = apdpn.getLaserMap().find(id);
                iLM = EcalLaserDbService::getLMNumber(id);
                if ( iLM-1 < apdpn.getTimeMap().size() ) {
                        ts = apdpn.getTimeMap()[iLM];
                }
                float p2  = -1234567890.;
                float eta = 99999;
                char name[64];
                sprintf(name, "p2_%ld", t);
                p2 = (*itAPDPN).p2;

                eta = geom_eta(id);

                hm_.h<TProfile>("etaProf", name)->Fill(eta, p2);
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
        iov_last_ = t;
        if (iov_first_ == -1) iov_first_ = iov_last_;
        if (iov_last_ - il_ > 3600 * 24 * 7) {
                il_ = iov_last_;
                sprintf(weekly_, "week_%ld", il_);
        }
        compute_averages(apdpn, t);
        float p2;
        for (size_t iid = 0; iid < ecalDetIds_.size(); ++iid) {
                DetId id(ecalDetIds_[iid]);

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
                EcalLaserAPDPNRatios::EcalLaserAPDPNRatiosMap::const_iterator itAPDPN;
                itAPDPN = apdpn.getLaserMap().find(id);
                p2 = (*itAPDPN).p2;

                float eta = geom_eta(id);
                char name[64];
                sprintf(name, "p2_%ld", t);
                TProfile * p = hm_.h<TProfile>("etaProf", name);
                float p2_mean = p->GetBinContent(p->FindBin(eta));
                hm_.h<TH1D>("distr", "eta_normalised_p2")->Fill(p2 / p2_mean);

                const char* templ[] = { "EEprof2", "EBprof2"};
                const char* subdet[] =  { "nZ_", "" , "pZ_" };

                sprintf(str, "%sp2", subdet[iz+1]);
                hm_.h<TProfile2D>( templ[isEB], str)->Fill(iy, ix, p2);
                sprintf(str, "%sp2%s", subdet[iz+1], weekly_);
                hm_.h<TProfile2D>( templ[isEB], str )->Fill(iy, ix, p2);

                sprintf(str, "%sp2Norm", subdet[iz+1]);
                hm_.h<TProfile2D>(templ[isEB], str )->Fill(iy, ix, p2 / p2_mean);
                if(iid==2) hm_.h<TProfile2D>(templ[iz+1], str)->SetErrorOption("s");
                sprintf(str, "%sp2Norm_%s", subdet[iz+1], weekly_);
                hm_.h<TProfile2D>(templ[isEB], str )->Fill(iy, ix, p2 / p2_mean);
                if(iid==2) hm_.h<TProfile2D>(templ[isEB], str)->SetErrorOption("s");
        }
        fill_histories(t);
}

#endif
