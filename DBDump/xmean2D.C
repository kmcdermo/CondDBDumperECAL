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

void xmean2D(){
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

  std::vector<std::vector<TH2F*> > histseb;
  std::vector<std::vector<TH2F*> > histseep;
  std::vector<std::vector<TH2F*> > histseem;
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
      histseb[i][j] = new TH2F(Form("hist_%i_x%i_eb",i,xs[j]),Form("mean_x%i runs:%i-%i 2D EB",xs[j],run1,run2),360,0,360,170,-85,85);
      histseb[i][j]->GetXaxis()->SetTitle("iphi");
      histseb[i][j]->GetYaxis()->SetTitle("ieta");

      histseep[i][j] = new TH2F(Form("hist_%i_x%i_eep",i,xs[j]),Form("mean_x%i runs:%i-%i 2D EE+",xs[j],run1,run2),100,0,100,100,0,100);
      histseep[i][j]->GetXaxis()->SetTitle("ix");
      histseep[i][j]->GetYaxis()->SetTitle("iy");

      histseem[i][j] = new TH2F(Form("hist_%i_x%i_eem",i,xs[j]),Form("mean_x%i runs:%i-%i 2D EE-",xs[j],run1,run2),100,0,100,100,0,100);
      histseem[i][j]->GetXaxis()->SetTitle("ix");
      histseem[i][j]->GetYaxis()->SetTitle("iy");
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
	histseb[i][0]->Fill(eids[id].i1_,eids[id].i2_,m1);
	histseb[i][1]->Fill(eids[id].i1_,eids[id].i2_,m2);
	histseb[i][2]->Fill(eids[id].i1_,eids[id].i2_,m3);
      }
      else if (eids[id].name_.Contains("EE+",TString::kExact)) {
	histseep[i][0]->Fill(eids[id].i1_,eids[id].i2_,m1);
	histseep[i][1]->Fill(eids[id].i1_,eids[id].i2_,m2);
	histseep[i][2]->Fill(eids[id].i1_,eids[id].i2_,m3);
      }
      else if (eids[id].name_.Contains("EE-",TString::kExact)) {
	histseem[i][0]->Fill(eids[id].i1_,eids[id].i2_,m1);
	histseem[i][1]->Fill(eids[id].i1_,eids[id].i2_,m2);
	histseem[i][2]->Fill(eids[id].i1_,eids[id].i2_,m3);
      }
    }
    input.close();
    
    for (int j = 0; j < 3; j++) {
      TCanvas * canveb = new TCanvas();
      canveb->cd();
      histseb[i][j]->Draw("colz");
      canveb->SaveAs(Form("mean_x%i_runs%i_%i_EB.png",xs[j],run1,run2));
      delete canveb;
      
      TCanvas * canveep = new TCanvas();
      canveep->cd();
      histseep[i][j]->Draw("colz");
      canveep->SaveAs(Form("mean_x%i_runs%i_%i_EEP.png",xs[j],run1,run2));
      delete canveep;

      TCanvas * canveem = new TCanvas();
      canveem->cd();
      histseem[i][j]->Draw("colz");
      canveem->SaveAs(Form("mean_x%i_runs%i_%i_EEM.png",xs[j],run1,run2));
      delete canveem;
    }
  }
}
