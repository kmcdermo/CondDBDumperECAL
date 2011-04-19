import FWCore.ParameterSet.Config as cms

process = cms.Process("Demo")

process.load("FWCore.MessageService.MessageLogger_cfi")

gTag = 'GR_R_311_V2::All'
#gTag = 'GR09_31X_V1::All'
#gTag = 'DESIGN_31X_V1::All'
#gTag = 'STARTUP31X_V1::All'
#lTag = 'no_local_tags'
#lTag = 'EcalChannelStatus_CRAFT_offline'
#lTag = 'EcalLaserAPDPNRatios_online'
lTag = 'EcalLaserAPDPNRatios_v3_online'

#process.load("CalibCalorimetry.EcalTrivialCondModules.EcalTrivialCondRetriever_cfi")

process.load('Configuration/StandardSequences/FrontierConditions_GlobalTag_cff')
#process.GlobalTag.globaltag = 'IDEAL_31X::All'
#process.GlobalTag.globaltag = 'MC_31X_V1::All'
#process.GlobalTag.globaltag = 'GR09_31X_V1::All'
process.GlobalTag.globaltag = gTag


process.es_prefer = cms.ESPrefer("PoolDBESSource","ecalConditions")
from CondCore.DBCommon.CondDBSetup_cfi import *
process.ecalConditions = cms.ESSource("PoolDBESSource",
    CondDBSetup,
    siteLocalConfig = cms.untracked.bool(True),
    toGet = cms.VPSet(
        ###cms.PSet(
        ###        record = cms.string('EcalPedestalsRcd'),
        ###        tag = cms.string('EcalPedestals_mc')
        ###), 
        ###cms.PSet(
        ###    record = cms.string('EcalADCToGeVConstantRcd'),
        ###    tag = cms.string('EcalADCToGeVConstant_EBg50_EEwithB_new')
        ###), 
        ###cms.PSet(
        ###    record = cms.string('EcalChannelStatusRcd'),
        ###    #tag = cms.string('EcalChannelStatus_may2009_mc')
        ###    tag = cms.string('EcalChannelStatus_CRAFT_offline')
        ###), 
        ###cms.PSet(
        ###    record = cms.string('EcalGainRatiosRcd'),
        ###    tag = cms.string('EcalGainRatios_mc')
        ###), 
        ###cms.PSet(
        ###    record = cms.string('EcalIntercalibConstantsRcd'),
        ###    tag = cms.string('EcalIntercalibConstants_EBg50_EEwithB_new')
        ###), 
        ###cms.PSet(
        ###    record = cms.string('EcalWeightXtalGroupsRcd'),
        ###    tag = cms.string('EcalWeightXtalGroups_mc')
        ###), 
        ###cms.PSet(
        ###    record = cms.string('EcalTBWeightsRcd'),
        ###    tag = cms.string('EcalTBWeights_mc')
        ###), 
        ###cms.PSet(
        ###    record = cms.string('EcalLaserAlphasRcd'),
        ###    tag = cms.string('EcalLaserAlphas_mc')
        ###), 
        cms.PSet(
            record = cms.string('EcalLaserAPDPNRatiosRcd'),
            ###record = cms.string('EcalLaserAPDPNRatiosRcd'),
            ###tag = cms.string('EcalLaserAPDPNRatios_mc')
            #tag = cms.string('EcalLaserAPDPNRatios_online_hlt')
            ###tag = cms.string('EcalLaserAPDPNRatios_v2_online')
            tag = cms.string('EcalLaserAPDPNRatios_v3_online')
        ), 
        ###cms.PSet(
        ###    record = cms.string('EcalLaserAPDPNRatiosRefRcd'),
        ###    tag = cms.string('EcalLaserAPDPNRatiosRef_mc')
        ###)
        ),
    messagelevel = cms.untracked.uint32(0),
    timetype = cms.untracked.string('timestamp'),
    ##timetype = cms.string('runnumber'),
    #connect = cms.string('frontier://cms_conditions_data/CMS_COND_ECAL'), ##cms_conditions_data/CMS_COND_ECAL"
    ##connect = cms.string('frontier://FrontierPrep/CMS_COND_ECAL_LT'), ##cms_conditions_data/CMS_COND_ECAL"
    #####connect = cms.string('frontier://cms_conditions_data/CMS_COND_31X_ECAL'),
    #####connect = cms.string('oracle://cms_orcon_prod/CMS_COND_31X_ECAL'),
    connect = cms.string('frontier://FrontierProd/CMS_COND_311X_ECAL_LAS'),
    #####connect = cms.string('frontier://FrontierPrep/CMS_COND_ECAL_LT'),
    ###connect = cms.string('oracle://cms_orcoff_prep/CMS_COND_ECAL_LT'),
    # at P5
    ##connect = cms.string('oracle://cms_orcon_prod/CMS_COND_ECAL_LT'),

    authenticationMethod = cms.untracked.uint32(1)
)
#process.CondDBCommon.DBParameters.authenticationPath = '/nfshome0/fra/CMSSW_3_1_0_pre9/src/CondTools/Ecal/python'
#process.CondDBCommon.DBParameters.authenticationPath = '/nfshome0/fra/CMSSW_3_1_0/src/CondTools/Ecal/python'



process.maxEvents = cms.untracked.PSet( input = cms.untracked.int32(1) )

process.source = cms.Source("EmptySource",
        firstRun = cms.untracked.uint32(96888),
        numberEventsInRun = cms.untracked.uint32(1),
        #firstTime = cms.untracked.uint32( 1244042540 ), # run 98723
        #firstTime = cms.untracked.uint32( 1224557391 ), # beginning of run 66951
        #firstTime = cms.untracked.uint32( 1224188905 ), # beginning of run 66615
        ##firstTime = cms.untracked.uint32( 1224147420 ), # inside t1 and t2 of a DB IOV before run 65940
        ##firstTime = cms.untracked.uint32( 1224146163 ),
        firstTime = cms.untracked.uint32( (1303120805 - 10*3600) ),
        timeBetweenEvents = cms.untracked.uint32(10)
)

process.demo = cms.EDAnalyzer('DBDump',
        outPlot = cms.bool(True),
        outDump = cms.bool(True),
        outDumpFile = cms.string('out_dump_' + gTag + '_' + lTag + '.log'),
        outPlotFile = cms.string('out_plot_' + gTag + '_' + lTag + '.root'),
        dumpIC         = cms.bool(False),
        plotIC         = cms.bool(False),
        dumpTC         = cms.bool(False),
        plotTC         = cms.bool(False),
        dumpADCToGeV   = cms.bool(False),
        dumpTransp     = cms.bool(True),
        plotTransp     = cms.bool(False),
        dumpTranspCorr = cms.bool(True),
        plotTranspCorr = cms.bool(True),
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
