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

process.cleanTracks = EDProducer('TracksForIsolation',
    srcParticles = InputTag("pixelMatchGsfElectrons"),
    srcTracks = InputTag("ctfWithMaterialTracks")
    #productName = 'cleanTracks'
)

out_string = vstring()
out_string.append( 'drop *' )
out_string.append( 'keep *_dumper_*_*' )
out_string.append( 'keep *_pixelMatchGsfElectrons_*_*' )
out_string.append( 'keep *_pixelMatchGsfFitBarrel_*_*' )
out_string.append( 'keep *_pixelMatchGsfFitEndcap_*_*' )
out_string.append( 'keep *_ctfWithMaterialTracks_*_*' )
process.out = OutputModule('PoolOutputModule',
    fileName = untracked.string('track_isolation.root'),
    outputCommands = untracked( out_string )
)

process.p1 = Path( process.cleanTracks )
process.p2 = EndPath( process.out )

ofile = open( 'process_config.log', 'w' )
ofile.write( process.dumpConfig() )
ofile.close()
