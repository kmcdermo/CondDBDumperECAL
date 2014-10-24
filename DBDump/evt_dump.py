import FWCore.ParameterSet.Config as cms
import os

process = cms.Process("Demo")

process.load("FWCore.MessageService.MessageLogger_cfi")

gTag = os.environ.get('GTAG')
if not gTag:
    #gTag = 'GR_R_44_V13::All'
    #gTag = 'FT_R_53_V6::All'
    #gTag = 'GR_P_V42::All'
    #gTag = 'GR_P_V42B::All'
    gTag = 'POSTLS172_V7::All'

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

process.GlobalTag.globaltag = gTag

process.source = cms.Source("PoolSource",
        fileNames = cms.untracked.vstring(
        #"file:/afs/cern.ch/user/a/argiro/public/phaseshift/photongun_pu25_ave20_0_ECALRECOredigi_shift.root"
        "file:/afs/cern.ch/user/a/argiro/public/phaseshift/photongun_pu25_ave20_0_ECALRECOstd_digi.root"
        ),
        #skipEvents = cms.untracked.uint32(10)
)

ofile = "out_dump__" + str(process.GlobalTag.globaltag.value()) + "__"
for i in process.source.fileNames:
        ofile += os.path.basename(i).replace(".root", "") + "__"

ofile = ofile.rstrip("_") + ".dat"


process.maxEvents = cms.untracked.PSet(
        input = cms.untracked.int32(-1)
)

process.demo = cms.EDAnalyzer('EventDump',
        outPlot = cms.bool(False),
        outDump = cms.bool(True),
        #outDumpFile = cms.string('./outdump.dat'),
        outDumpFile = cms.string(ofile),
        #ecalRecHitsEB = cms.InputTag("reducedEcalRecHitsEB"),
        #ecalRecHitsEE = cms.InputTag("reducedEcalRecHitsEE"),
        ecalRecHitsEB = cms.InputTag("ecalRecHit:EcalRecHitsEB"),
        ecalRecHitsEE = cms.InputTag("ecalRecHit:EcalRecHitsEE"),
        ecalUncalibratedRecHitsEB = cms.InputTag("reducedEcalRecHitsEB"),
        ecalUncalibratedRecHitsEE = cms.InputTag("reducedEcalRecHitsEE"),
        #ecalDigisEB   = cms.InputTag("selectDigi:selectedEcalEBDigiCollection"),
        #ecalDigisEE   = cms.InputTag("selectDigi:selectedEcalEEDigiCollection"),
        ecalDigisEB   = cms.InputTag("ecalDigis:ebDigis"),
        ecalDigisEE   = cms.InputTag("ecalDigis:eeDigis"),
        #ecalDigisEB   = cms.InputTag("simEcalDigis:ebDigis"),
        #ecalDigisEE   = cms.InputTag("simEcalDigis:eeDigis"),
        ecalSuperClustersEB = cms.InputTag(""),
        ecalSuperClustersEE = cms.InputTag(""),
        dumpEvent   = cms.bool(True),
        dumpDigis   = cms.bool(True),
        dumpRecHits = cms.bool(True),
        dumpUncalibratedRecHits = cms.bool(False),
        dumpClusters = cms.bool(False),
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

import EventFilter.EcalRawToDigi.EcalUnpackerData_cfi
process.ecalDigis = EventFilter.EcalRawToDigi.EcalUnpackerData_cfi.ecalEBunpacker.clone()

#process.p = cms.Path(process.ecalDigis * process.demo)
process.p = cms.Path(process.demo)
