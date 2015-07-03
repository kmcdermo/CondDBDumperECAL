#ifndef __ECAL_LASER_GREEN_ANALYSIS
#define __ECAL_LASER_GREEN_ANALYSIS

#include <ctime>
#include <vector>
#include <set>

#include "CondFormats/EcalObjects/interface/EcalChannelStatus.h"
#include "usercode/DBDump/interface/Quantile.h"
#include "usercode/DBDump/interface/HistoManager.h"

#define NSUBDET 3
#define NMAXVAL 10

class DetId;
class EcalLaserAPDPNRatios;
class HistoManager;

class EcalLaserGreenAnalysis {
        public:
                EcalLaserGreenAnalysis(const char * geom_filename = "detid_geom.dat");
                ~EcalLaserGreenAnalysis() { printSummary(); };

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
                void max_min(DetId id, float val);
                void max_min_plots();
                void max_min_values();

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
                std::vector<float> max_val_;
                std::vector<float> min_val_;
                std::vector<float> geom_eta_;
                std::vector<uint16_t> ch_status_;

                std::set<int> inf_; // +/- DetId according to inf sign
                std::set<int> nan_; // DetId

                std::set<float, std::greater<float> > min_values_[75848];
                std::set<float> max_values_[75848];

                char ecalPart[3];
                char eename[3];

                char str[128];

                int set_geom_;
                int set_ch_status_;
};

#endif
