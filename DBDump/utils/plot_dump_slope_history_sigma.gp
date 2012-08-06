reset

#set term pngcairo enhanced font "Helvetica,12"
#%PNG% set term pngcairo enhanced font "Helvetica,12"

set grid back
set key above
set mouse mouseformat "mouse = %.10g, %.10g"

set timefmt "%s"
set xdata time
set format x "%d/%m\n%H:%M"
#set format x "%d/%m/\n%Y"
#set format x "%d/%m"


#dir = "/home/ferri/tmp/gp_out_dump_189099_190702"
#dir = "/home/ferri/tmp/plots_dump_185768_191584/gp_out_dump_185768_191584"
#dir = "/home/ferri/work/laser/2012_troubles/gp_out_plot_GR_R_44_V15_EcalLaserAPDPNRatios_last/"
#dir = "/home/ferri/work/laser/2012_troubles/gp_out_dump_193104_193203/"
#dir = "/home/ferri/work/laser/2012_troubles/gp_out_dump_193104_193203_nochstatus/"
#dir = "/home/ferri/work/laser/2012_troubles/gp_out_dump_193091_193186"
#dir = "/home/ferri/work/laser/2012_troubles/gp_cross_check"
#dir = "/home/ferri/work/laser/2012_troubles/gp_cross_check_2"
#dir = "gp_out_dump_one_shot_193093_193825"
dir = "gp_prompt"
#%PNG% dir = "%DIR%"

region = 'LM31'
#%PNG% region = '%REGION%'
infile_1s = dir . '/history_p3_p2_' . region . '_1S.dat'
#infile_e = dir . '/history_p3_p2_' . region . '_E.dat'


#%PNG% set rmargin 5
#%PNG% set label "{/=9 %NOW%}" at screen .975, screen .65 rotate by 90

set yrange [-.01:.01]
set title 'Slope history in %TITLE%'
off = 946681200
xmin = 0
xmax = 9999999999.

# trick to get proper display of labels
set out "/dev/null"
p infile_1s u ($1 > xmin && $1 < xmax ? $1 : 1/0):($2+$5):($2-$6) not w filledcu
unset out

#set out "history_" . region . ".png"
#%PNG% set out dir . "/history_p3_p2_" . region . ".png"

l 'tmp_gp_macro.gp'
p infile_1s u ($1 > xmin && $1 < xmax ? $1 : 1/0):($2+$5):($2-$6) not w filledcu lc rgbcolor "#59d354", \
  '' u ($1 > xmin && $1 < xmax ? $1 : 1/0):2 not w l lt -1, \
  '' u ($1 > xmin && $1 < xmax ? $1 : 1/0):2 not w p pt 7 ps .5 lt -1, \
  1/0 t "median" w l lc rgbcolor "#000000", \
   1/0 t "68.2% of the channels" w p pt 5 ps 2 lc rgbcolor "#59d354", \
  1/0 t "collisions" w p pt 5 ps 2 lc rgbcolor "#ccccff"



