reset

#%PNG% set term pngcairo enhanced font "Helvetica,12"

dir = "%DIR%"
#dir = "/home/ferri/work/laser/2012_troubles/gp_out_dump_193091_193186"

#%PNG% set out dir . "/distr_eta_normalised_mean_p2.png"

set xlabel "transp / <transp>_{/Symbol h}"
set ylabel "entries (channels x IOV)"

#%PNG% set rmargin 5
#%PNG% set label "{/=9 %NOW%}" at screen .975, screen .65 rotate by 90

set grid y
set log y
set mytics 10
set mxtics  5
nbin = 2000; xmin = 0; xmax = 2
set title "{/=16 transparency normalised to the {/Symbol h}-ring average}"
p dir . "/distr_eta_normalised_mean_p2.dat" u 1:($2 - (xmax - xmin) / 2 / nbin) not w step lt 3
set mytics
set mxtics
unset grid
unset log
unset ymtics


#%PNG% set out dir . "/distr_p2.png"

set xlabel "transp / <transp>_{/Symbol h}"
set ylabel "entries (channels x IOV)"

#%PNG% set rmargin 5
#%PNG% set label "{/=9 %NOW%}" at screen .975, screen .65 rotate by 90

set grid y
set log y
set mytics 10
set mxtics  5
nbin = 2000; xmin = 0; xmax = 2
set title "{/=16 transparency (not normalized)}"
p dir . "/distr_p2.dat" u 1:($2 - (xmax - xmin) / 2 / nbin) not w step lt 3
set mytics
set mxtics
unset grid
unset log
unset ymtics

set xlabel "transp / <transp>_{/Symbol h}"
set ylabel "entries (channels x IOV)"

#%PNG% set rmargin 5
#%PNG% set label "{/=9 %NOW%}" at screen .975, screen .65 rotate by 90

set grid y
set log y
set mytics 10
set mxtics  5
nbin = 2000; xmin = 0; xmax = 2
set title "{/=16 transparency of p2}"
p dir . "/distr_p2.dat" u 1:($2 - (xmax - xmin) / 2 / nbin) not w step lt 3
set mytics
set mxtics
unset grid
unset log
unset ymtics


#%PNG% set out dir . "/distr_eta_normalised_median_p2.png"
set xlabel "transp / <transp>_{/Symbol h}"
set ylabel "entries (channels x IOV)"

#%PNG% set rmargin 5
#%PNG% set label "{/=9 %NOW%}" at screen .975, screen .65 rotate by 90

set grid y
set log y
set mytics 10
set mxtics  5
nbin = 2000; xmin = 0; xmax = 2
set title "{/=16 transparency normalised to the {/Symbol h}-ring median}"
p dir . "/distr_eta_normalised_median_p2.dat" u 1:($2 - (xmax - xmin) / 2 / nbin) not w step lt 3
set mytics
set mxtics
unset grid
unset log
unset ymtics

#pause -1 "Hit..."

unset key


#%PNG% set out dir . "/slope_eta_p3_p2_ncut.png"
set xlabel "iEta"
set ylabel "Slope (p3-p2) in 1/hour" 

#%PNG% set rmargin 5
#%PNG% set label "{/=9 %NOW%}" at screen .975, screen .65 rotate by 90

set grid y
set mytics 10
set mxtics  5
nbin = 250; xmin = -2.75; xmax = 2.75
set title "{/=16 Negative Slope (p3-p2) of Transparency Values vs iEta}"
p dir . "/etaProf_p3_p2_ncut_x.dat" u 1:2:4 with yerrorbars
set mytics
set mxtics
unset grid
unset ymtics


#%PNG% set out dir . "/slope_eta_p3_p2_pcut.png"

set xlabel "iEta"
set ylabel "Slope (p3-p2) in 1/hour" 

#%PNG% set rmargin 5
#%PNG% set label "{/=9 %NOW%}" at screen .975, screen .65 rotate by 90

set grid y
set mytics 10
set mxtics  5
nbin = 250; xmin = -2.75; xmax = 2.75
set title "{/=16 Positive Slope (p3-p2) of Transparency Values vs iEta}"
p dir . "/etaProf_p3_p2_pcut_x.dat" u 1:2:4 with yerrorbars
set mytics
set mxtics
unset grid
unset ymtics


#%PNG% set out dir . "/slope_eta_p2_p1_pcut.png"
set xlabel "iEta"
set ylabel "Slope (p2-p1) in 1/hour" 

#%PNG% set rmargin 5
#%PNG% set label "{/=9 %NOW%}" at screen .975, screen .65 rotate by 90

set grid y
set mytics 10
set mxtics  5
nbin = 250; xmin = -2.75; xmax = 2.75
set title "{/=16 Positive Slope (p2-p1) of Transparency Values vs iEta}"
p dir . "/etaProf_p2_p1_pcut_x.dat" u 1:2:4 with yerrorbars 
set mytics
set mxtics
unset grid
unset ymtics


#%PNG% set out dir . "/slope_eta_p2_p1_ncut.png"
set xlabel "iEta"
set ylabel "Slope (p2-p1) in 1/hour" 

#%PNG% set rmargin 5
#%PNG% set label "{/=9 %NOW%}" at screen .975, screen .65 rotate by 90

set grid y
set mytics 10
set mxtics  5
nbin = 250; xmin = -2.75; xmax = 2.75
set title "{/=16 Negative Slope (p2-p1) of Transparency Values vs iEta}"
p dir . "/etaProf_p2_p1_ncut_x.dat" u 1:2:4 with yerrorbars
set mytics
set mxtics
unset grid
unset ymtics


#%PNG% set out dir . "/slope_eta_p3_p2_cut.png"
set xlabel "iEta"
set ylabel "Slope (p3-p2) in 1/hour" 

#%PNG% set rmargin 5
#%PNG% set label "{/=9 %NOW%}" at screen .975, screen .65 rotate by 90

set grid y
set mytics 10
set mxtics  5
nbin = 250; xmin = -2.75; xmax = 2.75
set title "{/=16 Slope (p3-p2) of Transparency Values vs iEta}"
p dir . "/etaProf_p3_p2_cut_x.dat" u 1:2:4 with yerrorbars
set mytics
set mxtics
unset grid
unset ymtics


#%PNG% set out dir . "/slope_eta_p2_p1_cut.png"
set xlabel "iEta"
set ylabel "Slope (p2-p1) in 1/hour" 

#%PNG% set rmargin 5
#%PNG% set label "{/=9 %NOW%}" at screen .975, screen .65 rotate by 90

set grid y
set mytics 10
set mxtics  5
nbin = 250; xmin = -2.75; xmax = 2.75
set title "{/=16 Slope (p2-p1) of Transparency Values vs iEta}"
p dir . "/etaProf_p2_p1_cut_x.dat" u 1:2:4 with yerrorbars
set mytics
set mxtics
unset grid
unset ymtics




set auto fix
set pm3d 
#set palette model HSV functions .75 - .75 * gray, 1, 1

infile_ee_slope2     = dir . '/Slope2_EEh2_p3_p2_slope.dat'
infile_eb_slope2     = dir . '/Slope2_EBh2_p3_p2_slope.dat'
infile_bad_lm        = dir . '/channel_LM_bad_channel_map_LM.dat'
infile_bad_eta       = dir . '/channel_eta_bad_channel_map.dat'

infile_eb_max = dir . '/EBh2_max.dat'
infile_eb_min = dir . '/EBh2_min.dat'
infile_eb_min_max = dir . '/EBh2_min_max.dat'
infile_eb_average = dir . '/EBprof2_p2Norm_Mean.dat'
infile_eb_badchannel = dir . '/EBh2_BadChannel_Summary.dat'
infile_eb_nslope3    = dir . '/EBprof2_p3_p2_nslope.dat'
infile_eb_pslope3    = dir . '/EBprof2_p3_p2_pslope.dat'
infile_eb_propagation = dir . '/EBh2_propagation_map.dat'

infile_eem_max = dir . '/EEh2_nZ_max.dat'
infile_eem_min = dir . '/EEh2_nZ_min.dat'
infile_eem_min_max = dir . '/EEh2_nZ_min_max.dat'
infile_eem_average = dir . '/EEprof2_nZ_p2Norm_Mean.dat'
infile_eem_badchannel = dir . '/EEh2_nZ_BadChannel_Summary.dat'
infile_eem_nslope3    = dir . '/EEprof2_nZ_p3_p2_nslope.dat'
infile_eem_pslope3    = dir . '/EEprof2_nZ_p3_p2_pslope.dat'
infile_eem_propagation = dir . '/EEh2_nZ_propagation_map.dat'
                               
infile_eep_max = dir . '/EEh2_pZ_max.dat'
infile_eep_min = dir . '/EEh2_pZ_min.dat'
infile_eep_min_max = dir . '/EEh2_pZ_min_max.dat'
infile_eep_average = dir . '/EEprof2_pZ_p2Norm_Mean.dat'
infile_eep_badchannel = dir . '/EEh2_pZ_BadChannel_Summary.dat'
infile_eep_nslope3    = dir . '/EEprof2_pZ_p3_p2_nslope.dat'
infile_eep_pslope3    = dir . '/EEprof2_pZ_p3_p2_pslope.dat'
infile_eep_propagation = dir . '/EEh2_pZ_propagation_map.dat'

set ylabel "Slope (1/hour)"
set xlabel "P2 Transparency"



set palette model HSV functions .75 -.75 * gray, 1, 1
#%PNG% set out dir . "/Slope2_EBh2_p3_p2_slope.png"
#set title "EB Slope\n". infile_eb_slope2
set title "{/=16 EB Slope vs Tranparency}"
p [0.8:1.15][-.015:.015] infile_eb_slope2  u 1:2:($3 != 0 ? $3 : 1/0) not w p palette pt 5 ps .7



#%PNG% set out dir . "/Slope2_EEh2_p3_p2_slope.png"
#set title "EE Slope\n". infile_ee_slope2
set title "{/=16 EE Slope vs Tranparency}"
p [.4:1.15][-.015:.015] infile_ee_slope2  u 1:2:($3 != 0 ? $3 : 1/0) not w p palette pt 5 ps .7
#pause -1 "Hit return to continue"

set ylabel "LM Number"
set xlabel "IOV #"

#%PNG% set out dir . "/channel_LM_bad_channel_map_LM.png"
#set title "Number of Bad Channels as function of LM and time\n". infile_bad_lm
set title "{/=16 Bad Channels vs LM and time}"
p infile_bad_lm u 2:1:($3 !=0 ? $3 : 1/0) not w p palette pt 5 ps .7

set ylabel "i{/Symbol h}"

#%PNG% set out dir . "/channel_eta_bad_channel_map.png"
#set title "Ratio of Bad Channels (in {/Symbol h} range) as function of eta and time\n". infile_bad_eta  
set title "{/=16 Bad Channels vs eta and time}"
p infile_bad_eta  u 2:1:($3 !=0 ? $3 : 1/0) not w p palette pt 5 ps .7

unset cbrange
set xlabel "i{/Symbol f}"
set ylabel "i{/Symbol h}"

#%PNG% set out dir . "/EBh2_max.png"
#set title "EB min\n". infile_eb_min_max
set title "{/=16 EB min}"
set cbrange [.9:1.05]
p infile_eb_min_max  u 1:2:($3 != 0 ? $3 : 1/0) not w p palette pt 5 ps .7
set cbrange [*:*]
#pause -1 "Hit return to continue"

set xrange [0:360]
set yrange [-85:85]

#%PNG% set out dir . "/EBh2_min.png"
#set title "EB max\n". infile_eb_min_max
set title "{/=16 EB max}"
set cbrange [.9:1.05]
p infile_eb_min_max  u 1:2:($4 != 0 ? $4 : 1/0) not w p palette pt 5 ps .7
set cbrange [*:*]
#pause -1 "Hit return to continue"

#%PNG% set out dir . "/EBh2_max_min.png"
#set title "EB max - min\n". infile_eb_min_max
set title "{/=16 EB max - min}"
p infile_eb_min_max  u 1:2:($4 - $3 != 0 ? $4 - $3 : 1/0) not w p palette pt 5 ps .7
#pause -1 "Hit return to continue"

#%PNG% set out dir . "/EBprof2_p2Norm_Mean.png"
#set title "EB average\n". infile_eb_min_max
set title "{/=16 EB average}"
p infile_eb_average  u 1:2:($3 != 0 ? $3 : 1/0) not w p palette pt 5 ps .7
#pause -1 "Hit return to continue"

#%PNG% set out dir . "/EBprof2_p2Norm_Mean_RMS.png"
set title "{/=16 EB rms}"
p infile_eb_average  u 1:2:($7 != 0 ? $7 : 1/0) not w p palette pt 5 ps .7
#pause -1 "Hit return to continue"


#%PNG% set out dir . "/EBh2_BadChannel_Summary.png"
set title "{/=16 EB Bad Channel Summary}"
p [0:360][-85:85] infile_eb_badchannel u 1:2:($3 !=0 ? $3 :1/0) not w p palette pt 5 ps .7 


#%PNG% set out dir . "/EBprof2_p3_p2_pslope.png"
#infile_eb_pslope3
set title "{/=16 EB Positive Slope (p3-p2)}"
p infile_eb_pslope3  u 1:2:($3 != 0 ? $3 : 1/0) not w p palette pt 5 ps .7
#pause -1 "Hit return to continue"

#%PNG% set out dir . "/EBprof2_p3_p2_pslope_RMS.png"
set title "{/=16 EB Positive Slope (p3-p2) rms}"
p infile_eb_pslope3  u 1:2:($7 != 0 ? $7 : 1/0) not w p palette pt 5 ps .7
#pause -1 "Hit return to continue"

#%PNG% set out dir . "/EBprof2_p3_p2_nslope.png"
set title "{/=16 EB Negative Slope (p3-p2)}"
p infile_eb_nslope3  u 1:2:($3 != 0 ? $3 : 1/0) not w p palette pt 5 ps .7
#pause -1 "Hit return to continue"

#%PNG% set out dir . "/EBprof2_p3_p2_nslope_RMS.png"
set title "{/=16 EB Negative Slope (p3-p2) rms}"
p infile_eb_nslope3  u 1:2:($7 != 0 ? $7 : 1/0) not w p palette pt 5 ps .7
#pause -1 "Hit return to continue"


#%PNG% set out dir . "/EBh2_propagation_map.png"
set title "{/=16 EB # times previous p2 propagated}"
p infile_eb_propagation  u 1:2:($3 != 0 ? $3 : 1/0) not w p palette pt 5 ps .7


set xlabel "ix"
set ylabel "iy"

set xrange [0:100]
set yrange [0:100]


#%PNG% set out dir . "/EEh2_nZ_min.png"
#set title "EE- min\n". infile_eem_min_max
set title "{/=16 EE- min}"
set cbrange [.5:1.1]
p infile_eem_min_max  u 1:2:($3 != 0 ? $3 : 1/0) not w p palette pt 5 ps .7
set cbrange [*:*]
#pause -1 "Hit return to continue"

#%PNG% set out dir . "/EEh2_nZ_max.png"
#set title "EE- max\n". infile_eem_min_max
set title "{/=16 EE- max}"
set cbrange [.5:1.1]
p infile_eem_min_max  u 1:2:($4 != 0 ? $4 : 1/0) not w p palette pt 5 ps .7
set cbrange [*:*]
#pause -1 "Hit return to continue"

#%PNG% set out dir . "/EEh2_nZ_max_min.png"
#set title "EE- max - min\n". infile_eem_min_max
set title "{/=16 EE- max - min}"
p infile_eem_min_max  u 1:2:($4 - $3 != 0 ? $4 - $3 : 1/0) not w p palette pt 5 ps .7
#pause -1 "Hit return to continue"

#%PNG% set out dir . "/EEprof2_nZ_p2Norm_Mean.png"
#set title "EE- max - min\n". infile_eem_min_max
set title "{/=16 EE- average}"
p infile_eem_average  u 1:2:($3 != 0 ? $3 : 1/0) not w p palette pt 5 ps .7
#pause -1 "Hit return to continue"

#%PNG% set out dir . "/EEprof2_nZ_p2Norm_Mean_RMS.png"
set title "{/=16 EE- rms}"
p infile_eem_average  u 1:2:($7 != 0 ? $7 : 1/0) not w p palette pt 5 ps .7
#pause -1 "Hit return to continue"

#%PNG% set out dir . "/EEh2_nZ_BadChannel_Summary.png"
set title "{/=16 EE- Bad Channel Summary}"
p infile_eem_badchannel u 1:2:($3 !=0 ? $3 :1/0) not w p palette pt 5 ps .7 


#%PNG% set out dir . "/EEprof2_nZ_p3_p2_pslope.png"
#infile_eem_pslope3
set title "{/=16 EE- Positive Slope (p3-p2)}"
p infile_eem_pslope3  u 1:2:($3 != 0 ? $3 : 1/0) not w p palette pt 5 ps .7
#pause -1 "Hit return to continue"

#%PNG% set out dir . "/EEprof2_nZ_p3_p2_pslope_RMS.png"
set title "{/=16 EE- Positive Slope (p3-p2) rms}"
p infile_eem_pslope3  u 1:2:($7 != 0 ? $7 : 1/0) not w p palette pt 5 ps .7
#pause -1 "Hit return to continue"

#%PNG% set out dir . "/EEprof2_nZ_p3_p2_nslope.png"
set title "{/=16 EE- Negative Slope (p3-p2)}"
p infile_eem_nslope3  u 1:2:($3 != 0 ? $3 : 1/0) not w p palette pt 5 ps .7
#pause -1 "Hit return to continue"

#%PNG% set out dir . "/EEprof2_nZ_p3_p2_nslope_RMS.png"
set title "{/=16 EE- Negative Slope (p3-p2) rms}"
p infile_eem_nslope3  u 1:2:($7 != 0 ? $7 : 1/0) not w p palette pt 5 ps .7
#pause -1 "Hit return to continue"

#%PNG% set out dir . "/EEh2_nZ_propagation_map.png"
set title "{/=16 EE- # times previous p2 propagated}"
p infile_eem_propagation  u 1:2:($3 != 0 ? $3 : 1/0) not w p palette pt 5 ps .7


#%PNG% set out dir . "/EEh2_pZ_max.png"
#set title "EE+ max\n". infile_eep_min_max
set title "{/=16 EE+ max}"
set cbrange [.5:1.1]
p infile_eep_min_max  u 1:2:($3 != 0 ? $3 : 1/0) not w p palette pt 5 ps .7
set cbrange [*:*]
#pause -1 "Hit return to continue"

#%PNG% set out dir . "/EEh2_pZ_min.png"
#set title "EE+ min\n". infile_eep_min_max
set title "{/=16 EE+ min}"
set cbrange [.5:1.1]
p infile_eep_min_max  u 1:2:($4 != 0 ? $4 : 1/0) not w p palette pt 5 ps .7
set cbrange [*:*]
#pause -1 "Hit return to continue"

#%PNG% set out dir . "/EEh2_pZ_max_min.png"
#set title "EE+ max - min\n". infile_eep_min_max
set title "{/=16 EE+ max - min}"
p infile_eep_min_max  u 1:2:($4 - $3 != 0 ? $4 - $3 : 1/0) not w p palette pt 5 ps .7
#pause -1 "Hit return to continue"

#%PNG% set out dir . "/EEprof2_pZ_p2Norm_Mean.png"
set title "{/=16 EE+ average}"
p infile_eep_average  u 1:2:($3 != 0 != 0 ? $3 : 1/0) not w p palette pt 5 ps .7
#pause -1 "Hit return to continue"

#%PNG% set out dir . "/EEprof2_pZ_p2Norm_Mean_RMS.png"
set title "{/=16 EE+ rms}"
p infile_eep_average  u 1:2:($7 != 0 ? $7 : 1/0) not w p palette pt 5 ps .7
#pause -1 "Hit return to continue"

#%PNG% set out dir . "/EEh2_pZ_BadChannel_Summary.png"
set title "{/=16 EE+ Bad Channel Summary}"
p infile_eep_badchannel u 1:2:($3 !=0 ? $3 :1/0) not w p palette pt 5 ps .7 

#%PNG% set out dir . "/EEprof2_pZ_p3_p2_pslope.png"
set title "{/=16 EE+ Positive Slope (p3-p2)}"
p infile_eep_pslope3  u 1:2:($3 != 0 ? $3 : 1/0) not w p palette pt 5 ps .7
#pause -1 "Hit return to continue"

#%PNG% set out dir . "/EEprof2_pZ_p3_p2_pslope_RMS.png"
set title "{/=16 EE+ Positive Slope (p3-p2) rms}"
p infile_eep_pslope3  u 1:2:($7 != 0 ? $7 : 1/0) not w p palette pt 5 ps .7
#pause -1 "Hit return to continue"

#%PNG% set out dir . "/EEprof2_pZ_p3_p2_nslope.png"
set title "{/=16 EE+ Negative Slope (p3-p2)}"
p infile_eep_nslope3  u 1:2:($3 != 0 ? $3 : 1/0) not w p palette pt 5 ps .7
#pause -1 "Hit return to continue"

#%PNG% set out dir . "/EEprof2_pZ_p3_p2_nslope_RMS.png"
set title "{/=16 EE+ Negative Slope (p3-p2) rms}"
p infile_eep_nslope3  u 1:2:($7 != 0 ? $7 : 1/0) not w p palette pt 5 ps .7
#pause -1 "Hit return to continue"

#%PNG% set out dir . "/EEh2_pZ_propagation_map.png"
set title "{/=16 EE+ # times previous p2 propagated}"
p infile_eep_propagation  u 1:2:($3 != 0 ? $3 : 1/0) not w p palette pt 5 ps .7

unset xrange
unset yrange