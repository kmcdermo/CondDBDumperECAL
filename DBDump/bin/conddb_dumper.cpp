#include "CondDBDumperECAL/DBDump/interface/CondDBDumper.h"

#include "CondFormats/BeamSpotObjects/interface/BeamSpotObjects.h"
#include "CondFormats/EcalObjects/interface/EcalADCToGeVConstant.h"
#include "CondFormats/EcalObjects/interface/EcalChannelStatus.h"
#include "CondFormats/EcalObjects/interface/EcalClusterLocalContCorrParameters.h"
#include "CondFormats/EcalObjects/interface/EcalGainRatios.h"
#include "CondFormats/EcalObjects/interface/EcalIntercalibConstants.h"
#include "CondFormats/EcalObjects/interface/EcalIntercalibConstantsMC.h"
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


void list_supported(std::vector<std::string> & supported)
{
        fprintf(stderr, "Currently supported objects:\n");
        std::sort(supported.begin(), supported.end());
        for (size_t i = 0; i < supported.size(); ++i) {
                fprintf(stderr, "  %s\n", supported[i].c_str());
        }
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
        if (obj == "none" && !help) usage(argv[0]);

        supported.push_back("EcalIntercalibConstants");
        if (!help && obj == "EcalIntercalibConstants") {
                cond::CondDBDumper<EcalIntercalibConstants> d(obj);
                d.run(argc, argv);
                return 0;
        }

        supported.push_back("EcalIntercalibConstantsMC");
        if (!help && obj == "EcalIntercalibConstantsMC") {
                cond::CondDBDumper<EcalIntercalibConstantsMC> d(obj);
                d.run(argc, argv);
                return 0;
        }

        supported.push_back("EcalADCToGeVConstant");
        if (!help && obj == "EcalADCToGeVConstant") {
                cond::CondDBDumper<EcalADCToGeVConstant> d(obj);
                d.run(argc, argv);
                return 0;
        }

        supported.push_back("EcalLaserAlphas");
        if (!help && obj == "EcalLaserAlphas") {
                cond::CondDBDumper<EcalLaserAlphas> d(obj);
                d.run(argc, argv);
                return 0;
        }

        supported.push_back("EcalChannelStatus");
        if (!help && obj == "EcalChannelStatus") {
                cond::CondDBDumper<EcalChannelStatus> d(obj);
                d.run(argc, argv);
                return 0;
        }

        supported.push_back("EcalPedestals");
        if (!help && obj == "EcalPedestals") {
                cond::CondDBDumper<EcalPedestals> d(obj);
                d.run(argc, argv);
                return 0;
        }

        supported.push_back("EcalTimeCalibConstants");
        if (!help && obj == "EcalTimeCalibConstants") {
                cond::CondDBDumper<EcalTimeCalibConstants> d(obj);
                d.run(argc, argv);
                return 0;
        }

        supported.push_back("EcalTimeOffsetConstant");
        if (!help && obj == "EcalTimeOffsetConstant") {
                cond::CondDBDumper<EcalTimeOffsetConstant> d(obj);
                d.run(argc, argv);
                return 0;
        }

        supported.push_back("EcalTPGLinearizationConst");
        if (!help && obj == "EcalTPGLinearizationConst") {
                cond::CondDBDumper<EcalTPGLinearizationConst> d(obj);
                d.run(argc, argv);
                return 0;
        }

        supported.push_back("EcalClusterLocalContCorrParameters");
        if (!help && obj == "EcalClusterLocalContCorrParameters") {
                cond::CondDBDumper<EcalClusterLocalContCorrParameters> d(obj);
                d.run(argc, argv);
                return 0;
        }

        supported.push_back("EcalGainRatios");
        if (!help && obj == "EcalGainRatios") {
                cond::CondDBDumper<EcalGainRatios> d(obj);
                d.run(argc, argv);
                return 0;
        }

        supported.push_back("ESEEIntercalibConstants");
        if (!help && obj == "ESEEIntercalibConstants") {
                cond::CondDBDumper<ESEEIntercalibConstants> d(obj);
                d.run(argc, argv);
                return 0;
        }

        supported.push_back("ESGain");
        if (!help && obj == "ESGain") {
                cond::CondDBDumper<ESGain> d(obj);
                d.run(argc, argv);
                return 0;
        }

        supported.push_back("ESIntercalibConstants");
        if (!help && obj == "ESIntercalibConstants") {
                cond::CondDBDumper<ESIntercalibConstants> d(obj);
                d.run(argc, argv);
                return 0;
        }

        supported.push_back("BeamSpotObjects");
        if (!help && obj == "BeamSpotObjects") {
                cond::CondDBDumper<BeamSpotObjects> d(obj);
                d.run(argc, argv);
                return 0;
        }

        supported.push_back("RunInfo");
        if (!help && obj == "RunInfo") {
                cond::CondDBDumper<RunInfo> d(obj);
                d.run(argc, argv);
                return 0;
        }

        if (help) {
                cond::CondDBDumper<bool> d("Help");
                d.run(argc, argv);
                list_supported(supported);
                return 0;
        }

        fprintf(stderr, "Dumper for object `%s' not implemented.\n"
                "Please check the spelling and, if correct, ask the experts for its implementation.\n", obj.c_str());
        list_supported(supported);
        return 2;
}
