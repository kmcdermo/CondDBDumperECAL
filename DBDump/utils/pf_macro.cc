//#include "TAxis.h"
//#include "TFile.h"
//#include "TGraphAsymmErrors.h"

#include "autozoom.C"
#include "drawGraphEnv.C"
#include "tdrstyle.C"
#include "TMath.h"

void pf_macro(char * filename = "out_plot_GR_R_42_V19::All_EcalLaserAPDPNRatios_v3_online.root"){
  all(filename);
}

template<class T>
void draw(T* h, const char* opt = "")
{
    gStyle->SetHatchesLineWidth(2.);

    h->Draw(opt);
    return;
    //fix style for graph:
    TAxis * a[2];
    a[0] = h->GetXaxis();
    a[1] = h->GetYaxis();
    for(unsigned i = 0; i < sizeof(a)/sizeof(a[0]); ++i){
        a[i]->SetTitleFont(gStyle->GetTitleFont());
        a[i]->SetTitleSize(gStyle->GetTitleFontSize());
        a[i]->SetTitleColor(gStyle->GetTitleColor());
        a[i]->SetLabelFont(gStyle->GetLabelFont());
        a[i]->SetLabelSize(gStyle->GetLabelSize());
        a[i]->SetLabelColor(gStyle->GetLabelColor());
    }
    h->GetXaxis()->SetTitleOffset(gStyle->GetTitleXOffset());
    h->GetYaxis()->SetTitleOffset(gStyle->GetTitleYOffset());
}

void gplot(TFile * f, char * gname, char * title = 0)
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
        //draw(g, i == 0 ? "ape3" : "e3");
        if (i != 3) draw(g, i == 0 ? "ape3" : "e3");
        else {
            drawGraphEnv(g, 0.1, false, 7, 2, 1, false, &gg);
        }
        if (i == 3) draw(g, "xl");
        g->GetXaxis()->SetTimeDisplay(1);
        g->GetXaxis()->SetTitle("time");
        g->GetYaxis()->SetTitle("transparency change");
        //g->GetYaxis()->SetRangeUser(0.5, 1.15);
        gPad->SetTicks();
        //if (title) g->SetTitle(title);
        if (i == 0) draw(g, i == 0 ? "ape3" : "e3");
        if (i == 0) l->AddEntry(g, "median", "l");
        l->AddEntry(i == 3 ? gg : g, nleg[i], i == 3 ? "l" : "f");
    }
    l->SetFillStyle(0);
    l->SetBorderSize(0);
    l->SetTextFont(42);
    l->SetTextSize(0.0475);
    l->Draw();
    t->Draw();
    gPad->Print((std::string(gname) + ".png").c_str());
}

int all(char * filename = "out_plot_GR_R_42_V19::All_EcalLaserAPDPNRatios_v3_online.root")
{
    gROOT->SetStyle("Plain");
    gStyle->SetPalette(1);
    gStyle->SetTitleFont(42, "XYZ");
    gStyle->SetTitleSize(0.055, "XYZ");
    gStyle->SetTitleOffset(.75, "Y");
    gStyle->SetLabelFont(42, "XYZ");
    gStyle->SetLabelSize(0.055, "XYZ");

    gStyle->SetTimeOffset(0);

    gStyle->SetPadTopMargin(0.05);
    gStyle->SetPadBottomMargin(0.13);
    gStyle->SetPadLeftMargin(0.10);
    gStyle->SetPadRightMargin(0.26);

    //gStyle->SetOptTitle(1);

    printf("opening file %s\n", filename);
    TFile * fin = TFile::Open(filename);
    TCanvas * c = new TCanvas("history", "history", 800, 400);
    if (!fin || fin->IsZombie()) {
        return 0;
    }
    gplot(fin, "history_p2_All", "All ECAL");
    gplot(fin, "history_p2_EE-", "EE-");
    gplot(fin, "history_p2_EB-", "EB-");
    gplot(fin, "history_p2_EB+", "EB+");
    gplot(fin, "history_p2_EE+", "EE+");


    char buf1[256];
    char buf2[256];
    for(int i=1; i <= 92; ++i){
      sprintf(buf1, "LM%02d", i);
      sprintf(buf2, "history_p2_%s", buf1);
      gplot(fin, buf2, buf1);
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
      gplot(fin, buf2, buf1);
    }

    TProfile2D * pm = (TProfile2D*)gDirectory->Get("EBprof2_transpCorr_week");
    TIter next(gDirectory->GetListOfKeys());
    int n, t;
    while(1) {
        TObject * o = next();
        if (o == 0) break;
        if ((n = sscanf(o->GetName(), "EBprof2_transpCorr_week_%d", &t)) == 1) {
            printf("%s\n", o->GetName());
            TProfile2D * p = (TProfile2D*)gDirectory->Get(o->GetName());
            autozoom((TH2F*)p);
            p->Draw("colz");
            gPad->Print((std::string(o->GetName()) + ".png").c_str());
        }
    }
}

