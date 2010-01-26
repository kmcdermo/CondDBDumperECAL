import FWCore.ParameterSet.Config as cms

process = cms.Process("Demo")

process.load("FWCore.MessageService.MessageLogger_cfi")

process.maxEvents = cms.untracked.PSet( input = cms.untracked.int32(-1) )

process.source = cms.Source("PoolSource",
    # replace 'myfile.root' with the source file you want to use
    fileNames = cms.untracked.vstring(
        '/store/data/CRAFT09/Cosmics/RAW/v1/000/110/916/FCC2F612-5B88-DE11-BF6F-0030487D0D3A.root'
        #'file:/tmp/ferriff/FCC2F612-5B88-DE11-BF6F-0030487D0D3A.root'
    )
)

process.andata = cms.EDAnalyzer('Andata',
        EBDigiCollection = cms.InputTag("ecalDigis:ebDigis"),
        EEDigiCollection = cms.InputTag("ecalDigis:eeDigis"),
        outputFile = cms.string("histos.root"),
        verbosity = cms.bool(False)
)

# import the conditions (from DB)
process.load("Configuration.StandardSequences.FrontierConditions_GlobalTag_cff")
process.GlobalTag.globaltag = 'GR09_R_V6A::All'

# import the unpacker
process.load("EventFilter.EcalRawToDigi.EcalUnpackerMapping_cfi")
import EventFilter.EcalRawToDigiDev.EcalUnpackerData_cfi
process.ecalDigis = EventFilter.EcalRawToDigiDev.EcalUnpackerData_cfi.ecalEBunpacker.clone()

# to dump the content of an event in terms of CMSSW objects
process.dump = cms.EDFilter("EventContentAnalyzer")

process.p = cms.Path(
        process.ecalDigis
        * process.andata
        #* process.dump
)
