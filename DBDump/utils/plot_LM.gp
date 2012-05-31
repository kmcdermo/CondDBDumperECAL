reset

#%PNG% set term pngcairo enhanced font "Helvetica,12"
#set term pngcairo enhanced font "Helvetica,12"

set grid back
set key above
set mouse mouseformat "mouse = %.10g, %.10g"

set timefmt "%s"
set xdata time
set format x "%d/%m\n%H:%M"

dir = "~/www/prompt/gp_prompt"
#%PNG% dir = "%DIR%"

region = 'LM31'
#%PNG% region = '%REGION%'
infile_3s = dir . '/history_p2_' . region . '_3S.dat'
infile_2s = dir . '/history_p2_' . region . '_2S.dat'
infile_1s = dir . '/history_p2_' . region . '_1S.dat'
infile_e = dir . '/history_p2_' . region . '_E.dat'

#set out "history_" . region . ".png"
#%PNG% set out dir . "/history_p2_" . region . ".png"

#%PNG% set rmargin 5

#set title region
set title '%TITLE%'
#xmin = 1336741325 - 86400
off = 946681200
#xmin = 1336821127 - 12 * 86400
xmin = 0
xmax = 9999999999.
set cblabel "LM number"
set xlabel "date"
set ylabel "ECAL response to laser light"

point_size = .5
line_width = 1

#%PNG% set rmargin 5
#%PNG% set label "{/=9 %NOW%}" at screen .975, screen .65 rotate by 90

set out "/dev/null"
p for[i=1:1] dir . "/history_p2_LM" . sprintf("%02d", i) . "_1S.dat" u ($1 > xmin && $1 < xmax ? $1 : 1/0):2:(i) not

l 'tmp_gp_macro.gp'

set title "{/=16 EB-}"
set out dir . "/history_LM_EB-.png"
#p for[i=1:36] dir . "/history_p2_LM" . sprintf("%02d", i) . "_1S.dat" u ($1 > xmin && $1 < xmax ? $1 : 1/0):2:(i) not w p palette pt 7 ps  point_size
p for[i=1:36] dir . "/history_p2_LM" . sprintf("%02d", i) . "_1S.dat" u ($1 > xmin && $1 < xmax ? $1 : 1/0):2:(i) not w l palette lw line_width, \
  1/0 t "collisions" w p pt 5 ps 2 lc rgbcolor "#ccccff"

set title "{/=16 EB+}"
set out dir . "/history_LM_EB+.png"
#p for[i=37:72] dir . "/history_p2_LM" . sprintf("%02d", i) . "_1S.dat" u ($1 > xmin && $1 < xmax ? $1 : 1/0):2:(i) not w p palette pt 7 ps point_size
p for[i=37:72] dir . "/history_p2_LM" . sprintf("%02d", i) . "_1S.dat" u ($1 > xmin && $1 < xmax ? $1 : 1/0):2:(i) not w l palette lw line_width, \
  1/0 t "collisions" w p pt 5 ps 2 lc rgbcolor "#ccccff"

set title "{/=16 EE+}"
set out dir . "/history_LM_EE+.png"
#p for[i=73:82] dir . "/history_p2_LM" . sprintf("%02d", i) . "_1S.dat" u ($1 > xmin && $1 < xmax ? $1 : 1/0):2:(i) not w p palette pt 7 ps point_size
p for[i=73:82] dir . "/history_p2_LM" . sprintf("%02d", i) . "_1S.dat" u ($1 > xmin && $1 < xmax ? $1 : 1/0):2:(i) not w l palette lw line_width, \
  1/0 t "collisions" w p pt 5 ps 2 lc rgbcolor "#ccccff"

set title "{/=16 EE-}"
set out dir . "/history_LM_EE-.png"
#p for[i=83:92] dir . "/history_p2_LM" . sprintf("%02d", i) . "_1S.dat" u ($1 > xmin && $1 < xmax ? $1 : 1/0):2:(i) not w p palette pt 7 ps point_size
p for[i=83:92] dir . "/history_p2_LM" . sprintf("%02d", i) . "_1S.dat" u ($1 > xmin && $1 < xmax ? $1 : 1/0):2:(i) not w l palette lw line_width, \
  1/0 t "collisions" w p pt 5 ps 2 lc rgbcolor "#ccccff"
