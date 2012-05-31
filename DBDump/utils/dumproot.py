#!/usr/bin/env python

# federico.ferri@cern.ch

from ROOT import *
import os
import string
import sys

def usage(s):
    print "%s <filename> [directory]" % s
    print "dump all the histograms found in the root file into ascii files"
    print "suitable for gnuplot plotting."
    print "If not otherwise specified from command line, the directory will"
    print "have the same name of the root file, with prefix `gp_' and "
    print "removal of the suffix `.root'."
    sys.exit(1)

objects = ['TH1', 'TH1F', 'TH1D', 'TH2', 'TH2F', 'TH2D', 'TProfile', 'TProfile2D', 'TGraph', 'TGraphErrors', 'TGraphAsymmErrors']

def get_objects(d, l):
        for k in d.GetListOfKeys():
                if k.GetClassName() in objects:
                        l[ k.GetName() ] = d.Get( k.GetName() )
                if k.GetClassName() == 'TDirectoryFile':
                        get_objects( d.Get( k.GetName() ), l )

def d_h1(o, f):
    f.write("# " + o.ClassName() + "\n")
    f.write("#nbin = %d; xmin = %.10g; xmax = %.10g;\n" % (o.GetNbinsX(), o.GetXaxis().GetXmin(), o.GetXaxis().GetXmax()))
    f.write("# x y dx dy\n")
    f.write("#\n")
    for i in range(1, o.GetNbinsX() + 1):
        f.write("%.10g %.10g %.10g %.10g\n" % (o.GetBinCenter(i), o.GetBinContent(i), o.GetBinWidth(i), o.GetBinError(i)))

def d_g(o, f):
    f.write("# " + o.ClassName() + "\n")
    f.write("#x y\n")
    f.write("#\n")
    for i in range(0, o.GetN()):
        f.write("%.10g %.10g\n" % (o.GetX()[i], o.GetY()[i]))

def d_ge(o, f):
    f.write("# " + o.ClassName() + "\n")
    f.write("#x y +dx -dx +dy -dy\n")
    f.write("#\n")
    for i in range(0, o.GetN()):
        f.write("%.10g %.10g %.10g %.10g %.10g %.10g\n" % (o.GetX()[i], o.GetY()[i], o.GetErrorXhigh(i), o.GetErrorXlow(i), o.GetErrorYhigh(i), o.GetErrorYlow(i)))

def d_h2(o, f):
    f.write("# " + o.ClassName() + "\n")
    f.write("#x y z dx dy dz rms_z\n")
    f.write("#\n")
    ax = o.GetXaxis()
    ay = o.GetYaxis()
    for i in range(1, o.GetNbinsX() + 1):
        for j in range(1, o.GetNbinsY() + 1):
            f.write("%g %g %g %g %g %g\n" % (ax.GetBinCenter(i), ay.GetBinCenter(j), o.GetBinContent(i, j), ax.GetBinWidth(i), ay.GetBinWidth(j), o.GetBinError(i, j)))
            #f.write("\n")


def d_p2(o, f):
    f.write("# " + o.ClassName() + "\n")
    f.write("#x y z dx dy dz rms_z\n")
    f.write("#\n")
    ax = o.GetXaxis()
    ay = o.GetYaxis()
    p = o.ProjectionXY(o.GetName() + "_RMS", "C=E");
    for i in range(1, o.GetNbinsX() + 1):
        for j in range(1, o.GetNbinsY() + 1):
            f.write("%g %g %g %g %g %g %g\n" % (ax.GetBinCenter(i), ay.GetBinCenter(j), o.GetBinContent(i, j), ax.GetBinWidth(i), ay.GetBinWidth(j), o.GetBinError(i, j), p.GetBinContent(i, j)))
            #f.write("\n")


obj_fun = { 'TH1F' : d_h1, 'TH1D' : d_h1, 'TProfile' : d_h1, 'TH2F' : d_h2, 'TH2D' : d_h2, 'TGraph' : d_g, 'TGraphErrors' : d_ge, 'TGraphAsymmErrors' : d_ge, 'TProfile2D' : d_p2 }

def dump_object(o, n):
    f = open(n, "w")
    if o.ClassName() in obj_fun.keys():
        obj_fun[o.ClassName()](o, f)
    else:
        print "dump function for class object `" + o.ClassName() + "' not yet implemented"
        print "  --> skipping object `" + n + "'"
    f.close()
    

if len(sys.argv) < 2:
    usage(sys.argv[0])

ifile = sys.argv[1]

odir = "gp_" + string.replace(string.replace(ifile, ".root", ""), "/", "_")
if len(sys.argv) == 3:
    odir = sys.argv[2]

print "      input file:", ifile
print "output directory:", odir

os.system("mkdir -p '" + odir + "'")

if not os.path.isfile(ifile):
    print "Error opening file `" + ifile + "': no such file or directory."
    sys.exit(2)
f = TFile.Open(ifile)

o_list = {}

get_objects(gDirectory, o_list)

#o_matches = [ "LM31", "h2", "prof2" ]
o_matches = [ ]

for i, j in o_list.iteritems():
    print i, j
    m = False
    for k in o_matches:
        if string.find(i, k) != -1:
            m = True
    if len(o_matches) == 0 or m:
        dump_object(j, odir + "/" + i + ".dat")
