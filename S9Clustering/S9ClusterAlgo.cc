///////////////////////////////////////////////////////////////////
//                                                               //
//                  class S9ClusterAlgo                          //
//                                                               //
//   Author: Julien DESCAMPS, CEA Saclay DAPNIA/SPP, Sept 2007   //
//                                                               //
///////////////////////////////////////////////////////////////////

#include "RecoEcal/EgammaClusterAlgos/interface/S9ClusterAlgo.h"
#include "RecoEcal/EgammaClusterAlgos/interface/S9ClusterAlgo_SortingFunc.h"
#include "RecoEcal/EgammaClusterAlgos/interface/S9ClusterAlgo_LogPosFunc.h"
#include "RecoEcal/EgammaClusterAlgos/interface/S9ClusterAlgo_CorrFunc.h"

#include "RecoCaloTools/Navigation/interface/EcalBarrelNavigator.h"
#include "Geometry/CaloGeometry/interface/CaloCellGeometry.h"
#include "Geometry/CaloTopology/interface/EcalBarrelHardcodedTopology.h"
#include "DataFormats/DetId/interface/DetId.h"
#include "DataFormats/EcalDetId/interface/EBDetId.h"
#include "Geometry/CaloTopology/interface/CaloSubdetectorTopology.h"
#include "Geometry/CaloGeometry/interface/CaloGeometry.h"
#include "FWCore/Framework/interface/ESHandle.h"
#include <iostream>
#include <map>
#include <vector>
#include <set>

#define NXTALSMISSINGMAX 6
#define NXTALS_MISSING_FOR_NOFULL_S9 1
#define NXTALS_S9MAX_IN_SC 1





S9ClusterAlgo::S9ClusterAlgo()
{
}

//real constructor
S9ClusterAlgo::S9ClusterAlgo(double _ES1, double _ES2, double _ETSG, int _PhiStepsMax, int _EtaStepsMax, double _X0, double _W0, double _T0, double _ParticleType)
{
	ES1 = _ES1;
	ES2 = _ES2;
	ETSG = _ETSG;
	PhiStepsMax = _PhiStepsMax;
	EtaStepsMax = _EtaStepsMax;
	X0 = _X0;
	W0 = _W0;
	T0 = _T0;

	ParticleType = _ParticleType;
}

S9ClusterAlgo::~S9ClusterAlgo()
{
}


double S9ClusterAlgo::theta(double etap)
{
	return 2.0 * atan(exp(-etap));
}


// convert CMSSW indices in more natural ones:
void S9ClusterAlgo::EcalBarrelIndices(int iEta, int iPhi, int z, int &iEtaN, int &iPhiN)
{
	int _eta;
	int _phi;

	int _nCrystalInPhi = 360;
	int _nCrystalInHalfBarrelEta = 85;

	//let's convert CMSSW indices in more natural ones:
	if (z > 0) {		//Jura's half barrel, CMSSW eta index is 1 for eta=0., 85 for eta = 1.4775
		//.......................... CMSSW phi index is 1 for phi=10.5 deg, 360 for phi=9.5deg 
		_eta = abs(iEta) + _nCrystalInHalfBarrelEta - 1;
		_phi = iPhi + 9;	//_phi = 0 for phi = 0 deg.
	} else {		//Saleve half barrel, more complicates (some 6a...): phi index is clockwise!
		//.......................  phi index is 360 for phi=170.5deg in counter-clockwise
		_eta = _nCrystalInHalfBarrelEta - abs(iEta);
		_phi = _nCrystalInPhi / 2 - 10 - iPhi;	//_phi = 0 for phi = 0 deg. and in counter-clockwise
	}

	//expresses phi into [0,359] interval:
	_phi %= 360;
	if (_phi < 0)
		_phi += 360;
	iEtaN = _eta;
	iPhiN = _phi;
}


void S9ClusterAlgo::RetrieveTrueCoord(const int phi_deltacoord, const int theta_deltacoord, int &phi_truecoord, int &theta_truecoord, const int MaxXtalPhi, int const MaxXtalEta)
{
	// Retrieves the true coordinates displaced 
	// by phi_deltacoord and theta_deltacoord w.r.t. max coordinates

	//
	// find true phi value  
	//
	int truephi = MaxXtalPhi + phi_deltacoord;
	if (truephi < 0)
		truephi += 360;
	else if (truephi > 359)
		truephi -= 360;

	//
	// find true theta value
	//
	int maxtheta = MaxXtalEta;
	int truetheta = theta_deltacoord + maxtheta;
	if (			// if deltacoord has truetheta become 0 or less
		   // then the coordinates go to the other half with eta > 0
		   truetheta < 0
		   // if deltacoord has truetheta go from coordinates >= 86
		   // to coordinates <= 85 
		   // then the coordinates go to the other half with eta < 0
		   // as the two halves are different by their modulo of 85
		   // compare as such
		   || ((int) (maxtheta / 85) != (int) (truetheta / 85))
	    ) {
		phi_truecoord = truephi;
        }
	int intphi = truephi;
	phi_truecoord = intphi;
	theta_truecoord = truetheta;
	return;
}


// create a vector of CellGC structures from rechits
int S9ClusterAlgo::CreateCells(const EcalRecHitCollection * Init_recHits, const CaloSubdetectorGeometry * geometry, std::vector < CellGC > &myCell)
{
	int NumCells = 0;

	//Pass in a pointer to the collection.
	recHits_ = Init_recHits;

	EcalRecHitCollection::const_iterator it;

	int iEta = -999;
	int iPhi = -999;
	int iZ = -999;
	int iEtaN = -999;
	int iPhiN = -999;


	for (it = recHits_->begin(); it != recHits_->end(); it++) {
		DetId itID = it->id();

		if (itID.det() != 3 && itID.subdetId() != 1)	// Take only the EcalBarrel rechits
			continue;

		const CaloCellGeometry *this_cell = (*geometry).getGeometry(it->id());
		GlobalPoint position = this_cell->getPosition();

		NumCells++;

		EBDetId itEBID = EBDetId(it->id());

		iEta = itEBID.ieta();
		iPhi = itEBID.iphi();
		iZ = itEBID.zside();

		EcalBarrelIndices(iEta, iPhi, iZ, iEtaN, iPhiN);

		CellGC temp;

		temp.E = it->energy();
		temp.Phi = position.phi();
		temp.Eta = position.eta();
		temp.X = position.x();
		temp.Y = position.y();
		temp.Z = position.z();
		temp.iPhi = iPhiN;
		temp.iEta = iEtaN;
                temp.detId = it->id();

		myCell.push_back(temp);
	}
	fastSorting(myCell);
	return (NumCells);
}

// ECAL barrel mapping with CellGC structures
CellsGC S9ClusterAlgo::InitCellsGC(const EcalRecHitCollection * Init_recHits, const CaloSubdetectorGeometry * geometry)
{
	CellsGC MyCellsGC;
	MyCellsGC.nCells = CreateCells(Init_recHits, geometry, MyCellsGC.Cells);
	MyCellsGC.InitMaps();
	for (int i = 0; i < (int) (MyCellsGC.Cells.size()); i++) {
		if (MyCellsGC.Cells[i].iEta >= 0 && MyCellsGC.Cells[i].iPhi >= 0 && MyCellsGC.Cells[i].iEta < 170 && MyCellsGC.Cells[i].iPhi < 360) {
			MyCellsGC.MapE[MyCellsGC.Cells[i].iEta][MyCellsGC.Cells[i].iPhi] = MyCellsGC.Cells[i].E;
			MyCellsGC.MapEta[MyCellsGC.Cells[i].iEta][MyCellsGC.Cells[i].iPhi] = MyCellsGC.Cells[i].Eta;
			MyCellsGC.MapPhi[MyCellsGC.Cells[i].iEta][MyCellsGC.Cells[i].iPhi] = MyCellsGC.Cells[i].Phi;
			MyCellsGC.MapFlag[MyCellsGC.Cells[i].iEta][MyCellsGC.Cells[i].iPhi] = false;
			MyCellsGC.MapEm16[MyCellsGC.Cells[i].iEta][MyCellsGC.Cells[i].iPhi] = 0.;
			MyCellsGC.MapX[MyCellsGC.Cells[i].iEta][MyCellsGC.Cells[i].iPhi] = MyCellsGC.Cells[i].X;
			MyCellsGC.MapY[MyCellsGC.Cells[i].iEta][MyCellsGC.Cells[i].iPhi] = MyCellsGC.Cells[i].Y;
			MyCellsGC.MapZ[MyCellsGC.Cells[i].iEta][MyCellsGC.Cells[i].iPhi] = MyCellsGC.Cells[i].Z;
			MyCellsGC.MapDetId[MyCellsGC.Cells[i].iEta][MyCellsGC.Cells[i].iPhi] = MyCellsGC.Cells[i].detId;
		}
	}
	return MyCellsGC;
}

void S9ClusterAlgo::CreateSum9(CellsGC * MyCells, std::vector < Sum9GC > &S9)
{
	bool S9atBarrelEdge = false;
	bool S9NoFull = false;
	bool IsAdjacent = false;
	int TruePhi = -1;
	int TrueEta = -1;
	Sum9GC *temp = new Sum9GC;

	for (int iXtal = 0; iXtal < MyCells->nCells; iXtal++) {
		temp->Init();
		S9atBarrelEdge = false;
		S9NoFull = false;

		IsAdjacent = false;

		TruePhi = -1;
		TrueEta = -1;

		if (MyCells->Cells[iXtal].E < ES1)
			break;

		if (MyCells->MapFlag[MyCells->Cells[iXtal].iEta][MyCells->Cells[iXtal].iPhi] == true)
			continue;

		for (int j = -1; j <= 1; j++)
			for (int k = -1; k <= 1; k++) {
				RetrieveTrueCoord(k, j, TruePhi, TrueEta, MyCells->Cells[iXtal].iPhi, MyCells->Cells[iXtal].iEta);
				if (MyCells->Cells[iXtal].iEta + j >= 0 && MyCells->Cells[iXtal].iEta + j < 170)
					if (MyCells->MapFlag[TrueEta][TruePhi] == true)
						IsAdjacent = true;
			}

		if (IsAdjacent == true)
			if (MyCells->MapE[MyCells->Cells[iXtal].iEta][MyCells->Cells[iXtal].iPhi] <= MyCells->MapEm16[MyCells->Cells[iXtal].iEta][MyCells->Cells[iXtal].iPhi])
				continue;

		for (int i = 0; i < 25; i++)
			temp->S9Cells[i].FlagUsed = false;

		for (int j = -2; j <= 2; j++) {
			for (int k = -2; k <= 2; k++) {
				RetrieveTrueCoord(k, j, TruePhi, TrueEta, MyCells->Cells[iXtal].iPhi, MyCells->Cells[iXtal].iEta);
				if (MyCells->Cells[iXtal].iEta + j >= 0 && MyCells->Cells[iXtal].iEta + j < 170) {
					if (k == -1 && abs(j) <= 1) {
						temp->S9Cells[-j + 3].E = MyCells->MapE[TrueEta][TruePhi];
						temp->S9Cells[-j + 3].Eta = MyCells->MapEta[TrueEta][TruePhi];
						temp->S9Cells[-j + 3].Phi = MyCells->MapPhi[TrueEta][TruePhi];

						temp->S9Cells[-j + 3].X = MyCells->MapX[TrueEta][TruePhi];
						temp->S9Cells[-j + 3].Y = MyCells->MapY[TrueEta][TruePhi];
						temp->S9Cells[-j + 3].Z = MyCells->MapZ[TrueEta][TruePhi];

						temp->S9Cells[-j + 3].detId = MyCells->MapDetId[TrueEta][TruePhi];

						temp->S9Cells[-j + 3].iEta = TrueEta;
						temp->S9Cells[-j + 3].iPhi = TruePhi;

						if (MyCells->MapFlag[TrueEta][TruePhi] == true)
							temp->S9Cells[-j + 3].FlagUsed = true;
					}
					if (k == 1 && abs(j) <= 1) {
						temp->S9Cells[j + 7].E = MyCells->MapE[TrueEta][TruePhi];
						temp->S9Cells[j + 7].Eta = MyCells->MapEta[TrueEta][TruePhi];
						temp->S9Cells[j + 7].Phi = MyCells->MapPhi[TrueEta][TruePhi];


						temp->S9Cells[j + 7].X = MyCells->MapX[TrueEta][TruePhi];
						temp->S9Cells[j + 7].Y = MyCells->MapY[TrueEta][TruePhi];
						temp->S9Cells[j + 7].Z = MyCells->MapZ[TrueEta][TruePhi];

						temp->S9Cells[j + 7].detId = MyCells->MapDetId[TrueEta][TruePhi];

						temp->S9Cells[j + 7].iEta = TrueEta;
						temp->S9Cells[j + 7].iPhi = TruePhi;

						if (MyCells->MapFlag[TrueEta][TruePhi] == true)
							temp->S9Cells[j + 7].FlagUsed = true;
					}
					if (k == 0 && j == 0) {
						temp->S9Cells[0].E = MyCells->MapE[TrueEta][TruePhi];
						temp->S9Cells[0].Eta = MyCells->MapEta[TrueEta][TruePhi];
						temp->S9Cells[0].Phi = MyCells->MapPhi[TrueEta][TruePhi];
						temp->S9Cells[0].iEta = TrueEta;
						temp->S9Cells[0].iPhi = TruePhi;

						temp->S9Cells[0].X = MyCells->MapX[TrueEta][TruePhi];
						temp->S9Cells[0].Y = MyCells->MapY[TrueEta][TruePhi];
						temp->S9Cells[0].Z = MyCells->MapZ[TrueEta][TruePhi];

						temp->S9Cells[0].detId = MyCells->MapDetId[TrueEta][TruePhi];

						temp->iEta = MyCells->Cells[iXtal].iEta;
						temp->iPhi = MyCells->Cells[iXtal].iPhi;

						if (MyCells->MapFlag[TrueEta][TruePhi] == true)
							temp->S9Cells[0].FlagUsed = true;
					}

					if (k == 0 && j == -1) {
						temp->S9Cells[5].E = MyCells->MapE[TrueEta][TruePhi];
						temp->S9Cells[5].Eta = MyCells->MapEta[TrueEta][TruePhi];
						temp->S9Cells[5].Phi = MyCells->MapPhi[TrueEta][TruePhi];
						temp->S9Cells[5].iEta = TrueEta;
						temp->S9Cells[5].iPhi = TruePhi;

						temp->S9Cells[5].X = MyCells->MapX[TrueEta][TruePhi];
						temp->S9Cells[5].Y = MyCells->MapY[TrueEta][TruePhi];
						temp->S9Cells[5].Z = MyCells->MapZ[TrueEta][TruePhi];

						temp->S9Cells[5].detId = MyCells->MapDetId[TrueEta][TruePhi];

						if (MyCells->MapFlag[TrueEta][TruePhi] == true)
							temp->S9Cells[5].FlagUsed = true;
					}
					if (k == 0 && j == 1) {
						temp->S9Cells[1].E = MyCells->MapE[TrueEta][TruePhi];
						temp->S9Cells[1].Eta = MyCells->MapEta[TrueEta][TruePhi];
						temp->S9Cells[1].Phi = MyCells->MapPhi[TrueEta][TruePhi];
						temp->S9Cells[1].iEta = TrueEta;
						temp->S9Cells[1].iPhi = TruePhi;

						temp->S9Cells[1].X = MyCells->MapX[TrueEta][TruePhi];
						temp->S9Cells[1].Y = MyCells->MapY[TrueEta][TruePhi];
						temp->S9Cells[1].Z = MyCells->MapZ[TrueEta][TruePhi];

						temp->S9Cells[1].detId = MyCells->MapDetId[TrueEta][TruePhi];

						if (MyCells->MapFlag[TrueEta][TruePhi] == true)
							temp->S9Cells[1].FlagUsed = true;

					}
					if (k < -1) {
						temp->S9Cells[-j + 14].E = MyCells->MapE[TrueEta][TruePhi];
						temp->S9Cells[-j + 14].Eta = MyCells->MapEta[TrueEta][TruePhi];
						temp->S9Cells[-j + 14].Phi = MyCells->MapPhi[TrueEta][TruePhi];
						temp->S9Cells[-j + 14].iEta = TrueEta;
						temp->S9Cells[-j + 14].iPhi = TruePhi;

						temp->S9Cells[-j + 14].X = MyCells->MapX[TrueEta][TruePhi];
						temp->S9Cells[-j + 14].Y = MyCells->MapY[TrueEta][TruePhi];
						temp->S9Cells[-j + 14].Z = MyCells->MapZ[TrueEta][TruePhi];

						temp->S9Cells[-j + 14].detId = MyCells->MapDetId[TrueEta][TruePhi];
					}
					if (k > 1) {
						temp->S9Cells[j + 22].E = MyCells->MapE[TrueEta][TruePhi];
						temp->S9Cells[j + 22].Eta = MyCells->MapEta[TrueEta][TruePhi];
						temp->S9Cells[j + 22].Phi = MyCells->MapPhi[TrueEta][TruePhi];
						temp->S9Cells[j + 22].iEta = TrueEta;
						temp->S9Cells[j + 22].iPhi = TruePhi;

						temp->S9Cells[j + 22].X = MyCells->MapX[TrueEta][TruePhi];
						temp->S9Cells[j + 22].Y = MyCells->MapY[TrueEta][TruePhi];
						temp->S9Cells[j + 22].Z = MyCells->MapZ[TrueEta][TruePhi];

						temp->S9Cells[j + 22].detId = MyCells->MapDetId[TrueEta][TruePhi];
					}
					if (j == 2 && abs(k) <= 1) {
						temp->S9Cells[-k + 10].E = MyCells->MapE[TrueEta][TruePhi];
						temp->S9Cells[-k + 10].Eta = MyCells->MapEta[TrueEta][TruePhi];
						temp->S9Cells[-k + 10].Phi = MyCells->MapPhi[TrueEta][TruePhi];
						temp->S9Cells[-k + 10].iEta = TrueEta;
						temp->S9Cells[-k + 10].iPhi = TruePhi;

						temp->S9Cells[-k + 10].X = MyCells->MapX[TrueEta][TruePhi];
						temp->S9Cells[-k + 10].Y = MyCells->MapY[TrueEta][TruePhi];
						temp->S9Cells[-k + 10].Z = MyCells->MapZ[TrueEta][TruePhi];

						temp->S9Cells[-k + 10].detId = MyCells->MapDetId[TrueEta][TruePhi];
					}
					if (j == -2 && abs(k) <= 1) {
						temp->S9Cells[k + 18].E = MyCells->MapE[TrueEta][TruePhi];
						temp->S9Cells[k + 18].Eta = MyCells->MapEta[TrueEta][TruePhi];
						temp->S9Cells[k + 18].Phi = MyCells->MapPhi[TrueEta][TruePhi];
						temp->S9Cells[k + 18].iEta = TrueEta;
						temp->S9Cells[k + 18].iPhi = TruePhi;

						temp->S9Cells[k + 18].X = MyCells->MapX[TrueEta][TruePhi];
						temp->S9Cells[k + 18].Y = MyCells->MapY[TrueEta][TruePhi];
						temp->S9Cells[k + 18].Z = MyCells->MapZ[TrueEta][TruePhi];

						temp->S9Cells[k + 18].detId = MyCells->MapDetId[TrueEta][TruePhi];
					}



					if (abs(j) <= 1 && abs(k) <= 1)	// only sum of 9
					{
						if ((MyCells->MapEta[TrueEta][TruePhi] == -33 || MyCells->MapPhi[TrueEta][TruePhi] == -33)) {
							S9NoFull = true;
							temp->nMiss++;
						}
						if ((MyCells->MapEta[TrueEta][TruePhi] != -33 && MyCells->MapPhi[TrueEta][TruePhi] != -33))
							if (MyCells->MapFlag[TrueEta][TruePhi] == false)
								temp->E += MyCells->MapE[TrueEta][TruePhi];
					}

					if (abs(j) <= 1 && abs(k) <= 1)
						MyCells->MapFlag[TrueEta][TruePhi] = true;

					if (IsAdjacent == true)
						temp->FlagNoComplete = true;
				} else {
					temp->nMiss++;
					S9atBarrelEdge = true;
				}
			}
		}

		TruePhi = -1;
		TrueEta = -1;

		for (int j = -2; j <= 2; j++) {
			for (int k = -3; k <= 3; k++) {
				RetrieveTrueCoord(k, j, TruePhi, TrueEta, MyCells->Cells[iXtal].iPhi, MyCells->Cells[iXtal].iEta);

				if (MyCells->Cells[iXtal].iEta + j < 0 || MyCells->Cells[iXtal].iEta + j >= 170)
					continue;

				if (abs(k) <= 2) {

					if (temp->E * ES2 > MyCells->MapEm16[TrueEta][TruePhi])	///  if there is 2 values of cut for same crystal, take higher value;   
						MyCells->MapEm16[TrueEta][TruePhi] = temp->E * ES2;

					if (abs(k) > 1 && abs(j) <= 1)
						if ((MyCells->MapE[TrueEta][TruePhi] > MyCells->MapEm16[TrueEta][TruePhi]) && MyCells->MapE[TrueEta][TruePhi] > ES1) {

							temp->FlagNoIsol1 = true;
							temp->FlagNoIsol2 = true;
						}
				} else {
					if (abs(j) <= 1)
						if (MyCells->MapE[TrueEta][TruePhi] > ES1)
							temp->FlagNoIsol2 = true;

				}
			}
		}

		if (S9NoFull == true && temp->nMiss >= NXTALS_MISSING_FOR_NOFULL_S9) {
			temp->NoFull = true;
		}

		if (S9atBarrelEdge == true) {
			temp->AtBarrelEdge = true;
			temp->NoFull = true;
		}

		S9.push_back(*temp);
	}
	delete temp;

}


void S9ClusterAlgo::GroupSum9(std::vector < Sum9GC > &S9, std::vector < SuperClusterGC > &SClusOut)
{

	int nS9 = 0;
	int nS9p = 0;
	int iS9p = 0;
	int PHIMAX = PhiStepsMax;
	int ETAMAX = EtaStepsMax;

	int NS9MAX = S9.size();

	int nAlloc = 10;
	int PAS = 10;

	Sum9GC *S9p = NULL;
	S9p = (Sum9GC *) malloc(NS9MAX * sizeof(Sum9GC));
	if (S9p == NULL)
		printf("plus de mémoire\n");

	SuperClusterGC *SClus = NULL;
	SClus = (SuperClusterGC *) malloc(nAlloc * sizeof(SuperClusterGC));
	if (SClus == NULL)
		printf("plus de mémoire\n");

	for (int i = 0; i < nAlloc; i++) {
		SClus[i].nSum9 = 0;
		SClus[i].E = 0;
		SClus[i].S9atBarrelEdge = false;
	}

	for (nS9 = 0; nS9 < int (S9.size()); nS9++) {
		if (S9[nS9].E > 0 && S9[nS9].nMiss <= NXTALSMISSINGMAX) {
			S9p[nS9p] = S9[nS9];
			S9p[nS9p].flEta = false;
			S9p[nS9p].flPhi = false;
			nS9p++;
		}
	}

	int nSClusTrue = -1;
	int nS9ALL = nS9p;

	if (nS9p > 0)
		for (int nSClus = 0; nSClus < nS9p; nSClus++) {
			if (nS9ALL == 0)
				break;
			nSClusTrue++;
			bool firtnoflag = false;
			int ifirst = 0;
			int itest = 0;
			for (iS9p = 0; iS9p < nS9p; iS9p++) {

				if (S9p[iS9p].flEta == true)
					continue;

				itest++;
				if (firtnoflag == false) {
					firtnoflag = true;
					ifirst = iS9p;

					//****************** REALLOC ********************//     

					if (nSClus % PAS == 0 && nSClus > 0) {
						SuperClusterGC *tmp = NULL;
						nAlloc += PAS;
						tmp = (SuperClusterGC *) realloc(SClus, nAlloc * sizeof(SuperClusterGC));
						if (tmp != NULL) {
							SClus = tmp;
							for (int i = nSClus; i < nAlloc; i++) {
								SClus[i].nSum9 = 0;
								SClus[i].E = 0;
								SClus[i].S9atBarrelEdge = false;
							}
						}
						//*********************************************//
					}
					SClus[nSClus].addSum9(S9p[iS9p]);
					S9p[iS9p].flEta = true;
					nS9ALL--;
					if (S9p[iS9p].AtBarrelEdge == true)
						SClus[nSClus].S9atBarrelEdge = true;
				}

				if (firtnoflag == true && itest > 1)
				{
					if (abs(S9p[iS9p].iEta - S9p[ifirst].iEta) <= ETAMAX) {
						//$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$//

						if (S9p[ifirst].iPhi >= PHIMAX && S9p[ifirst].iPhi <= (359 - PHIMAX))
							if (abs(S9p[iS9p].iPhi - S9p[ifirst].iPhi) <= PHIMAX) {

								//****************** REALLOC ********************//

								if (nSClus % PAS == 0 && nSClus > 0) {
									nAlloc += PAS;
									SuperClusterGC *tmp = NULL;
									tmp = (SuperClusterGC *) realloc(SClus, nAlloc * sizeof(SuperClusterGC));
									if (tmp != NULL) {
										SClus = tmp;
										for (int i = nSClus; i < nAlloc; i++) {
											SClus[i].nSum9 = 0;
											SClus[i].E = 0;
											SClus[i].S9atBarrelEdge = false;
										}
									}
								}
								//*********************************************//

								SClus[nSClus].addSum9(S9p[iS9p]);
								S9p[iS9p].flEta = true;

								nS9ALL--;

								if (S9p[iS9p].AtBarrelEdge == true)
									SClus[nSClus].S9atBarrelEdge = true;

							}
						if (S9p[ifirst].iPhi < PHIMAX && S9p[ifirst].iPhi >= 0)
							if (S9p[iS9p].iPhi <= 359 && S9p[iS9p].iPhi > (359 - PHIMAX)) {
								if (S9p[ifirst].iPhi + (359 - S9p[iS9p].iPhi) <= PHIMAX) {
									//****************** REALLOC ********************//

									if (nSClus % PAS == 0 && nSClus > 0) {
										nAlloc += PAS;
										SuperClusterGC *tmp = NULL;
										tmp = (SuperClusterGC *) realloc(SClus, nAlloc * sizeof(SuperClusterGC));
										if (tmp != NULL) {
											SClus = tmp;
											for (int i = nSClus; i < nAlloc; i++) {
												SClus[i].nSum9 = 0;
												SClus[i].E = 0;
												SClus[i].S9atBarrelEdge = false;
											}
										}
									}
									//*********************************************//

									SClus[nSClus].addSum9(S9p[iS9p]);
									S9p[iS9p].flEta = true;

									nS9ALL--;

									if (S9p[iS9p].AtBarrelEdge == true)
										SClus[nSClus].S9atBarrelEdge = true;

								}
							} else if (abs(S9p[iS9p].iPhi - S9p[ifirst].iPhi) <= PHIMAX) {
								//****************** REALLOC ********************//

								if (nSClus % PAS == 0 && nSClus > 0) {
									nAlloc += PAS;
									SuperClusterGC *tmp = NULL;
									tmp = (SuperClusterGC *) realloc(SClus, nAlloc * sizeof(SuperClusterGC));
									if (tmp != NULL) {
										SClus = tmp;
										for (int i = nSClus; i < nAlloc; i++) {
											SClus[i].nSum9 = 0;
											SClus[i].E = 0;
											SClus[i].S9atBarrelEdge = false;

										}
									}

								}
								//*********************************************//

								SClus[nSClus].addSum9(S9p[iS9p]);
								S9p[iS9p].flEta = true;

								nS9ALL--;

								if (S9p[iS9p].AtBarrelEdge == true)
									SClus[nSClus].S9atBarrelEdge = true;
							}
						if (S9p[ifirst].iPhi <= 359 && S9p[ifirst].iPhi > (359 - PHIMAX))
							if (S9p[iS9p].iPhi >= 0 && S9p[iS9p].iPhi < PHIMAX) {
								if ((359 - S9p[ifirst].iPhi) + S9p[iS9p].iPhi <= PHIMAX) {

									//****************** REALLOC ********************//

									if (nSClus % PAS == 0 && nSClus > 0) {
										nAlloc += PAS;
										SuperClusterGC *tmp = NULL;
										tmp = (SuperClusterGC *) realloc(SClus, nAlloc * sizeof(SuperClusterGC));
										if (tmp != NULL) {
											SClus = tmp;
											for (int i = nSClus; i < nAlloc; i++) {
												SClus[i].nSum9 = 0;
												SClus[i].E = 0;
												SClus[i].S9atBarrelEdge = false;

											}
										}

									}
									//*********************************************//

									SClus[nSClus].addSum9(S9p[iS9p]);
									S9p[iS9p].flEta = true;

									nS9ALL--;

									if (S9p[iS9p].AtBarrelEdge == true)
										SClus[nSClus].S9atBarrelEdge = true;

								}
							} else if (abs(S9p[iS9p].iPhi - S9p[ifirst].iPhi) <= PHIMAX) {

								//****************** REALLOC ********************//

								if (nSClus % PAS == 0 && nSClus > 0) {
									nAlloc += PAS;
									SuperClusterGC *tmp = NULL;
									tmp = (SuperClusterGC *) realloc(SClus, nAlloc * sizeof(SuperClusterGC));
									if (tmp != NULL) {
										SClus = tmp;
										for (int i = nSClus; i < nAlloc; i++) {
											SClus[i].nSum9 = 0;
											SClus[i].E = 0;
											SClus[i].S9atBarrelEdge = false;

										}
									}

								}
								//*********************************************//

								SClus[nSClus].addSum9(S9p[iS9p]);
								S9p[iS9p].flEta = true;

								nS9ALL--;

								if (S9p[iS9p].AtBarrelEdge == true)
									SClus[nSClus].S9atBarrelEdge = true;
							}
					}
				}
			}
		}

	int nSC = nSClusTrue;

	if (S9p) {
		free(S9p);
		S9p = NULL;
	}

	for (int i = 0; i <= nSC; i++) {
		fastSorting(SClus[i]);
		ComputeLogPos(&(SClus[i]), X0, W0, T0);

		if (SClus[i].Sum9[0].S9Cells[0].E * sin(theta(SClus[i].PosEta)) <= ETSG)
			continue;

		if (SClus[i].Sum9[0].nMiss > NXTALS_S9MAX_IN_SC)
			continue;

		SClusOut.push_back(SClus[i]);
	}

	if (SClus) {
		free(SClus);
		SClus = NULL;
	}

}



reco::SuperClusterCollection S9ClusterAlgo::makeSuperClusters(const EcalRecHitCollection * recColl, const CaloSubdetectorGeometry * geometry, reco::BasicClusterRefVector &clusterRefVector_p)
{
	//Here's what we'll return.
	reco::SuperClusterCollection SCcoll;

	//std::cout << "--> Starting S9 clusterization..." << std::endl;

	CellsGC *myCellsGC = new CellsGC;
	*myCellsGC = InitCellsGC(recColl, geometry);

	std::vector < Sum9GC > S9;
	CreateSum9(myCellsGC, S9);

	int nCells = myCellsGC->nCells;

	if (nCells != 0) {
		std::vector < SuperClusterGC > MySClus;

		GroupSum9(S9, MySClus);
                int basicClusterCounter = 0;
		for (int iSC = 0; iSC < int (MySClus.size()); iSC++)	// Loop on SuperClusters 
		{

			MySClus[iSC].Print();

			double LocalCorFac = 1.;
			double GlobalCorFac = 1.;
			double TrackerCorFac = 1.;

			double SuperCluster_ECor = 0.;

                        reco::BasicClusterRefVector bc_ref;
                        int iseed = 0;
                        float maxEnergy = 0;
			for (int iS9 = 0; iS9 < MySClus[iSC].nSum9; iS9++)	// Loop on basic-clusters in the super-cluster
			{

				LocalCorFac = 1.;
				GlobalCorFac = 1.;

				if (MySClus[iSC].Sum9[iS9].NoFull == false) {

					if (ParticleType == 11) {

						LocalCorFac = LocalContCorFactor(MySClus[iSC].Sum9[iS9], false);
						GlobalCorFac = GlobalContCorFactor(MySClus[iSC].Sum9[iS9], "Electron");

					} else if (ParticleType == 22) {

						if (MySClus[iSC].nSum9 == 1)
							LocalCorFac = LocalContCorFactor(MySClus[iSC].Sum9[iS9], true);
						else
							LocalCorFac = LocalContCorFactor(MySClus[iSC].Sum9[iS9], false);


						GlobalCorFac = GlobalContCorFactor(MySClus[iSC].Sum9[iS9], "Photon");
					} else
						std::cout << "ParticleType must be 11 (Electron) or 22 (Photons)" << std::endl;
				} else {
					LocalCorFac = 1.;
					GlobalCorFac = 1;
				}
				SuperCluster_ECor += (MySClus[iSC].Sum9[iS9].E / (LocalCorFac * GlobalCorFac));
                                //std::cout << "Energy comparison: " << (*clusterRefVector_p[basicClusterCounter]).energy() << " vs. " << MySClus[iSC].Sum9[iS9].E / (LocalCorFac * GlobalCorFac) << std::endl;
                                bc_ref.push_back( clusterRefVector_p[basicClusterCounter] );
                                if ( MySClus[iSC].Sum9[iS9].E / (LocalCorFac * GlobalCorFac) > maxEnergy) {
                                        iseed = basicClusterCounter;
                                        maxEnergy = MySClus[iSC].Sum9[iS9].E / (LocalCorFac * GlobalCorFac);
                                }
                                ++basicClusterCounter;
			}

			if (ParticleType == 11)
				TrackerCorFac = TrackerCorFactor(MySClus[iSC], "Electron");
			else if (ParticleType == 22)
				TrackerCorFac = TrackerCorFactor(MySClus[iSC], "Photon");

			if (ParticleType == 11)
				SuperCluster_ECor /= TrackerCorFac;
			else if (ParticleType == 22)
				if (MySClus[iSC].nSum9 > 1)
					SuperCluster_ECor /= TrackerCorFac;

			//std::cout << "======>>>  SuperCluster_ECor =  " << SuperCluster_ECor << " GeV" << std::endl;
			reco::SuperCluster suCl(SuperCluster_ECor, math::XYZPoint(MySClus[iSC].PosX, MySClus[iSC].PosY, MySClus[iSC].PosZ ), clusterRefVector_p[iseed], bc_ref );
			SCcoll.push_back(suCl);
		}
	}
	delete myCellsGC;
	return SCcoll;
}



reco::BasicClusterCollection S9ClusterAlgo::makeBasicClusters(const EcalRecHitCollection * recColl, const CaloSubdetectorGeometry * geometry)
{

	//Here's what we'll return.
	reco::BasicClusterCollection BCcoll;

	//std::cout << "--> Starting S9 clusterization..." << std::endl;

	CellsGC *myCellsGC = new CellsGC;
	*myCellsGC = InitCellsGC(recColl, geometry);

	std::vector < Sum9GC > S9;
	CreateSum9(myCellsGC, S9);

//        for (unsigned int icl = 0; icl < S9.size(); ++icl) {
//                std::vector<DetId> v_detId;
//                for (int i=0; i<25; ++i) {
//                        uint32_t detId = S9[icl].S9Cells[i].detId;
//                        std::cout << " xxx " << i << " " << detId << " " << S9[icl].S9Cells[i].FlagUsed << std::endl;
//                        if ( detId != 0 && S9[icl].S9Cells[i].FlagUsed ) {
//                                v_detId.push_back( detId );
//                        }
//                }
//                reco::BasicCluster bc( BasicCluster_ECor, math::XYZPoint( MySClus[iSC].Sum9[iS9].PosX, MySClus[iSC].Sum9[iS9].PosY, MySClus[iSC].Sum9[iS9].PosZ ), -1, v_detId );
//                std::cout << "Putting in basicClusterCollection:  iS9 = " << iS9 << " E = " << BasicCluster_ECor << " x = " << MySClus[iSC].Sum9[iS9].PosX << std::endl;
//                BCcoll.push_back( bc );
//        }

	int nCells = myCellsGC->nCells;

	if (nCells != 0) {
		std::vector < SuperClusterGC > MySClus;

		GroupSum9(S9, MySClus);
		for (int iSC = 0; iSC < int (MySClus.size()); iSC++)	// Loop on SuperClusters 
		{

			MySClus[iSC].Print();

			double LocalCorFac = 1.;
			double GlobalCorFac = 1.;

			double BasicCluster_ECor = 0.;

			for (int iS9 = 0; iS9 < MySClus[iSC].nSum9; iS9++)	// Loop on basic-clusters in the super-cluster
			{

				LocalCorFac = 1.;
				GlobalCorFac = 1.;

				if (MySClus[iSC].Sum9[iS9].NoFull == false) {

					if (ParticleType == 11) {

						LocalCorFac = LocalContCorFactor(MySClus[iSC].Sum9[iS9], false);
						GlobalCorFac = GlobalContCorFactor(MySClus[iSC].Sum9[iS9], "Electron");

					} else if (ParticleType == 22) {

						if (MySClus[iSC].nSum9 == 1)
							LocalCorFac = LocalContCorFactor(MySClus[iSC].Sum9[iS9], true);
						else
							LocalCorFac = LocalContCorFactor(MySClus[iSC].Sum9[iS9], false);


						GlobalCorFac = GlobalContCorFactor(MySClus[iSC].Sum9[iS9], "Photon");
					} else
						std::cout << "ParticleType must be 11 (Electron) or 22 (Photons)" << std::endl;
				} else {
					LocalCorFac = 1.;
					GlobalCorFac = 1;
				}
				BasicCluster_ECor = (MySClus[iSC].Sum9[iS9].E / (LocalCorFac * GlobalCorFac));
                                std::vector<DetId> v_detId;
                                for (int i=0; i<25; ++i) {
                                        uint32_t detId = MySClus[iSC].Sum9[iS9].S9Cells[i].detId;
                                        //std::cout << " xxx " << i << " " << detId << " " << MySClus[iSC].Sum9[iS9].S9Cells[i].FlagUsed << std::endl;
                                        if ( detId != 0 && MySClus[iSC].Sum9[iS9].S9Cells[i].FlagUsed ) {
                                                v_detId.push_back( detId );
                                        }
                                }
                                reco::BasicCluster bc( BasicCluster_ECor, math::XYZPoint( MySClus[iSC].Sum9[iS9].PosX, MySClus[iSC].Sum9[iS9].PosY, MySClus[iSC].Sum9[iS9].PosZ ), -1, v_detId );
                                //std::cout << "Putting in basicClusterCollection:  iS9 = " << iS9 << " E = " << BasicCluster_ECor << " x = " << MySClus[iSC].Sum9[iS9].PosX << std::endl;
                                BCcoll.push_back( bc );
			}

                        // no need for SC corrections at BC level
		}
	}
	delete myCellsGC;
	return BCcoll;
}
