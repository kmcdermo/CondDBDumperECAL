// https://cms-conddb.cern.ch/cmsDbBrowser/list/Prod/gts/76X_dataRun2_16Dec2015_v0
// GT for 2015 data rereco 76X
// look up Tag matching Record from supported objects: https://github.com/kmcdermo/CondDBDumperECAL
// conddb_dumper -O EcalPedestals -c frontier://FrontierProd/CMS_CONDITIONS -t EcalPedestals_express -b 271000

// ADC conversion: EcalADCToGeVConstant
// EcalADCToGeVConstant_V1_express

#include <vector>
#include "TH2F.h"
#include "TString.h"

#include <map>

struct eid
{
public:
  eid() {}
  int i1_; // iphi (EB) or ix (EE)
  int i2_; // ieta (EB) or iy (EE)
  TString name_;
};

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

void convert()
{
  // input runs
  std::ifstream adcruns;
  adcruns.open("ecalpeds/adcruns.txt",std::ios::in);
  int t_arc_r1, t_adc_r2; // t is for temp
  std::vector<int> adc_r1s, adc_r2s;
  while (adcruns >> t_adc_r1 >> t_adc_r2){
    adc_r1s.push_back(t_adc_r1);
    adc_r2s.push_back(t_adc_r2);
  }
  adcruns.close();

  // input adc to gev conversion factors
  std::vector<adcconv> adcconvs;
  for (int i = 0; i < adc_r1s.size(); i++){
    std::ifstream inputadcs;
    inputadcs.open(Form("ecalpeds/dump_EcalADCToGeVConstant__since_00%i_till_since_00%i.dat",adc_r1s[i],adc_r2s[i]),std::ios::in);
    TString t_st_eb, t_st_ee;
    float t_adc2gev_eb, t_adc2gev_ee;
    while (inputadcs >>  >> rne >> ebadc2gev >> eeadc2gev) {
      adcconv adctogev;
      adctogev.runb_   = rnb;       adctogev.rune_   = rne;
      adctogev.ebconv_ = ebadc2gev; adctogev.eeconv_ = eeadc2gev;
      adcconvs.push_back(adctogev);
    }
    inputadcs.close();
  }

  // input maps for detids
  std::ifstream inputids;
  inputids.open("detidsietaiphi.txt",std::ios::in);
  std::map<uint32_t,eid> eids;
  uint32_t ID;
  int i1, i2;
  TString name;
  while (inputids >> ID >> i1 >> i2 >> name){
    eids[ID]       = eid();
    eids[ID].i1_   = i1;
    eids[ID].i2_   = i2;
    eids[ID].name_ = name;
  }
  inputids.close();

  for (int i = 0; i < r1s.size(); i++){
    int run1 = r1s[i];
    int run2 = r2s[i];

    // get adc to gev conversion constants for the run range
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

    TString name = Form("ecalpeds/dump_EcalPedestals__since_00%i_till_00%i.dat",run1,run2);
    std::ifstream input;
    input.open(name.Data(),std::ios::in);
    float x, y, z, m1, r1, m2, r2, m3, r3;
    uint32_t id;
    while (input >> x >> y >> z >> m1 >> r1 >> m2 >> r2 >> m3 >> r3 >> id){
      if      (eids[id].name_.Contains("EB",TString::kExact)) {
	r1 *= ebconv; r2 *= ebconv; r3 *= ebconv;
	histseb[i][0]->Fill(r1);
	histseb[i][1]->Fill(r2);
	histseb[i][2]->Fill(r3);
      }
      else if (eids[id].name_.Contains("EE+",TString::kExact) || eids[id].name_.Contains("EE-",TString::kExact)) {
	r1 *= eeconv; r2 *= eeconv; r3 *= eeconv;
	histsee[i][0]->Fill(r1);
	histsee[i][1]->Fill(r2);
	histsee[i][2]->Fill(r3);
        if      (eids[id].name_.Contains("EE+",TString::kExact)) {
	  histseep[i][0]->Fill(r1);
	  histseep[i][1]->Fill(r2);
	  histseep[i][2]->Fill(r3);
	}
	else if (eids[id].name_.Contains("EE-",TString::kExact)) {
	  histseem[i][0]->Fill(r1);
	  histseem[i][1]->Fill(r2);
	  histseem[i][2]->Fill(r3);
	}
      }
    }
    input.close();
    
  }
}
