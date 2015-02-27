#include "CondCore/Utilities/interface/Utilities.h"

#include "CondCore/DBCommon/interface/DbConnection.h"
#include "CondCore/DBCommon/interface/DbScopedTransaction.h"
#include "CondCore/DBCommon/interface/DbTransaction.h"
#include "CondCore/DBCommon/interface/Exception.h"
#include "CondCore/MetaDataService/interface/MetaData.h"

#include "CondCore/DBCommon/interface/Time.h"
#include "CondFormats/Common/interface/TimeConversions.h"

#include "CondCore/IOVService/interface/IOVProxy.h"
#include "CondFormats/EcalObjects/interface/EcalLaserAPDPNRatios.h"
#include "CondFormats/DataRecord/interface/EcalLaserAPDPNRatiosRcd.h"
#include "CondFormats/EcalObjects/interface/EcalLaserAPDPNRatiosRef.h"
#include "CondFormats/DataRecord/interface/EcalLaserAPDPNRatiosRefRcd.h"

#include <boost/program_options.hpp>
#include <iterator>
#include <iostream>

using namespace std;

float _values[75848];
int   _nvalues[75848];

namespace cond {
        class TriggerTag : public Utilities {
                public:
                        typedef EcalLaserAPDPNRatios A;
                        typedef EcalLaserAPDPNRatios::EcalLaserAPDPNRatiosMap AMap;
                        typedef EcalLaserAPDPNRatios::EcalLaserAPDPNRatiosMap::const_iterator AMapCit;
                        typedef EcalLaserAPDPNRatios::EcalLaserAPDPNpair AP;
                        typedef EcalLaserAPDPNRatios::EcalLaserTimeStamp AT;

                        std::string getToken(cond::DbSession & s, std::string & tag)
                        {
                                s = openDbSession("connect", true);
                                cond::MetaData metadata_svc(s);
                                cond::DbScopedTransaction transaction(s);
                                transaction.start(true);
                                std::string token = metadata_svc.getToken(tag);
                                transaction.commit();
                                std::cout << "Source iov token: " << token << "\n";
                                return token;
                        }

                        TriggerTag();
                        ~TriggerTag();
                        int execute();
                        void dump_time_average(FILE * fd, time_t tb, time_t te);
                        void dump_txt(FILE * fd);
                        void fill_time_average(const A & obja);
                        void reset_time_average();
                private:
                        std::vector<DetId> ecalDetIds_;
        };

}

cond::TriggerTag::TriggerTag():Utilities("cmscond_list_iov")
{
        addConnectOption();
        addAuthenticationOptions();
        addOption<bool>("verbose","v","verbose");
        addOption<bool>("all","a","list all tags (default mode)");
        addOption<cond::Time_t>("beginTime","b","begin time (first since) (optional)");
        addOption<cond::Time_t>("endTime","e","end time (last till) (optional)");
        addOption<std::string>("tag","t","first tag to be compared");
        addOption<int>("niov","n","number of IOV");
        addOption<int>("prescale","s","prescale factor");
        addOption<std::string>("dump", "d", "file for a txt dump");
        addOption<time_t>("deltaTime", "D", "time in [s] over which compute the average");
        //addOption<bool>("flat", "f", "flat iov, i.e. p1 = p2 = p3");
        addOption<bool>("txt", "x", "dump a txt file as per IC coeff. (NB: the time information of t1, t2, t3 is lost)");
        addOption<bool>("shift", "S", "in the filename, time = (time>>32)");

        ecalDetIds_.resize(EBDetId::MAX_HASH + 1 + EEDetId::kSizeForDenseIndexing);
        int idx = -1;
        for (int hi = EBDetId::MIN_HASH; hi <= EBDetId::MAX_HASH; ++hi ) {
                EBDetId ebId = EBDetId::unhashIndex(hi);
                if (ebId != EBDetId()) {
                        ecalDetIds_[hi] = ebId;
                        _nvalues[hi] = 0;
                        _values[hi] = 0.;
                }
        }
        for ( int hi = 0; hi < EEDetId::kSizeForDenseIndexing; ++hi ) {
                EEDetId eeId = EEDetId::unhashIndex(hi);
                if (eeId != EEDetId()) {
                        idx = EBDetId::MAX_HASH + 1 + hi;
                        ecalDetIds_[idx] = eeId;
                        _nvalues[idx] = 0;
                        _values[idx] = 0.;
                }
        }
        assert(ecalDetIds_.size() == 75848);
        assert(ecalDetIds_.size() == EBDetId::MAX_HASH + 1 + EEDetId::kSizeForDenseIndexing);
}

cond::TriggerTag::~TriggerTag(){
}


void cond::TriggerTag::dump_time_average(FILE * fd, time_t tb, time_t te)
{
        AT ta;
        AMapCit ita, itb;
        fprintf(fd, "T %ld %ld", tb, te);
        for (size_t i = 0; i < 92; ++i) {
                fprintf(fd, " %ld", tb + (te - tb) / 2);
        }
        fprintf(fd, "\n");
        float p2 = -1.;
        for (size_t i = 0; i < ecalDetIds_.size(); ++i) {
                p2 = _values[i] / _nvalues[i];
                fprintf(fd, "P %d %f %f %f\n", ecalDetIds_[i].rawId(), p2, p2, p2);
        }
}

void cond::TriggerTag::dump_txt(FILE * fd)
{
        AMapCit ita, itb;
        float p2 = -1.;
        for (size_t i = 0; i < ecalDetIds_.size(); ++i) {
                DetId id(ecalDetIds_[i]);
                p2 = _values[i] / _nvalues[i];
                if (id.subdetId() == EcalBarrel) {
                        EBDetId eid(id);
                        fprintf(fd, "%d %d %d %f  %u\n", eid.ieta(), eid.iphi(), 0,  
                                p2,
                                id.rawId());
                } else if (id.subdetId() == EcalEndcap) {
                        EEDetId eid(id);
                        fprintf(fd, "%d %d %d %f  %u\n", eid.ix(), eid.iy(), eid.zside(),
                                p2,
                                id.rawId());
                } else {
                        fprintf(stderr, "[dump] invalid DetId: %d\n", id.rawId());
                        exit(-1);
                }
        }
}

void cond::TriggerTag::fill_time_average(const A & obja)
{
        AMapCit it;
        for (size_t i = 0; i < ecalDetIds_.size(); ++i) {
                it = obja.getLaserMap().find(ecalDetIds_[i]);
                _values[i]  += it->p2;
                _nvalues[i] += 1;
        }
}

void cond::TriggerTag::reset_time_average()
{
        for (size_t i = 0; i < ecalDetIds_.size(); ++i) {
                _values[i] = 0.;
                _nvalues[i] = 0;
        }
}


int cond::TriggerTag::execute()
{
        initializePluginManager();

        bool listAll = hasOptionValue("all");
        cond::DbSession session  = openDbSession("connect", true);
        cond::DbScopedTransaction transaction(session);
        transaction.start(true);

        if(listAll){
                cond::MetaData metadata_svc(session);
                std::vector<std::string> alltags;
                cond::DbScopedTransaction transaction(session);
                transaction.start(true);
                metadata_svc.listAllTags(alltags);
                transaction.commit();
                std::copy (alltags.begin(),
                           alltags.end(),
                           std::ostream_iterator<std::string>(std::cout,"\n")
                          );
        } else {
                std::string tag1 = getOptionValue<std::string>("tag");

                cond::MetaData metadata_svc(session);
                cond::DbScopedTransaction transaction(session);
                transaction.start(true);
                transaction.commit();
                std::string token1, token2;
                token1 = metadata_svc.getToken(tag1);

                cond::Time_t since = std::numeric_limits<cond::Time_t>::min();
                if(hasOptionValue("beginTime")) since = getOptionValue<cond::Time_t>("beginTime");
                cond::Time_t till = std::numeric_limits<cond::Time_t>::max();
                if(hasOptionValue("endTime")) till = getOptionValue<cond::Time_t>("endTime");

                time_t t_interval = 1;
                if (hasOptionValue("deltaTime")) t_interval = getOptionValue<time_t>("deltaTime");
                bool shift = hasOptionValue("shift");

                FILE * fdump = NULL;
                char filename[256];
                if (hasOptionValue("dump")) {
                        fdump = fopen(getOptionValue<std::string>("dump").c_str(), "w");
                        assert(fdump);
                } else {
                        sprintf(filename, "dump_%s__avg_since_%08ld_till_%08ld.dat", tag1.c_str(), (long int)since>>(shift * 32), (long int)till>>(shift * 32));
                        fdump = fopen(filename, "w");
                        assert(fdump);
                }

                bool verbose = hasOptionValue("verbose");
                //bool flat = hasOptionValue("flat");
                bool txt = hasOptionValue("txt");

                //cond::IOVProxy iov(session, getToken(session, tag));
                cond::IOVProxy iov1(session, token1);

                std::cout << "since: " << since << "   till: " << till << "\n";

                iov1.range(since, till);

                //std::string payloadContainer = iov.payloadContainerName();
                const std::set<std::string> payloadClasses = iov1.payloadClasses();
                std::cout<<"Tag "<<tag1;
                if (verbose) std::cout << "\nStamp: " << iov1.iov().comment()
                        << "; time " <<  cond::time::to_boost(iov1.iov().timestamp())
                                << "; revision " << iov1.iov().revision();
                std::cout <<"\nTimeType " << cond::timeTypeSpecs[iov1.timetype()].name
                        <<"\nPayloadClasses:\n";
                for (std::set<std::string>::const_iterator it = payloadClasses.begin(); it != payloadClasses.end(); ++it) {
                        std::cout << " --> " << *it << "\n";
                }
                std::cout
                        <<"since \t till \t payloadToken"<<std::endl;

                int niov = -1;
                if (hasOptionValue("niov")) niov = getOptionValue<int>("niov");

                int prescale = 1;
                if (hasOptionValue("prescale")) prescale = getOptionValue<int>("prescale");
                assert(prescale > 0);

                static const unsigned int nIOVS = std::distance(iov1.begin(), iov1.end());

                std::cout << "nIOVS: " << nIOVS << "\n";

                int cnt = 0, cnt_iov = 0, one_dumped = 0;
                A res;
                time_t tb, te, tb_first = (iov1.begin() + 1)->since()>>32, te_last = (iov1.begin() + 1)->till()>>32;
                for (cond::IOVProxy::const_iterator ita = iov1.begin() + 1; ita != iov1.end(); ++ita, ++cnt) {
                        //if (cnt == 0 || cnt < 2) continue;
                        //if (cnt % prescale != 0) continue;
                        if (ita->since() < since || ita->till() > till) continue;
                        boost::shared_ptr<A> pa = session.getTypedObject<A>(ita->token());
                        if (niov > 0 && cnt_iov >= niov) break;
                        tb = (time_t)ita->since()>>32;
                        te = (time_t)ita->till()>>32;
                        printf("--> %lu %lu (%d/%d)\n", tb, te, cnt, nIOVS);
                        if (tb - tb_first > t_interval) {
                                printf("... writing tag with begin: %lu end: %lu (deltaT: %lu) - t_interval: %lu\n", tb_first, te_last, te_last - tb_first, t_interval);
                                if (!txt) dump_time_average(fdump, tb_first, te_last);
                                else      dump_txt(fdump);
                                reset_time_average();
                                tb_first = tb;
                                one_dumped = 1;
                        }
                        te_last = te;
                        fill_time_average(*pa);
                        ++cnt_iov;
                }
                if (!one_dumped) {
                        if (!txt) dump_time_average(fdump, tb_first, te_last);
                        else      dump_txt(fdump);
                }
                transaction.commit();
                fclose(fdump);
        }
        return 0;
}


int main( int argc, char** argv )
{
        cond::TriggerTag valida;
        return valida.run(argc,argv);
}
