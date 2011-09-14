#include "CondCore/Utilities/interface/Utilities.h"

#include "CondCore/DBCommon/interface/DbScopedTransaction.h"
#include "CondCore/DBCommon/interface/Exception.h"
#include "CondCore/MetaDataService/interface/MetaData.h"

#include "CondCore/DBCommon/interface/Time.h"
#include "CondFormats/Common/interface/TimeConversions.h"

#include "CondCore/IOVService/interface/IOVProxy.h"
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
        class ListIOVUtilities : public Utilities {
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
                                return token;
                        }

                        ListIOVUtilities();
                        ~ListIOVUtilities();
                        int execute();
                private:
                        void merge(const A & rcd1, const A & rcd2, A & res);
                        void mergePoints(const AP & pa, const AP & pb, AP & res);
                        void mergeTimes(const AT & ta, const AT & tb, AT & res);
                        std::vector<DetId> ecalDetIds_;
        };

}

cond::ListIOVUtilities::ListIOVUtilities():Utilities("cmscond_list_iov")
{
        addConnectOption();
        addAuthenticationOptions();
        addOption<bool>("verbose","v","verbose");
        addOption<bool>("all","a","list all tags (default mode)");
        addOption<bool>("summary","s","stprint also the summary for each payload");
        addOption<cond::Time_t>("beginTime","b","begin time (first since) (optional)");
        addOption<cond::Time_t>("endTime","e","end time (last till) (optional)");
        addOption<cond::Time_t>("BeginTime","B","begin time (first since) (optional)");
        addOption<cond::Time_t>("EndTime","E","end time (last till) (optional)");
        addOption<bool>("doPlot","p","Produce some plots for the selected interval (optional)");
        addOption<std::string>("tag","t","list info of the specified tag");
        addOption<std::string>("Tag","T","list info of the specified tag (optional)");
        addOption<std::string>("geom","g","geometry file (default: detid_geom.dat)");
        addOption<std::string>("output","o","output file (default: ecallaserplotter.root)");
        addOption<std::string>("niov","n","number of IOV");

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

cond::ListIOVUtilities::~ListIOVUtilities(){
}



void cond::ListIOVUtilities::mergePoints(const AP & pa, const AP & pb, AP & res)
{
        res.p1 = pa.p1 / pb.p1;
        res.p2 = pa.p2 / pb.p2;
        res.p3 = pa.p3 / pb.p3;
        //printf("points merged: %f %f %f\n", res.p1, pa.p1, pb.p1);
        //printf("               %f %f %f\n", res.p2, pa.p2, pb.p2);
        //printf("               %f %f %f\n", res.p3, pa.p3, pb.p3);
}

void cond::ListIOVUtilities::mergeTimes(const AT & ta, const AT & tb, AT & res)
{
        // FIXME: add conditions to check pa.t_i with pb.t_i
        res.t1 = ta.t1;
        res.t2 = ta.t2;
        res.t3 = ta.t3;
}

void cond::ListIOVUtilities::merge(const A & obja, const A & objb, A & res)
{
        AP p;
        AT ta, tb, ts;
        AMapCit ita, itb;
        for (size_t i = 0; i < ecalDetIds_.size(); ++i) {
                ita = obja.getLaserMap().find(ecalDetIds_[i]);
                itb = objb.getLaserMap().find(ecalDetIds_[i]);
                mergePoints(*ita, *itb, p);
                res.setValue(ecalDetIds_[i], p);
                //if (i > 9) break;
        }
        for (size_t i = 0; i < 92; ++i) {
                ta = obja.getTimeMap()[i];
                tb = objb.getTimeMap()[i];
                mergeTimes(ta, tb, ts);
                res.setTime(i, ts);
        }
}



int cond::ListIOVUtilities::execute()
{
        initializePluginManager();

        bool listAll = hasOptionValue("all");
        cond::DbSession session  = openDbSession("connect", true);
        cond::DbSession sessionb = openDbSession("connect", true);
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

                std::string tagb = hasOptionValue("Tag") ? getOptionValue<std::string>("Tag") : "";
                std::string geom = hasOptionValue("geom") ? getOptionValue<std::string>("geom") : "detid_geom.dat";
                std::string output = hasOptionValue("output") ? getOptionValue<std::string>("output") : "ecallaserplotter.root";
                //cond::MetaData metadata_svc(session);
                //std::string token;
                //cond::DbScopedTransaction transaction(session);
                //transaction.start(true);
                //token = metadata_svc.getToken(tag);

                //std::string tokenb = metadata_svc.getToken(tagb);

                //transaction.commit();

                cond::Time_t since = std::numeric_limits<cond::Time_t>::min();
                if( hasOptionValue("beginTime" )) since = getOptionValue<cond::Time_t>("beginTime");
                cond::Time_t till = std::numeric_limits<cond::Time_t>::max();
                if( hasOptionValue("endTime" )) till = getOptionValue<cond::Time_t>("endTime");

                bool verbose = hasOptionValue("verbose");
                bool details = hasOptionValue("summary");

                cond::IOVProxy iov(session, getToken(session, tag), false, true);
                cond::IOVProxy iovb(sessionb, getToken(sessionb, tagb), false, true);

                //cond::IOVProxy iov( session, token, false, true);
                //cond::IOVProxy iovb( session, tokenb, false, true);

                since = std::max((cond::Time_t)2, cond::timeTypeSpecs[iov.timetype()].beginValue); // avoid first IOV
                since = std::max(since, cond::timeTypeSpecs[iov.timetype()].beginValue);
                if (hasOptionValue("Tag")) since = std::max(cond::timeTypeSpecs[iov.timetype()].beginValue, cond::timeTypeSpecs[iovb.timetype()].beginValue);
                till  = std::min(till,  cond::timeTypeSpecs[iov.timetype()].endValue);
                if (hasOptionValue("Tag")) till = std::min(cond::timeTypeSpecs[iov.timetype()].endValue, cond::timeTypeSpecs[iovb.timetype()].endValue);

                std::cout << "since: " << since << "   till: " << till << "\n";

                iov.setRange(since,till);
                if (hasOptionValue("Tag")) iovb.setRange(since, till);

                std::string payloadContainer = iov.payloadContainerName();
                std::cout<<"Tag "<<tag;
                if (verbose) std::cout << "\nStamp: " << iov.iov().comment()
                        << "; time " <<  cond::time::to_boost(iov.iov().timestamp())
                                << "; revision " << iov.iov().revision();
                std::cout <<"\nTimeType " << cond::timeTypeSpecs[iov.timetype()].name
                        <<"\nPayloadContainerName "<<payloadContainer<<"\n"
                        <<"since \t till \t payloadToken"<<std::endl;

                int niov = -1;
                //if (hasOptionValue("niov")) niov = getOptionValue<int>("niov");

                static const unsigned int nIOVS = std::distance(iov.begin(), iov.end());
                static const unsigned int nIOVS2 = std::distance(iovb.begin(), iovb.end());

                std::cout << "nIOVS: " << nIOVS << "  -  nIOVS2: " << nIOVS2 << "\n";

                typedef unsigned int LuminosityBlockNumber_t;
                typedef unsigned int RunNumber_t;
                static unsigned int const shift = 8 * sizeof(unsigned int);

                RunNumber_t theRun = 0;

                int cnt = 0;
                A res;
                EcalLaserPlotter lp(geom.c_str());
                for (cond::IOVProxy::const_iterator ita = iov.begin(); ita != iov.end() - 2; ++ita, ++cnt) {
                        if (cnt == 0) continue;
                        boost::shared_ptr<A> pa = session.getTypedObject<A>(ita->wrapperToken());
                        boost::shared_ptr<A> pb = session.getTypedObject<A>(iovb.find(ita->since())->wrapperToken());
                        cond::IOVProxy::const_iterator it = iovb.find(ita->since());
                        std::cout << cnt << " " << ita->since() << " -> " << ita->till() << "  --  " << it->since() << " -> " << it->till() << "\n";
                        merge(*pa, *pb, res);
                        lp.fill(res, (time_t)ita->since()>>32);
                        if (niov > 0 && cnt >= niov) break;
                }
                lp.save(output.c_str());
        }
        return 0;
}


int main( int argc, char** argv )
{
        cond::ListIOVUtilities utilities;
        return utilities.run(argc,argv);
}
