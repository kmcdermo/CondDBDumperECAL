from FWCore.ParameterSet.Config import *

process = Process('isoAnalyzer')
process.extend(include("FWCore/MessageLogger/data/MessageLogger.cfi"))
process.MessageLogger.cerr.FwkReport.reportEvery = 50

input_files = vstring()
#for i in range(0,10):
#run_list = [ 0, 1, 2, 3, 4, 6, 7, 8, 9, 10 ]
run_list = [ 0 ]
for i in run_list:
    input_files.append( 'file:/sps/cms/ferri/cmssw/CMSSW_1_6_0/GEN-SIM-DIGI-RECO/CSA07EWK_Zee/CSA07EWK_Zee_%03d.root' % i )

process.source = Source("PoolSource",
    fileNames = untracked( input_files )
)
#process.source = Source("EmptySource")
process.maxEvents = untracked.PSet( input = untracked.int32( -1 ) )

random_gen_service = Service("RandomNumberGeneratorService",
    moduleSeeds = PSet( dumper = untracked.uint32( 123 ) )
)
process.add_( random_gen_service )

process.cleanTracks = EDProducer('TracksForIsolation',
    srcParticles = InputTag("pixelMatchGsfElectrons"),
    srcTracks = InputTag("ctfWithMaterialTracks")
    #productName = 'cleanTracks'
)

process.dumper = EDProducer('IsoEdmNtupleDumper',
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
    outerConeRadius = double( 0.25 ),
    vertexDz = double( 0.1 ),
    vertexDRt = double( 11.5 ),
    ptThreshold = double( 1.5 ),
    pThreshold = double( 0. ),
    kinematicsFile = string( "/sps/cms/ferri/cmssw/edmNtuple_kinematics/edmNtuple_kinematics__sps_cms_cms02_charlot_cmssw_CMSSW_1_6_0_GEN-SIM-DIGI-RECO_CSA07Higgs_ZZ_4l_CSA07Higgs_ZZ_4l_ALL.root" ),
    srcParticles = InputTag("pixelMatchGsfElectrons"),
    srcCleanedTracks = InputTag("cleanTracks"),
    #srcCleanedTracks = InputTag("ctfWithMaterialTracks"),
    srcTracks = InputTag("ctfWithMaterialTracks")
)

out_string = vstring()
out_string.append( 'drop *' )
out_string.append( 'keep *_dumper_*_*' )
#out_string.append( 'keep *_cleanTracks_*_*' )
process.out = OutputModule('PoolOutputModule',
    fileName = untracked.string('edmNtuple_isolation.root'),
    outputCommands = untracked( out_string )
)

process.p1 = Path( process.cleanTracks * process.dumper )
#process.p1 = Path( process.cleanTracks )
process.p2 = EndPath( process.out )

ofile = open( 'process_config.log', 'w' )
ofile.write( process.dumpConfig() )
ofile.close()
