from FWCore.ParameterSet.Config import *

process = Process('isoHistogrammer')
process.extend( include('FWCore/MessageLogger/data/MessageLogger.cfi') )
process.MessageLogger.cerr.FwkReport.reportEvery = 5000

input_files = vstring()
for i in range(0,5):
        input_files.append( 'file:/sps/cms/cms02/charlot/cmssw/CMSSW_1_6_0/GEN-SIM-DIGI-RECO/CSA07Higgs_ZZ_4l/CSA07Higgs_ZZ_4l_%03d.root' % i )

process.source = Source("PoolSource",
                fileNames = untracked( vstring( 'file:/sps/cms/ferri/cmssw/edmNtuple_isolation_Zee_imass_1.0/edmNtuple_isolation_Zee_imass_1.0__sps_cms_ferri_cmssw_CMSSW_1_6_0_GEN-SIM-DIGI-RECO_CSA07EWK_Zee_CSA07EWK_Zee_ALL.root' ) )
                )
#process.source = cms.Source("EmptySource")
process.maxEvents = untracked.PSet( input = untracked.int32( -1 ) )

s1 = Service('TFileService', fileName = string('histo.root'))
process.add_(s1)

process.dumper = EDAnalyzer('IsoHistogrammer',
        srcClass = InputTag( 'dumper','electronClass' ),
        srcEnergy = InputTag( 'dumper','electronEnergy' ),
        srcIsoC = InputTag( 'dumper','isoElectrons' ),
        srcIsoKC = InputTag( 'dumper','isoKinematicCones' ),
        srcIsoRC = InputTag( 'dumper','isoRandomCones' )
)


#out_string = vstring()
#out_string.append( 'drop *' )
#out_string.append( 'keep *_dumper_*_*' )
#process.out = OutputModule('PoolOutputModule',
#    fileName = untracked.string('edmNtuple_kinematics.root'),
#    outputCommands = untracked( out_string )
#)

process.p1 = Path( process.dumper )
#process.p2 = EndPath( process.out )

ofile = open( 'process_config.log', 'w' )
ofile.write( process.dumpConfig() )
ofile.close()
