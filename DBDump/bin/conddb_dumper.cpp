#include "CondCore/Utilities/interface/Utilities.h"
#include "CondCore/CondDB/interface/ConnectionPool.h"
#include "CondCore/CondDB/interface/IOVProxy.h"

#include <boost/program_options.hpp>
#include <iterator>
#include <iostream>

using namespace std;

namespace cond {
        class CondDBDumper : public cond::Utilities {
                public:
                        CondDBDumper();
                        ~CondDBDumper();
                        int execute();

                        virtual void dump(FILE * fd) {};

                private:
                        void print(int cnt, const cond::Iov_t & iov);
        };

}

cond::CondDBDumper::CondDBDumper():Utilities("conddb_dumper")
{
        addConnectOption();
        addAuthenticationOptions();
        addOption<bool>("verbose","v","verbose");
        addOption<bool>("all","a","list all tags (default mode)");
        addOption<cond::Time_t>("beginTime","b","begin time (first since) (optional)");
        addOption<cond::Time_t>("endTime","e","end time (last till) (optional)");
        addOption<std::string>("tag","t","list info of the specified tag");
        addOption<std::string>("geom","g","geometry file (default: detid_geom.dat)");
        addOption<std::string>("output","o","output file (default: ecallaserplotter.root)");
        addOption<int>("niov","n","number of IOV");
        addOption<int>("prescale","s","prescale factor");
}

cond::CondDBDumper::~CondDBDumper()
{
}


void cond::CondDBDumper::print(int cnt, const cond::Iov_t & iov)
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


int cond::CondDBDumper::execute()
{
        std::string connect = getOptionValue<std::string>("connect" );
        cond::persistency::ConnectionPool connPool;
        if( hasOptionValue("authPath") ){
                connPool.setAuthenticationPath( getOptionValue<std::string>( "authPath") ); 
        }
        connPool.configure();
        cond::persistency::Session session = connPool.createSession( connect );

        std::string tag = getOptionValue<std::string>("tag");

        printf("%s %s\n", connect.c_str(), tag.c_str());

        session.transaction().start( true );
        cond::persistency::IOVProxy iov = session.readIov(tag, true);
        session.transaction().commit();

        int cnt = 0;
        for (auto i : iov) {
                print(cnt++, i);
        }
        return 0;
}


int main( int argc, char** argv )
{
        cond::CondDBDumper d;
        return d.run(argc,argv);
}
