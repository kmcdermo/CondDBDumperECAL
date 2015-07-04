#include "usercode/DBDump/interface/CondDBDumper.h"

#include "CondFormats/BeamSpotObjects/interface/BeamSpotObjects.h"
#include "CondFormats/EcalObjects/interface/EcalADCToGeVConstant.h"
#include "CondFormats/EcalObjects/interface/EcalChannelStatus.h"
#include "CondFormats/EcalObjects/interface/EcalClusterLocalContCorrParameters.h"
#include "CondFormats/EcalObjects/interface/EcalIntercalibConstants.h"
#include "CondFormats/EcalObjects/interface/EcalLaserAlphas.h"
#include "CondFormats/EcalObjects/interface/EcalPedestals.h"
#include "CondFormats/EcalObjects/interface/EcalTimeCalibConstants.h"
#include "CondFormats/EcalObjects/interface/EcalTPGLinearizationConst.h"
#include "CondFormats/ESObjects/interface/ESEEIntercalibConstants.h"
#include "CondFormats/ESObjects/interface/ESGain.h"
#include "CondFormats/ESObjects/interface/ESIntercalibConstants.h"
#include "CondFormats/RunInfo/interface/RunInfo.h"


int usage(char * s)
{
        fprintf(stderr, "Usage: %s -O <object> [conddb options]\nFull help available with the -h/--help options.\n", s);
        exit(1);
}

int main(int argc, char** argv)
{
        if (argc < 2) {
                return usage(argv[0]);
        }

        std::string obj = "none";
        bool help = false;
        std::vector<std::string> supported;
        for (int i = 1; i < argc; ++i) {
                if (strcmp(argv[i], "-O") == 0 || strcmp(argv[i], "--object") == 0) {
                        obj = argv[i + 1];
                } else if (strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0) {
                        help = true;
                }
        }
        if (help) {
                cond::CondDBDumper<bool> d("Help");
                d.run(argc, argv);
                return 0;
        }
        if (obj == "none") usage(argv[0]);

        supported.push_back("EcalIntercalibConstants");
        if (obj == "EcalIntercalibConstants") {
                cond::CondDBDumper<EcalIntercalibConstants> d(obj);
                d.run(argc, argv);
                return 0;
        }

        supported.push_back("EcalADCToGeVConstant");
        if (obj == "EcalADCToGeVConstant") {
                cond::CondDBDumper<EcalADCToGeVConstant> d(obj);
                d.run(argc, argv);
                return 0;
        }

        supported.push_back("EcalLaserAlphas");
        if (obj == "EcalLaserAlphas") {
                cond::CondDBDumper<EcalLaserAlphas> d(obj);
                d.run(argc, argv);
                return 0;
        }

        supported.push_back("EcalChannelStatus");
        if (obj == "EcalChannelStatus") {
                cond::CondDBDumper<EcalChannelStatus> d(obj);
                d.run(argc, argv);
                return 0;
        }

        supported.push_back("EcalPedestals");
        if (obj == "EcalPedestals") {
                cond::CondDBDumper<EcalPedestals> d(obj);
                d.run(argc, argv);
                return 0;
        }

        supported.push_back("EcalTimeCalibConstants");
        if (obj == "EcalTimeCalibConstants") {
                cond::CondDBDumper<EcalTimeCalibConstants> d(obj);
                d.run(argc, argv);
                return 0;
        }

        supported.push_back("EcalTimeOffsetConstant");
        if (obj == "EcalTimeOffsetConstant") {
                cond::CondDBDumper<EcalTimeOffsetConstant> d(obj);
                d.run(argc, argv);
                return 0;
        }

        supported.push_back("EcalTPGLinearizationConst");
        if (obj == "EcalTPGLinearizationConst") {
                cond::CondDBDumper<EcalTPGLinearizationConst> d(obj);
                d.run(argc, argv);
                return 0;
        }

        supported.push_back("EcalClusterLocalContCorrParameters");
        if (obj == "EcalClusterLocalContCorrParameters") {
                cond::CondDBDumper<EcalClusterLocalContCorrParameters> d(obj);
                d.run(argc, argv);
                return 0;
        }

        supported.push_back("ESEEIntercalibConstants");
        if (obj == "ESEEIntercalibConstants") {
                cond::CondDBDumper<ESEEIntercalibConstants> d(obj);
                d.run(argc, argv);
                return 0;
        }

        supported.push_back("ESGain");
        if (obj == "ESGain") {
                cond::CondDBDumper<ESGain> d(obj);
                d.run(argc, argv);
                return 0;
        }

        supported.push_back("ESIntercalibConstants");
        if (obj == "ESIntercalibConstants") {
                cond::CondDBDumper<ESIntercalibConstants> d(obj);
                d.run(argc, argv);
                return 0;
        }

        supported.push_back("BeamSpotObjects");
        if (obj == "BeamSpotObjects") {
                cond::CondDBDumper<BeamSpotObjects> d(obj);
                d.run(argc, argv);
                return 0;
        }

        supported.push_back("RunInfo");
        if (obj == "RunInfo") {
                cond::CondDBDumper<RunInfo> d(obj);
                d.run(argc, argv);
                return 0;
        }

        fprintf(stderr, "Dumper for object `%s' not implemented.\n"
                "Please check the spelling and, if correct, ask the experts for its implementation.\n"
                "Currently supported objects:\n"
                , obj.c_str());
        std::sort(supported.begin(), supported.end());
        for (size_t i = 0; i < supported.size(); ++i) {
                fprintf(stderr, " %s\n", supported[i].c_str());
        }
        return 2;
}
