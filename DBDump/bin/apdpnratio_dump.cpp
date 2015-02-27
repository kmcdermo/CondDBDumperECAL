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

namespace cond {
        class LaserValidation : public Utilities {
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

                        LaserValidation();
                        ~LaserValidation();
                        int execute();
                        void dump_txt(char * filename, const A & obja, time_t tb, time_t te, bool flat);
                private:
                        std::vector<DetId> ecalDetIds_;
        };

}

cond::LaserValidation::LaserValidation():Utilities("cmscond_list_iov")
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
        addOption<bool>("flat", "f", "flat iov, i.e. p1 = p2 = p3");

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

cond::LaserValidation::~LaserValidation(){
}


void cond::LaserValidation::dump_txt(char * filename, const A & obja, time_t tb, time_t te, bool flat)
{
        FILE * fd = fopen(filename, "w");
        if (!fd) {
                char err[256];
                sprintf(err, "[cond::LaserValidation::dump] Impossible to open file `%s' for dumping:", filename);
                perror(err);
                exit(1);
        }
        AT ta;
        AMapCit ita, itb;
        fprintf(fd, "T %ld %ld", tb, te);
        for (size_t i = 0; i < 92; ++i) {
                ta = obja.getTimeMap()[i];
                fprintf(fd, " %d", ta.t2.unixTime());
        }
        fprintf(fd, "\n");
        for (size_t i = 0; i < ecalDetIds_.size(); ++i) {
                ita = obja.getLaserMap().find(ecalDetIds_[i]);
                if (!flat) fprintf(fd, "P %d %f %f %f\n", ecalDetIds_[i].rawId(), ita->p1, ita->p2, ita->p3);
                else       fprintf(fd, "P %d %f %f %f\n", ecalDetIds_[i].rawId(), ita->p2, ita->p2, ita->p2);
        }
        //std::vector<DetId> ids;
        //ids.resize(EBDetId::MAX_HASH + 1 + EEDetId::kSizeForDenseIndexing);
        //for (int hi = EBDetId::MIN_HASH; hi <= EBDetId::MAX_HASH; ++hi ) {
        //        EBDetId ebId = EBDetId::unhashIndex(hi);
        //        if (ebId != EBDetId()) {
        //                ids[hi] = ebId;
        //        }
        //}
        //for ( int hi = 0; hi < EEDetId::kSizeForDenseIndexing; ++hi ) {
        //        EEDetId eeId = EEDetId::unhashIndex(hi);
        //        if (eeId != EEDetId()) {
        //                int idx = EBDetId::MAX_HASH + 1 + hi;
        //                ids[idx] = eeId;
        //        }
        //}
        //assert(ids.size() == 75848);
        //assert(ids.size() == EBDetId::MAX_HASH + 1 + EEDetId::kSizeForDenseIndexing);
        //AMapCit ita, itb;
        //for (size_t i = 0; i < ids.size(); ++i) {
        //        DetId id(ids[i]);
        //        ita = obja.getLaserMap().find(ecalDetIds_[i]);
        //        if (ita == obja.getLaserMap().end()) {
        //                fprintf(stderr, "Cannot find IC for DetId %u", id.rawId());
        //        }
        //        if (id.subdetId() == EcalBarrel) {
        //                EBDetId eid(id);
        //                fprintf(fd, "%d %d %d %f %f %f  %u\n", eid.ieta(), eid.iphi(), 0,  
        //                        ita->p1, ita->p2, ita->p3,
        //                        id.rawId());
        //        } else if (id.subdetId() == EcalEndcap) {
        //                EEDetId eid(id);
        //                fprintf(fd, "%d %d %d %f %f %f  %u\n", eid.ix(), eid.iy(), eid.zside(),
        //                        ita->p1, ita->p2, ita->p3,
        //                        id.rawId());
        //        } else {
        //                fprintf(stderr, "[dump] invalid DetId: %d\n", id.rawId());
        //                exit(-1);
        //        }
        //}
}


int cond::LaserValidation::execute()
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

                FILE * fdump = NULL;
                if (hasOptionValue("dump")) {
                        fdump = fopen(getOptionValue<std::string>("dump").c_str(), "w");
                        assert(fdump);
                }

                bool verbose = hasOptionValue("verbose");
                bool flat = hasOptionValue("flat");

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

                int cnt = 0, cnt_iov = 0;
                char filename[256];
                A res;
                for (cond::IOVProxy::const_iterator ita = iov1.begin(); ita != iov1.end(); ++ita, ++cnt) {
                        //if (cnt == 0 || cnt < 2) continue;
                        if (cnt % prescale != 0) continue;
                        if (ita->since() < since || ita->till() > till) continue;
                        boost::shared_ptr<A> pa = session.getTypedObject<A>(ita->token());
                        if (niov > 0 && cnt_iov >= niov) break;
                        std::cout << cnt << " " << ita->since() << " -> " << ita->till() << "\n";
                        //if (fdump != NULL) dump_txt(fdump, *pa, (time_t)ita->since()>>32, (time_t)ita->till()>>32, flat);
                        //else fprintf(stderr, "sure you do not want to dump it? otherwise specify the `dump' option.\n See -h for help.\n");
                        sprintf(filename, "dump_%s__since_%08ld_till_%08ld.dat", tag1.c_str(), (long int)ita->since(), (long int)ita->till());
                        fprintf(stderr, "going to dump on `%s'\n", filename);
                        dump_txt(filename, *pa, (time_t)ita->since()>>32, (time_t)ita->till()>>32, flat);
                        ++cnt_iov;
                }
                transaction.commit();
        }
        return 0;
}


int main( int argc, char** argv )
{
        cond::LaserValidation valida;
        return valida.run(argc,argv);
}
