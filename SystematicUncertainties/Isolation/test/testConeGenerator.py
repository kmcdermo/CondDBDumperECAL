import FWCore.ParameterSet.Config as cms

process = cms.Process('testConeGenerator')
process.extend(cms.include("FWCore/MessageLogger/data/MessageLogger.cfi"))

process.analysis = cms.EDAnalyzer('TestConeGenerator')

process.source = cms.Source("EmptySource")

process.maxEvents = cms.untracked.PSet( input = cms.untracked.int32( 1000000 ) )

random_gen_service = cms.Service("RandomNumberGeneratorService",
                moduleSeeds = cms.PSet( analysis = cms.untracked.uint32( 123 ) )
                )
process.add_( random_gen_service )

process.analysis.MeanX = cms.double( 0. )
process.analysis.MeanY = cms.double( 0. )
process.analysis.MeanZ = cms.double( 0. )
process.analysis.SigmaX = cms.double( 0.0015 )
process.analysis.SigmaY = cms.double( 0.0015 )
process.analysis.SigmaZ = cms.double( 5.3 )
process.analysis.MinEta = cms.double( -2.5 )
process.analysis.MaxEta = cms.double( 2.5 )
process.analysis.MinPhi = cms.double( 0. )
process.analysis.MaxPhi = cms.double( 3.14159265358979323846 )

process.p = cms.Path( process.analysis )
