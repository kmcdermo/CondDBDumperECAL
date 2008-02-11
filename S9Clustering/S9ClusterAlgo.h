///////////////////////////////////////////////////////////////////
//                                                               //
//                  class S9ClusterAlgo                          //
//                                                               //
//   Author: Julien DESCAMPS, CEA Saclay DAPNIA/SPP, Sept 2007   //
//                                                               //
///////////////////////////////////////////////////////////////////

#ifndef RecoEcal_EgammaClusterAlgos_S9ClusterAlgo_h
#define RecoEcal_EgammaClusterAlgos_S9ClusterAlgo_h

#include "DataFormats/EgammaReco/interface/BasicClusterFwd.h"
#include "DataFormats/EcalRecHit/interface/EcalRecHitCollections.h"
#include "DataFormats/EgammaReco/interface/BasicCluster.h"
#include "DataFormats/EgammaReco/interface/SuperCluster.h"
#include "RecoCaloTools/Navigation/interface/EcalBarrelNavigator.h"
#include "DataFormats/EcalRecHit/interface/EcalRecHit.h"
#include "Geometry/CaloGeometry/interface/CaloSubdetectorGeometry.h"
#include "Geometry/CaloGeometry/interface/CaloGeometry.h"
#include "FWCore/Framework/interface/ESHandle.h"
#include "DataFormats/DetId/interface/DetId.h"
#include "DataFormats/EcalDetId/interface/EBDetId.h"
#include "Geometry/CaloTopology/interface/CaloSubdetectorTopology.h"
#include "Geometry/CaloGeometry/interface/CaloGeometry.h"
#include "FWCore/Framework/interface/ESHandle.h"
#include <vector>
#include <set>

#define NSUM9 60

//*****************************************************//
//*****************************************************//

// ECAL barrel single cell structure 
typedef struct CellGC    
{
  double E;
  double Eta;
  double Phi;

  double X;
  double Y;
  double Z;

  int iEta;
  int iPhi;
 
  bool FlagUsed;           // Flag is true if the cell is already used


  void Print()
  {	
    std::cout << "         CellGC->E ** " << E <<  " CellGC->iEta ** " 
	      << iEta <<  " CellGC->iPhi ** " << iPhi <<  " CellGC->Eta ** " 
	      << Eta <<  " CellGC->Phi ** " << Phi << " FlagUsed " << FlagUsed << std::endl;

  }


  void Init()
  {
      
    E    =  0.   ;
    Eta  =  -999;
    Phi  =  -999;

    iEta =  -999;
    iPhi =  -999;
  }
    
}CellGC;

//*****************************************************//

// ECAL barrel mapping structure
typedef struct CellsGC       
{
  
  std::vector<CellGC> Cells;

  int nCells;

  double MapE[170][360];
  double MapEta[170][360];
  double MapPhi[170][360];
  bool MapFlag[170][360];
  double MapX[170][360];
  double MapY[170][360];
  double MapZ[170][360];
  double MapEm16[170][360];       //ES2 threshold mapping


  
  void InitMaps()
  { 
    
    for(int i=0;i<170;i++)
      for(int j=0;j<360;j++)
	{
	 
	  MapE[i][j]    = 0;
	  MapEta[i][j]  = -33;
	  MapPhi[i][j]  = -33;
	  MapFlag[i][j] = false;
	  MapX[i][j]  = -33;
	  MapY[i][j]  = -33;
	  MapZ[i][j]  = -33;
	  MapEm16[i][j]  = -33.;

	}
    
  }

}CellsGC;

//*****************************************************//


/*      ___________________    */
/*     | 20| 21| 22| 23| 24|   */
/*     |___|___|___|___|___|   */
/*     | 19| 6 | 7 | 8 | 9 |   */
/* phi |___|___|___|___|___|   */
/*  /  | 18| 5 | 0 | 1 | 10|   */
/*  k  |___|___|___|___|___|   */
/*     | 17| 4 | 3 | 2 | 11|   */
/*  ,  |___|___|___|___|___|   */
/* /|\ | 16| 15| 14| 13| 12|   */
/*  |  |___|___|___|___|___|   */
/*  |                          */
/*   ------> eta / j           */



// BASIC-CLUSTER STRUCTURE
typedef struct Sum9GC       
{
  
 
  double PosEta;             // Logarithmic barycenter eta angular position
  double PosPhi;             // Logarithmic barycenter phi angular position

  double PosX;               // Logarithmic barycenter X cartesian position
  double PosY;               // Logarithmic barycenter Y cartesian position
  double PosZ;               // Logarithmic barycenter Z cartesian position
  
  double E;                  // uncorrected Energy 

  
  int iEta;                  // Discrete eta index of the central Xtal [0:169]
  int iPhi;                  // Discrete eta index of the central Xtal [0:359]

 
  bool NoFull;               // Is "true" if the # of missing xtals in basic-cluster >= NXTALS_MISSING_FOR_NOFULL_S9
  bool AtBarrelEdge;         // Is "true" if the basic-cluster is located at edge of barrel
  int  nMiss;                // Number of missing Xtals in the basic-cluster 
  

  bool flEta;
  bool flPhi;
 

  
  bool FlagNoIsol1;         // Is "true" if there's a seed Xtal found adjacent to this basic-cluster: so we do not correct in the Phi view
  bool FlagNoIsol2;         // Is "true" if there's a seed Xtal found adjacent to at least 3 Xtals from the central Xtal of this basic-cluster
  bool FlagNoComplete;     
  
  CellGC S9Cells[25];
 


  void Print()
  {	 

    std::cout << "   " << std::endl ;
    std::cout << "           Sum9GC->E = " << E <<  " ***  iEta " << iEta  << " iPhi " << iPhi << std::endl;
    std::cout << "           ***********  " << std::endl ;
    std::cout << "   " << std::endl ;
       
    for(int i=0;i<9;i++)
      S9Cells[i].Print();
      
  }

  void Init()
  {
    E = 0;
    nMiss = 0; 
    NoFull=false;
    AtBarrelEdge = false;
    FlagNoIsol1 = false; 
    FlagNoIsol2 = false; 
    FlagNoComplete = false; 
    
      
    

    for(int i=0;i<25;i++)
      {
      
	S9Cells[i].E   =  0;
	S9Cells[i].Eta   =  -9999;
	S9Cells[i].Phi   =  -9999;
	S9Cells[i].iEta =  -9999;
	S9Cells[i].iPhi = -9999;

      }
      
  }
  

}Sum9GC;

//*****************************************************//

// SUPER-CLUSTER STRUCTURE
typedef struct SuperClusterGC  
{
  
 
  double PosEta;       // Energy barycenter eta angular position
  double PosPhi;       // Energy barycenter phi angular position

  double PosX;         // Energy barycenter eta cartesian position
  double PosY;         // Energy barycenter eta cartesian position
  double PosZ;         // Energy barycenter eta cartesian position

  
  double E;            // Uncorrected energy

  bool S9atBarrelEdge; // Is "true" if there's at least 1 basic-cluster at edges of barrel
  int nSum9;           // Number of basic-clusters in the super-cluster

 
  Sum9GC Sum9[NSUM9];

  void Print()
  {	 

    std::cout << "   " << std::endl ;
    std::cout << " SuperCluster->E ** " <<  E <<  " PosPhi " << PosPhi << " PosEta " << PosEta <<  std::endl;
    std::cout << " ***********  " << std::endl ;
    std::cout << "   " << std::endl ;
       
    for(int i=0;i<nSum9;i++)
      Sum9[i].Print();
      
  }

  

  void addSum9(Sum9GC& S9)
  {
    Sum9[nSum9] = S9;
   
    E += S9.E;
    nSum9++;
    
  }


}SuperClusterGC;


//*****************************************************//
//*****************************************************//

class S9ClusterAlgo {

private:

  // S9 algorithm parameters
  double ES1;
  double ES2;
  double ETSG;
  int    PhiStepsMax;
  int    EtaStepsMax;
  
  // S9 algorithm logposition measuremement parameters
  double X0;
  double W0;
  double T0;
  

  double ParticleType;  // 11 = Electron // 22 = Photon




  // collection of all rechits
  const EcalRecHitCollection *recHits_;
   


  double theta(double etap);
  
  // convert CMSSW indices in more natural ones:
  void  EcalBarrelIndices(int iEta,int iPhi,int z,int &iEtaN,int &iPhiN); 
  
  void RetrieveTrueCoord(const int phi_deltacoord, const int theta_deltacoord,int& phi_truecoord, int& theta_truecoord ,const int MaxXtalPhi,int const MaxXtalEta);
  

  // create a vector of CellGC structures from rechits
  int CreateCells(const EcalRecHitCollection *Init_recHits,const CaloSubdetectorGeometry *geometry,std::vector<CellGC>& myCell);

  // ECAL barrel mapping with CellGC structures
  CellsGC InitCellsGC(const EcalRecHitCollection *Init_recHits,const CaloSubdetectorGeometry *geometry);
  
  
  void  CreateSum9(CellsGC *MyCells,std::vector<Sum9GC>& S9);


  void  GroupSum9(std::vector<Sum9GC>& S9,std::vector<SuperClusterGC> &SClusOut);
  
 
  

public:
   
  S9ClusterAlgo();
  
  //real constructor
  S9ClusterAlgo(double _ES1,double _ES2,double _ETSG, int _PhiStepsMax,int _EtaStepsMax,double _X0,double _W0,double _T0,double _ParticleType);
  
  ~S9ClusterAlgo();

  reco::SuperClusterCollection makeSuperClusters(const EcalRecHitCollection *recColl,const CaloSubdetectorGeometry *geometry);
 
};
#endif
