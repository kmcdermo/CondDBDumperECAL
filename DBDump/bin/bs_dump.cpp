#include "CondCore/Utilities/interface/Utilities.h"

#include "CondCore/DBCommon/interface/DbConnection.h"
#include "CondCore/DBCommon/interface/DbScopedTransaction.h"
#include "CondCore/DBCommon/interface/DbTransaction.h"
#include "CondCore/DBCommon/interface/Exception.h"
#include "CondCore/MetaDataService/interface/MetaData.h"

#include "CondCore/DBCommon/interface/Time.h"
#include "CondFormats/Common/interface/TimeConversions.h"

#include "CondCore/IOVService/interface/IOVProxy.h"
#include "CondFormats/BeamSpotObjects/interface/BeamSpotObjects.h"
#include "CondFormats/DataRecord/interface/BeamSpotObjectsRcd.h"

#include <boost/program_options.hpp>
#include <iterator>
#include <iostream>

using namespace std;

namespace cond {
        class BeamSpot : public Utilities {
                public:
                        typedef BeamSpotObjects A;

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

                        BeamSpot();
                        ~BeamSpot();
                        int execute();
                        void dump(const A & o, int run, int lumi, FILE * fd);
        };

}

cond::BeamSpot::BeamSpot():Utilities("cmscond_list_iov")
{
        addConnectOption();
        addAuthenticationOptions();
        addOption<bool>("verbose","v","verbose");
        addOption<bool>("all","a","list all tags (default mode)");
        addOption<bool>("summary","s","stprint also the summary for each payload");
        addOption<int>("min_iov","r","begin iov (first since) (optional)");
        addOption<int>("max_iov","R","end iov (last till) (optional)");
        addOption<std::string>("tag","t","list info of the specified tag");
        addOption<std::string>("output","o","output file (default: ic_dump_<tagname>_<IOV_start>_<IOV_stop>.dat)");
}

cond::BeamSpot::~BeamSpot(){
}


void cond::BeamSpot::dump(const A & bs, int run, int lumi, FILE * fd)
{
        fprintf(fd, "%d %d  %f %f %f %f\n", run, lumi,
                bs.GetZ(), bs.GetZError(), bs.GetSigmaZ(), bs.GetSigmaZError()
               );
}

int cond::BeamSpot::execute()
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

                int min_iov = std::numeric_limits<int>::min();
                if( hasOptionValue("min_iov" )) min_iov = getOptionValue<int>("min_iov");
                int max_iov = std::numeric_limits<int>::max();
                if( hasOptionValue("max_iov" )) max_iov = getOptionValue<int>("max_iov");

                //bool verbose = hasOptionValue("verbose");

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

                //static const unsigned int nIOVS = std::distance(iov.begin(), iov.end());
                //std::cout << "nIOVS: " << nIOVS << "\n";

                typedef unsigned int LuminosityBlockNumber_t;
                typedef unsigned int RunNumber_t;

                static unsigned int const shift = 8 * sizeof(unsigned int);

                int cnt = 0;
                char filename[256];
                sprintf(filename, "dump_%s.dat", tag.c_str());
                FILE * fd = fopen(filename, "w");
                if (!fd) {
                        char err[256];
                        sprintf(err, "[cond::BeamSpot::dump] Impossible to open file `%s' for dumping:", filename);
                        perror(err);
                        exit(1);
                }
                fprintf(fd, "#run_since lumi_since bs.GetZ(), bs.GetZError(), bs.GetSigmaZ(), bs.GetSigmaZError()\n");
                for (cond::IOVProxy::const_iterator ita = iov.begin(); ita != iov.end(); ++ita, ++cnt) {
                        //std::cout << cnt << " " << ita->since() << " -> " << ita->till() << "\n";
                        int iov = (int)(ita->since() >> shift);
                        if (iov >= min_iov && iov <= max_iov) {
                                boost::shared_ptr<A> pa = session.getTypedObject<A>(ita->token());
                                //sprintf(filename, "dump_%s__since_%08ld_till_%08ld.dat", tag.c_str(), (long int)ita->since(), (long int)ita->till());
                                //sprintf(filename, "dump_%s.dat", tag.c_str());
                                //fprintf(stderr, "going to dump on `%s'\n", filename);
                                RunNumber_t run_since = static_cast<RunNumber_t>(ita->since() >> shift);
                                LuminosityBlockNumber_t lumi_since = static_cast<LuminosityBlockNumber_t>(std::numeric_limits<unsigned int>::max() & ita->since());
                                dump(*pa, run_since, lumi_since, fd);
                        }
                }
                if (fd) fclose(fd);
                transaction.commit();
        }
        return 0;
}


int main( int argc, char** argv )
{
        cond::BeamSpot dump;
        return dump.run(argc,argv);
}
