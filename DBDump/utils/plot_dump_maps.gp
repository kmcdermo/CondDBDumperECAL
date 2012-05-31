reset

#%PNG% set term pngcairo enhanced font "Helvetica,12"

dir = "%DIR%"
#dir = "/home/ferri/work/laser/2012_troubles/gp_out_dump_193091_193186"

#%PNG% set out dir . "/distr_eta_normalised_p2.png"

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
p dir . "/distr_eta_normalised_p2.dat" u 1:($2 - (xmax - xmin) / 2 / nbin) not w step lt 3
set mytics
set mxtics
unset grid
unset log
unset ymtics

#pause -1 "Hit..."

set auto fix
#set palette model HSV functions .75 - .75 * gray, 1, 1

infile_eb_max = dir . '/EBh2_max.dat'
infile_eb_min = dir . '/EBh2_min.dat'
infile_eb_min_max = dir . '/EBh2_min_max.dat'
infile_eb_average = dir . '/EBprof2_p2Norm.dat'

infile_eem_max = dir . '/EEh2_nZ_max.dat'
infile_eem_min = dir . '/EEh2_nZ_min.dat'
infile_eem_min_max = dir . '/EEh2_nZ_min_max.dat'
infile_eem_average = dir . '/EEprof2_nZ_p2Norm.dat'

infile_eep_max = dir . '/EEh2_pZ_max.dat'
infile_eep_min = dir . '/EEh2_pZ_min.dat'
infile_eep_min_max = dir . '/EEh2_pZ_min_max.dat'
infile_eep_average = dir . '/EEprof2_pZ_p2Norm.dat'

set xlabel "i{/Symbol f}"
set ylabel "i{/Symbol h}"

#%PNG% set out dir . "/EBh2_max.png"
#set title "EB min\n". infile_eb_min_max
set title "{/=16 EB min}"
set cbrange [.9:1.05]
p infile_eb_min_max  u 1:2:($3 != 0 ? $3 : 1/0) not w p palette pt 5 ps .7
set cbrange [*:*]
#pause -1 "Hit return to continue"

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

#%PNG% set out dir . "/EBprof2_p2Norm.png"
#set title "EB average\n". infile_eb_min_max
set title "{/=16 EB average}"
p infile_eb_average  u 1:2:($3 != 0 ? $3 : 1/0) not w p palette pt 5 ps .7
#pause -1 "Hit return to continue"

#%PNG% set out dir . "/EBprof2_p2Norm_RMS.png"
set title "{/=16 EB rms}"
p infile_eb_average  u 1:2:($7 != 0 ? $7 : 1/0) not w p palette pt 5 ps .7
#pause -1 "Hit return to continue"

set xlabel "ix"
set ylabel "iy"


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

#%PNG% set out dir . "/EEprof2_nZ_p2Norm.png"
#set title "EE- max - min\n". infile_eem_min_max
set title "{/=16 EE- average}"
p infile_eem_average  u 1:2:($3 != 0 ? $3 : 1/0) not w p palette pt 5 ps .7
#pause -1 "Hit return to continue"

#%PNG% set out dir . "/EEprof2_nZ_p2Norm_RMS.png"
set title "{/=16 EE- rms}"
p infile_eem_average  u 1:2:($7 != 0 ? $7 : 1/0) not w p palette pt 5 ps .7
#pause -1 "Hit return to continue"


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

#%PNG% set out dir . "/EEprof2_pZ_p2Norm.png"
set title "{/=16 EE+ average}"
p infile_eep_average  u 1:2:($3 != 0 != 0 ? $3 : 1/0) not w p palette pt 5 ps .7
#pause -1 "Hit return to continue"

#%PNG% set out dir . "/EEprof2_pZ_p2Norm_RMS.png"
set title "{/=16 EE+ rms}"
p infile_eep_average  u 1:2:($7 != 0 ? $7 : 1/0) not w p palette pt 5 ps .7
#pause -1 "Hit return to continue"
