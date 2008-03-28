from FWCore.ParameterSet.Config import *

process = Process('kineDumper')
process.extend( include('FWCore/MessageLogger/data/MessageLogger.cfi') )
process.MessageLogger.cerr.FwkReport.reportEvery = 1000

ifile = open('ifile.dat','r')
runlist = []
for l in ifile:
        runlist.append( int(l) )
ifile.close()
#runlist = [ 0 ]

input_files = vstring()
#for i in range(0,100):
for i in runlist:
        input_files.append( 'file:/sps/cms/ferri/cmssw/CMSSW_1_6_0/GEN-SIM-DIGI-RECO/CSA07EWK_Zee/CSA07EWK_Zee_%03d.root' % i )
        #input_files.append( 'file:/sps/cms/cms02/charlot/cmssw/CMSSW_1_6_0/GEN-SIM-DIGI-RECO/CSA07Higgs_ZZ_4l/CSA07Higgs_ZZ_4l_%03d.root' % i )

process.source = Source("PoolSource",
                fileNames = untracked( input_files )
                )
#process.source = cms.Source("EmptySource")
process.maxEvents = untracked.PSet( input = untracked.int32( -1 ) )


process.mcSelector = EDProducer('PdgIdAndStatusCandSelector',
    src     = InputTag('genParticleCandidates'),
    pdgId   = vint32( 11 ),
    status  = vint32( 3 )
)


process.dumper = EDProducer('KineEdmNtupleDumper',
    srcParticles = InputTag('mcSelector')
)


out_string = vstring()
out_string.append( 'drop *' )
out_string.append( 'keep *_dumper_*_*' )
process.out = OutputModule('PoolOutputModule',
    fileName = untracked.string('edmNtuple_kinematics.root'),
    outputCommands = untracked( out_string )
)

process.p1 = Path( process.mcSelector * process.dumper )
process.p2 = EndPath( process.out )

ofile = open( 'process_config.log', 'w' )
ofile.write( process.dumpConfig() )
ofile.close()
