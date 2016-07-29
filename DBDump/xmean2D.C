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

void xmean2D()
{
  // output file
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

  std::vector<std::vector<TH2F*> > histseb;  histseb.resize(r1s.size());
  std::vector<std::vector<TH2F*> > histsee;  histsee.resize(r1s.size());
  std::vector<std::vector<TH2F*> > histseep; histseep.resize(r1s.size());
  std::vector<std::vector<TH2F*> > histseem; histseem.resize(r1s.size());

  std::vector<int> xs; xs.push_back(12); xs.push_back(6); xs.push_back(1);
  for (int i = 0; i < r1s.size(); i++) {
    int run1 = r1s[i];
    int run2 = r2s[i];
    histseb[i].resize(xs.size());
    histsee[i].resize(xs.size());
    histseep[i].resize(xs.size());
    histseem[i].resize(xs.size());
    for (int j = 0; j < xs.size(); j++) {
      histseb[i][j] = new TH2F(Form("hist2D_%i_x%i_eb",i,xs[j]),Form("rms_x%i runs:%i-%i 2D EB",xs[j],run1,run2),361,0,361,172,-86,86);
      histseb[i][j]->GetXaxis()->SetTitle("iphi");
      histseb[i][j]->GetYaxis()->SetTitle("ieta");

      histsee[i][j] = new TH2F(Form("hist2D_%i_x%i_ee",i,xs[j]),Form("rms_x%i runs:%i-%i 2D EE (Inclusive)",xs[j],run1,run2),102,0,102,102,0,102);
      histsee[i][j]->GetXaxis()->SetTitle("ix");
      histsee[i][j]->GetYaxis()->SetTitle("iy");

      histseep[i][j] = new TH2F(Form("hist2D_%i_x%i_eep",i,xs[j]),Form("rms_x%i runs:%i-%i 2D EE+",xs[j],run1,run2),102,0,102,102,0,102);
      histseep[i][j]->GetXaxis()->SetTitle("ix");
      histseep[i][j]->GetYaxis()->SetTitle("iy");

      histseem[i][j] = new TH2F(Form("hist2D_%i_x%i_eem",i,xs[j]),Form("rms_x%i runs:%i-%i 2D EE-",xs[j],run1,run2),102,0,102,102,0,102);
      histseem[i][j]->GetXaxis()->SetTitle("ix");
      histseem[i][j]->GetYaxis()->SetTitle("iy");
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
      if (eids[id].name_.Contains("EB",TString::kExact)) {
	r1 *= ebconv; r2 *= ebconv; r3 *= ebconv;
	histseb[i][0]->Fill(eids[id].i1_,eids[id].i2_,r1);
	histseb[i][1]->Fill(eids[id].i1_,eids[id].i2_,r2);
	histseb[i][2]->Fill(eids[id].i1_,eids[id].i2_,r3);
      }
      else if (eids[id].name_.Contains("EE-",TString::kExact) || eids[id].name_.Contains("EE+",TString::kExact)) {
	r1 *= eeconv; r2 *= eeconv; r3 *= eeconv;

	const float i1 = histsee[i][0]->GetBinContent(eids[id].i1_+1,eids[id].i2_+1);
	const float i2 = histsee[i][1]->GetBinContent(eids[id].i1_+1,eids[id].i2_+1);
	const float i3 = histsee[i][2]->GetBinContent(eids[id].i1_+1,eids[id].i2_+1);

	if (i1 == 0) {histsee[i][0]->Fill(eids[id].i1_,eids[id].i2_,r1);}
	else         {histsee[i][0]->SetBinContent(eids[id].i1_+1,eids[id].i2_+1,(r1+i1)/2.f);}
	if (i2 == 0) {histsee[i][1]->Fill(eids[id].i1_,eids[id].i2_,r2);}
	else         {histsee[i][1]->SetBinContent(eids[id].i1_+1,eids[id].i2_+1,(r2+i2)/2.f);}
	if (i3 == 0) {histsee[i][2]->Fill(eids[id].i1_,eids[id].i2_,r3);}
	else         {histsee[i][2]->SetBinContent(eids[id].i1_+1,eids[id].i2_+1,(r3+i3)/2.f);}

	if      (eids[id].name_.Contains("EE+",TString::kExact)) {
	  histseep[i][0]->Fill(eids[id].i1_,eids[id].i2_,r1);
	  histseep[i][1]->Fill(eids[id].i1_,eids[id].i2_,r2);
	  histseep[i][2]->Fill(eids[id].i1_,eids[id].i2_,r3);
	}
	else if (eids[id].name_.Contains("EE-",TString::kExact)) {
	  histseem[i][0]->Fill(eids[id].i1_,eids[id].i2_,r1);
	  histseem[i][1]->Fill(eids[id].i1_,eids[id].i2_,r2);
	  histseem[i][2]->Fill(eids[id].i1_,eids[id].i2_,r3);
	}
      }
    }
    input.close();
    
    for (int j = 0; j < xs.size(); j++) {
      TCanvas * canveb = new TCanvas();
      canveb->cd();
      histseb[i][j]->Draw("colz");
      histseb[i][j]->Write(histseb[i][j]->GetName(),TObject::kWriteDelete);
      canveb->SaveAs(Form("rms_x%i_runs%i_%i_EB.png",xs[j],run1,run2));
      delete canveb;
      
      TCanvas * canvee = new TCanvas();
      canvee->cd();
      histsee[i][j]->Draw("colz");
      histsee[i][j]->Write(histsee[i][j]->GetName(),TObject::kWriteDelete);
      canvee->SaveAs(Form("rms_x%i_runs%i_%i_EE.png",xs[j],run1,run2));
      delete canvee;
      
      TCanvas * canveep = new TCanvas();
      canveep->cd();
      histseep[i][j]->Draw("colz");
      histseep[i][j]->Write(histseep[i][j]->GetName(),TObject::kWriteDelete);
      canveep->SaveAs(Form("rms_x%i_runs%i_%i_EEP.png",xs[j],run1,run2));
      delete canveep;

      TCanvas * canveem = new TCanvas();
      canveem->cd();
      histseem[i][j]->Draw("colz");
      histseem[i][j]->Write(histseem[i][j]->GetName(),TObject::kWriteDelete);
      canveem->SaveAs(Form("rms_x%i_runs%i_%i_EEM.png",xs[j],run1,run2));
      delete canveem;
    }
  }

  // average 2D plots
  std::vector<TH2F*> histsaveb;  histsaveb.resize(xs.size());
  std::vector<TH2F*> histsavee;  histsavee.resize(xs.size());
  std::vector<TH2F*> histsaveep; histsaveep.resize(xs.size());
  std::vector<TH2F*> histsaveem; histsaveem.resize(xs.size());

  for (int j = 0; j < xs.size(); j++) {
    histsaveb[j] = new TH2F(Form("hist2D_x%i_av_eb",xs[j]),Form("rms_x%i 2D (Average over Runs) EB",xs[j]),361,0,361,172,-86,86);
    histsaveb[j]->GetXaxis()->SetTitle("iphi");
    histsaveb[j]->GetYaxis()->SetTitle("ieta");
    
    histsavee[j] = new TH2F(Form("hist2D_x%i_av_ee",xs[j]),Form("rms_x%i 2D (Average over Runs) EE (Inclusive)",xs[j]),102,0,102,102,0,102);
    histsavee[j]->GetXaxis()->SetTitle("ix");
    histsavee[j]->GetYaxis()->SetTitle("iy");

    histsaveep[j] = new TH2F(Form("hist2D_x%i_av_eep",xs[j]),Form("rms_x%i 2D (Average over Runs) EE+ (Inclusive)",xs[j]),102,0,102,102,0,102);
    histsaveep[j]->GetXaxis()->SetTitle("ix");
    histsaveep[j]->GetYaxis()->SetTitle("iy");

    histsaveem[j] = new TH2F(Form("hist2D_x%i_av_eem",xs[j]),Form("rms_x%i 2D (Average over Runs) EE- (Inclusive)",xs[j]),102,0,102,102,0,102);
    histsaveem[j]->GetXaxis()->SetTitle("ix");
    histsaveem[j]->GetYaxis()->SetTitle("iy");
  }

  for (int i = 0; i < r1s.size(); i++) {  
    for (int j = 0; j < xs.size(); j++) {
      histsaveb[j]->Add(histseb[i][j]);
      histsavee[j]->Add(histsee[i][j]);
      histsaveep[j]->Add(histseem[i][j]);
      histsaveem[j]->Add(histseem[i][j]);
    }
  }

  for (int j = 0; j < xs.size(); j++) {
    TCanvas * canveb = new TCanvas();
    canveb->cd();
    histsaveb[j]->Scale(1.f/r1s.size());
    histsaveb[j]->Draw("colz");
    histsaveb[j]->Write(histsaveb[j]->GetName(),TObject::kWriteDelete);
    canveb->SaveAs(Form("rms_x%i_average_EB.png",xs[j]));
    delete canveb;

    TCanvas * canvee = new TCanvas();
    canvee->cd();
    histsavee[j]->Scale(1.f/r1s.size());
    histsavee[j]->Draw("colz");
    histsavee[j]->Write(histsavee[j]->GetName(),TObject::kWriteDelete);
    canvee->SaveAs(Form("rms_x%i_average_EE.png",xs[j]));
    delete canvee;

    TCanvas * canveep = new TCanvas();
    canveep->cd();
    histsaveep[j]->Scale(1.f/r1s.size());
    histsaveep[j]->Draw("colz");
    histsaveep[j]->Write(histsaveep[j]->GetName(),TObject::kWriteDelete);
    canveep->SaveAs(Form("rms_x%i_average_EEP.png",xs[j]));
    delete canveep;
      
    TCanvas * canveem = new TCanvas();
    canveem->cd();
    histsaveem[j]->Scale(1.f/r1s.size());
    histsaveem[j]->Draw("colz");
    histsaveem[j]->Write(histsaveem[j]->GetName(),TObject::kWriteDelete);
    canveem->SaveAs(Form("rms_x%i_average_EEM.png",xs[j]));
    delete canveem;
  }

  delete outfile;
}
