#include "CondDBDumperECAL/DBDump/interface/EcalLaserDumper.h"

#include "CondFormats/EcalObjects/interface/EcalLaserAPDPNRatios.h"
#include "CondFormats/EcalObjects/interface/EcalChannelStatus.h"
#include "CalibCalorimetry/EcalLaserCorrection/interface/EcalLaserDbService.h"
#include "CalibCalorimetry/EcalLaserAnalyzer/interface/MEEBGeom.h"
#include "CalibCalorimetry/EcalLaserAnalyzer/interface/MEEEGeom.h"

#include <assert.h>
#include <errno.h>
#include <string>
#include <sys/stat.h>


EcalLaserDumper::EcalLaserDumper(std::string dir) :
        dir_(dir)
{
        int rc = mkdir(dir_.c_str(), 0755);
        if (rc) {
                fprintf(stderr, "failed to create `%s': %s (%d)\n", dir_.c_str(), strerror(errno), errno);
                assert(0);
        }

        ecalDetIds_.resize(EBDetId::MAX_HASH + 1 + EEDetId::kSizeForDenseIndexing);
        int idx = -1;
        for (int hi = EBDetId::MIN_HASH; hi <= EBDetId::MAX_HASH; ++hi ) {
                EBDetId ebId = EBDetId::unhashIndex(hi);
                if (ebId != EBDetId()) {
                        ecalDetIds_[hi] = ebId;
                }
        }
        for ( int hi = 0; hi < EEDetId::kSizeForDenseIndexing; ++hi ) {
                EEDetId eeId = EEDetId::unhashIndex(hi);
                if (eeId != EEDetId()) {
                        idx = EBDetId::MAX_HASH + 1 + hi;
                        ecalDetIds_[idx] = eeId;
                }
        }
        assert(ecalDetIds_.size() == 75848);
        assert(ecalDetIds_.size() == EBDetId::MAX_HASH + 1 + EEDetId::kSizeForDenseIndexing);
        char tmp[512];
        for (size_t i = 0; i < ecalDetIds_.size(); ++i) {
                Coord c;
                coord(ecalDetIds_[i], &c);
                sprintf(tmp, "%s/%d_%d_%d.dat", dir_.c_str(), c.ix_, c.iy_, c.iz_);
                f_ = fopen(tmp, "w");
                if (f_ == NULL) {
                        fprintf(stderr, "failed to open `%s': %s (%d)\n", dir_.c_str(), strerror(errno), errno);
                        assert(0);
                } else {
                        fprintf(f_, "# DetId=%d\n", ecalDetIds_[i].rawId());
                        fclose(f_);
                }
        }
}


EcalLaserDumper::~EcalLaserDumper()
{
        //for (int i = 0; i < EBDetId::MAX_HASH + 1 + EEDetId::kSizeForDenseIndexing; ++i) {
        //        fclose(f_[i]);
        //}
}


void EcalLaserDumper::coord(DetId id, Coord * c)
{
        if (id.subdetId() == EcalBarrel) {
                EBDetId eid(id);
                c->ix_ = eid.ieta();
                c->iy_ = eid.iphi();
                c->iz_ = 0;
        } else if (id.subdetId() == EcalEndcap) {
                EEDetId eid(id);
                c->ix_ = eid.ix();
                c->iy_ = eid.iy();
                c->iz_ = eid.zside();
        } else {
                fprintf(stderr, "[coord] ERROR: invalid DetId %d", id.rawId());
                assert(0);
        }
}


void EcalLaserDumper::dump(time_t t, const EcalLaserAPDPNRatios * apdpn, const EcalLaserAPDPNRatios * apdpn2)
{
        char tmp[512];
        for (size_t iid = 0; iid < ecalDetIds_.size(); ++iid) {
                DetId id(ecalDetIds_[iid]);
                EcalLaserAPDPNRatios::EcalLaserAPDPNRatiosMap::const_iterator itAPDPN;
                itAPDPN = apdpn->getLaserMap().find(id);
                float p2 = (*itAPDPN).p2;
                Coord c;
                coord(ecalDetIds_[iid], &c);
                sprintf(tmp, "%s/%d_%d_%d.dat", dir_.c_str(), c.ix_, c.iy_, c.iz_);
                FILE * f_ = fopen(tmp, "a");
                fprintf(f_, "%lu %f", t, p2);
                if (apdpn2 != NULL) {
                        itAPDPN = apdpn2->getLaserMap().find(id);
                        p2 = (*itAPDPN).p2;
                        fprintf(f_, " %f", p2);
                }
                fprintf(f_, "\n");
                fclose(f_);
        }
}

void EcalLaserDumper::dumpForDB(time_t iov_begin, time_t iov_end, const EcalLaserAPDPNRatios * apdpn, const char * file_name)
{
        f_ = fopen(file_name, "w");
        fprintf(f_, "T %ld %ld ", iov_begin, iov_end);
        const EcalLaserAPDPNRatios::EcalLaserTimeStampMap & times = apdpn->getTimeMap();
        for (EcalLaserAPDPNRatios::EcalLaserTimeStampMap::const_iterator it = times.begin(); it != times.end(); ++it)
        {
                fprintf(f_, " %d", it->t2.unixTime());
        }
        EcalLaserAPDPNRatios::EcalLaserAPDPNRatiosMap::const_iterator itAPDPN;
        for (size_t iid = 0; iid < ecalDetIds_.size(); ++iid) {
                DetId id(ecalDetIds_[iid]);
                itAPDPN = apdpn->getLaserMap().find(id);
                fprintf(f_, "P %d %f %f %f\n", id.rawId(), (*itAPDPN).p1, (*itAPDPN).p2, (*itAPDPN).p3);
        }
        fclose(f_);
}
