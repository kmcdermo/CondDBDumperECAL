from FWCore.ParameterSet.Config import *

process = Process("iso")
process.extend(include("FWCore/MessageLogger/data/MessageLogger.cfi"))
process.MessageLogger.cerr.FwkReport.reportEvery = 50

input_files = vstring()
#for i in range(0,10):
#run_list = [ 0, 1, 2, 3, 4, 6, 7, 8, 9, 10 ]
run_list = [ 347 ]
for i in run_list:
    #input_files.append( "file:/sps/cms/ferri/cmssw/CMSSW_1_6_0/GEN-SIM-DIGI-RECO/CSA07EWK_Zee/CSA07EWK_Zee_%03d.root" % i )
    input_files.append( "root://ccxroot.in2p3.fr:1999//hpss/in2p3.fr/group/cms/users/charlot/cmssw/CMSSW_1_6_0/GEN-SIM-DIGI-RECO/CSA07Higgs_ZZ_4l/CSA07Higgs_ZZ_4l_%03d.root" % i )
    #input_files.append( "rfio:/hpss/in2p3.fr/group/cms/users/charlot/cmssw/CMSSW_1_6_0/GEN-SIM-DIGI-RECO/CSA07Higgs_ZZ_4l/CSA07Higgs_ZZ_4l_%03d.root" % i )

process.source = Source("PoolSource",
    fileNames = untracked( input_files )
)
#process.source = Source("EmptySource")
process.maxEvents = untracked.PSet( input = untracked.int32( -1 ) )

random_gen_service = Service("RandomNumberGeneratorService",
    moduleSeeds = PSet( dumper = untracked.uint32( 123 ) )
)
process.add_( random_gen_service )

process.mcSelector = EDProducer('PdgIdAndStatusCandSelector',
    src     = InputTag('genParticleCandidates'),
    pdgId   = vint32( 11 ),
    status  = vint32( 3 )
)

process.mcFilter = EDFilter('CandCountFilter',
                src = InputTag('mcSelector'),
                minNumber = uint32( 4 )
)

process.cleanTracks = EDProducer("TracksForIsolation",
    srcParticles = InputTag("pixelMatchGsfElectrons"),
    srcTracks = InputTag("ctfWithMaterialTracks")
)

process.dumper = EDAnalyzer("IsoAnalyzer",
    MeanX = double( 0. ),
    MeanY = double( 0. ),
    MeanZ = double( 0. ),
    SigmaX = double( 0.0015 ),
    SigmaY = double( 0.0015 ),
    SigmaZ = double( 5.3 ),
    MinEta = double( -2.5 ),
    MaxEta = double(  2.5 ),
    MinPhi = double( 0. ),
    MaxPhi = double( 3.14159265358979323846 ),
    innerConeRadius = double( 0.015 ),
    outerConeRadius = double( 0.3 ),
    vertexDz = double( 0.1 ),
    vertexDRt = double( 11.5 ),
    ptThreshold = double( 1.5 ),
    pThreshold = double( 0. ),
    kinematicsZZFile = string( "/sps/cms/ferri/cmssw/edmNtuple_kinematics/edmNtuple_kinematics__sps_cms_cms02_charlot_cmssw_CMSSW_1_6_0_GEN-SIM-DIGI-RECO_CSA07Higgs_ZZ_4l_CSA07Higgs_ZZ_4l_ALL.root" ),
    kinematicsZFile = string( "/sps/cms/ferri/cmssw/edmNtuple_kinematics/edmNtuple_kinematics__CSA07EWK_Zee.root" ),
    electronCollection = InputTag("pixelMatchGsfElectrons"),
    isoTrackCollection = InputTag("cleanTracks"),
    outputFile = string( "output.root" ),
    ##srcCleanedTracks = InputTag("ctfWithMaterialTracks"),
    #srcTracks = InputTag("ctfWithMaterialTracks")

     doCutBased = bool( True ),
     doLikelihood = bool( False ),
     doNeuralNet = bool( False ),
 
     # Selection of input variables:
     useEoverPIn      = bool( True ),
     useDeltaEtaIn    = bool( True ),
     useDeltaPhiIn    = bool( True ),
     useHoverE        = bool( True ),
     useE9overE25     = bool( True ),
     useEoverPOut     = bool( True ),
     useDeltaPhiOut   = bool( True ),
     useInvEMinusInvP = bool( False ),
     useBremFraction  = bool( False ),
     useSigmaEtaEta   = bool( True ),
     useSigmaPhiPhi   = bool( True ),
 
     barrelClusterShapeAssociation = InputTag( "hybridSuperClusters","hybridShapeAssoc" ),
     endcapClusterShapeAssociation = InputTag( "islandBasicClusters","islandEndcapShapeAssoc" ),
 
     # Electron quality for cut based ID. Can be:
     # "loose"  - e.g. suitable for H->ZZ->4l
     # "medium" - intermediate quality
     # "tight"  - e.g. suitable for H->WW->2l2nu
     electronQuality = string( "loose" ),
 
     # cut value arrays of form {barrel golden, barrel bigbrem, barrel narrow, barrel showering,
     #                           endcap golden, endcap bigbrem, endcap narrow, endcap showering}
 
     looseEleIDCuts = PSet(
         EoverPInMax    = vdouble( 1.3,   1.2,   1.3,   999.,  999.,  999.,  999.,  999.  ),
         EoverPInMin    = vdouble( 0.9,   0.9,   0.9,   0.6,   0.9,   0.9,   0.9,   0.7   ),
         deltaEtaIn     = vdouble( 0.004, 0.006, 0.005, 0.007, 0.007, 0.008, 0.007, 0.008 ),
         deltaPhiIn     = vdouble( 0.04,  0.07,  0.04,  0.08,  0.06,  0.07,  0.06,  0.07  ),
         HoverE         = vdouble( 0.06,  0.05,  0.06,  0.14,  0.1,   0.1,   0.1,   0.12  ),
         E9overE25      = vdouble( 0.7,   0.75,  0.8,   0.,    0.85,  0.75,  0.8,   0.    ),
         EoverPOutMax   = vdouble( 2.5,   999.,  999.,  999.,  2.,    999.,  999.,  999.  ),
         EoverPOutMin   = vdouble( 0.6,   1.8,   1.,    0.75,  0.6,   1.5,   1.,    0.8   ),
         deltaPhiOut    = vdouble( 0.011, 999.,  999.,  999.,  0.02,  999.,  999.,  999.  ),
         invEMinusInvP  = vdouble( 0.02,  0.02,  0.02,  0.02,  0.02,  0.02,  0.02,  0.02  ),
         bremFraction   = vdouble( 0.,    0.1,   0.1,   0.1,   0.,    0.2,   0.2,   0.2   ),
         sigmaEtaEtaMax = vdouble( 0.011, 0.011, 0.011, 0.011, 0.022, 0.022, 0.022, 0.3   ),
         sigmaEtaEtaMin = vdouble( 0.005, 0.005, 0.005, 0.005, 0.008, 0.008, 0.008, 0.    ),
         sigmaPhiPhiMax = vdouble( 0.015, 999.,  999.,  999.,  0.02,  999.,  999.,  999.  ),
         sigmaPhiPhiMin = vdouble( 0.005, 0.,    0.,    0.,    0.,    0.,    0.,    0.    )
     ),
  
     # For the moment the medium and tight cuts are the same as loose
     mediumEleIDCuts = PSet(
         EoverPInMax    = vdouble( 1.3,   1.2,   1.3,   999.,  999.,  999.,  999.,  999.  ),
         EoverPInMin    = vdouble( 0.9,   0.9,   0.9,   0.6,   0.9,   0.9,   0.9,   0.7   ),
         deltaEtaIn     = vdouble( 0.004, 0.006, 0.005, 0.007, 0.007, 0.008, 0.007, 0.008 ),
         deltaPhiIn     = vdouble( 0.04,  0.07,  0.04,  0.08,  0.06,  0.07,  0.06,  0.07  ),
         HoverE         = vdouble( 0.06,  0.05,  0.06,  0.14,  0.1,   0.1,   0.1,   0.12  ),
         E9overE25      = vdouble( 0.7,   0.75,  0.8,   0.,    0.85,  0.75,  0.8,   0.    ),
         EoverPOutMax   = vdouble( 2.5,   999.,  999.,  999.,  2.,    999.,  999.,  999.  ),
         EoverPOutMin   = vdouble( 0.6,   1.8,   1.,    0.75,  0.6,   1.5,   1.,    0.8   ),
         deltaPhiOut    = vdouble( 0.011, 999.,  999.,  999.,  0.02,  999.,  999.,  999.  ),
         invEMinusInvP  = vdouble( 0.02,  0.02,  0.02,  0.02,  0.02,  0.02,  0.02,  0.02  ),
         bremFraction   = vdouble( 0.,    0.1,   0.1,   0.1,   0.,    0.2,   0.2,   0.2   ),
         sigmaEtaEtaMax = vdouble( 0.011, 0.011, 0.011, 0.011, 0.022, 0.022, 0.022, 0.3   ),
         sigmaEtaEtaMin = vdouble( 0.005, 0.005, 0.005, 0.005, 0.008, 0.008, 0.008, 0.    ),
         sigmaPhiPhiMax = vdouble( 0.015, 999.,  999.,  999.,  0.02,  999.,  999.,  999.  ),
         sigmaPhiPhiMin = vdouble( 0.005, 0.,    0.,    0.,    0.,    0.,    0.,    0.    )
     ),
 
     # For the moment the medium and tight cuts are the same as loose
     tightEleIDCuts = PSet(
         EoverPInMax    = vdouble( 1.3,   1.2,   1.3,   999.,  999.,  999.,  999.,  999.  ),
         EoverPInMin    = vdouble( 0.9,   0.9,   0.9,   0.6,   0.9,   0.9,   0.9,   0.7   ),
         deltaEtaIn     = vdouble( 0.004, 0.006, 0.005, 0.007, 0.007, 0.008, 0.007, 0.008 ),
         deltaPhiIn     = vdouble( 0.04,  0.07,  0.04,  0.08,  0.06,  0.07,  0.06,  0.07  ),
         HoverE         = vdouble( 0.06,  0.05,  0.06,  0.14,  0.1,   0.1,   0.1,   0.12  ),
         E9overE25      = vdouble( 0.7,   0.75,  0.8,   0.,    0.85,  0.75,  0.8,   0.    ),
         EoverPOutMax   = vdouble( 2.5,   999.,  999.,  999.,  2.,    999.,  999.,  999.  ),
         EoverPOutMin   = vdouble( 0.6,   1.8,   1.,    0.75,  0.6,   1.5,   1.,    0.8   ),
         deltaPhiOut    = vdouble( 0.011, 999.,  999.,  999.,  0.02,  999.,  999.,  999.  ),
         invEMinusInvP  = vdouble( 0.02,  0.02,  0.02,  0.02,  0.02,  0.02,  0.02,  0.02  ),
         bremFraction   = vdouble( 0.,    0.1,   0.1,   0.1,   0.,    0.2,   0.2,   0.2   ),
         sigmaEtaEtaMax = vdouble( 0.011, 0.011, 0.011, 0.011, 0.022, 0.022, 0.022, 0.3   ),
         sigmaEtaEtaMin = vdouble( 0.005, 0.005, 0.005, 0.005, 0.008, 0.008, 0.008, 0.    ),
         sigmaPhiPhiMax = vdouble( 0.015, 999.,  999.,  999.,  0.02,  999.,  999.,  999.  ),
         sigmaPhiPhiMin = vdouble( 0.005, 0.,    0.,    0.,    0.,    0.,    0.,    0.    )
     )
    
)

#out_string = vstring()
#out_string.append( "drop *" )
#out_string.append( "keep *_dumper_*_*" )
##out_string.append( "keep *_cleanTracks_*_*" )
#process.out = OutputModule("PoolOutputModule",
#    fileName = untracked.string("edmNtuple_isolation.root"),
#    outputCommands = untracked( out_string )
#)

process.p1 = Path( process.mcSelector * process.mcFilter * process.cleanTracks * process.dumper )
#process.p1 = Path( process.cleanTracks )
#process.p2 = EndPath( process.out )

ofile = open( "process_config.log", "w" )
ofile.write( process.dumpConfig() )
ofile.close()
