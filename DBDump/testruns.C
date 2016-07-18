#include <vector>
#include "TH2F.h"
#include "TString.h"

#include <map>

struct adcconv
{
public:
  adcconv() {}
  //  adcconv(int runb, int rune, float ebconv, float eeconv) : rune_(rune), runb_(rub), ebconv_(ebconv), eeconv_(eeconv) {}
  int runb_; // starting run for ADC to GeV
  int rune_; // ending   run for ADC to GeV
  float ebconv_; // EB ADC to GeV conversion factor
  float eeconv_; // EE ADC to GeV conversion factor
};

void testruns() 
{
  // input runs
  std::ifstream inputruns;
  inputruns.open("runs.txt",std::ios::in);
  int rn1, rn2;
  std::vector<int> r1s, r2s;
  while (inputruns >> rn1 >> rn2){
    r1s.push_back(rn1);
    r2s.push_back(rn2);
  }
  inputruns.close();

  std::ifstream inputadcs;
  inputadcs.open("ecalpeds/dump_EcalADCToGeVConstant__since_00000001_jtill_since_00262466.dat",std::ios::in);
  int rnb, rne;
  float ebadc2gev, eeadc2gev;
  std::vector<adcconv> adcconvs;
  while (inputadcs >> rnb >> rne >> ebadc2gev >> eeadc2gev) {
    adcconv adctogev;
    adctogev.runb_   = rnb;       adctogev.rune_   = rne;
    adctogev.ebconv_ = ebadc2gev; adctogev.eeconv_ = eeadc2gev;
    adcconvs.push_back(adctogev);
  }
  inputadcs.close();
  
  for (int i = 0; i < r1s.size(); i++){
    int run1 = r1s[i];
    int run2 = r2s[i];

    float ebconv = 0;
    float eeconv = 0;
    bool found = false;
    for (int r = 0; r < adcconvs.size(); r++){
      int runb = adcconvs[r].runb_;
      int rune = adcconvs[r].rune_;

      if (run1 <= runb && run2 <= rune){
	ebconv = adcconvs[r].ebconv_;
	eeconv = adcconvs[r].eeconv_;
	found = true;
      }
      if (found) break;
    }
  }


}
