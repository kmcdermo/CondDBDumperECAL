///////////////////////////////////////////////////////////////////
//                                                               //
//           Sorting algorithms for S9ClusterAlgo                //
//                                                               //
// Author: Julien DESCAMPS, CEA Saclay DAPNIA/SPP, Sept 2007     //
//                                                               //
///////////////////////////////////////////////////////////////////


#ifndef RecoEcal_EcalClusterAlgos_S9ClusterAlgo_SortingFunc_h
#define RecoEcal_EcalClusterAlgos_S9ClusterAlgo_SortingFunc_h

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


void exchange(std::vector<CellGC> &Cell, const int i, const int j)
{
  CellGC temporaire;
  temporaire = Cell[i];
  Cell[i] = Cell[j];
  Cell[j] = temporaire;
}


int partition(std::vector<CellGC> &Cell, const int debut, const int fin)
{
  int compteur = debut;
  double pivot = Cell[debut].E;
  int i;

  for(i=debut+1; i<=fin; i++)
    {
      if(Cell[i].E>pivot) // si élément inférieur au pivot
	{
	  compteur++; // incrémente compteur cad la place finale du pivot
	  exchange(Cell, compteur, i); // élément positionné
	}
    }
  exchange(Cell, compteur, debut); // le pivot est placé
   
  return compteur; // et sa position est retournée
}


void fastSortingAux(std::vector<CellGC> &Cell, const int debut, const int fin)
{
  if(debut<fin) // cas d'arrêt pour la récursivité
    {
      int pivot = partition(Cell, debut, fin); // division du tableau
      fastSortingAux(Cell, debut, pivot-1); // trie partie1
      fastSortingAux(Cell, pivot+1, fin); // trie partie2
    }
}

void fastSorting(std::vector<CellGC> &Cell)
{
  fastSortingAux(Cell, 0, Cell.size()-1);
}




void exchange(SuperClusterGC &mySC, const int i, const int j)
{
  Sum9GC temporaire;
  temporaire = mySC.Sum9[i];
  mySC.Sum9[i] = mySC.Sum9[j];
  mySC.Sum9[j] = temporaire;
}

int partition(SuperClusterGC &mySC, const int debut, const int fin)
{
  int compteur = debut;
  double pivot = mySC.Sum9[debut].S9Cells[0].E;
  int i;

  for(i=debut+1; i<=fin; i++)
    {
      if(mySC.Sum9[i].S9Cells[0].E > pivot) // si élément inférieur au pivot
	{
	  compteur++; // incrémente compteur cad la place finale du pivot
	  exchange(mySC, compteur, i); // élément positionné
	}
    }
  exchange(mySC, compteur, debut); // le pivot est placé
   
  return compteur; // et sa position est retournée
}


void fastSortingAux(SuperClusterGC &mySC, const int debut, const int fin)
{
  if(debut<fin) // cas d'arrêt pour la récursivité
    {
      int pivot = partition(mySC, debut, fin); // division du tableau
      fastSortingAux(mySC, debut, pivot-1); // trie partie1
      fastSortingAux(mySC, pivot+1, fin); // trie partie2
    }
}

void fastSorting(SuperClusterGC &mySC)
{
  fastSortingAux(mySC, 0, mySC.nSum9 - 1);
}




#endif
