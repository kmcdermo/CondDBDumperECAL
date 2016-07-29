// https://cms-conddb.cern.ch/cmsDbBrowser/list/Prod/gts/76X_dataRun2_16Dec2015_v0
// GT for 2015 data rereco 76X
// look up Tag matching Record from supported objects: https://github.com/kmcdermo/CondDBDumperECAL
// 2015  start: 254231
// 2015D start: 256630
// 2015D end:   260627
// conddb_dumper -O EcalPedestals -c frontier://FrontierProd/CMS_CONDITIONS -t EcalPedestals_hlt -b 254231

// ADC conversion: EcalADCToGeVConstant
// EcalADCToGeVConstant_Run1_Run2_V02_offline

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

void xmean()
{
  // output root 
  TFile * outfile = TFile::Open("plots.root","UPDATE");

  //  gStyle->SetOptStat("emou");
  gStyle->SetOptStat(0);

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

  // input adc to gev conversion factors
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

  std::vector<std::vector<TH1F*> > histseb;  histseb.resize(r1s.size());
  std::vector<std::vector<TH1F*> > histsee;  histsee.resize(r1s.size());
  std::vector<std::vector<TH1F*> > histseep; histseep.resize(r1s.size());
  std::vector<std::vector<TH1F*> > histseem; histseem.resize(r1s.size());

  std::vector<int> xs; xs.push_back(12); xs.push_back(6); xs.push_back(1);
  const float xhigh  = 0.5;
  const float nxbins = 1000; 
  for (int i = 0; i < r1s.size(); i++) {
    int run1 = r1s[i];
    int run2 = r2s[i];
    histseb[i].resize(xs.size());
    histsee[i].resize(xs.size());
    histseep[i].resize(xs.size());
    histseem[i].resize(xs.size());
    for (int j = 0; j < xs.size(); j++) {
      histseb[i][j] = new TH1F(Form("hist1D_%i_x%i_eb",i,xs[j]),Form("rms x%i runs:%i-%i EB",xs[j],run1,run2),nxbins,0,xhigh);
      histseb[i][j]->GetXaxis()->SetTitle(Form("x%i rms",xs[j]));
      histseb[i][j]->GetYaxis()->SetTitle("nCrystals");

      histsee[i][j] = new TH1F(Form("hist1D_%i_x%i_ee",i,xs[j]),Form("rms x%i runs:%i-%i EE (Inclusive)",xs[j],run1,run2),nxbins,0,xhigh);
      histsee[i][j]->GetXaxis()->SetTitle(Form("x%i rms",xs[j]));
      histsee[i][j]->GetYaxis()->SetTitle("nCrystals");

      histseep[i][j] = new TH1F(Form("hist1D_%i_x%i_eep",i,xs[j]),Form("rms x%i runs:%i-%i EE+",xs[j],run1,run2),nxbins,0,xhigh);
      histseep[i][j]->GetXaxis()->SetTitle(Form("x%i rms",xs[j]));
      histseep[i][j]->GetYaxis()->SetTitle("nCrystals");

      histseem[i][j] = new TH1F(Form("hist1D_%i_x%i_eem",i,xs[j]),Form("rms x%i runs:%i-%i EE-",xs[j],run1,run2),nxbins,0,xhigh);
      histseem[i][j]->GetXaxis()->SetTitle(Form("x%i rms",xs[j]));
      histseem[i][j]->GetYaxis()->SetTitle("nCrystals");
    }
  }

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
    
    for (int j = 0; j < xs.size(); j++) {
      TCanvas * canveb = new TCanvas();
      canveb->cd();
      canveb->SetLogy(1);
      histseb[i][j]->Draw();
      histseb[i][j]->Write(histseb[i][j]->GetName(),TObject::kWriteDelete);
      canveb->SaveAs(Form("rms_x%i_runs%i_%i_EB.png",xs[j],run1,run2));
      delete canveb;

      TCanvas * canvee = new TCanvas();
      canvee->cd();
      canvee->SetLogy(1);
      histsee[i][j]->Draw();
      histsee[i][j]->Write(histsee[i][j]->GetName(),TObject::kWriteDelete);
      canvee->SaveAs(Form("rms_x%i_runs%i_%i_EE.png",xs[j],run1,run2));
      delete canvee;
      
      TCanvas * canveep = new TCanvas();
      canveep->cd();
      canveep->SetLogy(1);
      histseep[i][j]->Draw();
      histseep[i][j]->Write(histseep[i][j]->GetName(),TObject::kWriteDelete);
      canveep->SaveAs(Form("rms_x%i_runs%i_%i_EEP.png",xs[j],run1,run2));
      delete canveep;

      TCanvas * canveem = new TCanvas();
      canveem->cd();
      canveem->SetLogy(1);
      histseem[i][j]->Draw();
      histseem[i][j]->Write(histseem[i][j]->GetName(),TObject::kWriteDelete);
      canveem->SaveAs(Form("rms_x%i_runs%i_%i_EEM.png",xs[j],run1,run2));
      delete canveem;
    }
  }

  std::vector<TH1F*> histstoteb;  histstoteb.resize(xs.size());
  std::vector<TH1F*> histstotee;  histstotee.resize(xs.size());
  std::vector<TH1F*> histstoteep; histstoteep.resize(xs.size());
  std::vector<TH1F*> histstoteem; histstoteem.resize(xs.size());
  for (int j = 0; j < xs.size(); j++) {
    histstoteb[j] = new TH1F(Form("hist1D_total_x%i_eb",xs[j]),Form("rms x%i total EB",xs[j]),nxbins,0,xhigh);
    histstoteb[j]->GetXaxis()->SetTitle(Form("x%i rms",xs[j]));
    histstoteb[j]->GetYaxis()->SetTitle("nCrystals");

    histstotee[j] = new TH1F(Form("hist1D_total_x%i_ee",xs[j]),Form("rms x%i total EE",xs[j]),nxbins,0,xhigh);
    histstotee[j]->GetXaxis()->SetTitle(Form("x%i rms",xs[j]));
    histstotee[j]->GetYaxis()->SetTitle("nCrystals");

    histstoteep[j] = new TH1F(Form("hist1D_total_x%i_eep",xs[j]),Form("rms x%i total EE+",xs[j]),nxbins,0,xhigh);
    histstoteep[j]->GetXaxis()->SetTitle(Form("x%i rms",xs[j]));
    histstoteep[j]->GetYaxis()->SetTitle("nCrystals");

    histstoteem[j] = new TH1F(Form("hist1D_total_x%i_eem",xs[j]),Form("rms x%i total EE-",xs[j]),nxbins,0,xhigh);
    histstoteem[j]->GetXaxis()->SetTitle(Form("x%i rms",xs[j]));
    histstoteem[j]->GetYaxis()->SetTitle("nCrystals");
  }

  for (int i = 0; i < r1s.size(); i++) {
    for (int j = 0; j < xs.size(); j++) {
      histstoteb[j]->Add(histseb[i][j]);
      histstotee[j]->Add(histsee[i][j]);
      histstoteep[j]->Add(histseep[i][j]);
      histstoteem[j]->Add(histseem[i][j]);
    }
  }

  for (int j = 0; j < xs.size(); j++) {
    TCanvas * canveb = new TCanvas();
    canveb->cd();
    canveb->SetLogy(1);
    histstoteb[j]->Draw();
    histstoteb[j]->Write(histstoteb[j]->GetName(),TObject::kWriteDelete);
    canveb->SaveAs(Form("rms_x%i_total_EB.png",xs[j]));
    delete canveb;

    TCanvas * canvee = new TCanvas();
    canvee->cd();
    canvee->SetLogy(1);
    histstotee[j]->Draw();
    histstotee[j]->Write(histstotee[j]->GetName(),TObject::kWriteDelete);
    canvee->SaveAs(Form("rms_x%i_total_EE.png",xs[j]));
    delete canvee;

    TCanvas * canveep = new TCanvas();
    canveep->cd();
    canveep->SetLogy(1);
    histstoteep[j]->Draw();
    histstoteep[j]->Write(histstoteep[j]->GetName(),TObject::kWriteDelete);
    canveep->SaveAs(Form("rms_x%i_total_EEP.png",xs[j]));
    delete canveep;

    TCanvas * canveem = new TCanvas();
    canveem->cd();
    canveem->SetLogy(1);
    histstoteem[j]->Draw();
    histstoteem[j]->Write(histstoteem[j]->GetName(),TObject::kWriteDelete);
    canveem->SaveAs(Form("rms_x%i_total_EEM.png",xs[j]));
    delete canveem;
  }

  std::vector<TString> parts; parts.push_back("EB"); parts.push_back("EE"); parts.push_back("EEP"); parts.push_back("EEM");

  std::vector<std::vector<TCanvas*> > canvalls;
  std::vector<std::vector<TLegend*> > legalls;
  canvalls.resize(parts.size());
  legalls.resize(parts.size());
  for (int k = 0; k < parts.size(); k++){
    canvalls[k].resize(xs.size());
    legalls[k].resize(xs.size());
    for (int j = 0; j < xs.size(); j++){
      canvalls[k][j] = new TCanvas();
      canvalls[k][j]->cd();
      legalls[k][j] = new TLegend(0.7,0.6,1.0,1.0);
    }
  }

  for (int i = 0; i < r1s.size(); i++){
    int run1 = r1s[i];
    int run2 = r2s[i];
    for (int j = 0; j < xs.size(); j++) {
      //// do the overall stuffs
      canvalls[0][j]->cd();
      histseb[i][j]->SetLineColor(i<10?(i+1):(i+1)+30);
      histseb[i][j]->SetTitle(Form("rms x%i all runs EB",xs[j]));
      histseb[i][j]->Scale(1.0/histseb[i][j]->Integral());
      histseb[i][j]->Draw(i>0?"same":"");
      legalls[0][j]->AddEntry(histseb[i][j],Form("%i-%i",run1,run2),"l");

      canvalls[1][j]->cd();
      histseb[i][j]->SetLineColor(i<10?(i+1):(i+1)+30);
      histseb[i][j]->SetTitle(Form("rms x%i all runs EE (inclusive)",xs[j]));
      histseb[i][j]->Scale(1.0/histseb[i][j]->Integral());
      histseb[i][j]->Draw(i>0?"same":"");
      legalls[1][j]->AddEntry(histseb[i][j],Form("%i-%i",run1,run2),"l");

      canvalls[2][j]->cd();
      histseep[i][j]->SetLineColor(i<10?(i+1):(i+1)+30);
      histseep[i][j]->SetTitle(Form("rms x%i all runs EE+",xs[j]));
      histseep[i][j]->Scale(1.0/histseep[i][j]->Integral());
      histseep[i][j]->Draw(i>0?"same":"");
      legalls[2][j]->AddEntry(histseep[i][j],Form("%i-%i",run1,run2),"l");

      canvalls[3][j]->cd();
      histseem[i][j]->SetLineColor(i<10?(i+1):(i+1)+30);
      histseem[i][j]->SetTitle(Form("rms x%i all runs EE-",xs[j]));
      histseem[i][j]->Scale(1.0/histseem[i][j]->Integral());
      histseem[i][j]->Draw(i>0?"same":"");
      legalls[3][j]->AddEntry(histseem[i][j],Form("%i-%i",run1,run2),"l");
    }
  }

  // print overalls
  for (int k = 0; k < canvalls.size(); k++){
    for (int j = 0; j < xs.size(); j++){
      canvalls[k][j]->cd();
      legalls[k][j]->Draw("same");
      canvalls[k][j]->SaveAs(Form("rms_x%i_overall_%s.png",xs[j],parts[k].Data()));

      delete legalls[k][j];
      delete canvalls[k][j];
    }
  }

  delete outfile;
}
