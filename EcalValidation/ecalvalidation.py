import FWCore.ParameterSet.Config as cms

process = cms.Process("Validation")
process.source = cms.Source("PoolSource",
    fileNames = cms.untracked.vstring('file:/data/ferriff/data/relval_206/001E9C4B-E819-DD11-A016-0019B9F72CE5.root')
)

process.maxEvents = cms.untracked.PSet(
    input = cms.untracked.int32(-1)
)
process.MessageLogger = cms.Service("MessageLogger")

process.validate = cms.EDFilter("EcalValidation",
    superClusterCollection_EB = cms.InputTag("correctedHybridSuperClusters"),
    superClusterCollection_EE = cms.InputTag("correctedMulti5x5SuperClustersWithPreshower"),
    basicClusterCollection_EE = cms.InputTag("multi5x5BasicClusters","multi5x5EndcapBasicClusters"),
    recHitCollection_EE = cms.InputTag("ecalRecHit","EcalRecHitsEE"),
    recHitCollection_EB = cms.InputTag("ecalRecHit","EcalRecHitsEB"),
    basicClusterCollection_EB = cms.InputTag("hybridSuperClusters","hybridBarrelBasicClusters")
)

process.TFileService = cms.Service("TFileService",
    fileName = cms.string('histo_elept35_300pre6.root')
)

process.p = cms.Path(process.validate)
process.PoolSource.fileNames = [
        '/store/relval/CMSSW_3_0_0_pre7/RelValSingleElectronPt35/GEN-SIM-RECO/IDEAL_30X_v1/0006/2C35C79B-37E8-DD11-AC1D-000423D6CAF2.root',
        '/store/relval/CMSSW_3_0_0_pre7/RelValSingleElectronPt35/GEN-SIM-RECO/IDEAL_30X_v1/0006/ACCDE581-6AE8-DD11-B2D6-001D09F2B2CF.root'
]
