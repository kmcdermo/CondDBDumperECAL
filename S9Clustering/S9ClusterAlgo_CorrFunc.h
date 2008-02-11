///////////////////////////////////////////////////////////////////
//                                                               //
//           Correction Functions for S9ClusterAlgo              //
//                                                               //
// Author: Julien DESCAMPS, CEA Saclay DAPNIA/SPP, Sept 2007     //
//                                                               //
///////////////////////////////////////////////////////////////////


#ifndef RecoEcal_EcalClusterAlgos_S9ClusterAlgo_CorrFunc_h
#define RecoEcal_EcalClusterAlgos_S9ClusterAlgo_CorrFunc_h

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



double LocalContGapsFunc(double rlog,char* angle)
{
  double retval=1.;
  
  if(angle == "Phi")
    {
 

      if(rlog<=0 && rlog>=-3.2)
	{ 
	  retval =  0.985491 + 0.000656*rlog + 0.006111 *pow(rlog,2) +  0.001441*pow(rlog,3);
      
	}
      else
	if(rlog> 0 && rlog<= 3.2)
	  {
	    retval =  0.979954+ 0.007699*rlog + 0.001732*pow(rlog,2) -0.000695 *pow(rlog,3);
	  }
  
    }
  else
    if(angle == "Eta")
      {
	

	if(rlog<=0 && rlog>=-3.2)
	  { 
	    retval =  0.985491 + 0.000656*rlog + 0.006111 *pow(rlog,2) +  0.001441*pow(rlog,3);
      
	  }
	else
	  if(rlog> 0 && rlog<= 3.2)
	    {
	      retval =  0.979954+ 0.007699*rlog + 0.001732*pow(rlog,2) -0.000695 *pow(rlog,3);
	    }

       
      }
    else
      std::cout << "  S9ClusterAlgo_CorrFunc:: Wrong angle !! " << std::endl;

     

  return retval;
}



double LocalContBeforeCrackFunc(double rlog,char* angle)
{
  double retval=1.;
 

  if(angle == "Phi")
    {
      if(rlog<=0 && rlog>=-3.2)
	{ 
      
	  retval =   0.990694+ 0.010959 *rlog +   0.013447*pow(rlog,2) +  0.003052*pow(rlog,3);
	}
      else
	if(rlog>0 && rlog<= 3.2) 
	  {
      
	    retval =  0.558341 + 0.411432*(rlog+0.3)    -0.135635*pow(rlog+0.3,2) +  0.015736*pow(rlog+0.3,3);
 
	  }

 
    }
  else
    if(angle == "Eta")
      {
    
	if(rlog<=0 && rlog>=-3.2)
	  { 
    
	    retval =   0.990694+ 0.010959 *rlog +   0.013447*pow(rlog,2) +  0.003052*pow(rlog,3);
	  }
	else
	  if(rlog>0 && rlog<= 3.2) 
	    {
	
	      retval =  0.558341 + 0.411432*rlog    -0.135635*pow(rlog,2) +  0.015736*pow(rlog,3);
	    }
      }
    else
      std::cout << "  S9ClusterAlgo_CorrFunc:: Wrong angle !! " << std::endl;




  return retval;
}



double  LocalContAfterCrackFunc(double rlog,char* angle)
{
  double retval=1.;
  double rlogN;
  

  if(angle == "Phi")
    {
      rlogN = -rlog; 
 
      if(rlogN<=0 && rlogN>=-3.2)
	{ 
      
	  retval =   0.990694+ 0.010959 *rlogN +   0.013447*pow(rlogN,2) +  0.003052*pow(rlogN,3);
	}
      else
	if(rlogN>0 && rlogN<= 3.2) 
	  {
      
	    retval =  0.558341 + 0.411432*(rlogN+0.3)    -0.135635*pow(rlogN+0.3,2) +  0.015736*pow(rlogN+0.3,3);
 
	  }

    }
  else
    if(angle == "Eta")
      {

  
	if(rlog<=-1.6 && rlog>=-3.2)   
	  { 
	    retval =  0.779886  -0.062156*rlog +  0.026534*pow(rlog,2) +   0.007903*pow(rlog,3);
	  }
	else
	  if(rlog<= 0 && rlog> -1.6)
	    {
	      retval =   0.703501+  0.097090*rlog +   0.043185*pow(rlog,2);
	    }
	  else
	    if(rlog<= 3.2 && rlog> 0)
	      {
		retval =  0.985366 + 0.005925*rlog +   0.003476*pow(rlog,2)   -0.001422*pow(rlog,3);
	      }
      }
    else
      std::cout << "  S9ClusterAlgo_CorrFunc:: Wrong angle !! " << std::endl;

 


  return retval;
}


int DetermineGapCrackID(Sum9GC &S9, char* angle)
{

  int RetVal = 0;
  
  int anglecoord;
  
  int PhiCrack_[18];
  int EtaCrack_[7] = {20, 40, 60, 85, 110, 130, 150};
  
  for (int k = 0; k < 18; k++)
    PhiCrack_[k] = 9 + 20*k;




  // 0  = Gap
  // 1  = BeforeCrack
  // 2  = AfterCrack

 
  if(angle == "Phi")
    {
      anglecoord = S9.S9Cells[0].iPhi;
      
      for (int i = 0; i < 18; i++)
	{
      
	  if (anglecoord == PhiCrack_[i])
	    RetVal = 1;
	  else if (anglecoord == (PhiCrack_[i]+1) )
	    RetVal = 2;
	  else 
	    RetVal = 0;
	}

    }
  else
    if(angle == "Eta")
      {
	anglecoord = S9.S9Cells[0].iEta;
	
	for (int i = 0; i < 7; i++)
	  {
	    if (anglecoord == EtaCrack_[i])
	      {
		if(anglecoord >=85)
		  RetVal = 2;
		else
		  RetVal = 1;

	      }
	    else if (anglecoord == (EtaCrack_[i]-1) )
	      {	 
		if(anglecoord >=85)
		  RetVal = 1;
		else
		  RetVal = 2;
	      }
	    else 
	      RetVal = 0;

	  }
      }
    else
      {
	std::cout << "  S9ClusterAlgo_CorrFunc:: Wrong angle !! " << std::endl;
      	RetVal = 0;
      }

  return RetVal;
  
}


double  LocalContFunc(Sum9GC &S9,char *angle,double &rlog,bool CorCracks)
{
  double E1E=-1.;
  double E2E=-1.;
  double E1P=-1.;
  double E2P=-1.;
  double ratioEta3x3=-1.;
  double ratioPhi3x3=-1.;

  int iCrack=0;

  double retval=1.;
  
 
  
  if(S9.S9Cells[5].E > S9.S9Cells[1].E)
    {
      E2E=S9.S9Cells[4].E+S9.S9Cells[5].E+S9.S9Cells[6].E;
      E1E=S9.S9Cells[3].E+S9.S9Cells[0].E+S9.S9Cells[7].E+S9.S9Cells[2].E+S9.S9Cells[1].E+S9.S9Cells[8].E;
    }
  else
    {
      E1E=S9.S9Cells[2].E+S9.S9Cells[1].E+S9.S9Cells[8].E;
      E2E=S9.S9Cells[4].E+S9.S9Cells[5].E+S9.S9Cells[6].E+S9.S9Cells[3].E+S9.S9Cells[0].E+S9.S9Cells[7].E;
			
    }

  if(S9.S9Cells[3].E > S9.S9Cells[7].E)
    {
      E2P=S9.S9Cells[4].E+S9.S9Cells[3].E+S9.S9Cells[2].E;
      E1P=S9.S9Cells[6].E+S9.S9Cells[7].E+S9.S9Cells[8].E+S9.S9Cells[5].E+S9.S9Cells[0].E+S9.S9Cells[1].E;
			
		
    }
  else
    {
      E1P=S9.S9Cells[6].E+S9.S9Cells[7].E+S9.S9Cells[8].E;
      E2P=S9.S9Cells[4].E+S9.S9Cells[3].E+S9.S9Cells[2].E+S9.S9Cells[1].E+S9.S9Cells[0].E+S9.S9Cells[5].E;
		
    }
		    
  if(S9.S9Cells[0].iEta>=85)
    {	
      if(E2E/E1E > 0)
	ratioEta3x3 = TMath::Log(E2E/E1E);
      else
	ratioEta3x3 = -6;
        
    }
  else
    {	
      if(E1E/E2E > 0) 
	ratioEta3x3 = TMath::Log(E1E/E2E);
      else
	ratioEta3x3 = -6;
       
    }
  

  if(E2P/E1P > 0)
    ratioPhi3x3 = TMath::Log(E2P/E1P);
  else
    ratioPhi3x3 = -6;
  
  
  if(angle == "Phi")
    rlog = ratioPhi3x3;
  else if(angle == "Eta")
    rlog = ratioEta3x3;
  else
    {
      rlog=-6;
      std::cout << "  S9ClusterAlgo_CorrFunc:: Wrong angle !! " << std::endl;
    }
 
  if(CorCracks == true)
    iCrack = DetermineGapCrackID(S9,angle);
  else
    iCrack=0;
  

  if(iCrack==0)
    retval = LocalContGapsFunc(rlog,angle);
  else if(iCrack==1)
    retval = LocalContBeforeCrackFunc(rlog,angle);
  else if(iCrack==2)
    retval = LocalContAfterCrackFunc(rlog,angle);
  else
    retval=1.;
  
  
    
  return retval;

}

double  LocalContCorFactor(Sum9GC &S9,bool CorCracks)
{
 
  double retval=1.;

  double LocalEtaCorr=1.;
  double LocalPhiCorr=1.;
  
  double rlogPhi=-6.;
  double rlogEta=-6.;

  
  LocalEtaCorr =  LocalContFunc(S9,"Eta",rlogEta,CorCracks);
  LocalPhiCorr =  LocalContFunc(S9,"Phi",rlogPhi,CorCracks);


  if(LocalEtaCorr>0.) {
    retval *= LocalEtaCorr;
  }
  
  if(S9.FlagNoIsol1 == false) 
    if(LocalPhiCorr>0.) {
      retval *= LocalPhiCorr;
    }
  return retval;
}
  
double GlobalContCorFactor(Sum9GC &S9, char* ParticleType)
{
  double retval=1.;


  if(S9.FlagNoIsol2==true)
    {
			
      if(ParticleType == "Electron")
	retval = 0.950624;
      else if(ParticleType == "Photon")
	retval = 0.9480;
      else
	std::cout << "  S9ClusterAlgo_CorrFunc:: Wrong ParticleType !! " << std::endl;	      
    }
  else
    {
      retval = 0.937969;
    }
    
    
    
  if(DetermineGapCrackID(S9,"Phi") > 0 || DetermineGapCrackID(S9,"Eta") > 0 )
    retval*=0.992490; // perd un peu d'energie dans le crack , vu en TB
    
    

  return retval;
}


double TrackerCorFactor(SuperClusterGC &MySC, char* ParticleType)
{
    
  double retval=1.;
  
  double EScale=1.;
  double E9SC=1.;
  double corPar[3]={0.};
  double corParE9SC[5]={0.};
   
	    

  if(ParticleType == "Electron")
    {
      corPar[0] =9.94067e-01;
      corPar[1] =  -2.70874e-04;
      corPar[2] = -1.19221e-02;
	      

    }
  else
    if(ParticleType == "Photon")
      {
	corPar[0] = 9.98772e-01;
	corPar[1] = -1.64518e-04;
	corPar[2] =  -6.77500e-03;
      }
    else
      std::cout << "  S9ClusterAlgo_CorrFunc:: Wrong ParticleType !! " << std::endl;  
  

	   


  EScale = corPar[0] +  corPar[1]*pow(MySC.PosEta,1) + corPar[2]*pow(MySC.PosEta,2) ;
	  
  corParE9SC[0] =  1.00871;
  corParE9SC[1] = -2.78209e-01 ;
  corParE9SC[2] =  1.03135e+00;
  corParE9SC[3] = -1.36109e+00 ;
  corParE9SC[4] =  6.07960e-01;



  if(MySC.E != 0.)
    E9SC =  corParE9SC[0] + corParE9SC[1]*pow((MySC.Sum9[0].E/MySC.E),1) + corParE9SC[2]*pow((MySC.Sum9[0].E/MySC.E),2) +  corParE9SC[3]*pow((MySC.Sum9[0].E/MySC.E),3) + corParE9SC[4]*pow((MySC.Sum9[0].E/MySC.E),4);
  
	    
  if(ParticleType == "Electron")
    {
      retval = EScale;

    }
  else
    if(ParticleType == "Photon")
      {
	retval = (EScale * E9SC); // pour photon converti , corrige de
                                  // la profondeur de la conversion , rescale a 1

      }
    else
      std::cout << "  S9ClusterAlgo_CorrFunc:: Wrong ParticleType !! " << std::endl;  
	  
 

  return retval;
}


#endif
