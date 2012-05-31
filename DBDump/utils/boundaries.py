#!/usr/bin/env python

import os
import sys
import time

def usage():
    print "Usage: %s <rr file>" % sys.argv[0]

if len(sys.argv) < 2:
    usage()
    sys.exit(1)

fin = sys.argv[1]

cnt = 0

os.environ['TZ'] = 'UTC'
time.tzset()
tformat = "%Y.%m.%d %H:%M:%S"
pcoll = True
bstart, bstop = 0, 0
not_bstart, not_bstop = 0, 0
bcnt, not_bcnt = 1, 1
boff = 946681200
gpmacro_name = "tmp_gp_macro.gp"
not_gpmacro_name = "tmp_gp_macro_not.gp"
gpdata_name = "tmp_gp_data.dat"
not_gpmacro = open(not_gpmacro_name, "w")
gpmacro = open(gpmacro_name, "w")
gpdata  = open(gpdata_name, "w")
rflag = { "Commissioning12" : 1, "Cosmics12" : 2, "Collisions12": 3 }
#gpmacro.write("reset\n")
#gpmacro.write('l "~/work/gp/time_display.gp"\n')

#gpmacro.write("lxmin = GPVAL_DATA_X_MIN + (GPVAL_DATA_X_MAX - GPVAL_DATA_X_MIN) * .005\n")
#gpmacro.write("lxmax = GPVAL_DATA_X_MAX - (GPVAL_DATA_X_MAX - GPVAL_DATA_X_MIN) * .005\n")
gpmacro.write("lxmin = GPVAL_DATA_X_MIN\n")
gpmacro.write("lxmax = GPVAL_DATA_X_MAX\n")

for l in open(fin):
    #if cnt > 10: break
    if l[0] == '#': continue
    v = l.split()
    if v[3] == 'None': continue
    run = int(v[0])
    flag = v[8]
    start = v[1] + " " + v[2]
    stop  = v[3] + " " + v[4]
    field = float(v[7])
    if flag.find("Collision") != -1:
        coll = True
    else :
        coll = False

    utc_start = time.mktime(time.strptime(start, tformat))
    utc_stop = time.mktime(time.strptime(stop, tformat))

    if coll != pcoll and coll:
        bstart = utc_start
        bstop  = utc_stop
        gpmacro.write("xlh%d = (%d > lxmin && %d < lxmax ? '%d' : '1/0'); set label %d '{/=8 %d}' at first xlh%d, graph .975 rotate by 90 front right\n" % (bcnt, bstart - boff, bstart - boff, bstart, run * 10, run, bcnt))
        not_gpmacro.write("nb%03d = '%d'; ne%03d = '%d'; set object %d rect from first nb%03d, graph 0.0 to first ne%03d, graph 1 front fc rgb '#000000' fillstyle transparent solid .4 noborder\n" % (not_bcnt, not_bstart, not_bcnt, not_bstop, not_bcnt + 1000, not_bcnt, not_bcnt))
        not_bcnt += 1
    if coll == pcoll and coll:
        bstop = utc_stop
    if coll != pcoll and not coll:
        not_bstart = utc_start
        not_bstop  = utc_stop
        #print "Collision runs:", bstart, bstop
        #gpmacro.write("b%03d = '%d'; e%03d = '%d'; set object %d rect from first b%03d, graph 0.05 to first e%03d, graph .95 front fc rgb '#eeeeee' fillstyle transparent solid .5 noborder\n" % (bcnt, bstart, bcnt, bstop, bcnt, bcnt, bcnt))
        gpmacro.write("xll%d = (%d > lxmin && %d < lxmax ? '%d' : '1/0'); set label %d '{/=8 %d}' at first xll%d, graph .025 rotate by 90 front offset .5\n" % (bcnt, bstop - boff, bstop - boff, bstop, run, run, bcnt))
        gpmacro.write("b%03d = (%d > lxmin && %d < lxmax ? '%d' : '1'); e%03d = (%d > lxmin && %d < lxmax ? '%d' : ( b%03d == '1' ? '1' : GPVAL_DATA_X_MAX )); set object %d rect from first b%03d, graph 0.015 to first e%03d, graph .985 front fc rgb '#0000ff' fillstyle transparent solid .15 noborder\n" % (bcnt, bstart - boff, bstart - boff, bstart, bcnt, bstop - boff, bstop - boff, bstop, bcnt, bcnt, bcnt, bcnt))
        bcnt += 1
    if coll == pcoll and not coll:
        not_bstop = utc_stop

    if flag not in rflag:
        print "Error: flag " + flag + " not supported, please implement it ;-)"
        sys.exit(2)
    
    gpdata.write("%d %f %f %d\n" % (run, utc_start, utc_stop, rflag[flag]))
    pcoll = coll
    cnt += 1

#gpmacro.write('p [][0:' + str(len(rflag) + 1) + '] "' + gpdata_name + '" u 2:4 not, ' + "'' u 3:4 not\n")

gpmacro.close()
not_gpmacro.close()
gpdata.close()
