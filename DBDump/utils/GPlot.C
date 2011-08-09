/** Usage from ROOT: .L GPlot.C+
    GPlot p
    p.draw(myhist)

    Usage from PyROOT:

    from ROOT import *
    gSystem.Load("GPlot")
    gSystem.Load("GPlot_C")
    from ROOT import GPlot
    GPlot p
    p.draw(myhist)

    Author: Ph. Gras. 
*/


#include "stdio.h"

#include <iostream>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>


#include "TString.h"
#include "TObjArray.h"
#include "TObjString.h"
#include "TH1.h"
#include "TAxis.h"
#include "TStyle.h"
#include "TGraph.h"
#include "TGraphErrors.h"
#include "TGraphAsymmErrors.h"

using namespace std;

class GPlot{

  FILE* p;
  FILE* log;
  FILE* fbuf_cmd;
  FILE* fbuf_data;

  enum { CMD = 1, DATA };
  int save_;

  void fcopy(FILE* fin, FILE* fout){
    rewind(fin);
    int c;
    while((c = fgetc(fin)) >= 0/* && !feof(fin)*/){
      fputc(c, fout);
    }
    fflush(fout);
  }



public:
  GPlot(): save_(false){
    p = popen("gnuplot", "w");
    const char* h = getenv("HOME");
    if(h){
      //truncate log file:
      int rc = system("tail -n 1000 ~/.gplot_hist > ~/.#gplot_hist# && mv  ~/.#gplot_hist# ~/.gplot_hist");
      ++rc; //make both gcc and CLic happy

      log = fopen(TString(h) + "/.gplot_hist", "a");

      // fbuf_cmd = tmpfile();
      // fbuf_data = tmpfile();

      fbuf_cmd = fopen("fbuf_cmd", "w+");
      fbuf_data = fopen("fbuf_data", "w+");

    }
  }

  ~GPlot(){
    if(log) fclose(log);
    if(p) pclose(p);
    if(fbuf_cmd) fclose(fbuf_cmd);
    if(fbuf_data) fclose(fbuf_data);
  }

  void sendn(const char* line = ""){
    send("%s\n", line);
    fflush(p);
  }

  bool hasErr(TH1* h){
    return h->GetSumw2N() ? true: false;
  }

  bool hasErr(TGraph* g){
    return false;
  }
  
  bool hasErr(TGraphErrors* g){
    return true;
  }

  bool hasErr(TGraphAsymmErrors* g){
    return true;
  }
  
  void send(const char* fmt, ...){
    va_list ap;
    va_start(ap, fmt);
    if(log){
      vfprintf(log, fmt, ap);
      fflush(log); }
    if(save_==CMD) { vfprintf(fbuf_cmd, fmt, ap); fflush(fbuf_cmd); }
    if(save_==DATA) { vfprintf(fbuf_data, fmt, ap); fflush(fbuf_data); }
    vfprintf(p, fmt, ap);
    va_end(ap);
  }

  void replot(){
    send("\nplot ");
    if(log){ fcopy(fbuf_cmd, log); }
    fcopy(fbuf_cmd, p);
    send("\n");
    if(log) { fcopy(fbuf_data, log); fflush(log); }
    fcopy(fbuf_data, p);
  }

  void saveAs(const char* filename){
    TObjArray* toks = TString(filename).Tokenize(".");
    TString ext = ((TObjString*)toks->Last())->GetString();
    cout << "File format: " << ext << endl;

    if(!ext.CompareTo("gpi")){
      FILE* f = fopen(filename, "w");

      if(!f){
        cerr << "Failed to write to file " << filename << ". " << endl;
      } else{
        fputs("#!/usr/bin/gnuplot\n", f);
        fputs("plot ", f);
        fcopy(fbuf_cmd, f);
        fputs("\n", f);
        fcopy(fbuf_data, f);
        fputs("pause -1", f);
        //adds execution write to the file:
        mode_t mask = umask( 0 );
        umask(mask);
        mask = 0777 & ~mask;
        cout << "mask: " <<  mask << endl;
        fchmod(fileno(f), mask);
        fclose(f);
      }
    } else{
      if(!ext.CompareTo("png")) sendn("set term png");
      else if(!ext.CompareTo("eps")) sendn("set term corel");
      else if(!ext.CompareTo("ps"))  sendn("set term postscript");
      else if(!ext.CompareTo("html")) sendn("set term canvas");
      else if(!ext.CompareTo("svg")) sendn("set term svg");
      else { cerr << "Format not supported." << endl ; return; }
      sendn(TString("set output '") + filename + "'");
      replot();
      sendn("set term wxt");
      sendn("set output");
    }
  }


  struct DrawOpt{
    bool same;
    bool witherr;
    bool asymerr;
    bool errorBand;
    bool step;
    bool rot;
    bool hist;
    bool line;
    bool spline;
    bool point;
  } opt_;

  template<class T>
  void setOpt(const char* option, T* h){
    TString opt(option);
    opt.ToLower();
    opt_.same = false;
    opt_.witherr = false;
    if(opt.Contains("same")) opt_.same = true;
    opt.ReplaceAll("same", "");
    opt.ReplaceAll("text", "");
    opt.ReplaceAll("lego", "");
    if(opt.Contains("e")) opt_.witherr = true;
    opt_.errorBand = false;
    if((opt.Contains("e3") || opt.Contains("e4"))
       && h->InheritsFrom("TGraph")){
      opt_.errorBand = true;
    }
    if(hasErr(h))    opt_.witherr = true;
    if(opt.Contains("hist")) opt_.witherr = false;
    opt.ReplaceAll("hist", "");
    opt_.step = false;
    if(!opt_.witherr) opt_.step = true;
    if(opt.Contains("h")) opt_.step = true;
    opt_.hist = false;
    opt_.rot = false;
    if(opt.Contains("hbar")) {opt_.rot = true; opt_.hist = true; opt_.step = false;}
    opt.ReplaceAll("hbar", "");
    if(opt.Contains("bar")) {opt_.hist = true; opt_.step = false; }
    opt_.line = false;
    if(opt.Contains("l") && !opt_.witherr) { opt_.line = true; }
    opt_.spline = false;
    if(opt.Contains("c") && !opt_.witherr) { opt_.spline = true; }
    opt_.point = false;
    if(opt.Contains("p")) opt_.point = true;
  }

  void sendData(TH1* h){
      TAxis* ax = h->GetXaxis();
      for(int i = 1; i <= h->GetXaxis()->GetNbins(); ++i){
      save_ = DATA;
      send("%.8g\t%.8g", ax->GetBinCenter(i), h->GetBinContent(i));
      if(opt_.witherr) send("\t%.8g\n", h->GetBinError(i));
      else send("\n");
    }
  }

  void sendData(TGraphAsymmErrors* h){
    int n = h->GetN();
    Double_t* x = h->GetX();
    Double_t* y = h->GetY();
    Double_t* eyl = h->GetEYlow();
    Double_t* eyh = h->GetEYhigh();
    for(int i = 0; i < n; ++i){
      save_ = DATA;
      send("%.8g\t%.8g\t%.8g\t%.8g\n", x[i], y[i], y[i] - eyl[i], y[i] + eyh[i]);
    }
  }

  void sendData(TGraphErrors* h){
    int n = h->GetN();
    Double_t* x = h->GetX();
    Double_t* y = h->GetY();
    Double_t* ey = h->GetEY();
    for(int i = 1; i <= n; ++i){
      save_ = DATA;
      send("%.8g\t%.8g\t%.8g\t%.8g\n", x[i], y[i], ey[i]);
    }
  }

  void sendData(TGraph* h){
    int n = h->GetN();
    Double_t* x = h->GetX();
    Double_t* y = h->GetY();
    for(int i = 1; i <= n; ++i){
      save_ = DATA;
      send("%.8g\t%.8g\t%.8g\n", x[i], y[i]);
    }
  }

  template<class T>
  void draw(T* h, const char* option = ""){
    save_ = 0;

    setOpt(option, h);

    if(!opt_.same) {int r = ftruncate(fileno(fbuf_cmd), 0); r = ftruncate(fileno(fbuf_data), 0); ++r; }

    //TODO: support of 'same' option. For that it is needed to store draw command
    //either in gnuplot format or in ROOT format (histo + draw options)
    if(true/*h->GetDimension()==1*/){
      TAxis* ax = h->GetXaxis();
      if(opt_.same){
        send("plot ");
        fcopy(fbuf_cmd, p);
        if(log) fcopy(fbuf_cmd, log);
        fflush(log);
        save_ = CMD;
        send(", \\\n");
      } else{
        send("\nplot [%f:%f]", ax->GetBinLowEdge(ax->GetFirst()), ax->GetBinUpEdge(ax->GetLast()));
      }
      save_ = CMD;
      send(" \"-\"");

      if(opt_.errorBand) send(" using 1:3:4");
      else if(opt_.rot && opt_.witherr && opt_.asymerr) send(" using 2:1:3:4");
      else if(opt_.rot && opt_.witherr) send(" using 2:1:3");
      else if(opt_.rot) send(" using 2:1");
      
      send(" title '%s'", h->GetTitle());

      send(" with");
      if(opt_.errorBand) send(" filledcurves");
      else if(opt_.step) send(" histeps");
      else if(opt_.hist && opt_.witherr) send(" boxerrorbars");
      else if(opt_.hist) send(" boxes");
      else if(opt_.line && opt_.witherr) send(" yerrorlines");
      else if(opt_.witherr) send(" yerrorbars");
      else if(opt_.point && opt_.line) send(" linespoints");
      else if(opt_.point) send(" points");
      //TODO: point + error bars: required two plot commands

      save_ = 0;
      sendn("");
      if(opt_.same){ fcopy(fbuf_data, p); if(log) fcopy(fbuf_cmd, log); }

      sendData(h);

      sendn("e");
      save_ = 0;
    } else{
      cerr << "Online 1D histgram is supported yet!" << endl;
    }
  }
};

template
void GPlot::draw(TH1* h, const char* option = "");

template
void GPlot::draw(TGraphAsymmErrors* h, const char* option = "");
