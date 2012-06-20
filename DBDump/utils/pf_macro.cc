//#include "TAxis.h"
//#include "TFile.h"
//#include "TGraphAsymmErrors.h"
//#include "TString.h"

#include "autozoom.C"
#include "drawGraphEnv.C"
//#include "tdrstyle.C"
#include "TMath.h"
#include "xtime.C"

void pf_macro(const char * filename = "out_plot_GR_R_42_V19::All_EcalLaserAPDPNRatios_v3_online.root", const char * img_suffix = "eps"){
  all(filename, img_suffix);
}

void ovfIntoBins(TH1* h){
  TAxis* a = h->GetXaxis();
  int nbins = a->GetNbins();
  h->SetBinContent(1, h->GetBinContent(1) + h->GetBinContent(0));
  h->SetBinContent(0,0);
  h->SetBinContent(nbins, h->GetBinContent(nbins) + h->GetBinContent(nbins+1));
  h->SetBinContent(nbins+1, 0);
}

template<class T>
void draw(T* h, const char* opt = "")
{
  //   return;
  //fix style for graph:
  TAxis * a[2];
  char* aname[] = { "X", "Y"};
  a[0] = h->GetXaxis();
  a[1] = h->GetYaxis();
  for(unsigned i = 0; i < sizeof(a)/sizeof(a[0]); ++i){
    a[i]->SetTitleFont(gStyle->GetTitleFont(aname[i]));
    a[i]->SetTitleSize(gStyle->GetTitleSize(aname[i]));
    a[i]->SetTitleColor(gStyle->GetTitleColor(aname[i]));
    a[i]->SetLabelFont(gStyle->GetLabelFont(aname[i]));
    a[i]->SetLabelSize(gStyle->GetLabelSize(aname[i]));
    a[i]->SetLabelColor(gStyle->GetLabelColor(aname[i]));
  }
  h->GetXaxis()->SetTitleOffset(gStyle->GetTitleXOffset());
  h->GetYaxis()->SetTitleOffset(gStyle->GetTitleYOffset());
  
  h->Draw(opt);
}

void gplot(TDirectory * f, char * gname, char * title = 0, const char * img_suffix = "eps")
{
    const char * nfrac[] = { "3S", "2S", "1S", "E" };
    const char * nleg[] = { "99.7% of channels", "95.4% of channels", "68.2% of channels", "extrema #times 0.1" };
    //const int ncol[] = { kOrange + 1, TColor::GetColor("#ffff00"), TColor::GetColor("#33ee33"), 0 };
    const int ncol[] = { kOrange + 1, TColor::GetColor("#ffff00"), 8, 0 };
    char str[64];
    TGraphAsymmErrors * g = 0;
    TH1 * gg = 0;

    TLatex  * t = new TLatex(0.87, 0.8, title);
    t->SetTextAlign(21);
    t->SetNDC();
    t->SetTextFont(42);
    t->SetTextSize(0.06);
    TLegend * l = new TLegend(0.775, 0.13, 0.95, 0.13 + 0.35);

    for (int i = 0; i < sizeof(nfrac)/sizeof(char*); ++i) {
        sprintf(str, "%s_%s", gname, nfrac[i]);
	//        printf("--> %s\n", str);
        //g = (TGraphAsymmErrors*)gDirectory->Get(str);
        g = (TGraphAsymmErrors*)f->Get(str);
	//        printf("--> %p\n", g);
        g->SetMarkerStyle(20);
        g->SetMarkerSize(.5);
        g->SetFillColor(ncol[i]);
        //draw(g, i == 0 ? "ae3" : "e3");
	if(i == 0){
	  TH1* haxis = xtime(g);
	  haxis->GetXaxis()->SetLabelOffset(0.04);
	  haxis->GetXaxis()->SetLabelSize(0.045);
	  haxis->Draw();
	}
        if (i != 3) draw(g, "e3"); //draw(g, i == 0 ? "ae3" : "e3");
        else {
            drawGraphEnv(g, 0.1, false, 7, 2, 1, false, &gg);
        }
        if (i == 3) draw(g, "xl");
	//g->GetXaxis()->SetTimeDisplay(1);
	//g->GetXaxis()->SetTimeFormat("#splitline{%d/%m}{%H:%M}");
	//g->GetXaxis()->SetNdivisions(505);
        g->GetXaxis()->SetTitle("time");
        g->GetYaxis()->SetTitle("transparency change");
        //g->GetYaxis()->SetRangeUser(0.5, 1.15);
        gPad->SetTicks();
        //if (title) g->SetTitle(title);
        if (i == 0) draw(g, "e3"); //draw(g, i == 0 ? "ae3" : "e3");
        if (i == 0) l->AddEntry(g, "median", "l");
        l->AddEntry(i == 3 ? gg : g, nleg[i], i == 3 ? "l" : "f");
    }
    l->SetFillStyle(0);
    l->SetBorderSize(0);
    l->SetTextFont(42);
    l->SetTextSize(0.0475);
    l->Draw();
    t->Draw();
    t->SetTextFont(haxis->GetXaxis()->GetLabelFont());
    t->SetTextSize(haxis->GetXaxis()->GetLabelSize());
    t->DrawLatex(0.804, 0.0538, "(UTC)");
    gPad->Print((std::string(gname) + "." + img_suffix).c_str());
    gPad->SaveAs((std::string(gname) + ".root").c_str());
    //gPad->Print("anim.gif+10");
}

void mplot(TDirectory * f, char * gname, char * title = 0, const char * img_suffix = "eps")
{
    const char * nfrac[] = { "3S", "2S", "1S", "E" };
    const char * nleg[] = { "99.7% of channels", "95.4% of channels", "68.2% of channels", "extrema #times 0.1" };
    //const int ncol[] = { kOrange + 1, TColor::GetColor("#ffff00"), TColor::GetColor("#33ee33"), 0 };
    const int ncol[] = { kOrange + 1, TColor::GetColor("#ffff00"), 8, 0 };
    char str[64];
    TGraphAsymmErrors * g = 0;
    TH1 * gg = 0;

    TLatex  * t = new TLatex(0.87, 0.8, title);
    t->SetTextAlign(21);
    t->SetNDC();
    t->SetTextFont(42);
    t->SetTextSize(0.06);
    TLegend * l = new TLegend(0.775, 0.13, 0.95, 0.13 + 0.35);

    //for (int i = 0; i < sizeof(nfrac)/sizeof(char*); ++i) {
        int i=2;
	sprintf(str, "%s_%s", gname, nfrac[i]);
	std::cout << "plot name " << str << std::endl;
	g = (TGraphAsymmErrors*)f->Get(str);
	TH1* haxis = xtime(g);
	haxis->GetXaxis()->SetLabelOffset(0.04);
	haxis->GetXaxis()->SetLabelSize(0.045);
	haxis->Draw();
	
	g->SetMarkerStyle(20);
        g->SetMarkerSize(.5);
        g->SetFillColor(ncol[i]);
	draw(g, "e3"); 
	g->GetXaxis()->SetTitle("time");
        g->GetYaxis()->SetTitle("transparency change");
	gPad->SetTicks();
	//if (i == 0) draw(g, "e3"); //draw(g, i == 0 ? "ae3" : "e3");
    l->AddEntry(g, "median", "l");
        //l->AddEntry(i == 3 ? gg : g, nleg[i], i == 3 ? "l" : "f");
	//}
    l->SetFillStyle(0);
    l->SetBorderSize(0);
    l->SetTextFont(42);
    l->SetTextSize(0.0475);
    l->Draw();
    t->Draw();
    t->SetTextFont(haxis->GetXaxis()->GetLabelFont());
    t->SetTextSize(haxis->GetXaxis()->GetLabelSize());
    t->DrawLatex(0.804, 0.0538, "(UTC)");
    gPad->Print((std::string(gname) + "." + img_suffix).c_str());
    gPad->SaveAs((std::string(gname) + ".root").c_str());
    //gPad->Print("anim.gif+10");
}


void setStyle(){
  gROOT->SetStyle("Plain");
  gStyle->SetPalette(1);
  gStyle->SetTitleFont(42, "XYZ");
  gStyle->SetTitleSize(0.055, "XYZ");
  gStyle->SetTitleOffset(1., "X");
  gStyle->SetTitleOffset(.75, "Y");
  gStyle->SetLabelFont(42, "XYZ");
  gStyle->SetLabelSize(0.055, "XYZ");
  
  gStyle->SetPadTopMargin(0.05);
  gStyle->SetPadBottomMargin(0.13);
  gStyle->SetPadLeftMargin(0.10);
  gStyle->SetPadRightMargin(0.26);
  
  gStyle->SetHatchesLineWidth(2.);
  
  gStyle->SetTimeOffset(0);
  
  gStyle->SetOptTitle(0);
  gStyle->SetOptStat(0);
}

TH1* getErrorHist(const TH1* h){
  TH1* he = (TH1*) h->Clone();
  he->Reset();
  int nx = h->GetNbinsX();
  if(nx>1) nx += 2;
  int ny = h->GetNbinsY();
  if(ny>1) ny += 2;
  int nz = h->GetNbinsZ();
  if(nz>1) nz += 2;
  for(int ix = 0; ix < nx; ++ix){
    for(int iy = 0; iy < nx; ++iy){
      for(int iz = 0; iz < nx; ++iz){
	double e = h->GetBinError(ix,iy,iz);
	cout << ix << "," << iy << "," << iz << ": " << e << endl;
	he->SetBinContent(ix, iy, iz, e);
      }
    }
  }
  return he;
}

void drawNormP2Hist(const char * img_suffix = "eps"){
  const char* names[] = { "EEprof2_nZ_BadChannel", "EBprof2_BadChannel"};
  const char* ave[] = {"mean", "median"};
  for(int j=0;j<2;++j)
  for(int i=0;i<2;++i)
    {
      char str[64];
      sprintf(str,"distr_eta_normalised_%s_%s_p2",ave[j],names[i]);
      TH1* hp2norm = (TH1*) gDirectory->Get(str);
      
      if(hp2norm){
	hp2norm->GetXaxis()->SetTitle("tranps./<transp.>_{#eta}");
	hp2norm->GetYaxis()->SetTitle("entries (channel#timesIOV)");
	TCanvas* c = new TCanvas(TString("c_") + hp2norm->GetName(), hp2norm->GetTitle(), 800, 400);
	c->SetLogy();
	//      ovfIntoBins(hp2norm);
	gStyle->SetStatBorderSize(0);
	gStyle->SetOptStat("ou");
	
	draw(hp2norm);
	
	c->Update();
	TPaveStats* ps = (TPaveStats*) hp2norm->GetListOfFunctions()->FindObject("stats");
	if(ps){
	  ps->SetX1NDC(1.30e-01);
	  ps->SetX2NDC(2.7e-01);
	  ps->SetY1NDC(8.467e-01);
	  ps->SetY2NDC(9.274e-01);
	  ps->Draw();
	} else{
	  cerr << "Statistics pave of " << hp2norm->GetName() << " was not found." << endl;
	}
	
	TPad* p = new TPad("p","p", .5, .5, .99-c->GetRightMargin(), .99-c->GetTopMargin());
	p->SetRightMargin(0.05);
	p->SetTopMargin(0.09);
	p->Draw();
	p->cd();
	TH1* h = hp2norm->Clone(TString(hp2norm->GetName()) + "_lin");
	h->SetStats(0);
	
	h->GetXaxis()->SetRangeUser(0.95, 1.05);
	h->GetXaxis()->SetNdivisions(505);
	h->GetXaxis()->SetTitle("");
	h->GetYaxis()->SetTitle("");
	draw(h);
	h->GetYaxis()->SetLabelSize(0.07);
	h->GetXaxis()->SetLabelSize(0.07);
	
	c->cd();
	
	TLatex  * t = new TLatex(0.87, 0.8, "Transparency");
	t->SetTextAlign(21);
	t->SetNDC();
	t->SetTextFont(42);
	t->SetTextSize(0.06);
	t->Draw();
	t->DrawLatex(0.87, 0.74, "normalized to");
	t->DrawLatex(0.87, 0.68, "the eta-ring");
	t->DrawLatex(0.87, 0.62, names[i]);
	
	c->Print(TString(hp2norm->GetName()) + "." + img_suffix);
      }
      //restore our default style:
      setStyle();
    }
}

void drawMichaelPlots(const char * img_suffix)
{
  const char* names[] = { "EEprof2_nZ_BadChannel", "EBprof2_BadChannel", "EEprof2_pZ_BadChannel"};
  int nIOV =2000; //get this from file instead of initializing here!
  const char* part[]  = { "EE-", "EB", "EE+" };
  TCanvas* c = new TCanvas("bad", "bad", 800, 400);
  c->SetLogz();
  c->SetLeftMargin(0.07);
  Int_t mPalette[3];
  Double_t r[]    = {1.0, 1.0, 0.0};
  Double_t g[]    = {0., 1.0, 1.0};
  Double_t b[]    = {0., 1.0, 0.2};
  Double_t stop[] = {0., 0.5, 1.0};
  Int_t FI = TColor::CreateGradientColorTable(3, stop, r, g, b, 3);  //sets color palette for G,R,W
  for (int l=0;l<3;l++) 
    mPalette[l] = FI+l;  
  
  /********copied frrom histories*********/
  char pstr[64];
  char str[64];
  char name[64];
  for (int j = 0; j <nIOV; ++j) 
    for(int i=0;i<3; ++i)
      {
	TProfile2D* h = 0;
	TProfile2D* ph =0;
	if(j>=1000)
	sprintf(str, "%s_%i", names[i], j); //name for current IOV bad channel hist
	else if(j>=100)
	  sprintf(str, "%s_0%i", names[i], j); //name for current IOV bad channel hist
	else if(j>=10)
	  sprintf(str, "%s_00%i", names[i], j); //name for current IOV bad channel hist
	else
	  sprintf(str, "%s_000%i", names[i], j); //name for current IOV bad channel hist
	if(j-1>=1000)
	  sprintf(pstr, "%s_%i", names[i], j-1); //name for preivous IOV bad channel hist
	else if(j-1>=100)
	  sprintf(pstr, "%s_0%i", names[i], j-1); //name for previous IOV bad channel hist
	else if(j-1>=10)
	  sprintf(pstr, "%s_00%i", names[i], j-1); //name for previous IOV bad channel hist
	else
	  sprintf(pstr, "%s_000%i", names[i], j-1); //name for previous IOV bad channel hist
	
	h = (TProfile2D*) gDirectory->Get(str);
	ph = (TProfile2D*) gDirectory->Get(pstr);
	
	if(!ph) continue;
	if(!h) continue;
	TH2* hadd = (TH2*) ph;
	hadd->Add(h,ph,1,-1);
	int nXbin = hadd->GetNbinsX();
	int nYbin = hadd->GetNbinsY();
	bool changed = 0;
	
	for(int x =0; x<nXbin;++x)
	  for(int y =0;y<nYbin;++y)
	    {
	       if(hadd->GetBinContent(x,y)!=0)
		 {
		   changed=1;
		   break;
		}
	    }
	if(j!=1)
	  if(changed==0) //don't make plots if there was no change from the last IOV
	    continue;
	
	TH2* h2 = (TH2*) h;
	
	gStyle->SetPalette(3,mPalette);
	
	c->SetLogz(0);
	h2->SetMinimum(-1);   //sets range so -1 == red, 0==white,+1==green
	h2->SetMaximum(1);
	h2->Draw("col");
	
	c->Update();
	
	h2->GetXaxis()->SetTitle(i==1 ? "iphi" : "ix");
	h2->GetYaxis()->SetTitle(i==1 ? "ieta" : "iy");
	
	c->Paint();
	
	sprintf(name,"Bad Ch IOV#%i",j);
	TLatex * t = new TLatex(0.87, 0.9, name);
	t->SetTextAlign(21);
	t->SetNDC();
	t->SetTextFont(42);
	t->SetTextSize(0.06);
	t->Draw();
	TLatex* t2 = (TLatex*) t->Clone();
	t2->SetTextColor(kRed+2);
	TLatex* t3 = (TLatex*) t->Clone();
	TLatex* t4 = (TLatex*) t->Clone();
	TLatex* t5 = (TLatex*) t->Clone();
	t3->SetTextColor(kGreen);
	t3->DrawLatex(0.87, 0.76, "Green: GOOD");
	t4->SetTextColor(kRed);
	t4->DrawLatex(0.87, 0.70, "Red: BAD");
	t5->SetTextColor(kBlack);
	t5->DrawLatex(0.87, 0.64, "White: absent");
	t2->DrawLatex(0.9, 0.2, part[i]);
	
	c->Print(TString(h2->GetName()) + "." + img_suffix);
	setStyle(); //reset to default style
	
      } 
}

void drawNormP2Map(const char * img_suffix){
  const char* names[] = { "EEprof2_nZ_p2Norm_Mean", "EBprof2_p2Norm_Mean", "EEprof2_pZ_p2Norm_Mean","EEprof2_nZ_p2Norm_Median", "EBprof2_p2Norm_Median", "EEprof2_pZ_p2Norm_Median"};
 
  
  const char* part[]  = { "EE-", "EB", "EE+" };
  TCanvas* c = new TCanvas("prof", "prof", 800, 400);
  c->SetLogz();
  c->SetLeftMargin(0.07);
  for(int i = 0; i < sizeof(names)/sizeof(names[0]); ++i){
    cout << names[i] << endl;
    TProfile2D* h = (TProfile2D*) gDirectory->Get(names[i]);
    if(!h) continue;
    TH2* h2[2];
    h2[0] = (TH2*) h;
    h2[1]= h->ProjectionXY(TString(h->GetName()) + "_RMS", "C=E");
    char* type[] = {"Mean", "RMS"};
    for(int j = 0; j < 2; ++j){
      c->SetLogz(j);
      h2[j]->Draw("colz");

      c->Update();
      
      TPaletteAxis *palette = (TPaletteAxis*)h2[j]->GetListOfFunctions()->FindObject("palette");

      if(palette){
	palette->SetX2NDC(0.5*palette->GetX1NDC() + 0.5*palette->GetX2NDC());
	palette->SetY2NDC(0.3*palette->GetY1NDC() + 0.7*palette->GetY2NDC());
	//	palette->ConvertNDCtoPad();
	palette->Paint("NDC");
      } else{
	cerr << "Palette not found!" << endl;
      }
    
      h2[j]->GetXaxis()->SetTitle(i==1 ? "iphi" : "ix");
      h2[j]->GetYaxis()->SetTitle(i==1 ? "ieta" : "iy");

      if(j==0) autozoom((TH2F*)h2[j]);

      c->Paint();
      
      TLatex * t = new TLatex(0.87, 0.9, "Transparency");
      t->SetTextAlign(21);
      t->SetNDC();
      t->SetTextFont(42);
      t->SetTextSize(0.06);
      t->Draw();
      TLatex* t2 = (TLatex*) t->Clone();
      t2->SetTextColor(kRed+2);
      t->DrawLatex(0.87, 0.84, "normalized to the");
      t->DrawLatex(0.87, 0.78, "eta-ring average");
      t->DrawLatex(0.87, 0.69, type[j]);
      t2->DrawLatex(0.9, 0.2, part[i]);
      c->Print(TString(h2[j]->GetName()) + "." + img_suffix);
    }
  } 
}

void drawH2Maps(const char * img_suffix)
{
  const char* names[] = 
    {
      "EEh2_nZ_nan", "EBh2_nan", "EEh2_pZ_nan",
      "EEh2_nZ_infp", "EBh2_infp", "EEh2_pZ_infp",
      "EEh2_nZ_infn", "EBh2_infn", "EEh2_pZ_infn",
      "EEh2_nZ_max", "EBh2_max", "EEh2_pZ_max",
      "EEh2_nZ_min", "EBh2_min", "EEh2_pZ_min"
    };
  const char * title[] = 
    {
      "Problematic channels",
      "Transparency"
    };
  const char * type[] = 
    {
      "nan", "positive inf", "negative inf",
      "maximum", "minimum"
    };
  const char * descr[] = {"z: # of occurr.", ""};
  const char* part[]  = { "EE-", "EB", "EE+" };
  TCanvas* c = new TCanvas("h2", "h2", 800, 400);
  c->SetLogz();
  c->SetLeftMargin(0.07);
  c->cd();
  for(int i = 0; i < sizeof(names)/sizeof(names[0]); ++i) {
          cout << names[i] << endl;
          TH2D* h = (TH2D*) gDirectory->Get(names[i]);
          if(!h) continue;
          h->Draw("colz");

          if (i >= 9) c->SetLogz(0);

          c->Update();

          TPaletteAxis *palette = (TPaletteAxis*)h->GetListOfFunctions()->FindObject("palette");

          if(palette){
                  palette->SetX2NDC(0.5*palette->GetX1NDC() + 0.5*palette->GetX2NDC());
                  palette->SetY2NDC(0.3*palette->GetY1NDC() + 0.7*palette->GetY2NDC());
                  //	palette->ConvertNDCtoPad();
                  palette->Paint("NDC");
          } else{
                  cerr << "Palette not found!" << endl;
          }

          h->GetXaxis()->SetTitle(i==1 ? "iphi" : "ix");
          h->GetYaxis()->SetTitle(i==1 ? "ieta" : "iy");

          autozoom((TH2F*)h, 0.02);

          c->Paint();

          TLatex * t = new TLatex(0.87, 0.9, title[i/9]);
          t->SetTextAlign(21);
          t->SetNDC();
          t->SetTextFont(42);
          t->SetTextSize(0.06);
          //t->DrawLatex(0.87, 0.84, "normalized to the");
          //t->DrawLatex(0.87, 0.78, "eta-ring average");
          //t->DrawLatex(0.87, 0.69, type[i/3]);
          t->DrawLatex(0.87, 0.84, type[i/3]);
          t->DrawLatex(0.87, 0.78, descr[i/9]);
          t->Draw();
          TLatex* t2 = (TLatex*) t->Clone();
          t2->SetTextColor(kRed+2);
          t2->DrawLatex(0.9, 0.2, part[i%3]);
          c->Print(TString(h->GetName()) + "." + img_suffix);
  }
}

 int all(const char * filename, const char * img_suffix)
{

  setStyle();

  TDirectory * fin = 0;
  if(filename){
    printf("opening file %s\n", filename);
    TFile* fin_ = TFile::Open(filename);
    if (!fin_ || fin_->IsZombie()) {
      return 0;
    }
    fin = fin_;
  } else{
    fin = gDirectory;
  }
  
  drawNormP2Hist(img_suffix);

  drawNormP2Map(img_suffix);

  drawH2Maps(img_suffix);

  drawMichaelPlots(img_suffix);

  TCanvas * c = new TCanvas("history", "history", 800, 400);
  
  gplot(fin, "history_p2_All", "All ECAL", img_suffix);
  gplot(fin, "history_p2_EE-", "EE-", img_suffix);
  gplot(fin, "history_p2_EB-", "EB-", img_suffix);
  gplot(fin, "history_p2_EB+", "EB+", img_suffix);
  gplot(fin, "history_p2_EE+", "EE+", img_suffix);
  
    
    char buf1[256];
    char buf2[256];
    for(int i=1; i <= 92; ++i){
      sprintf(buf1, "LM%02d", i);
      sprintf(buf2, "history_p2_%s", buf1);
      gplot(fin, buf2, buf1, img_suffix);
    }
    
    const int netabins = 20;
    const float deta = 2*2.964 / netabins;
    for(int i=1; i <= 20; ++i){
      float etamin = -2.964 + (i-1) * deta;
      float etamax = etamin + deta;
      etamin = TMath::Nint(etamin  * 100) / 100.;
      etamax = TMath::Nint(etamax  * 100) / 100.;
      sprintf(buf1, "%.2g < eta < %.2g", etamin, etamax);
      sprintf(buf2, "history_p2_eta%02d", i);
      gplot(fin, buf2, buf1, img_suffix);
    }


    TProfile2D * pm = (TProfile2D*)gDirectory->Get("EBprof2_p2_week");
    TIter next(gDirectory->GetListOfKeys());
    int n, t;
    while(1)
      {
	TObject * o = next();
	if (o == 0) break;
	if ((n = sscanf(o->GetName(), "EBprof2_p2_week_%d", &t)) == 1) {
	  printf("%s\n", o->GetName());
	  TProfile2D * p = (TProfile2D*)gDirectory->Get(o->GetName());
	  autozoom((TH2F*)p);
	  p->Draw("colz");
	  gPad->Print((std::string(o->GetName()) + "." + img_suffix).c_str());
	}
      }

    
    /*******************copied from above for slope histories**************************/
    /*  
    mplot(fin, "history_p2_p1_All", "All ECAL", img_suffix);
    mplot(fin, "history_p2_p1_EE-", "EE-", img_suffix);
    mplot(fin, "history_p2_p1_EB-", "EB-", img_suffix);
    mplot(fin, "history_p2_p1_EB+", "EB+", img_suffix);
    mplot(fin, "history_p2_p1_EE+", "EE+", img_suffix);
    
    
    char buf1[256];
    char buf2[256];
    for(int i=1; i <= 92; ++i){
      sprintf(buf1, "LM%02d", i);
      sprintf(buf2, "history_p2_p1_%s", buf1);
      mplot(fin, buf2, buf1, img_suffix);
    }
    
    const int netabins = 20;
    const float deta = 2*2.964 / netabins;
    for(int i=1; i <= 20; ++i){
      float etamin = -2.964 + (i-1) * deta;
      float etamax = etamin + deta;
      etamin = TMath::Nint(etamin  * 100) / 100.;
      etamax = TMath::Nint(etamax  * 100) / 100.;
      sprintf(buf1, "%.2g < eta < %.2g", etamin, etamax);
      sprintf(buf2, "history_p2_p1_eta%02d", i);
      mplot(fin, buf2, buf1, img_suffix);
    }

    mplot(fin, "history_p3_p2_All", "All ECAL", img_suffix);
    mplot(fin, "history_p3_p2_EE-", "EE-", img_suffix);
    mplot(fin, "history_p3_p2_EB-", "EB-", img_suffix);
    mplot(fin, "history_p3_p2_EB+", "EB+", img_suffix);
    mplot(fin, "history_p3_p2_EE+", "EE+", img_suffix);
    
    
    char buf1[256];
    char buf2[256];
    for(int i=1; i <= 92; ++i){
      sprintf(buf1, "LM%02d", i);
      sprintf(buf2, "history_p3_p2_%s", buf1);
      mplot(fin, buf2, buf1, img_suffix);
    }
    
    const int netabins = 20;
    const float deta = 2*2.964 / netabins;
    for(int i=1; i <= 20; ++i){
      float etamin = -2.964 + (i-1) * deta;
      float etamax = etamin + deta;
      etamin = TMath::Nint(etamin  * 100) / 100.;
      etamax = TMath::Nint(etamax  * 100) / 100.;
      sprintf(buf1, "%.2g < eta < %.2g", etamin, etamax);
      sprintf(buf2, "history_p3_p2_eta%02d", i);
      mplot(fin, buf2, buf1, img_suffix);
    }
    */
    /************************end copy for slope histories***************/
    //         gPad->Print("anim.gif++");

}

