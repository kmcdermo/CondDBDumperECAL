import FWCore.ParameterSet.Config as cms
import os

process = cms.Process("Demo")

process.load("FWCore.MessageService.MessageLogger_cfi")

gTag = os.environ.get('LAS_GTAG')
if not gTag:
    gTag = 'GR_R_44_V13::All'

lTag = os.environ.get('LAS_LTAG')
if not lTag:
    lTag = 'EcalLaserAPDPNRatios_V4_172400_172619_110804'

dbconnect = os.environ.get('LAS_DBCONNECT')
if not dbconnect:
    dbconnect = 'frontier://FrontierProd/CMS_COND_311X_ECAL_LASP'

dbconnect = 'frontier://FrontierPrep/CMS_COND_ECAL'

plotdir = os.environ.get('LAS_PLOTDIR')
if not plotdir:
    plotdir = '.'

dumpdir = './'

process.load('Configuration/StandardSequences/FrontierConditions_GlobalTag_cff')
process.load('Configuration.StandardSequences.GeometryDB_cff')
#process.GlobalTag.globaltag = 'IDEAL_31X::All'
#process.GlobalTag.globaltag = 'MC_31X_V1::All'
#process.GlobalTag.globaltag = 'GR09_31X_V1::All'
process.GlobalTag.globaltag = gTag

    

##process.es_prefer = cms.ESPrefer("PoolDBESSource","ecalConditions")
##from CondCore.DBCommon.CondDBSetup_cfi import *
##CondDBSetup.DBParameters.authenticationPath = '/afs/cern.ch/cms/DB/conddb'
##process.ecalConditions = cms.ESSource("PoolDBESSource",
##    CondDBSetup,
##    siteLocalConfig = cms.untracked.bool(True),
##    toGet = cms.VPSet(
##        ###cms.PSet(
##        ###        record = cms.string('EcalPedestalsRcd'),
##        ###        tag = cms.string('EcalPedestals_mc')
##        ###), 
##        ###cms.PSet(
##        ###    record = cms.string('EcalADCToGeVConstantRcd'),
##        ###    tag = cms.string('EcalADCToGeVConstant_EBg50_EEwithB_new')
##        ###), 
##        ###cms.PSet(
##        ###    record = cms.string('EcalChannelStatusRcd'),
##        ###    #tag = cms.string('EcalChannelStatus_may2009_mc')
##        ###    tag = cms.string('EcalChannelStatus_v07_offline')
##        ###), 
##        ###cms.PSet(
##        ###    record = cms.string('EcalGainRatiosRcd'),
##        ###    tag = cms.string('EcalGainRatios_mc')
##        ###), 
##        ###cms.PSet(
##        ###    record = cms.string('EcalIntercalibConstantsRcd'),
##        ###    tag = cms.string('EcalIntercalibConstantsMC_2010_V3_Bon_mc')
##        ###    #tag = cms.string('EcalIntercalibConstants_2010_V3_Bon_startup_mc')
##        ###), 
##        ###cms.PSet(
##        ###    record = cms.string('EcalWeightXtalGroupsRcd'),
##        ###    tag = cms.string('EcalWeightXtalGroups_mc')
##        ###), 
##        ###cms.PSet(
##        ###    record = cms.string('EcalTBWeightsRcd'),
##        ###    tag = cms.string('EcalTBWeights_mc')
##        ###), 
##        ###cms.PSet(
##        ###    record = cms.string('EcalLaserAlphasRcd'),
##        ###    tag = cms.string('EcalLaserAlphas_hlt')
##        ###), 
##        cms.PSet(
##            record = cms.string('EcalLaserAPDPNRatiosRcd'),
##            ###record = cms.string('EcalLaserAPDPNRatiosRcd'),
##            ###tag = cms.string('EcalLaserAPDPNRatios_mc')
##            #tag = cms.string('EcalLaserAPDPNRatios_online_hlt')
##            ###tag = cms.string('EcalLaserAPDPNRatios_v2_online')
##            tag = cms.string('EcalLaserAPDPNRatios_last')
##            ##tag = cms.string('EcalLaserAPDPNRatios_V3_160400_172308_110802')
##            ###tag = cms.string(lTag)
##        ), 
##        ###cms.PSet(
##        ###    record = cms.string('EcalLaserAPDPNRatiosRefRcd'),
##        ###    tag = cms.string('EcalLaserAPDPNRatiosRef_mc')
##        ###)
##        ),
##    messagelevel = cms.untracked.uint32(0),
##    timetype = cms.untracked.string('timestamp'),
##    ##timetype = cms.untracked.string('runnumber'),
##    #connect = cms.string('frontier://cms_conditions_data/CMS_COND_ECAL'), ##cms_conditions_data/CMS_COND_ECAL"
##    ##connect = cms.string('frontier://FrontierPrep/CMS_COND_ECAL_LT'), ##cms_conditions_data/CMS_COND_ECAL"
##    #####connect = cms.string('frontier://cms_conditions_data/CMS_COND_31X_ECAL'),
##    #####connect = cms.string('oracle://cms_orcon_prod/CMS_COND_31X_ECAL'),
##    ##connect = cms.string('frontier://FrontierProd/CMS_COND_311X_ECAL_LAS'),
##    ##connect = cms.string(dbconnect),
##    #####connect = cms.string('sqlite:////tmp/ferriff/model_V3_lumiOK_160400_172308_110802.db'),
##    ##connect = cms.string('sqlite:////tmp/ferriff/model_V3_160400_172308_110802.db'),
##    ##connect = cms.string('sqlite:///tmp/ferriff/model_V4_160400_172308_110802.db'),
##    ##connect = cms.string('oracle://cms_orcoff_prod/CMS_COND_311X_ECAL_LASP'),
##    #connect = cms.string('frontier://FrontierProd/CMS_COND_31X_ECAL'),
##    connect = cms.string('oracle://cms_orcoff_prod/CMS_COND_42X_ECAL_LASP'),
##    #connect = cms.string('frontier://FrontierProd/CMS_COND_43X_ECAL'),
##    ###connect = cms.string('oracle://cms_orcoff_prep/CMS_COND_ECAL_LT'),
##    # at P5
##    ##connect = cms.string('oracle://cms_orcon_prod/CMS_COND_ECAL_LT'),
##
##    authenticationMethod = cms.untracked.uint32(1)
##)
###process.CondDBCommon.DBParameters.authenticationPath = '/nfshome0/fra/CMSSW_3_1_0_pre9/src/CondTools/Ecal/python'
###process.CondDBCommon.DBParameters.authenticationPath = '/nfshome0/fra/CMSSW_3_1_0/src/CondTools/Ecal/python'

#process.MaxEvents = cms.untracked.PSet( input = cms.untracked.int32(2628) )
process.maxEvents = cms.untracked.PSet( input = cms.untracked.int32(1) )

#process.source = cms.Source("EmptySource",
#        firstRun = cms.untracked.uint32(190705),
#        numberEventsInRun = cms.untracked.uint32(1),
#        #firstTime = cms.untracked.uint32( 1244042540 ), # run 98723
#        #firstTime = cms.untracked.uint32( 1224557391 ), # beginning of run 66951
#        #firstTime = cms.untracked.uint32( 1224188905 ), # beginning of run 66615
#        ##firstTime = cms.untracked.uint32( 1224147420 ), # inside t1 and t2 of a DB IOV before run 65940
#        ##firstTime = cms.untracked.uint32( 1224146163 ),
#        #firstTime = cms.untracked.uint32( (1303120805 - 10*3600) ),
#        firstTime = cms.untracked.uint32( 1333582717 ), # run 190389
#        timeBetweenEvents = cms.untracked.uint32(10)
#)

process.source = cms.Source("PoolSource",
        fileNames = cms.untracked.vstring(
        #'file:F08F782B-77E8-DE11-B1FC-0019B9F72BFF.root'
        #'file:ZeeSummer09.root'
        #'file:/afs/cern.ch/user/s/sbrochet/public/jetmet/pickevents.root'
        'file:/afs/cern.ch/user/m/mpierini/work/public/pickevents_merged.root'
        ),
        #skipEvents = cms.untracked.uint32(10)
)

process.maxEvents = cms.untracked.PSet(
        input = cms.untracked.int32(-1)
)

process.demo = cms.EDAnalyzer('EventDump',
        outPlot = cms.bool(False),
        outDump = cms.bool(True),
        outDumpFile = cms.string('./outdump.dat'),
        ecalRecHitsEB = cms.InputTag("reducedEcalRecHitsEB"),
        ecalRecHitsEE = cms.InputTag("reducedEcalRecHitsEE"),
        dumpRecHits = cms.bool(True),
        dumpGeometry   = cms.bool(False),
        dumpIC         = cms.bool(False),
        plotIC         = cms.bool(False),
        dumpTC         = cms.bool(False),
        plotTC         = cms.bool(False),
        dumpADCToGeV   = cms.bool(False),
        dumpTransp     = cms.bool(False),
        plotTransp     = cms.bool(False),
        dumpAlpha      = cms.bool(False),
        plotAlpha      = cms.bool(False),
        dumpTranspCorr = cms.bool(False),
        plotTranspCorr = cms.bool(False),
        dumpChStatus   = cms.bool(False),
        plotChStatus   = cms.bool(False),
        dumpPedestals  = cms.bool(False),
        plotPedestals  = cms.bool(False),
        dumpGainRatios = cms.bool(False),
        plotGainRatios = cms.bool(False)
)

####process.TFileService = cms.Service("TFileService",
####        fileName = cms.string('out_plot.root')
####)

process.p = cms.Path(process.demo)
