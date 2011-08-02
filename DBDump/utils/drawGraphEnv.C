//#include "TSpline.h"
#include "TGraphAsymmErrors.h"
#include "TGraph.h"
#include "TH1.h"
#include "TVirtualPad.h"

//Draw an error envelop from a TGraphAsymmErrors
void drawGraphEnv(TGraphAsymmErrors* g, float scale = 1, bool doAxis = false,
                  int linestyle = 1, int linewidth = 2, int linecolor = 4,
                  bool fill = false,
                  TH1** ppGl = 0, TH1** ppGh = 0){
  //                  const char* opth = "", const char* optl = ""){
  char buffer[256];
  int n = g->GetN();
  double* x = new double[n+2];
  double* yh = new double[n+2];
  double* yl = new double[n+2];

  for(int i = 0; i < n; ++i){
    x[i+1]  = g->GetX()[i];
    yh[i+1] = g->GetY()[i] + g->GetEYhigh()[i] * scale;
    yl[i+1] = g->GetY()[i] - g->GetEYlow()[i] * scale;
  }

  x[0] = g->GetX()[0] - g->GetEXlow()[0];
  yl[0] = yl[1] - (yl[2]-yl[1])/(x[2]-x[1])*g->GetEXlow()[0];
  yh[0] = yh[1] - (yh[2]-yh[1])/(x[2]-x[1])*g->GetEXlow()[0];

  x[n+1] = g->GetX()[n-1] + g->GetEXhigh()[n-1];
  yl[n+1] = yl[n] + (yl[n]-yl[n-1])/(x[n]-x[n-1])*g->GetEXhigh()[n-1];
  yh[n+1] = yh[n] + (yh[n]-yh[n-1])/(x[n]-x[n-1])*g->GetEXhigh()[n-1];

  TGraph* gl = new TGraph(n+2, x, yl);
  gl->SetTitle(g->GetTitle());
  TGraph* gh = new TGraph(n+2, x, yh);
  gh->SetTitle(g->GetTitle());
  const char* opt = "l,same";
  
  sprintf(buffer, "%s_eh", g->GetName());
  gh->SetName(buffer);
  
  sprintf(buffer, "%s_el", g->GetName());
  gl->SetName(buffer);

  if(doAxis){
    g->Draw("AP");
    TH1* h = g->GetHistogram();
    gPad->Clear();
    h->Draw();
  }

  if(fill){
    gl->SetFillStyle(linestyle);
    gl->SetFillColor(linecolor);
    gl->SetLineColor(linecolor);
  
    gh->SetFillStyle(linestyle);
    gh->SetFillColor(linecolor);
    gh->SetLineColor(linecolor);
  } else{
    gl->SetLineStyle(linestyle);
    gl->SetLineWidth(linewidth);
    gl->SetLineColor(linecolor);
    
    gh->SetLineStyle(linestyle);
    gh->SetLineWidth(linewidth);
    gh->SetLineColor(linecolor);
  }
  
  gh->Draw(opt);
  gl->Draw(opt);

  if(ppGh) *ppGh = gh;
  if(ppGl) *ppGl = gl;
}
