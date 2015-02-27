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

#include "../src/EcalLaserDumper.h"

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
                        void dump_txt(FILE * fd, const A & obja, time_t tb, time_t te);
                private:
                        float verify(const A & rcd1);
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
        addOption<std::string>("geom","g","geometry file (default: detid_geom.dat)");
        addOption<std::string>("output","o","output directory (default: ecal_laser_dumper)");
        addOption<int>("niov","n","number of IOV");
        addOption<int>("prescale","s","prescale factor");
        addOption<std::string>("dump", "d", "file for a txt dump");

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


void cond::LaserValidation::dump_txt(FILE * fd, const A & obja, time_t tb, time_t te)
{
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
                fprintf(fd, "P %d %f %f %f\n", ecalDetIds_[i].rawId(), ita->p1, ita->p2, ita->p3);
        }
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

                std::string geom = hasOptionValue("geom") ? getOptionValue<std::string>("geom") : "detid_geom.dat";
                std::string odir = hasOptionValue("output") ? getOptionValue<std::string>("output") : "ecal_laser_dumper";

                cond::MetaData metadata_svc(session);
                cond::DbScopedTransaction transaction(session);
                transaction.start(true);
                transaction.commit();
                std::string token1;
                token1 = metadata_svc.getToken(tag1);

                cond::Time_t since = std::numeric_limits<cond::Time_t>::min();
                if( hasOptionValue("beginTime" )) since = getOptionValue<cond::Time_t>("beginTime");
                cond::Time_t till = std::numeric_limits<cond::Time_t>::max();
                if( hasOptionValue("endTime" )) till = getOptionValue<cond::Time_t>("endTime");

                FILE * fdump = NULL;
                //if (hasOptionValue("dump")) {
                        fdump = fopen(getOptionValue<std::string>("dump").c_str(), "w");
                        assert(fdump);
                //}

                bool verbose = hasOptionValue("verbose");

                cond::IOVProxy iov1(session, token1);

                std::cout << "since: " << since << "   till: " << till << "\n";

                iov1.range(since, till);

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

                typedef unsigned int LuminosityBlockNumber_t;
                typedef unsigned int RunNumber_t;

                int cnt = 0;
                //for (cond::IOVProxy::const_iterator ita = iov1.begin(); ita != iov1.end() - 2; ++ita, ++cnt) {
                for (cond::IOVProxy::const_iterator ita = iov1.begin(); ita != iov1.end(); ++ita, ++cnt) {
                        //if (cnt == 0 || cnt < 2) continue;
                        if (cnt % prescale != 0) continue;
                        if (ita->since() < since || ita->till() > till) continue;
                        std::cout << cnt << " " << ita->since() << " -> " << ita->till() << " " << ita->token() << "\n";

                        boost::shared_ptr<A> pa = session.getTypedObject<A>(ita->token());
                        if (niov > 0 && cnt >= niov) break;
                        if (fdump) dump_txt(fdump, *pa, (time_t)ita->since()>>32, (time_t)ita->till()>>32);
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
