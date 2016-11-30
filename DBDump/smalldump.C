#include <fstream>
#include <vector>
#include "TH2F.h"
#include "TString.h"
#include <iostream>

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

void getADCs(std::vector<adcconv>& adcconvs);
void getDetIDs(std::map<uint32_t,eid>& eids);
void getPedRuns(std::vector<int>& ped_r1s, std::vector<int>& ped_r2s);

void smalldump()
{
  std::vector<adcconv> adcconvs;
  getADCs(adcconvs);

  std::vector<int> ped_r1s, ped_r2s;
  getPedRuns(ped_r1s,ped_r2s);

  for (int i = 0; i < ped_r1s.size(); i++){
    int run1 = ped_r1s[i];
    int run2 = ped_r2s[i];
    TString name = Form("ecalpeds/dump_EcalPedestals__since_00%i_till_00%i.dat",run1,run2);
    std::ifstream input;
    input.open(name.Data(),std::ios::in);
    float x, y, z, m1, r1, m2, r2, m3, r3;
    uint32_t id;

    std::ofstream output;
    output.open(Form("ecalpeds/pednoise_%i-%i.txt",run1,run2),std::ios_base::trunc);

    while (input >> x >> y >> z >> m1 >> r1 >> m2 >> r2 >> m3 >> r3 >> id)
    {
      output << id << " " << r1 << std::endl;
    }
    input.close();
    output.close();
  }
}

void getADCs(std::vector<adcconv>& adcconvs)
{
  // input runs
  std::ifstream adcruns;
  adcruns.open("ecalpeds/adcruns.txt",std::ios::in);
  int t_adc_r1, t_adc_r2; // t is for temp
  std::vector<int> adc_r1s, adc_r2s;
  while (adcruns >> t_adc_r1 >> t_adc_r2){
    adc_r1s.push_back(t_adc_r1);
    adc_r2s.push_back(t_adc_r2);
  }
  adcruns.close();

  // input adc to gev conversion factors
  ofstream output;
  output.open(Form("ecalpeds/adc2gev_%i-%i.txt",adc_r1s[0],adc_r2s[adc_r2s.size()-1]),std::ios_base::trunc);
  for (int i = 0; i < adc_r1s.size(); i++){
    std::ifstream inputadcs;
    inputadcs.open(Form("ecalpeds/dump_EcalADCToGeVConstant__since_00%i_till_00%i.dat",adc_r1s[i],adc_r2s[i]),std::ios::in);
    TString t_st_eb, t_st_ee;
    float t_adc2gev_eb, t_adc2gev_ee;
    while (inputadcs >> t_st_eb >> t_adc2gev_eb >> t_st_ee >> t_adc2gev_ee) {
      adcconv adctogev;
      adctogev.runb_ = adc_r1s[i]; 
      adctogev.rune_ = adc_r2s[i];
      adctogev.ebconv_ = t_adc2gev_eb; 
      adctogev.eeconv_ = t_adc2gev_ee;
      adcconvs.push_back(adctogev);

      output << adctogev.runb_ << " " 
	     << adctogev.rune_ << " "
	     << adctogev.ebconv_ << " "
	     << adctogev.eeconv_ << " "
	     << std::endl;
    }
    inputadcs.close();
  }
  output.close();
}

void getDetIDs(std::map<uint32_t,eid>& eids)
{
  // input maps for detids
  std::ifstream inputids;
  inputids.open("detidsietaiphi.txt",std::ios::in);
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
}

void getPedRuns(std::vector<int>& ped_r1s, std::vector<int>& ped_r2s)
{  
  std::ifstream pedruns;
  pedruns.open("ecalpeds/pedruns.txt",std::ios::in);
  int t_ped_r1, t_ped_r2; // t is for temp
  while (pedruns >> t_ped_r1 >> t_ped_r2){
    ped_r1s.push_back(t_ped_r1);
    ped_r2s.push_back(t_ped_r2);
  }
  pedruns.close();
}






  // mc

//  std::vector<adcconv> adcconvs;
//  getADCs(adcconvs);


  // TString name = "ecalpeds/dump_EcalPedestals__MC.dat";
  // std::ifstream input;
  // input.open(name.Data(),std::ios::in);
  // float x, y, z, m1, r1, m2, r2, m3, r3;
  // uint32_t id;

  // std::ofstream output;
  // output.open("pedestals/pedestals_MC.txt",std::ios_base::trunc);

  // while (input >> x >> y >> z >> m1 >> r1 >> m2 >> r2 >> m3 >> r3 >> id)
  // {
  //   output << id << " " << r1 << std::endl;
  // }
  // input.close();
  // output.close();

  // data





  // std::map<uint32_t,eid> eids;
  // getDetIDs(eids);

  // float ebconv = adcconvs[adcconvs.size()-1].ebconv_;
  // float eeconv = adcconvs[adcconvs.size()-1].eeconv_;
  //TH1F * quick = new TH1F("h","h12",100,0,0.5);


      //if (z == 0){ quick->Fill(r1 * ebconv); }
      //if (std::abs(z) == 1){ quick->Fill(r1 * eeconv); }

  //  quick->Draw();
