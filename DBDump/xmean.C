#include <vector>
#include "TH1F.h"

void xmean(){
  int m = 3;

  std::ifstream inputruns;
  inputruns.open("list",std::ios::in);
  int rn1, rn2;
  std::vector<int> r1s, r2s;
  while (inputruns >> rn1 >> rn2){
    r1s.push_back(rn1);
    r2s.push_back(rn2);
  }
  inputruns.close();
  
  TCanvas * overall = new TCanvas();
  overall->cd();
  
  TLegend * leg = new TLegend(0.7,0.2,1.0,1.0);

  std::vector<TH1F*> hists;
  hists.resize(r2s.size());

  for (int i = 0; i < r1s.size(); i++){
    int run1 = r1s[i];
    int run2 = r2s[i];
    TString name = Form("ecalpeds/dump_EcalPedestals__since_00%i_till_00%i.dat",run1,run2);
    
    hists[i] = new TH1F(Form("hist_%i",i),Form("mean_x12-%i runs:%i-%i",m,run1,run2),100,150,250);
    hists[i]->GetXaxis()->SetTitle(Form("mean_x12 (%i)",m));
    hists[i]->GetYaxis()->SetTitle("nCrystals");
    
    std::ifstream input;
    input.open(name.Data(),std::ios::in);
    float x, y, z, m1, r1, m2, r2, m3, r3, id;
    while (input >> x >> y >> z >> m1 >> r1 >> m2 >> r2 >> m3 >> r3 >> id){
      if (m == 1) {
	hists[i]->Fill(m1);
      }
      else if (m == 2) {
	hists[i]->Fill(m2);
      }
      else if (m == 3) {
	hists[i]->Fill(m3);
      }
    }
    input.close();
    
    TCanvas * canv = new TCanvas();
    canv->cd();
    hists[i]->Draw();
    
    canv->SetLogy(1);
    canv->SaveAs(Form("mean_x12_%i_runs%i_%i.png",m,run1,run2));

    delete canv;
    
    // now do overall stuff
    overall->cd();
    hists[i]->SetLineColor(i+1);
    hists[i]->SetTitle(Form("mean_x12 (%i) runs",m));
    hists[i]->Scale(1.0/hists[i]->Integral());
    hists[i]->Draw(i>0?"same":"");

    leg->AddEntry(hists[i],Form("%i-%i",run1,run2),"l");

  }

  overall->cd();
  leg->Draw("same");

  overall->SaveAs(Form("mean_x12_%i_overall.png",m));

  delete leg;
  delete overall;

}

