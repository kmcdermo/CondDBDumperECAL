#include "CondCore/Utilities/interface/Utilities.h"

#include "CondCore/DBCommon/interface/DbConnection.h"
#include "CondCore/DBCommon/interface/DbScopedTransaction.h"
#include "CondCore/DBCommon/interface/DbTransaction.h"
#include "CondCore/DBCommon/interface/Exception.h"
#include "CondCore/MetaDataService/interface/MetaData.h"

#include "CondCore/DBCommon/interface/Time.h"
#include "CondFormats/Common/interface/TimeConversions.h"

#include "CondCore/IOVService/interface/IOVProxy.h"
#include "CondFormats/ESObjects/interface/ESEEIntercalibConstants.h"
#include "CondFormats/DataRecord/interface/ESEEIntercalibConstantsRcd.h"

#include "DataFormats/EcalDetId/interface/ESDetId.h"

#include <boost/program_options.hpp>
#include <iterator>
#include <iostream>

using namespace std;

namespace cond {
        class ESEEICDump : public Utilities {
                public:
                        typedef ESEEIntercalibConstants A;

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

                        ESEEICDump();
                        ~ESEEICDump();
                        int execute();
                        void dump(const A & o, const char * filename);
        };

}

cond::ESEEICDump::ESEEICDump():Utilities("cmscond_list_iov")
{
        addConnectOption();
        addAuthenticationOptions();
        addOption<bool>("verbose","v","verbose");
        addOption<bool>("all","a","list all tags (default mode)");
        addOption<bool>("summary","s","stprint also the summary for each payload");
        addOption<int>("min_run","r","begin run (first since) (optional)");
        addOption<int>("max_run","R","end run (last till) (optional)");
        addOption<std::string>("tag","t","list info of the specified tag");
        addOption<std::string>("output","o","output file (default: ic_dump_<tagname>_<IOV_start>_<IOV_stop>.dat)");
}

cond::ESEEICDump::~ESEEICDump(){
}


void cond::ESEEICDump::dump(const A & o, const char * filename)
{
        FILE * fd = fopen(filename, "w");
        if (!fd) {
                char err[256];
                sprintf(err, "[cond::ESEEICDump::dump] Impossible to open file `%s' for dumping:", filename);
                perror(err);
                exit(1);
        }
        fprintf(fd,
                "GammaLow0:  %f   AlphaLow0:  %f\n"
                "GammaLow1:  %f   AlphaLow1:  %f\n"
                "GammaLow2:  %f   AlphaLow2:  %f\n"
                "GammaLow3:  %f   AlphaLow3:  %f\n"
                "GammaHigh0: %f   AlphaHigh0: %f\n"
                "GammaHigh1: %f   AlphaHigh1: %f\n"
                "GammaHigh2: %f   AlphaHigh2: %f\n"
                "GammaHigh3: %f   AlphaHigh3: %f\n",
                o.getGammaLow0(),  o.getAlphaLow0(),
                o.getGammaLow1(),  o.getAlphaLow1(),
                o.getGammaLow2(),  o.getAlphaLow2(),
                o.getGammaLow3(),  o.getAlphaLow3(),
                o.getGammaHigh0(), o.getAlphaHigh0(),
                o.getGammaHigh1(), o.getAlphaHigh1(),
                o.getGammaHigh2(), o.getAlphaHigh2(),
                o.getGammaHigh3(), o.getAlphaHigh3()
               );
        fclose(fd);
}

int cond::ESEEICDump::execute()
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
                std::string tag = getOptionValue<std::string>("tag");

                std::string output = hasOptionValue("output") ? getOptionValue<std::string>("output") : "ecallaserplotter.root";

                cond::MetaData metadata_svc(session);
                std::string token;
                cond::DbScopedTransaction transaction(session);
                transaction.start(true);
                transaction.commit();
                token = metadata_svc.getToken(tag);

                //std::string tokenb = metadata_svc.getToken(tagb);

                //transaction.commit();

                int min_run = std::numeric_limits<int>::min();
                if( hasOptionValue("min_run" )) min_run = getOptionValue<int>("min_run");
                int max_run = std::numeric_limits<int>::max();
                if( hasOptionValue("max_run" )) max_run = getOptionValue<int>("max_run");

                bool verbose = hasOptionValue("verbose");

                //cond::IOVProxy iov(session, getToken(session, tag));
                cond::IOVProxy iov(session, token);

                //since = std::max((cond::Time_t)2, cond::timeTypeSpecs[iov.timetype()].beginValue); // avoid first IOV
                //till  = std::min(till,  cond::timeTypeSpecs[iov.timetype()].endValue);

                //std::cout << "since: " << since << "   till: " << till << "\n";

                //iov.range(since,till);

                //std::string payloadContainer = iov.payloadContainerName();
                //const std::set<std::string> payloadClasses = iov.payloadClasses();
                //std::cout<<"Tag "<<tag;
                //if (verbose) std::cout << "\nStamp: " << iov.iov().comment()
                //        << "; time " <<  cond::time::to_boost(iov.iov().timestamp())
                //                << "; revision " << iov.iov().revision();
                //std::cout <<"\nTimeType " << cond::timeTypeSpecs[iov.timetype()].name
                //        <<"\nPayloadClasses:\n";
                //for (std::set<std::string>::const_iterator it = payloadClasses.begin(); it != payloadClasses.end(); ++it) {
                //        std::cout << " --> " << *it << "\n";
                //}
                //std::cout
                //        <<"since \t till \t payloadToken"<<std::endl;

                static const unsigned int nIOVS = std::distance(iov.begin(), iov.end());

                //std::cout << "nIOVS: " << nIOVS << "\n";

                typedef unsigned int LuminosityBlockNumber_t;
                typedef unsigned int RunNumber_t;

                int cnt = 0;
                char filename[256];
                for (cond::IOVProxy::const_iterator ita = iov.begin(); ita != iov.end(); ++ita, ++cnt) {
                        //std::cout << cnt << " " << ita->since() << " -> " << ita->till() << "\n";
                        int run = (int)ita->since();
                        if (run >= min_run && run <= max_run) {
                                boost::shared_ptr<A> pa = session.getTypedObject<A>(ita->token());
                                sprintf(filename, "dump_%s__since_%08ld_till_%08ld.dat", tag.c_str(), (long int)ita->since(), (long int)ita->till());
                                //sprintf(filename, "dump_%s.dat", tag.c_str());
                                fprintf(stderr, "going to dump on `%s'\n", filename);
                                dump(*pa, filename);
                        }
                }
                transaction.commit();
        }
        return 0;
}


int main( int argc, char** argv )
{
        cond::ESEEICDump dump;
        return dump.run(argc,argv);
}
