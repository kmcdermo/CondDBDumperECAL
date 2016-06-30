#ifndef CONDDB_DUMPER_hh
#define CONDDB_DUMPER_hh

#include "CondCore/CondDB/interface/IOVProxy.h"
#include "CondCore/Utilities/interface/Utilities.h"

#include "CondCore/CondDB/interface/ConnectionPool.h"

#include <boost/program_options.hpp>
#include <iterator>
#include <iostream>

#include "CondFormats/BeamSpotObjects/interface/BeamSpotObjects.h"
#include "CondFormats/EcalObjects/interface/EcalADCToGeVConstant.h"
#include "CondFormats/EcalObjects/interface/EcalChannelStatus.h"
#include "CondFormats/EcalObjects/interface/EcalClusterLocalContCorrParameters.h"
#include "CondFormats/EcalObjects/interface/EcalCondObjectContainer.h"
#include "CondFormats/EcalObjects/interface/EcalGainRatios.h"
#include "CondFormats/EcalObjects/interface/EcalPedestals.h"
#include "CondFormats/EcalObjects/interface/EcalTimeOffsetConstant.h"
#include "CondFormats/EcalObjects/interface/EcalTPGLinearizationConst.h"
#include "CondFormats/ESObjects/interface/ESEEIntercalibConstants.h"
#include "CondFormats/ESObjects/interface/ESGain.h"
#include "CondFormats/ESObjects/interface/ESIntercalibConstants.h"
#include "CondFormats/RunInfo/interface/RunInfo.h"

#include "DataFormats/EcalDetId/interface/EBDetId.h"
#include "DataFormats/EcalDetId/interface/EEDetId.h"

namespace cond {

        template<class C>
        class CondDBDumper : public cond::Utilities {

                public:

                        CondDBDumper(std::string class_name) : Utilities("conddb_dumper"), _class_name(class_name)
                        {
                                addConnectOption();
                                addAuthenticationOptions();
                                addOption<bool>("join", "j", "produce one single output file, where IOVs are separated by double line break and a comment line starting with `#'");
                                addOption<bool>("verbose","v","verbose");
                                addOption<cond::Time_t>("beginTime","b","begin time (first since) (optional)");
                                addOption<cond::Time_t>("endTime","e","end time (last till) (optional)");
                                addOption<int>("niov","n","number of IOVs to be dumped");
                                addOption<int>("prescale","s","prescale factor, i.e. dump 1 in N IOVs");
                                addOption<std::string>("object","O","object to be dumped (required)");
                                addOption<std::string>("output","o","output file");
                                addOption<std::string>("tag","t","tag to be dumped (required)");

                                _ids.resize(EBDetId::MAX_HASH + 1 + EEDetId::kSizeForDenseIndexing);
                                for (int hi = EBDetId::MIN_HASH; hi <= EBDetId::MAX_HASH; ++hi ) {
                                        EBDetId ebId = EBDetId::unhashIndex(hi);
                                        if (ebId != EBDetId()) {
                                                _ids[hi] = ebId;
                                        }
                                }
                                for ( int hi = 0; hi < EEDetId::kSizeForDenseIndexing; ++hi ) {
                                        EEDetId eeId = EEDetId::unhashIndex(hi);
                                        if (eeId != EEDetId()) {
                                                int idx = EBDetId::MAX_HASH + 1 + hi;
                                                _ids[idx] = eeId;
                                        }
                                }
                                assert(_ids.size() == 75848);
                                assert(_ids.size() == EBDetId::MAX_HASH + 1 + EEDetId::kSizeForDenseIndexing);

                        }

                        ~CondDBDumper()
                        {
                        }


                        void print(int cnt, const cond::Iov_t & iov)
                        {
                                printf("%d  %llu %llu\n", cnt, iov.since, iov.till);
                                return;
                                time_t s = iov.since>>32;
                                time_t t = iov.till>>32;
                                char ss[64], st[64];
                                ctime_r(&s, ss);
                                ctime_r(&s, st);
                                ss[strlen(ss) - 1] = '\0';
                                st[strlen(st) - 1] = '\0';
                                printf("%d  %s (%ld)  -->  %s (%ld)\n", cnt, ss, s, st, t);
                        }


                        FILE * open_file(char * filename)
                        {
                                FILE * fd = fopen(filename, "a");
                                if (!fd) {
                                        char err[256];
                                        sprintf(err, "[cond::ICDump::dump] Impossible to open file `%s' for dumping:", filename);
                                        perror(err);
                                        exit(1);
                                }
                                fprintf(stderr, "going to dump on `%s'\n", filename);
                                return fd;
                        }

                        // main loop
                        int execute()
                        {
                                std::string connect;
                                if (hasOptionValue("connect")) {
                                        connect = getOptionValue<std::string>("connect" );
                                } else {
                                        connect = "frontier://FrontierProd/CMS_CONDITIONS";
                                }

                                cond::persistency::ConnectionPool connPool;

                                if(hasOptionValue("authPath")){
                                        connPool.setAuthenticationPath( getOptionValue<std::string>( "authPath") ); 
                                }

                                int niov = -1;
                                if (hasOptionValue("niov")) niov = getOptionValue<int>("niov");

                                int prescale = 1;
                                if (hasOptionValue("prescale")) prescale = getOptionValue<int>("prescale");
                                assert(prescale > 0);

                                cond::Time_t since = std::numeric_limits<cond::Time_t>::min();
                                if( hasOptionValue("beginTime" )) since = getOptionValue<cond::Time_t>("beginTime");
                                cond::Time_t till = std::numeric_limits<cond::Time_t>::max();
                                if( hasOptionValue("endTime" )) till = getOptionValue<cond::Time_t>("endTime");

                                bool join = false;
                                if (hasOptionValue("join")) join = true;

                                connPool.configure();
                                cond::persistency::Session session = connPool.createSession( connect );

                                std::string tag = getOptionValue<std::string>("tag");

                                printf("%s %s\n", connect.c_str(), tag.c_str());

                                session.transaction().start( true );
                                cond::persistency::IOVProxy iov = session.readIov(tag, true);
                                std::string obj_type = iov.payloadObjectType();

                                auto first_iov = iov.begin();
                                auto last_iov  = iov.begin();
                                for (int i = 0; i < iov.loadedSize() - 1; ++i) ++last_iov;

                                std::cout << (*first_iov).since << " " << (*last_iov).since << "\n";
                                char filename[512];
                                //sprintf(filename, "dump_%s__since_%08llu_jtill_since_%08llu.dat", obj_type.c_str(), (*first_iov).since, (*last_iov).since);
                                sprintf(filename, "dump_%s__since_%08llu_jtill_since_%08llu.dat", _class_name.c_str(), (*first_iov).since, (*last_iov).since);
                                if (hasOptionValue("output")) sprintf(filename, getOptionValue<std::string>("output").c_str());

                                int cnt = 0, cnt_iov = -1;
                                FILE * fout;
                                if (join) remove(filename);
                                for (auto i : iov) {
                                        ++cnt_iov;
                                        if (i.since < since || i.till > till) continue;
                                        if (cnt_iov % prescale != 0) continue;
                                        ++cnt;
                                        print(cnt_iov, i);
                                        boost::shared_ptr<C> pa = session.fetchPayload<C>(i.payloadId);
                                        if (!join) sprintf(filename, "dump_%s__since_%08llu_till_%08llu.dat", _class_name.c_str(), i.since, i.till);
                                        fout = open_file(filename);
                                        if (join) fprintf(fout, "# new IOV: since %llu  till %llu\n", i.since, i.till);
                                        dump(fout, *pa);
                                        if (join) fprintf(fout, "\n\n");
                                        fclose(fout);
                                        if (niov > 0 && cnt >= niov) break;
                                }
                                session.transaction().commit();
                                return 0;
                        }

                        struct Coord {
                                int ix_;
                                int iy_;
                                int iz_;
                        } _c;


                        void coord(DetId id)
                        {
                                if (id.subdetId() == EcalBarrel) {
                                        EBDetId eid(id);
                                        _c.ix_ = eid.ieta();
                                        _c.iy_ = eid.iphi();
                                        _c.iz_ = 0;
                                } else if (id.subdetId() == EcalEndcap) {
                                        EEDetId eid(id);
                                        _c.ix_ = eid.ix();
                                        _c.iy_ = eid.iy();
                                        _c.iz_ = eid.zside();
                                } else {
                                        fprintf(stderr, "[coord] ERROR: invalid DetId %d", id.rawId());
                                        assert(0);
                                }
                        }


                        void dump(FILE * fd, bool) {}

                        void dump(FILE * fd, EcalCondObjectContainer<float> & o)
                        {
                                for (size_t i = 0; i < _ids.size(); ++i) {
                                        DetId id(_ids[i]);
                                        EcalCondObjectContainer<float>::const_iterator it = o.find(id);
                                        if (it == o.end()) {
                                                fprintf(stderr, "Cannot find value for DetId %u", id.rawId());
                                        }
                                        coord(_ids[i]);
                                        fprintf(fd, "%d %d %d %f %u\n", _c.ix_, _c.iy_, _c.iz_, *it, id.rawId());
                                }

                        }

                        void dump(FILE * fd, EcalChannelStatus & o)
                        {
                                for (size_t i = 0; i < _ids.size(); ++i) {
                                        DetId id(_ids[i]);
                                        EcalChannelStatus::const_iterator it = o.find(id);
                                        if (it == o.end()) {
                                                fprintf(stderr, "Cannot find value for DetId %u", id.rawId());
                                        }
                                        coord(_ids[i]);
                                        fprintf(fd, "%d %d %d %d %d\n", _c.ix_, _c.iy_, _c.iz_, (*it).getStatusCode(), id.rawId());
                                }
                        }

                        void dump(FILE * fd, EcalTPGLinearizationConst & o)
                        {
                                for (size_t i = 0; i < _ids.size(); ++i) {
                                        DetId id(_ids[i]);
                                        EcalTPGLinearizationConst::const_iterator it = o.find(id);
                                        if (it == o.end()) {
                                                fprintf(stderr, "Cannot find value for DetId %u", id.rawId());
                                        }
                                        coord(_ids[i]);
                                        fprintf(fd, "%d %d %d %u %u %u %u %u %u  %u\n", _c.ix_, _c.iy_, _c.iz_,
                                                it->mult_x12, it->shift_x12,
                                                it->mult_x6,  it->shift_x6,
                                                it->mult_x1,  it->shift_x1,
                                                id.rawId());
                                }
                        }

                        void dump(FILE * fd, EcalPedestals & o)
                        {
                                for (size_t i = 0; i < _ids.size(); ++i) {
                                        DetId id(_ids[i]);
                                        EcalPedestals::const_iterator it = o.find(id);
                                        if (it == o.end()) {
                                                fprintf(stderr, "Cannot find value for DetId %u", id.rawId());
                                        }
                                        coord(_ids[i]);
                                        fprintf(fd, "%d %d %d  %f %f  %f %f  %f %f %u\n", _c.ix_, _c.iy_, _c.iz_,
                                                (*it).mean_x12, (*it).rms_x12,
                                                (*it).mean_x6,  (*it).rms_x6,
                                                (*it).mean_x1,  (*it).rms_x1,
                                                id.rawId());
                                }
                        }

                        void dump(FILE * fd, EcalADCToGeVConstant & a)
                        {
                                fprintf(fd, "EB= %f  EE= %f\n", a.getEBValue(), a.getEEValue());
                        }

                        void dump(FILE * fd, EcalTimeOffsetConstant & o)
                        {
                                fprintf(fd, "EB= %f  EE= %f\n", o.getEBValue(), o.getEEValue());
                        }

                        void dump(FILE * fd, EcalClusterLocalContCorrParameters & a)
                        {
                                const EcalFunctionParameters & p = a.params();
                                fprintf(fd, "# %lu parameter(s)\n", p.size());
                                for (size_t s = 0; s < p.size(); ++s) {
                                        fprintf(fd, " %f", p[s]);
                                }
                                fprintf(fd, "\n");
                        }

                        void dump(FILE * fd, EcalGainRatios & o)
                        {
                                for (size_t i = 0; i < _ids.size(); ++i) {
                                        DetId id(_ids[i]);
                                        EcalGainRatios::const_iterator it = o.find(id);
                                        if (it == o.end()) {
                                                fprintf(stderr, "Cannot find value for DetId %u", id.rawId());
                                        }
                                        coord(_ids[i]);
                                        fprintf(fd, "%d %d %d  %f %f  %u\n", _c.ix_, _c.iy_, _c.iz_,
                                                (*it).gain12Over6(), (*it).gain6Over1(),
                                                id.rawId());
                                }
                        }

                        void dump(FILE * fd, ESEEIntercalibConstants & ic)
                        {
                                fprintf(fd,
                                        "GammaLow0:  %f   AlphaLow0:  %f\n"
                                        "GammaLow1:  %f   AlphaLow1:  %f\n"
                                        "GammaLow2:  %f   AlphaLow2:  %f\n"
                                        "GammaLow3:  %f   AlphaLow3:  %f\n"
                                        "GammaHigh0: %f   AlphaHigh0: %f\n"
                                        "GammaHigh1: %f   AlphaHigh1: %f\n"
                                        "GammaHigh2: %f   AlphaHigh2: %f\n"
                                        "GammaHigh3: %f   AlphaHigh3: %f\n",
                                        ic.getGammaLow0(),  ic.getAlphaLow0(),
                                        ic.getGammaLow1(),  ic.getAlphaLow1(),
                                        ic.getGammaLow2(),  ic.getAlphaLow2(),
                                        ic.getGammaLow3(),  ic.getAlphaLow3(),
                                        ic.getGammaHigh0(), ic.getAlphaHigh0(),
                                        ic.getGammaHigh1(), ic.getAlphaHigh1(),
                                        ic.getGammaHigh2(), ic.getAlphaHigh2(),
                                        ic.getGammaHigh3(), ic.getAlphaHigh3()
                                       );

                        }

                        void dump(FILE * fd, ESGain & g)
                        {
                                fprintf(fd, "gain= %f\n", g.getESGain());
                        }

                        void dump(FILE * fd, ESIntercalibConstants & ic)
                        {
                                for (int i = 0; i < ESDetId::kSizeForDenseIndexing; ++i) {
                                        ESDetId id(ESDetId::detIdFromDenseIndex(i));
                                        ESIntercalibConstants::const_iterator it = ic.find(id);
                                        assert(it != ic.end());
                                        fprintf(fd, "%d %f\n", id.rawId(), *it);
                                }
                        }

                        void dump(FILE * fd, RunInfo & ri)
                        {
                                fprintf(fd, "%d %lld %lld (%s -> %s)\n", 
                                        ri.m_run,
                                        ri.m_start_time_ll,
                                        ri.m_stop_time_ll,
                                        ri.m_start_time_str.c_str(),
                                        ri.m_stop_time_str.c_str());
                        }

                        void dump(FILE * fd, BeamSpotObjects & bs)
                        {
                                fprintf(fd, "z= %f  z_err= %f  sigma_z= %f sigma_z_err= %f\n",
                                        bs.GetZ(), bs.GetZError(), bs.GetSigmaZ(), bs.GetSigmaZError());
                        }


                private:
                        std::string _class_name;
                        std::vector<DetId> _ids;
        };
}

#endif
