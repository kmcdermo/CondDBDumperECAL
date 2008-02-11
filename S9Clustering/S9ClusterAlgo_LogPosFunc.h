///////////////////////////////////////////////////////////////////
//                                                               //
//         Log Position Computing for S9ClusterAlgo              //
//                                                               //
// Author: Julien DESCAMPS, CEA Saclay DAPNIA/SPP, Sept 2007     //
//                                                               //
///////////////////////////////////////////////////////////////////


#ifndef RecoEcal_EcalClusterAlgos_S9ClusterAlgo_LogPosFunc_h
#define RecoEcal_EcalClusterAlgos_S9ClusterAlgo_LogPosFunc_h

#include "DataFormats/EcalRecHit/interface/EcalRecHit.h"
#include "DataFormats/EcalRecHit/interface/EcalRecHitCollections.h"
#include "DataFormats/EcalDetId/interface/EBDetId.h"
#include "DataFormats/EgammaReco/interface/BasicCluster.h"
#include "DataFormats/EcalRecHit/interface/EcalRecHitCollections.h"
#include "Geometry/CaloGeometry/interface/CaloSubdetectorGeometry.h"
#include "Geometry/CaloGeometry/interface/CaloGeometry.h"
#include "FWCore/Framework/interface/ESHandle.h"
#include "DataFormats/Math/interface/Point3D.h"
#include "RecoEcal/EgammaClusterAlgos/interface/S9ClusterAlgo.h"
#include <string>
#include <vector>

#include "TMath.h"
#include "TVector3.h"


double theta(double etap)
{
  return 2.0*atan(exp(-etap));
}

double phi2PI(float phi)
{

  return (phi>=0)? phi : 2*M_PI+phi;
}



void ComputeLogPos(Sum9GC *mySum9,double X0,double W0,double T0)
{

  double w[25]={-1.} ; 
  double w0=W0;
  double sumwi=0. ; 

  double sumxi=0;
  double sumyi=0;
  double CalPosX[25]={0.};
  double CalPosY[25]={0.};
  
  TVector3 Vu;
  TVector3 Vv;
  TVector3 Vw;
  TVector3 Vcor;
 
  double ClusRadius = 0;
  int nXtals        = 9;
  double ECluster   = mySum9->E;

    
    
  for(int i=0 ; i<nXtals ; i++) { 
      

    if(mySum9->S9Cells[i].FlagUsed == true)
      continue;

  
    double Ru = sqrt(pow(mySum9->S9Cells[i].X,2)+pow(mySum9->S9Cells[i].Y,2)+pow(mySum9->S9Cells[i].Z,2));
    double Rv = X0*(log(ECluster) + T0);
   
  
    Vu.SetMagThetaPhi(Ru,theta(mySum9->S9Cells[i].Eta),(mySum9->S9Cells[i].Phi));
    
    if(mySum9->S9Cells[i].iEta>=80 && mySum9->S9Cells[i].iEta<=89)
      {
	if(mySum9->S9Cells[i].iEta>=80 && mySum9->S9Cells[i].iEta<=84)
	  {
	  
	  
	    Vv.SetMagThetaPhi(Rv,theta(mySum9->S9Cells[i].Eta)-((theta(mySum9->S9Cells[i].Eta) - (( 3*M_PI*abs(mySum9->S9Cells[i].iEta-84)/(180*5)))) - M_PI/2.),(mySum9->S9Cells[i].Phi)- 3.*M_PI/180.);
	 
	  }
	else
	  {
	
	    Vv.SetMagThetaPhi(Rv,theta(mySum9->S9Cells[i].Eta)+(M_PI/2.-(theta(mySum9->S9Cells[i].Eta) + (( 3*M_PI*(mySum9->S9Cells[i].iEta-85)/(180*5))))) ,(mySum9->S9Cells[i].Phi)+ 3.*M_PI/180.);
	 
	
	  }
      }
    else
      if(mySum9->S9Cells[i].Eta>0)
	Vv.SetMagThetaPhi(Rv,theta(mySum9->S9Cells[i].Eta)+ 3*M_PI/180.,(mySum9->S9Cells[i].Phi)+ 3*M_PI/180.);
      else
	Vv.SetMagThetaPhi(Rv,theta(mySum9->S9Cells[i].Eta)- 3*M_PI/180.,(mySum9->S9Cells[i].Phi)- 3*M_PI/180.);

    Vw=Vu+Vv;
   
    CalPosX[i] = Vw.Eta();
    CalPosY[i] = Vw.Phi();  
  
  
 
    if(i==0)
      ClusRadius = Vw.Mag();

    if(mySum9->S9Cells[i].E>0)
      {
	w[i] = w0 + TMath::Log(mySum9->S9Cells[i].E/ECluster) ;
     
      }
    else
     w[i]=0;

    if(w[i]<0.) continue ; 
    sumwi += w[i] ; 


   
  } 

  for(int i=0 ; i<nXtals ; i++) { 
  
    if(w[i]<0.) continue ; 
    sumxi += CalPosX[i]*w[i];
    
    if((CalPosY[i] - mySum9->S9Cells[0].Phi)  > M_PI) 
      CalPosY[i] -= 2*M_PI;
    if((CalPosY[i] - mySum9->S9Cells[0].Phi) < - M_PI) 
      CalPosY[i] += 2*M_PI;
    
    sumyi += CalPosY[i]*w[i];
 
   
  } 
 
  
 
  mySum9->PosEta = sumxi/sumwi;
  mySum9->PosPhi = sumyi/sumwi;
 
  if(mySum9->PosPhi > 2*M_PI)
    mySum9->PosPhi -= 2*M_PI;
    
  if(mySum9->PosPhi < 0)
    mySum9->PosPhi += 2*M_PI;


  
  Vcor.SetMagThetaPhi(ClusRadius,theta(mySum9->PosEta),mySum9->PosPhi);

  mySum9->PosX = Vcor.X();
  mySum9->PosY = Vcor.Y();
  mySum9->PosZ = Vcor.Z();

}



void ComputeLogPos(SuperClusterGC *mySC,double X0,double W0,double T0)
{
  double sinphi=0.;
  double cosphi=0.;
  double E_uncor=0.;
  double theta_=0.;
  double phi_=0.;
  double eta_=0.;
  
 
  for(int i=0 ; i<mySC->nSum9 ; i++) { 
    
    // cout << mySC->Sum9[i].PosPhi << endl;

  
    ComputeLogPos(&(mySC->Sum9[i]),X0,W0,T0);
    E_uncor+=mySC->Sum9[i].E;
    sinphi+=mySC->Sum9[i].E*sin(mySC->Sum9[i].PosPhi);
    cosphi+=mySC->Sum9[i].E*cos(mySC->Sum9[i].PosPhi);
    theta_+=mySC->Sum9[i].E*theta(mySC->Sum9[i].PosEta);
  }


  if(E_uncor>0.)
    {
     
     
      sinphi/=E_uncor;
      cosphi/=E_uncor;
      theta_/=E_uncor;

      // Solve 2*PI wrap problem and stay most linear


      if(cosphi>=M_SQRT1_2)
	{
	  if(sinphi>0)
	    phi_=asin(sinphi);
	  else
	    phi_=2*M_PI+asin(sinphi);
	}
      else if(cosphi<-M_SQRT1_2)
	phi_=M_PI-asin(sinphi);
      else if(sinphi>0)
	phi_=acos(cosphi);
      else
	phi_=2*M_PI-acos(cosphi);
    }

  eta_ = -log(tan(theta_/2.));

  mySC->PosEta =  eta_;
  mySC->PosPhi =  phi2PI(phi_);
  
  double ClusRadius = sqrt(pow(mySC->Sum9[0].PosX,2) + pow(mySC->Sum9[0].PosY,2) + pow(mySC->Sum9[0].PosZ,2));
  
  TVector3 Vcor;

  Vcor.SetMagThetaPhi(ClusRadius,theta(mySC->PosEta),mySC->PosPhi);

  mySC->PosX = Vcor.X();
  mySC->PosY = Vcor.Y();
  mySC->PosZ = Vcor.Z();


}

#endif
