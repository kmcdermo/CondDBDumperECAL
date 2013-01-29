#include "CondCore/Utilities/interface/Utilities.h"

#include "FWCore/PluginManager/interface/PluginManager.h"
#include "FWCore/PluginManager/interface/standard.h"
#include "FWCore/ServiceRegistry/interface/ServiceRegistry.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"

#include "CondCore/DBCommon/interface/Auth.h"
#include "CondCore/DBCommon/interface/DbConnection.h"
#include "CondCore/DBCommon/interface/DbScopedTransaction.h"
#include "CondCore/DBCommon/interface/DbSession.h"
#include "CondCore/DBCommon/interface/DbTransaction.h"
#include "CondCore/DBCommon/interface/Exception.h"
#include "CondCore/MetaDataService/interface/MetaData.h"

#include "CondCore/DBCommon/interface/Time.h"
#include "CondFormats/Common/interface/TimeConversions.h"
#include "CondFormats/RunInfo/interface/RunInfo.h"

#include "CondCore/IOVService/interface/IOVProxy.h"
#include "CondFormats/EcalObjects/interface/EcalIntercalibConstants.h"
#include "CondFormats/EcalObjects/interface/EcalLaserAPDPNRatios.h"
#include "CondFormats/DataRecord/interface/EcalLaserAPDPNRatiosRcd.h"
#include "CondFormats/EcalObjects/interface/EcalLaserAPDPNRatiosRef.h"
#include "CondFormats/DataRecord/interface/EcalLaserAPDPNRatiosRefRcd.h"

#include "../src/EcalLaserPlotter.h"

#include <err.h>
#include <signal.h>

#include <boost/program_options.hpp>
#include <iterator>
#include <iostream>

//--------------------------------------------------------------

#define NIOV 50000
#define PRESCALE 50
#define RATIO_QUALITY 1
#define QUALITY_FILE "/tmp/ferriff/id_quality.dat"

#define RUN_MIN 203773
//#define RUN_MAX 197000
//#define RUN_MAX 195050
#define RUN_MAX 300000
#define SINCE   INT_MIN
#define TILL    INT_MAX

#define CONN_LT1 "frontier://FrontierProd/CMS_COND_42X_ECAL_LAS"
//#define TAG_LT1  "EcalLaserAPDPNRatios_20121020_447_p1_v2"
#define TAG_LT1  "EcalLaserAPDPNRatios_20130124_447_p1_v2"

//#define CONN_LT2 "frontier://FrontierProd/CMS_COND_42X_ECAL_LAS"
//#define TAG_LT2  "EcalLaserAPDPNRatios_prompt"
#define CONN_LT2 "sqlite:////tmp/ferriff/dump_one_shot_v1.5_447_190114_209160.db"
#define TAG_LT2  "EcalLaserAPDPNRatios_test"

//#define CONN_IC1 "frontier://FrontierProd/CMS_COND_31X_ECAL"
//#define TAG_IC1  "EcalIntercalibConstants_V20121215_NLT_MeanPizPhiABC_EleABC_HR9EtaScABC_PhisymCor2_EoPCorAvg1_mixed"
#define CONN_IC1 "frontier://FrontierProd/CMS_COND_31X_ECAL"
#define TAG_IC1  "EcalIntercalibConstants_V20121215_NLT_MeanPizPhiABC_EleABC_HR9EtaScABC_PhisymCor2_EoPCorAvg1_mixed"

#define CONN_IC2 "frontier://FrontierProd/CMS_COND_31X_ECAL"
//#define TAG_IC2  "EcalIntercalibConstants_V1_express" // prompt
#define TAG_IC2  "EcalIntercalibConstants_V20121215_NLT_MeanPizPhiABC_EleABC_HR9EtaScABC_PhisymCor2_EoPCorAvg1_mixed"

#define CONN_CHS "frontier://FrontierProd/CMS_COND_31X_ECAL"
#define TAG_CHS "EcalChannelStatus_v07_offline"

#define CONN_RI  "frontier://PromptProd/CMS_COND_31X_RUN_INFO"
#define TAG_RI   "runinfo_31X_hlt"

//--------------------------------------------------------------

typedef EcalLaserAPDPNRatios A;
typedef EcalLaserAPDPNRatios::EcalLaserAPDPNRatiosMap AMap;
typedef EcalLaserAPDPNRatios::EcalLaserAPDPNRatiosMap::const_iterator AMapCit;
typedef EcalLaserAPDPNRatios::EcalLaserAPDPNpair AP;
typedef EcalLaserAPDPNRatios::EcalLaserTimeStamp AT;
typedef EcalIntercalibConstants IC;
typedef EcalIntercalibConstants::const_iterator ICit;

std::vector<DetId> ecalDetIds_;
EcalLaserPlotter lp;
std::set<int> toBeCheckedIds_;

int rquality(const AP & p)
{
        if (fabs(p.p1 - 1) > 0.001) return 1;
        return 0;
}

void rquality(const A o)
{
        AMapCit it;
        for (size_t i = 0; i < ecalDetIds_.size(); ++i) {
                DetId id(ecalDetIds_[i]);
                it = o.getLaserMap().find(id);
                if (rquality(*it)) toBeCheckedIds_.insert(id.rawId());
        }
}

int dump_quality(const char * filename)
{
        FILE * fd = fopen(filename, "w");
        if (!fd) err(3, "cannot open file `%s'", filename);
        // print rquality
        for (std::set<int>::const_iterator it = toBeCheckedIds_.begin(); it != toBeCheckedIds_.end(); ++it) {
                fprintf(fd, "rq %d\n", *it);
        }
        fclose(fd);
        return 0;

}

void mergePoints(const AP & pa, const AP & pb, AP & res)
{
        res.p1 = pa.p1 / pb.p1;
        res.p2 = pa.p2 / pb.p2;
        res.p3 = pa.p3 / pb.p3;
        //printf("points merged: %f %f %f\n", res.p1, pa.p1, pb.p1);
        //printf("               %f %f %f\n", res.p2, pa.p2, pb.p2);
        //printf("               %f %f %f\n", res.p3, pa.p3, pb.p3);
}

void mergeTimes(const AT & ta, const AT & tb, AT & res)
{
        // FIXME: add conditions to check pa.t_i with pb.t_i
        res.t1 = ta.t1;
        res.t2 = ta.t2;
        res.t3 = ta.t3;
}

void multiplyPoints(AP & res, float c)
{
        res.p1 = res.p1 * c;
        res.p2 = res.p2 * c;
        res.p3 = res.p3 * c;
}

void merge(const A & oA1, const A & oA2, const IC & ic1, const IC & ic2, A & res)
{
        AP p;
        AT ta, tb, ts;
        AMapCit ita1, ita2;
        ICit it1, it2;
        for (size_t i = 0; i < ecalDetIds_.size(); ++i) {
                DetId id(ecalDetIds_[i]);
                ita1 = oA1.getLaserMap().find(id);
                ita2 = oA2.getLaserMap().find(id);
                mergePoints(*ita1, *ita2, p);
                it1 = ic1.find(id);
                if (it1 != ic1.end()) multiplyPoints(p, (*it1));
                it2 = ic2.find(id);
                if (it2 != ic2.end()) multiplyPoints(p, 1./(*it2));
                //fprintf(stderr, "IC ratio: %f  %f %f\n", *it1 / *it2, *it1, *it2);
                //fprintf(stderr, "points multiplied: %f %f %f\n", p.p1, p.p2, p.p3);
                res.setValue(id, p);
                //if (i > 9) break;
        }
        for (size_t i = 0; i < 92; ++i) {
                ta = oA1.getTimeMap()[i];
                tb = oA1.getTimeMap()[i];
                mergeTimes(ta, tb, ts);
                res.setTime(i, ts);
        }
}

//typedef struct Dbo {
//        cond::DbConnection * _c;
//        cond::DbSession _s;
//        cond::MetaData _m;
//        cond::DbScopedTransaction _t;
//        cond::IOVProxy _i;
//        std::string _connection;
//        std::string _tag;
//} Dbo;
//
//void init(Dbo & d)
//{
//        d._s = d._c->createSession();
//        d._s.open(d._connection, "connect", true);
//        //d._m(d._s);
//        //d._t(d._s);
//        d._t.start(true);
//        std::string token = d._m.getToken(d._tag);
//        //d._i(d._s, token);
//        //cond::IOVProxy i(d._s, token);
//}

void init()
{
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
}

void print_info(cond::IOVProxy & i)
{
        const std::set<std::string> payloadClasses = i.payloadClasses();
        std::cout << "\nStamp: " << i.iov().comment()
                << "; time " <<  cond::time::to_boost(i.iov().timestamp())
                << "; revision " << i.iov().revision();
        std::cout <<"\nTimeType " << cond::timeTypeSpecs[i.timetype()].name
                <<"\nPayloadClasses:\n";
        for (std::set<std::string>::const_iterator it = payloadClasses.begin(); it != payloadClasses.end(); ++it) {
                std::cout << " --> " << *it << "\n";
        }
        std::cout
                <<"since \t till \t payloadToken"<<std::endl;
        int cnt = 0;
        for (cond::IOVProxy::const_iterator ita = i.begin(); ita != i.end(); ++ita, ++cnt) {
                std::cout << cnt << " " << ita->since() << " -> " << ita->till() << "\n";
        }
}


typedef struct RI {
        int _run;
        int _start;
} RI;

bool comp(const RI & p1, const RI & p2)
{
            return p1._start < p2._start;
}


void get_run_start(cond::IOVProxy & i, cond::DbSession & s, std::vector<RI> & r, int min_run, int max_run)
{
        for (cond::IOVProxy::const_iterator it = i.begin(); it != i.end(); ++it) {
                int run = (int)it->since();
                if (run >= min_run && run <= max_run) {
                        boost::shared_ptr<RunInfo> p = s.getTypedObject<RunInfo>(it->token());
                        RI ri;
                        ri._run = p->m_run;
                        ri._start = (int)(p->m_start_time_ll / 1e+06);
                        if (ri._run != -1) {
                                printf("%d %d\n", ri._run, ri._start);
                                r.push_back(ri);
                        }
                }
        }
        std::sort(r.begin(), r.end(), comp);
}


void get_run_start(const char * filename, std::vector<RI> & r, int min_run, int max_run)
{
        fprintf(stdout, "Initialising run starts from `%s'.\n", filename);
        FILE * fd = fopen(filename, "r");
        if (!fd) {
                err(2, "cannot open file `%s'.\n", filename);
        }
        RI ri;
        while (fscanf(fd, "%d %d", &ri._run, &ri._start) != EOF) {
                if (ri._run < min_run || ri._run > max_run) continue;
                r.push_back(ri);
        }
        std::sort(r.begin(), r.end(), comp);
}


int get_run(int t, std::vector<RI> & r)
{
        static RI ri;
        ri._start = t;
        std::vector<RI>::const_iterator it = std::lower_bound(r.begin(), r.end(), ri, comp);
        std::vector<RI>::const_iterator jt;
        if (it != r.end() && it - 1 != r.begin()) --it;
        return it != r.end() && t > it->_start ? it->_run : -1;
}


void save(int )
{
        lp.save();
        dump_quality(QUALITY_FILE);
        exit(0);
}


int main( int argc, char** argv )
{
        bool v = false;

        signal(SIGINT, save);

        // begfrom "CondCore/Utilities/src/Utilities.cc"
        edmplugin::PluginManager::Config config;
        edmplugin::PluginManager::configure(edmplugin::standard::config());
        std::vector<edm::ParameterSet> psets;
        edm::ParameterSet pSet;
        pSet.addParameter("@service_type",std::string("SiteLocalConfigService"));
        psets.push_back(pSet);
        static edm::ServiceToken services(edm::ServiceRegistry::createSet(psets));
        static edm::ServiceRegistry::Operate operate(services);
        // endfrom

        cond::DbConnection conn;
        conn.configure();

        std::string connection, tag, token;

        // session for laser tag 1
        cond::DbSession s_lt1 = conn.createSession();
        s_lt1.open(CONN_LT1, "connect", true);
        cond::MetaData m_lt1(s_lt1);
        cond::DbScopedTransaction t_lt1(s_lt1);
        t_lt1.start(true);
        connection = CONN_LT1;
        tag = TAG_LT1;
        token = m_lt1.getToken(tag);
        cond::IOVProxy i_lt1(s_lt1, token);
        fprintf(stderr, "connection: %s  tag: %s\n", connection.c_str(), tag.c_str());
        if (v) print_info(i_lt1);

        // the following is awful but the interfaces of MetaData and DbScopedTransaction
        // make it hard to enclose these bloks in a function... to be
        // better developed -- FIXME, TODO

        // session for laser tag 2
        cond::DbSession s_lt2 = conn.createSession();
        s_lt2.open(CONN_LT2, "connect", true);
        cond::MetaData m_lt2(s_lt2);
        cond::DbScopedTransaction t_lt2(s_lt2);
        t_lt2.start(true);
        connection = CONN_LT2;
        tag = TAG_LT2;
        token = m_lt2.getToken(tag);
        cond::IOVProxy i_lt2(s_lt2, token);
        fprintf(stderr, "connection: %s  tag: %s\n", connection.c_str(), tag.c_str());
        if (v) print_info(i_lt2);

        // session for IC 1
        cond::DbSession s_ic1 = conn.createSession();
        s_ic1.open(CONN_IC1, "connect", true);
        cond::MetaData m_ic1(s_ic1);
        cond::DbScopedTransaction t_ic1(s_ic1);
        t_ic1.start(true);
        connection = CONN_IC1;
        tag = TAG_IC1;
        token = m_ic1.getToken(tag);
        cond::IOVProxy i_ic1(s_ic1, token);
        fprintf(stderr, "connection: %s  tag: %s\n", connection.c_str(), tag.c_str());
        if (v) print_info(i_ic1);

        // session for IC 2
        cond::DbSession s_ic2 = conn.createSession();
        s_ic2.open(CONN_IC2, "connect", true);
        cond::MetaData m_ic2(s_ic2);
        cond::DbScopedTransaction t_ic2(s_ic2);
        t_ic2.start(true);
        connection = CONN_IC2;
        tag = TAG_IC2;
        token = m_ic2.getToken(tag);
        cond::IOVProxy i_ic2(s_ic2, token);
        fprintf(stderr, "connection: %s  tag: %s\n", connection.c_str(), tag.c_str());
        if (v) print_info(i_ic2);

        // session for EcalChannelStatus
        cond::DbSession s_chs = conn.createSession();
        s_chs.open(CONN_CHS, "connect", true);
        cond::MetaData m_chs(s_chs);
        cond::DbScopedTransaction t_chs(s_chs);
        t_chs.start(true);
        connection = CONN_CHS;
        tag = TAG_CHS;
        token = m_chs.getToken(tag);
        cond::IOVProxy i_chs(s_chs, token);
        fprintf(stderr, "connection: %s  tag: %s\n", connection.c_str(), tag.c_str());
        if (v) print_info(i_chs);

        // session for runInfo
        cond::DbSession s_ri = conn.createSession();
        s_ri.open(CONN_RI, "connect", true);
        cond::MetaData m_ri(s_ri);
        cond::DbScopedTransaction t_ri(s_ri);
        t_ri.start(true);
        connection = CONN_RI;
        tag = TAG_RI;
        token = m_ri.getToken(tag);
        cond::IOVProxy i_ri(s_ri, token);
        fprintf(stderr, "connection: %s  tag: %s\n", connection.c_str(), tag.c_str());
        if (v) print_info(i_ri);

        fprintf(stderr, "comparing:\n%s * %s\nwith\n%s * %s\n", TAG_LT1, TAG_IC1, TAG_LT2, TAG_IC2);
        
        // parameters to get from command line options - TODO
        int niov = NIOV;
        int time, run;
        int prescale = PRESCALE;
        int run_min = RUN_MIN;
        int run_max = RUN_MAX;
        int since = SINCE;
        int till  = TILL;
        int ratio_quality = RATIO_QUALITY;
        char fquality[] = QUALITY_FILE;
        char output[] = "/tmp/ferriff/out.root";
        char * ri_file = getenv("LAVA_DB_COND_RI_FILE");

        fprintf(stderr, "going to analyse %d iov's\n"
                "from run %d to run %d\n"
                "from time %d to time %d\n",
                niov, run_min, run_max, since, till);

        std::vector<RI> run_start;
        if (ri_file == NULL) get_run_start(i_ri, s_ri, run_start, run_min, run_max);
        else get_run_start(ri_file, run_start, run_min, run_max);

        init();

        int cnt = 0, icnt = 0;
        //EcalLaserPlotter lp(geom.c_str());
        lp.setEcalGeometry("detid_geom.dat"); // FIXME
        A res;
        cond::IOVProxy::const_iterator itic;
        std::string ptk_ic1 = "", ptk_ic2 = "", ptk_chs = "";

        // outside the loop for poor man caching capabilities...
        boost::shared_ptr<EcalChannelStatus> chs;
        boost::shared_ptr<IC> pica;
        boost::shared_ptr<IC> picb;
        for (cond::IOVProxy::const_iterator ita = i_lt1.begin(); ita != i_lt1.end(); ++ita, ++cnt) {
                if (cnt % prescale != 0) continue;
                time = (time_t)ita->since()>>32;
                if (time == 0) continue;
                if (time < since || time > till) continue;
                run = get_run(time, run_start);
                if (run > run_max || run < run_min) continue;
                printf("% 4d  %llu -> %llu  %d\n", cnt, ita->since(), ita->till(), run);
                cond::IOVProxy::const_iterator itb = i_lt2.find(ita->since());

                boost::shared_ptr<A> pa = s_lt1.getTypedObject<A>(ita->token());
                boost::shared_ptr<A> pb = s_lt2.getTypedObject<A>(itb->token());

                // some basic caching...
                cond::IOVProxy::const_iterator itc = i_ic1.find(run);
                if (itc->token() != ptk_ic1) {
                        printf("loading new IC1 at run %d\n", run);
                        ptk_ic1 = itc->token();
                        pica = s_ic1.getTypedObject<IC>(itc->token());
                }
                cond::IOVProxy::const_iterator itd = i_ic2.find(run);
                if (itd->token() != ptk_ic2) {
                        printf("loading new IC2 at run %d\n", run);
                        ptk_ic2 = itd->token();
                        picb = s_ic2.getTypedObject<IC>(itd->token());
                }
                cond::IOVProxy::const_iterator itch = i_chs.find(run);
                if (itch->token() != ptk_chs) {
                        printf("loading new EcalChannelStatus at run %d\n", run);
                        ptk_chs = itch->token();
                        chs = s_ic2.getTypedObject<EcalChannelStatus>(itch->token());
                        lp.setEcalChannelStatus(*chs);
                }

                merge(*pa, *pb, *pica, *picb, res);
                if (ratio_quality) rquality(res);
                lp.fill(res, time);
                ++icnt;
                if (niov > 0 && icnt >= niov) break;
        }
        lp.save(output);
        dump_quality(fquality);
        return 0;
}
