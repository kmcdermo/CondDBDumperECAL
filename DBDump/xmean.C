#include <vector>
#include "TH2F.h"
#include "TString.h"

#include <map>

struct eid{
public:
  eid() {}
  int i1_; // iphi (EB) or ix (EE)
  int i2_; // ieta (EB) or iy (EE)
  TString name_;
};

void xmean(){
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

  // for (std::map<int,eid>::const_iterator iter = eids.begin(); iter != eids.end(); ++iter){
  //   std::cout << iter->first << " " << iter->second.i1_ << " " << iter->second.i2_ << " " << iter->second.name_ << std::endl;
  // }

  std::vector<std::vector<TH1F*> > histseb;
  std::vector<std::vector<TH1F*> > histseep;
  std::vector<std::vector<TH1F*> > histseem;
  histseb.resize(r1s.size());
  histseep.resize(r1s.size());
  histseem.resize(r1s.size());

  std::vector<int> xs; xs.push_back(12); xs.push_back(6); xs.push_back(1);
  for (int i = 0; i < r1s.size(); i++) {
    int run1 = r1s[i];
    int run2 = r2s[i];
    histseb[i].resize(3);
    histseep[i].resize(3);
    histseem[i].resize(3);
    for (int j = 0; j < 3; j++) {
      histseb[i][j] = new TH1F(Form("hist_%i_x%i_eb",i,xs[j]),Form("mean x%i runs:%i-%i EB",xs[j],run1,run2),100,150,250);
      histseb[i][j]->GetXaxis()->SetTitle(Form("x%i mean",xs[j]));
      histseb[i][j]->GetYaxis()->SetTitle("nCrystals");

      histseep[i][j] = new TH1F(Form("hist_%i_x%i_eep",i,xs[j]),Form("mean x%i runs:%i-%i EE+",xs[j],run1,run2),100,150,250);
      histseep[i][j]->GetXaxis()->SetTitle(Form("x%i mean",xs[j]));
      histseep[i][j]->GetYaxis()->SetTitle("nCrystals");

      histseem[i][j] = new TH1F(Form("hist_%i_x%i_eem",i,xs[j]),Form("mean x%i runs:%i-%i EE-",xs[j],run1,run2),100,150,250);
      histseem[i][j]->GetXaxis()->SetTitle(Form("x%i mean",xs[j]));
      histseem[i][j]->GetYaxis()->SetTitle("nCrystals");
    }
  }

  std::vector<TString> parts; parts.push_back("EB"); parts.push_back("EEP"); parts.push_back("EEM");

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
      legalls[k][j] = new TLegend(0.7,0.2,1.0,1.0);
    }
  }
  
  for (int i = 0; i < r1s.size(); i++){
    int run1 = r1s[i];
    int run2 = r2s[i];
    TString name = Form("ecalpeds/dump_EcalPedestals__since_00%i_till_00%i.dat",run1,run2);
        
    std::ifstream input;
    input.open(name.Data(),std::ios::in);
    float x, y, z, m1, r1, m2, r2, m3, r3;
    uint32_t id;
    while (input >> x >> y >> z >> m1 >> r1 >> m2 >> r2 >> m3 >> r3 >> id){
      if (eids[id].name_.Contains("EB",TString::kExact)) {
	histseb[i][0]->Fill(m1);
	histseb[i][1]->Fill(m2);
	histseb[i][2]->Fill(m3);
      }
      else if (eids[id].name_.Contains("EE+",TString::kExact)) {
	histseep[i][0]->Fill(m1);
	histseep[i][1]->Fill(m2);
	histseep[i][2]->Fill(m3);
      }
      else if (eids[id].name_.Contains("EE-",TString::kExact)) {
	histseem[i][0]->Fill(m1);
	histseem[i][1]->Fill(m2);
	histseem[i][2]->Fill(m3);
      }
    }
    input.close();
    
    for (int j = 0; j < xs.size(); j++) {
      TCanvas * canveb = new TCanvas();
      canveb->cd();
      canveb->SetLogy(1);
      histseb[i][j]->Draw();
      canveb->SaveAs(Form("mean_x%i_runs%i_%i_EB.png",xs[j],run1,run2));
      delete canveb;
      
      TCanvas * canveep = new TCanvas();
      canveep->cd();
      canveep->SetLogy(1);
      histseep[i][j]->Draw();
      canveep->SaveAs(Form("mean_x%i_runs%i_%i_EEP.png",xs[j],run1,run2));
      delete canveep;

      TCanvas * canveem = new TCanvas();
      canveem->cd();
      canveem->SetLogy(1);
      histseem[i][j]->Draw();
      canveem->SaveAs(Form("mean_x%i_runs%i_%i_EEM.png",xs[j],run1,run2));
      delete canveem;

      //// do the overall stuffs
      canvalls[0][j]->cd();
      histseb[i][j]->SetLineColor(i<10?(i+1):(i+1)+30);
      histseb[i][j]->SetTitle(Form("mean x%i all runs EB",xs[j]));
      histseb[i][j]->Scale(1.0/histseb[i][j]->Integral());
      histseb[i][j]->Draw(i>0?"same":"");
      legalls[0][j]->AddEntry(histseb[i][j],Form("%i-%i",run1,run2),"l");

      canvalls[1][j]->cd();
      histseep[i][j]->SetLineColor(i<10?(i+1):(i+1)+30);
      histseep[i][j]->SetTitle(Form("mean x%i all runs EE+",xs[j]));
      histseep[i][j]->Scale(1.0/histseep[i][j]->Integral());
      histseep[i][j]->Draw(i>0?"same":"");
      legalls[1][j]->AddEntry(histseep[i][j],Form("%i-%i",run1,run2),"l");

      canvalls[2][j]->cd();
      histseem[i][j]->SetLineColor(i<10?(i+1):(i+1)+30);
      histseem[i][j]->SetTitle(Form("mean x%i all runs EE-",xs[j]));
      histseem[i][j]->Scale(1.0/histseem[i][j]->Integral());
      histseem[i][j]->Draw(i>0?"same":"");
      legalls[2][j]->AddEntry(histseem[i][j],Form("%i-%i",run1,run2),"l");
    }
  }

  // print overalls
  for (int k = 0; k < canvalls.size(); k++){
    for (int j = 0; j < xs.size(); j++){
      canvalls[k][j]->cd();
      legalls[k][j]->Draw("same");
      canvalls[k][j]->SaveAs(Form("mean_x%i_overall_%s.png",xs[j],parts[k].Data()));

      delete legalls[k][j];
      delete canvalls[k][j];
    }
  }


}
