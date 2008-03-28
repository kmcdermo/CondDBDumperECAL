#!/usr/bin/env python

from PhysicsTools.PythonAnalysis import *
from ROOT import *
import math as m

print 'Loading FWLite...'
gSystem.Load('libFWCoreFWLite.so')
AutoLibraryLoader.enable()
print 'done.'

gSystem.Load('libDataFormatsMath.so')
gSystem.Load('libSystematicUncertaintiesIsolation.so')
#gSystem.Load('libDataFormatsTrackReco.so')
#gSystem.Load('libDataFormatsEgammaCandidates.so')
gSystem.Load('libEgammaAnalysisElectronIDAlgos.so')

class IsoRes:

        def __init__(self):
                self.nTracks = 0
                self.ptSum = 0.

        def display(self):
                print '..ptSum   = ', self.ptSum
                print '..nTracks = ', self.nTracks

class IsoStats:

        def __init__(self):
                self.pt   = 0.
                self.dr   = 0.
                self.dz   = 0.
                self.dRt  = 0.
                self.evts = 0.

        def display(self):
                print '------------------------------------'
                print 'Isolation statistics: events failing'
                print 'pt  = ', self.pt / self.evts
                print 'dr  = ', self.dr / self.evts
                print 'dz  = ', self.dz / self.evts
                print 'dRt = ', self.dRt / self.evts
                print '------------------------------------'


class IsoCone:
        
        def __init__(self):
                self.iradius_ = 0.015
                self.oradius_ = 0.3
                self.dz_      = 0.1
                self.dRt_     = 25. # not used - uncomment below
                self.ptThr_   = 1.5
                self.dir_     = math.XYZVector(0,1,0)
                self.pos_     = math.XYZPoint(0,0,0)
                self.isostats = IsoStats()
                
        #def getIsolation( self, tracks, iel ):
        def getIsolation( self, tracks ):
                ir = IsoRes()
                for itk in tracks:
                        self.isostats.evts += 1
                        dr  = ROOT.Math.VectorUtil.DeltaR( itk.momentum(), self.dir_ )
                        h_deltaR.Fill( dr )
                        dz  = m.fabs( self.pos_.z() - itk.vz() )
                        dRt = itk.innerPosition().R()
                        cont = False
                        if itk.pt() < self.ptThr_:
                                self.isostats.pt += 1
                                cont = True
                        if dr < self.iradius_ or dr > self.oradius_:
                                self.isostats.dr += 1
                                cont = True
                        if dz > self.dz_:
                                self.isostats.dz += 1
                                cont = True
                        #if dRt > self.dRt_:
                        #        self.isostats.dRt += 1
                        #        cont = True
                        if cont: continue

                        #p1 = itk.momentum()
                        #p2 = iel.momentum()
                        #dr = ROOT.Math.VectorUtil.DeltaR( p1, p2 )
                        #imass = 2. * itk.p() * iel.p() * ( 1. - ROOT.Math.VectorUtil.CosTheta( p1, p2 ))
                        #if imass < 1.:
                        #        print 'iel', iel.p(),iel.pt(), '-', iel.px(), iel.py(), iel.pz()
                        #        print 'itk', itk.p(),itk.pt(), '-', itk.px(), itk.py(), itk.pz()
                        #        print '...dr', dr, 'imass', imass
                        
                        ir.ptSum += itk.pt()
                        ir.nTracks += 1
                return ir
        

iso    = IsoCone()
isoRC  = IsoCone()
isoRKC = IsoCone()

coneGen = ConeGenerator()
coneGen.setVertexSigma( 0.0015, 0.0015, 5.3 )
coneGen.setVertexMean( 0., 0., 0. )
coneGen.setEtaRange( -2.5, 2.5 )
coneGen.setPhiRange( 0., m.pi )

###coneGen.loadKineCones('/sps/cms/ferri/cmssw/edmNtuple_kinematics/edmNtuple_kinematics__sps_cms_cms02_charlot_cmssw_CMSSW_1_6_0_GEN-SIM-DIGI-RECO_CSA07Higgs_ZZ_4l_CSA07Higgs_ZZ_4l_ALL.root')
#coneGen.loadKineCones('/sps/cms/ferri/cmssw/edmNtuple_kinematics/edmNtuple_kinematics__CSA07EWK_Zee.root')
coneGen.init()

## cone generator test
#h_x = TH1F("x","x",1000,-0.5,0.5);
#h_y = TH1F("y","y",1000,-0.5,0.5);
#h_z = TH1F("z","z",1000,-250.,250.);
#h_eta = TH1F("eta","eta",100,-2.7,2.7);
#h_phi = TH1F("phi","phi",100,-0.5,3.6415);
#
#for i in range(10000):
#        if i % 1000 == 0 : print i
#        cone = coneGen.randomCone()
#        h_x.Fill( cone.second.x() )
#        h_y.Fill( cone.second.y() )
#        h_z.Fill( cone.second.z() )
#        h_eta.Fill( cone.first.eta() )
#        h_phi.Fill( cone.first.phi() )

# histograms
global h_deltaR
h_deltaR = TH1F('h_deltaR','h_deltaR',100,0.,10.)
h_iso    = TH1F('h_iso','h_iso',100,0.,50.)
h_isoRC  = TH1F('h_isoRC','h_isoRC',100,0.,50.)
h_isoRKC  = TH1F('h_isoRKC','h_isoRKC',100,0.,50.)

# get data
print 'getting event tree...'
#events = EventTree('track_isolation.root')
#########events = EventTree('/sps/cms/ferri/cmssw/isolationTracks_Zee/isolationTracks_Zee__sps_cms_ferri_cmssw_CMSSW_1_6_0_GEN-SIM-DIGI-RECO_CSA07EWK_Zee_CSA07EWK_Zee_ALL.root')
#events = EventTree('/sps/cms/ferri/cmssw/isolationTracks_Zee/isolationTracks_Zee__sps_cms_ferri_cmssw_CMSSW_1_6_0_GEN-SIM-DIGI-RECO_CSA07EWK_Zee_CSA07EWK_Zee_ALL.root')
#events = EventTree('/sps/cms/ferri/cmssw/isolationTracks_HZZ4e/isolationTracks_HZZ4e__sps_cms_baffioni_CMSSW_CMSSW_1_6_0_GEN-SIM-DIGI-RECO_CSA07Higgs_H150_ZZ_4l_CSA07Higgs_H150_ZZ_4l_ALL.root')
#events = EventTree('/sps/cms/ferri/cmssw/isolationTracks_ZZ4l/isolationTracks_ZZ4l__sps_cms_cms02_charlot_cmssw_CMSSW_1_6_0_GEN-SIM-DIGI-RECO_CSA07Higgs_ZZ_4l_CSA07Higgs_ZZ_4l_ALL.root')
#events = EventTree('')

#events = EventTree('/sps/cms/ferri/cmssw/CMSSW_1_6_0/GEN-SIM-DIGI-RECO/CSA07EWK_Zee/CSA07EWK_Zee_000.root')
events = EventTree('root://ccxroot.in2p3.fr:1999//hpss/in2p3.fr/group/cms/users/charlot/cmssw/CMSSW_1_6_0/GEN-SIM-DIGI-RECO/CSA07Higgs_ZZ_4l/CSA07Higgs_ZZ_4l_347.root')
print 'done.'

# save output variables to text file
ascii_ntu = []

# event loop
iev = 0
for event in events:
        if iev > 300: break
        #if iev % 500 == 0: print iev
        if iev % 1 == 0: print iev
        #print event.EventAuxiliary.id_.run_, event.EventAuxiliary.id_.event_
        
        electrons = event.pixelMatchGsfElectrons
        tracks = event.ctfWithMaterialTracks
        #tracks = event.pixelMatchGsfFitTracks
        event_vtx = math.XYZPoint(0.,0.,0.)
        cleanedTracks = reco.TrackCollection()
        cnt = 0
        for iel in electrons:
                # try electron id
                to_take = {}
                for i in range(tracks.size()):
                        itk = tracks[i]
                        p1 = itk.momentum()
                        p2 = iel.momentum()
                        dr = ROOT.Math.VectorUtil.DeltaR( p1, p2 )
                        imass = 2. * itk.p() * iel.p() * ( 1. - ROOT.Math.VectorUtil.CosTheta( p1, p2 ))
                        ## take tracks with imass cut
                        if imass > 1. and dr < 2. :
                                to_take[ i ] = True
                        elif imass < 1.:
                                print itk.numberOfLostHits(), itk.numberOfValidHits(), itk.normalizedChi2()
                        ###elif imass < 1.:
                        ###        print 'event number', iev
                        ###        print '..electron track momentum', iel.p(), "-", iel.px(), iel.py(), iel.pz()
                        ###        print '....track momentum', itk.p(), "-", itk.px(), itk.py(), itk.pz()
                        ###        print '....imass', imass
                        ###        print '....dr', ROOT.Math.VectorUtil.DeltaR( itk.momentum(), iel.momentum() )
                        ## take everything
                        #to_take[ i ] = True

                for i in to_take.keys():
                        cleanedTracks.push_back( tracks[i] )
                
                # set cone direction
                iso.dir_ = iel.momentum()
                # set cone position
                iso.pos_ = iel.gsfTrack().referencePoint()
                if iel.momentum().R() > 10. : event_vtx = iso.pos_
                # compute isolation
                ir = iso.getIsolation( cleanedTracks )
                ##ir = iso.getIsolation( cleanedTracks, iel )
                h_iso.Fill( ir.ptSum )


                print iel.momentum().x(), iel.momentum().y(), iel.momentum().z(), ' -- ', ir.ptSum, ir.nTracks

                #ir.display()
                ntu = []
                ntu.append( iev )
                ntu.append( 0 )
                ntu.append( cnt )
                ntu.append( ir.ptSum )
                ntu.append( ir.nTracks )
                ntu.append( iso.dir_.eta() )
                ntu.append( iso.dir_.phi() )
                ntu.append( iel.classification() )
                ascii_ntu.append( ntu )
                cnt += 1
        #######cnt = 0
        #######for irc in range(len(electrons)):
        #######        cone = coneGen.randomCone()
        #######        # set cone direction
        #######        d = cone.first
        #######        isoRC.dir_ = math.XYZVector( d.x(), d.y(), d.z() )
        #######        # set cone position
        #######        isoRC.pos_ = event_vtx
        #######        # compute isolation
        #######        ir = isoRC.getIsolation( cleanedTracks )
        #######        h_isoRC.Fill( ir.ptSum )
        #######        ntu = []
        #######        ntu.append( iev )
        #######        ntu.append( 1 )
        #######        ntu.append( cnt )
        #######        ntu.append( ir.ptSum )
        #######        ntu.append( ir.nTracks )
        #######        ntu.append( isoRC.dir_.eta() )
        #######        ntu.append( isoRC.dir_.phi() )
        #######        ntu.append( -1 )
        #######        ascii_ntu.append( ntu )
        #######        cnt += 1
        #######cnt = 0
        ########cones = coneGen.fourKinematicCones()
        #######cones = coneGen.kinematicCones()
        #######for cone in cones:
        #######        # set cone direction
        #######        d = cone.first
        #######        isoRKC.dir_ = math.XYZVector( d.x(), d.y(), d.z() )
        #######        # set cone position
        #######        isoRKC.pos_ = event_vtx
        #######        # compute isolation
        #######        ir = isoRKC.getIsolation( cleanedTracks )
        #######        h_isoRKC.Fill( ir.ptSum )
        #######        ntu = []
        #######        ntu.append( iev )
        #######        ntu.append( 2 )
        #######        ntu.append( cnt )
        #######        ntu.append( ir.ptSum )
        #######        ntu.append( ir.nTracks )
        #######        ntu.append( isoRKC.dir_.eta() )
        #######        ntu.append( isoRKC.dir_.phi() )
        #######        ntu.append( -1 )
        #######        ascii_ntu.append( ntu )
        #######        cnt += 1
        iev += 1

import cPickle
ofile = open('output_test.dat','w')
pickle = cPickle.Pickler( ofile )
pickle.dump( ascii_ntu )
ofile.close()

iso.isostats.display()
isoRC.isostats.display()
isoRKC.isostats.display()

# plotting
h_iso.SetLineColor(2)
h_iso.SetLineColor(4)
h_iso.DrawNormalized()
h_isoRC.DrawNormalized('same')
