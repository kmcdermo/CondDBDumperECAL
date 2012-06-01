#include "TAxis.h"
#include "TMath.h"
#include "TH1C.h"

TH1* xtime(TGraph* g, int ndivs = 5){
//void xtime2(TAxis* a, int ndivs = 5){

   double tmin;
   double tmax;
   double ymin;
   double ymax;
   g->ComputeRange(tmin, ymin, tmax, ymax);

   //  double tmin = a->GetBinLowEdge(1);
   // double tmax = a->GetBinUpEdge(a->GetNbins());
   
  double dt = tmax - tmin;
  if(dt<0) dt = -dt;
  int div_width  =  (dt +.5) / ndivs;

  enum { f_day, f_hour, f_min, f_sec};
  const char* time_format[] =  { //"%d/%m",
				 "#splitline{%d/%m}{%H:%M}", 
				 "#splitline{%d/%m}{%H:%M}", 
				 "#splitline{%d/%m}{%H:%M}",
				 "#splitline{%d/%m}{%H:%M:%S}",
  };

  enum { day = 24*3600, hour = 3600, min = 60, sec = 1};

  int div_widths[] = { day, 12*hour, 8*hour, 6*hour, 4*hour, 3*hour, 2*hour,
		       hour, 30*min, 20*min, 10*min,  5*min,  4*min,  3*min, 2*min, 
		       min,  30*sec, 20*sec, 10*sec,  5*sec,  4*sec,  3*sec, 2*sec
  };

  int formats[]    = { f_day, f_hour, f_hour, f_hour, f_hour, f_hour, f_hour, f_hour,
		       f_hour, f_min, f_min, f_min, f_min, f_min, f_min, f_min,
		       f_min, f_sec, f_sec, f_sec, f_sec, f_sec, f_sec, f_sec
  };

  int sub_divs[]  = {    6,      6,       4,      6,      4,      6,     4,
			 6,      6,       4,      5,      5,      4,     6,    4,
			 6,      6,       4,      5,      5,      4,     6,    4
  };

  const unsigned n_div_widths = sizeof(div_widths)/sizeof(div_widths[0]);

  int div_sel = 0;
  int err     = 99999;
  for(unsigned idiv = 0; idiv < n_div_widths; ++idiv){
    if(TMath::Abs(div_widths[idiv]-div_width) < err){
      err = TMath::Abs(div_widths[idiv]-div_width);
      div_sel = idiv;
    }
  }

  const char* format = time_format[formats[div_sel]];
  div_width = div_widths[div_sel];
  int nsubdivs = sub_divs[div_sel];

  //adjust bounds:
  //  int subdiv_width = div_width / nsubdivs;
  tmin = int(tmin / div_width) * div_width;
  tmax = int(tmax / div_width + 0.999) * div_width;

  //adjust ndivs:
  ndivs = ((tmax-tmin) / div_width);

  //calculates number of bins
  double dtmin = tmax - tmin;
  for(int i = 0; i < g->GetN()-1; ++i){
    double dt_ = TMath::Abs(g->GetX()[i+1]-g->GetX()[i]);
    if( dt_ < dtmin) dtmin = dt_;
  }

  int nbins = (tmax-tmin) / int(dtmin);
  if(nbins < 360) nbins = 360;
  nbins = (nbins / (ndivs*nsubdivs) + 1) (ndivs*nsubdivs);

#if 0
  cout << "ndivs = " << ndivs << "\tnsubdivs = " << nsubdivs 
       << "\tnbins = " << nbins
       << "\ttmin = " << int(tmin) << "\ttmax = " << int(tmax) << endl;
#endif

  TH1* ha = new TH1C(TString(g->GetName()) + "_axes", g->GetName(), nbins, tmin, tmax);
  ha->SetMinimum(ymin - 0.1 *(ymax-ymin));
  ha->SetMaximum(ymax + 0.1 *(ymax-ymin));

  TAxis* a = ha->GetXaxis();
  a->SetRangeUser(tmin, tmax);
  a->SetTimeDisplay(1);
  a->SetNdivisions(-(ndivs + nsubdivs * 100));
  //ROOT seems to not support time offset prior to 1970-01-01 01:00:00 UTC
  //As the year is not represented, shift the time by 4 years, might be tricky
  //for the leap years.
  a->SetTimeFormat(TString(format) + "%F1974-01-01 00:00:00 GMT");
  return ha;
}
