#!/usr/bin/env python

from PhysicsTools.PythonAnalysis import *
from ROOT import *
import os

print 'Loading FWLite...',
gSystem.Load('libFWCoreFWLite.so')
AutoLibraryLoader.enable()
print 'done.'

class eventDisplay:
        'Displays events for isolation studies'
        
        def __init__(self, inputFile):
                print 'Opening file...',
                self.itree_ = TFile.Open( inputFile ).Get('Events')
                print 'done.'
                self.itree_.GetEntry()
                self.iev_ = 0
                self.setElectronBranch()
                #self.gp_ = os.popen('gnuplot -persist','w')
                self.gp_ = os.popen('gnuplot ','w')

        def setElectronBranch(self):
                self.b_electrons_ = self.itree_.GetBranch( self.itree_.GetAlias('pixelMatchGsfElectrons') )
                self.electrons_ = reco.PixelMatchGsfElectronCollection()
                self.b_electrons_.SetAddress( self.electrons_ )

        def setTrackBranch(self):
                self.b_tracks_ = events.GetBranch( self.itree_.GetAlias('ctfWithMaterialTracks') )
                self.tracks_ = reco.Tracks()
                self.b_tracks_.SetAddress( self.tracks_ )

        def displayElectrons(self):
                self.b_electrons_.GetEntry( self.iev_ )
                print len( self.electrons_ ), 'electrons found.'
                tmp = open('tmp_electrons.dat','w')
                for ele in self.electrons_:
                        print ele.eta(), ele.phi(), ele.p()
                        ele_track = ele.gsfTrack()
                        for i in range( ele_track.recHitsSize() ):
                                p = ele_track.recHit(i).localPosition()
                                print ' .. ', p
                                tmp.write( '%f %f %f\n' % (p.x(), p.y(), p.z()) )
                        tmp.write( '\n' )
                tmp.close()
                print >>self.gp_, 'sp \'tmp_electrons.dat\' w p, \'\' w l'
                self.gp_.flush()

        def displayEvent(self, iev):
                self.iev_ = iev
                self.displayElectrons()

file = '/sps/cms/ferri/cmssw/CMSSW_1_6_0/GEN-SIM-DIGI-RECO/CSA07EWK_Zee/CSA07EWK_Zee_000.root'
e = eventDisplay( file )
e.displayEvent( 0 )
