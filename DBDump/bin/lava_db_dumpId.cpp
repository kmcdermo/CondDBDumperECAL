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

#include <signal.h>

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
                        void save() { printf("saving file %p\n", (void*)fd); if (fd) fclose(fd); }
                private:
                        FILE * fd;
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
        addOption<std::string>("tag","t","list info of the specified tag");
        addOption<std::string>("geom","g","geometry file (default: detid_geom.dat)");
        addOption<std::string>("output","o","output file (default: ecal_laser_dumped_ids.dat)");
        addOption<std::string>("id","i","DetId (raw Id) to be dumped, quoted and blank separated list of raw DetIds");
        addOption<int>("niov","n","number of IOV");
        addOption<int>("prescale","s","prescale factor");
}

cond::LaserValidation::~LaserValidation()
{
        save();
}


int cond::LaserValidation::execute()
{
        initializePluginManager();

        bool listAll = hasOptionValue("all");
        cond::DbSession session  = openDbSession("connect", true);
        cond::DbScopedTransaction transaction(session);
        transaction.start(true);

        //cond::DbConnection connection;
        //connection.configuration().setPoolAutomaticCleanUp( false );
        //connection.configure();
        //cond::DbSession session = connection.createSession();
        //session.open(getConnectValue(), true);

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
                std::string tag = getOptionValue<std::string>("tag");

                std::string geom = hasOptionValue("geom") ? getOptionValue<std::string>("geom") : "detid_geom.dat";
                std::string output = hasOptionValue("output") ? getOptionValue<std::string>("output") : "ecal_laser_dumped_ids.dat";

                if      (output == "stdout") fd = stdout;
                else if (output == "stderr") fd = stderr;
                else {
                        fd = fopen(output.c_str(), "w");
                }
                assert(fd != NULL);

                fprintf(fd, "# %s @ %s\n", tag.c_str(), getConnectValue().c_str());

                cond::MetaData metadata_svc(session);
                std::string token;
                cond::DbScopedTransaction transaction(session);
                transaction.start(true);
                transaction.commit();
                token = metadata_svc.getToken(tag);

                //std::string tokenb = metadata_svc.getToken(tagb);

                //transaction.commit();

                cond::Time_t since = std::numeric_limits<cond::Time_t>::min();
                if( hasOptionValue("beginTime" )) since = getOptionValue<cond::Time_t>("beginTime");
                cond::Time_t till = std::numeric_limits<cond::Time_t>::max();
                if( hasOptionValue("endTime" )) till = getOptionValue<cond::Time_t>("endTime");

                bool verbose = hasOptionValue("verbose");

                //cond::IOVProxy iov(session, getToken(session, tag));
                cond::IOVProxy iov(session, token);

                //since = std::max((cond::Time_t)2, cond::timeTypeSpecs[iov.timetype()].beginValue); // avoid first IOV
                //till  = std::min(till,  cond::timeTypeSpecs[iov.timetype()].endValue);

                std::cout << "since: " << since << "   till: " << till << "\n";

                iov.range(since, till);

                //std::string payloadContainer = iov.payloadContainerName();
                const std::set<std::string> payloadClasses = iov.payloadClasses();
                std::cout<<"Tag "<<tag;
                if (verbose) std::cout << "\nStamp: " << iov.iov().comment()
                        << "; time " <<  cond::time::to_boost(iov.iov().timestamp())
                                << "; revision " << iov.iov().revision();
                std::cout <<"\nTimeType " << cond::timeTypeSpecs[iov.timetype()].name
                        <<"\nPayloadClasses:\n";
                for (std::set<std::string>::const_iterator it = payloadClasses.begin(); it != payloadClasses.end(); ++it) {
                        std::cout << " --> " << *it << "\n";
                }
                std::cout
                        <<"since \t till \t payloadToken"<<std::endl;

                int niov = -1;
                if (hasOptionValue("niov")) niov = getOptionValue<int>("niov");

                std::vector<int> id;
                if (!hasOptionValue("id")) {
                        fprintf(stderr, "Fatal error: <id> option is mandatory, abort.\n");
                        assert(0);
                } else {
                        std::string ids = getOptionValue<std::string>("id");
                        const char * beg = ids.c_str();
                        const char * lim = beg + ids.size();
                        char * end;
                        int num = 1;
                        printf("Going to dump data for the following DetId:");
                        while (beg < lim && (num = strtol(beg, &end, 0)) != 0) {
                                printf(" %d", num);
                                id.push_back(num);
                                beg = end;
                        }
                        printf("\n");
                }

                int prescale = 1;
                if (hasOptionValue("prescale")) prescale = getOptionValue<int>("prescale");
                assert(prescale > 0);

                static const unsigned int nIOVS = std::distance(iov.begin(), iov.end());

                std::cout << "nIOVS: " << nIOVS << "\n";

                typedef unsigned int LuminosityBlockNumber_t;
                typedef unsigned int RunNumber_t;

                fprintf(fd, "#time     ");
                for (size_t i = 0; i < id.size(); ++i) {
                        fprintf(fd, " %d", id[i]);
                }
                fprintf(fd, "\n");

                int cnt = 0;
                for (cond::IOVProxy::const_iterator ita = iov.begin(); ita != iov.end() - 2; ++ita, ++cnt) {
                        if (cnt == 0 || cnt < 2) continue;
                        if (cnt % prescale != 0) continue;
                        if (ita->since() < since || ita->till() > till) continue;
                        std::cout << cnt << " " << ita->since() << " -> " << ita->till() << "\n";
                        boost::shared_ptr<A> pa = session.getTypedObject<A>(ita->token());
                        EcalLaserAPDPNRatios::EcalLaserAPDPNRatiosMap::const_iterator itAPDPN;
                        fprintf(fd, "%ld", (time_t)ita->since()>>32);
                        for (size_t i = 0; i < id.size(); ++i) {
                                itAPDPN = (*pa).getLaserMap().find(id[i]);
                                assert(itAPDPN != (*pa).getLaserMap().end());
                                fprintf(fd, " %f", (*itAPDPN).p2);
                        }
                        fprintf(fd, "\n");
                        if (niov > 0 && cnt >= niov) break;
                }
                transaction.commit();
        }
        return 0;
}

cond::LaserValidation * valida;

void save_file(int)
{
        valida->save();
        exit(1);
}

int main( int argc, char** argv )
{
        signal(SIGINT, save_file);
        valida = new cond::LaserValidation();
        valida->run(argc,argv);
        return 0;
}
