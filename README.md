## Tools for dumping CMS database payloads

Package contents:
   * a `conddb_dumper` executable that can be used to dump (mostly ECAL)
     conditions in txt files from a given object-specific tag, with several options
   * a general plugin for dumping several conditions at the same time/run
     from a given Global Tag, via cmsRun + configuration file.
   * a general plugin for dumping ECAL events for closer inspection
   * some utilities developed for the monitoring of the ECAL laser monitoring corrections

Dumper list:
   * `conddb_dumper.cpp`: self-explicative via the `-h/--help` options. Currently supported objects: 
      * `BeamSpotObjects`
      * `ESEEIntercalibConstants`
      * `ESGain`
      * `ESIntercalibConstants`
      * `EcalADCToGeVConstant`
      * `EcalChannelStatus`
      * `EcalClusterLocalContCorrParameters`
      * `EcalIntercalibConstants`
      * `EcalLaserAlphas`
      * `EcalPedestals`
      * `EcalTPGLinearizationConst`
      * `EcalTimeCalibConstants`
      * `EcalTimeOffsetConstant`
      * `RunInfo`
   * `lava_db.cpp`: validate a tag of the ECAL monitoring corrections
   * `lava_db_compare.cpp`: compare two tags of ECAL monitoring corrections
   * `lava_db_cond.cpp`: for ECAL monitoring corrections, compare the content
                         of a tag to the content of another tag, by matching the IOV of one tag to the
                         closest IOV of the other tag
   * `lava_db_dumpId.cpp`: dump ECAL monitoring correction histories for a given set of DetId (or all of them)
   * `lava_text.cpp`: convert the content of ECAL monitoring corrections to a txt file
   * `merge_dump.cpp`: merges different txt files of ECAL monitoring
                       corrections with overlapping IOV into one single
                       coherent txt file (N.B. does not need the CMSSW environment to work)
