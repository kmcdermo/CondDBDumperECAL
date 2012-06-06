#!/usr/bin/env python

# federico.ferri@cern.ch
# 16.07.2011

# include XML-RPC client library
# see  https://twiki.cern.ch/twiki/bin/view/CMS/DqmRrApi for more detail
# RR API uses XML-RPC webservices interface for data access
import string
import time
import xmlrpclib
#from lxml import etree
from xml.dom.minidom import *
from optparse import OptionParser

# get handler to RR XML-RPC server
#server = xmlrpclib.ServerProxy('http://pccmsdqm04.cern.ch/runregistry_api/xmlrpc')
#server = xmlrpclib.ServerProxy('https://cmswbmoff.web.cern.ch/cmswbmoff/runregistry_api/xmlrpc')
#server = xmlrpclib.ServerProxy('http://pccmsdqm04.cern.ch/runregistry_api/xmlrpc')
server = xmlrpclib.ServerProxy('http://cms-service-runregistry-api.web.cern.ch/cms-service-runregistry-api/xmlrpc')

dets = [ 'CASTOR', 'CSC', 'DT', 'ECAL', 'ES', 'HCAL', 'L1T', 'PIX', 'RPC', 'STRIP' ]
#dets = [ 'CASTOR', 'CSC', 'DT', 'ECAL', 'EGAM', 'ES', 'HCAL', 'HLT', 'JMET', 'L1T', 'MUON', 'PIX', 'RPC', 'STRIP', 'TRACK' ]


### def print_data(root):
###     print '# RUN_NUMBER  T_START  T_STOP  N_EVTS  RATE  BFIELD  SUBS_STATUS_CASTOR_CSC_DT_ECAL_ES_HCAL_L1T_PIX_RPC_STRIP  DATASET'
###     for it in root.iter('RUN'):
###         runnum = it.find('NUMBER').text
###         runstart = string.replace(it.find('START_TIME').text, '-', '.')
###         runstart = string.replace(runstart, 'T', ' ')
###         runstart = string.replace(runstart[::-1], '0.', '', 1)[::-1]
###         runstop  = string.replace(str(it.find('END_TIME').text), '-', '.')
###         runstop  = string.replace(runstop, 'T', ' ')
###         runstop  = string.replace(runstop[::-1], '0.', '', 1)[::-1]
###         events   = int(it.find('EVENTS').text)
###         #rate     = float(it.find('RATE').text)
###         rate     = float(-1)
###         bfield   = float(it.find('BFIELD').text)
###         #for jt in it.iter('DATASETS'):
###         name     = it.find('GROUP_NAME').text
###         dataset  = it.find('DATASETS').find('DATASET').find('NAME').text
###         comment  = ''
###         #for jt in it.find('DATASETS').find('DATASET').find('CMPS').iter('CMP'):
###         #    for det in dets:
###         #        if (jt.find('NAME').text == det):
###         #            comment += jt.find('VALUE').text[0]
### 
###         print runnum, ' ', runstart, ' ', runstop, '%11d' % events, '%9.2f ' % rate, '%.2f' % bfield, comment, name, dataset

def getprop(request, prop_name):
    nodes = request.getElementsByTagName(prop_name)[0].childNodes
    if nodes.length > 0:
        return nodes[0].nodeValue
    else:
        return "None"

def print_data(root):
    print '# RUN_NUMBER  T_START  T_STOP  N_EVTS  RATE  BFIELD  SUBS_STATUS_CASTOR_CSC_DT_ECAL_ES_HCAL_L1T_PIX_RPC_STRIP  DATASET'

    for it in root.getElementsByTagName('RUN'):
        runnum = getprop(it, 'NUMBER')
        runstart = getprop(it, 'START_TIME').replace('-', '.')
        runstart = string.replace(runstart, 'T', ' ')
        runstart = string.replace(runstart[::-1], '0.', '', 1)[::-1]
        runstop  = getprop(it, 'END_TIME').replace('-', '.')
        runstop  = string.replace(runstop, 'T', ' ')
        runstop  = string.replace(runstop[::-1], '0.', '', 1)[::-1]
        events   = int(getprop(it, 'EVENTS'))
        #rate     = float(getprop(it, 'RATE')
        rate     = float(-1)
        bfield   = float(getprop(it, 'BFIELD'))
        #for jt in it.iter('DATASETS'):
        name     = getprop(it, 'GROUP_NAME')
        try:
            dataset  = it.getElementsByTagName('DATASETS')[0].getElementsByTagName('DATASET')[0].getElementsByTagName('NAME')[0].childNodes[0].nodeValue
        except IndexError:
            dataset = ''
        comment  = ''
        #for jt in it.find('DATASETS').find('DATASET').find('CMPS').iter('CMP'):
        #    for det in dets:
        #        if (jt.find('NAME').text == det):
        #            comment += jt.find('VALUE').text[0]

        print runnum, ' ', runstart, ' ', runstop, '%11d' % events, '%9.2f ' % rate, '%.2f' % bfield, comment, name, dataset



parser = OptionParser()
parser.add_option('-r', '--runmin', dest='runmin', default='0', help='minimum run')
parser.add_option('-R', '--runmax', dest='runmax', default='99999999', help='maximum run')
parser.add_option('-t', '--tmin',   dest='tmin',   default='', help='minimun time in the format YYYY.MM.DD hh:mm:ss')
parser.add_option('-T', '--tmax',   dest='tmax',   default='', help='maximum time in the format YYYY.MM.DD hh:mm:ss')
parser.add_option('-l', '--last',   dest='last',   default='', help='last 24 h')
parser.add_option('-c', '--current', dest='curr', action='store_true', help='current run, if any')

(options, args) = parser.parse_args()

tmin = options.tmin
tmax = options.tmax
runmin = options.runmin
runmax = options.runmax
curr = options.curr

def add_condition(c, s, a):
    if a:
        c += ' and '
    c += s
    return c

#data = server.RunDatasetTable.export('GLOBAL', 'csv_runs', "{runStartTime} >= TO_DATE('2010-06-01','YYYY-MM-DD') and {runStartTime} < TO_DATE('2010-06-10','YYYY-MM-DD') and {events} > 1000000")
s = ''
a = False
if runmin != '0':
    s = add_condition(s, '{runNumber} >= ' + runmin, a)
    a = True
if runmax != '99999999':
    s = add_condition(s, '{runNumber} <= ' + runmax, a)
    a = True
if tmin != '':
    s = add_condition(s, '{runStartTime} >= TO_DATE(\'' + tmin + '\', \'YYYY.MM.DD HH24:MI:SS\')', a)
    a = True
if tmax != '':
    s = add_condition(s, '{runStartTime} <= TO_DATE(\'' + tmax + '\', \'YYYY.MM.DD HH24:MI:SS\')', a)
    a = True
if curr:
    a = False
    s = ''
    s = add_condition(s, '{runEndTime} is null and {runStartTime} >= TO_DATE(\'' + time.strftime("%Y.%m.%d %H:%M:%S", time.localtime( time.time() - 3600*24*5)) + '\', \'YYYY.MM.DD HH24:MI:SS\')', a)

print '# query:', s

data = server.RunDatasetTable.export('GLOBAL', 'xml_all', s)

#print data
#import sys
#sys.exit(1)
#
#root = etree.fromstring(data)
#root = etree.parse('example.xml')
root = parseString(data.encode('utf-8')) #'us-ascii', 'ignore'))

print_data(root)
