#include "CondCore/Utilities/interface/Utilities.h"
#include "CondCore/CondDB/interface/ConnectionPool.h"
#include "CondCore/CondDB/interface/IOVProxy.h"

#include "CondFormats/Common/interface/TimeConversions.h"
#include "CondFormats/EcalObjects/interface/EcalLaserAPDPNRatios.h"
#include "CondFormats/DataRecord/interface/EcalLaserAPDPNRatiosRcd.h"
#include "CondFormats/EcalObjects/interface/EcalLaserAPDPNRatiosRef.h"
#include "CondFormats/DataRecord/interface/EcalLaserAPDPNRatiosRefRcd.h"

#include "../src/EcalLaserPlotter.h"

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

                        LaserValidation();
                        ~LaserValidation();
                        int execute();
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
        addOption<std::string>("output","o","output file (default: ecallaserplotter.root)");
        addOption<int>("niov","n","number of IOV");
        addOption<int>("prescale","s","prescale factor");
}

cond::LaserValidation::~LaserValidation(){
}


int cond::LaserValidation::execute()
{
        std::string connect = getOptionValue<std::string>("connect" );
        cond::persistency::ConnectionPool connPool;
        if( hasOptionValue("authPath") ){
                connPool.setAuthenticationPath( getOptionValue<std::string>( "authPath") ); 
        }
        connPool.configure();
        cond::persistency::Session session = connPool.createSession( connect );

        std::string tag = getOptionValue<std::string>("tag");

        std::string geom = hasOptionValue("geom") ? getOptionValue<std::string>("geom") : "detid_geom.dat";
        std::string output = hasOptionValue("output") ? getOptionValue<std::string>("output") : "ecallaserplotter.root";

        cond::Time_t since = std::numeric_limits<cond::Time_t>::min();
        if( hasOptionValue("beginTime" )) since = getOptionValue<cond::Time_t>("beginTime");
        cond::Time_t till = std::numeric_limits<cond::Time_t>::max();
        if( hasOptionValue("endTime" )) till = getOptionValue<cond::Time_t>("endTime");

        //bool verbose = hasOptionValue("verbose");

        session.transaction().start( true );
        cond::persistency::IOVProxy iov = session.readIov(tag, true);

        //since = std::max((cond::Time_t)2, cond::timeTypeSpecs[iov.timetype()].beginValue); // avoid first IOV
        //till  = std::min(till,  cond::timeTypeSpecs[iov.timetype()].endValue);

        std::cout << "since: " << since << "   till: " << till << "\n";

        //FIXME//iov.range(since, till);

        //std::string payloadContainer = iov.payloadContainerName();
        //FIXME//std::set<std::string> payloadClasses = iov.payloadObjectType();
        std::cout<<"Tag "<<tag <<"\n";
        //FIXME//if (verbose) std::cout << "\nStamp: " << iov.iov().comment()
        //FIXME//        << "; time " <<  cond::time::to_boost(iov.iov().timestamp())
        //FIXME//                << "; revision " << iov.iov().revision();
        //FIXME//std::cout <<"\nTimeType " << cond::timeTypeSpecs[iov.timetype()].name
        //FIXME//        <<"\nPayloadClasses:\n";
        //FIXME//for (std::set<std::string>::const_iterator it = payloadClasses.begin(); it != payloadClasses.end(); ++it) {
        //FIXME//        std::cout << " --> " << *it << "\n";
        //FIXME//}
        //FIXME//std::cout
        //FIXME//        <<"since \t till \t payloadToken"<<std::endl;

        int niov = -1;
        if (hasOptionValue("niov")) niov = getOptionValue<int>("niov");

        int prescale = 1;
        if (hasOptionValue("prescale")) prescale = getOptionValue<int>("prescale");
        assert(prescale > 0);

        static const unsigned int nIOVS = std::distance(iov.begin(), iov.end());

        std::cout << "nIOVS: " << nIOVS << "\n";

        typedef unsigned int LuminosityBlockNumber_t;
        typedef unsigned int RunNumber_t;

        int cnt = 0;
        EcalLaserPlotter lp(geom.c_str());
        //for (cond::IOVProxy::const_iterator ita = iov.begin(); ita != iov.end() - 2; ++ita, ++cnt) {
        for (auto i : iov) {
                ++cnt;
                if (cnt == 0 || cnt < 2) continue;
                if (cnt % prescale != 0) continue;
                std::cout << cnt << " " << i.since << " -> " << i.till << "\n";
                boost::shared_ptr<A> pa = session.fetchPayload<A>(i.payloadId);
                lp.fill(*pa, (time_t)i.since>>32);
                if (niov > 0 && cnt >= niov) break;
        }
        lp.save(output.c_str());
        session.transaction().commit();
        return 0;
}


int main( int argc, char** argv )
{
        cond::LaserValidation valida;
        return valida.run(argc,argv);
}
