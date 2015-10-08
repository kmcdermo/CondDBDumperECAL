## Tools for dumping CMS database payloads

#### Package contents
   * a `conddb_dumper` executable that can be used to dump (mostly ECAL)
     conditions in txt files from a given object-specific tag, with several options
   * a general plugin for dumping several conditions at the same time/run
     from a given Global Tag, via cmsRun + configuration file.
   * a general plugin for dumping ECAL events for closer inspection
   * some utilities developed for the monitoring of the ECAL laser monitoring corrections

#### Dumper list
   * `conddb_dumper.cpp`: self-explicative via the `-h/--help` options. Currently supported objects: 
      * `BeamSpotObjects`
      * `ESEEIntercalibConstants`
      * `ESGain`
      * `ESIntercalibConstants`
      * `EcalADCToGeVConstant`
      * `EcalChannelStatus`
      * `EcalClusterLocalContCorrParameters`
      * `EcalIntercalibConstants`
      * `EcalIntercalibConstantsMC`
      * `EcalLaserAlphas`
      * `EcalPedestals`
      * `EcalTPGLinearizationConst`
      * `EcalTimeCalibConstants`
      * `EcalTimeOffsetConstant`
      * `RunInfo`
   * `lava_db.cpp`: validate a tag of the ECAL monitoring corrections
   * `lava_text.cpp`: validate a set of ECAL monitoring corrections starting from a txt file with dumped-corrections
   * `lava_db_compare.cpp`: compare two tags of ECAL monitoring corrections
   * `lava_db_cond.cpp`: for ECAL monitoring corrections, compare the content
                         of a tag to the content of another tag, by matching the IOV of one tag to the
                         closest IOV of the other tag
   * `lava_db_dumpId.cpp`: dump ECAL monitoring correction histories for a given set of DetId (or all of them)
   * `lava_db2txt.cpp`: convert the content of ECAL monitoring corrections to a txt file
   * `merge_dump.cpp`: merges different txt files of ECAL monitoring
                       corrections with overlapping IOV into one single
                       coherent txt file (N.B. does not need the CMSSW environment to work)

#### Dumper setup
Setup a working area for example in `CMSSW_7_4_0_pre5`. Any release `>=7XY`
should work just fine, contact me in case not.
```bash
cmsrel CMSSW_7_4_6_patch2
cd CMSSW_7_4_6_patch2/src
cmsenv
git cms-init
git clone git@github.com:ferriff/usercode.git
git cms-merge-topic -u ferriff:ecal_calib_tools
cd usercode/
scram b
```

Example of dump of one object:
```bash
conddb_dumper -O EcalIntercalibConstants -c frontier://FrontierProd/CMS_CONDITIONS -t EcalIntercalibConstants_2012ABCD_offline
```

#### Additional documentation
   * https://twiki.cern.ch/twiki/bin/view/CMS/DBDump
