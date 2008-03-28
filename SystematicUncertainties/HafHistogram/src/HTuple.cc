//////////////////////////////////////////////////////////////////////////
// $Id: HTuple.cc,v 1.1.1.1 2008/03/26 08:41:07 ferriff Exp $                                                                 //
//                                                                      //
// HTuple		                                                //
//                                                                      //
// Nested class hierarchy to hold information about HTuple columns.	//
//									//
// Author List:								//
// Marcel Kunze, RUB,Mar. 99						//
// Apr.2001 (MK),Faster implementation based on THashList		//
// Copyright (C) 1999-2001,Ruhr-University Bochum.			//
//									//
//////////////////////////////////////////////////////////////////////////

#include "TTree.h"
#include "TBranch.h"
#include "TString.h"

#include "SystematicUncertainties/HafHistogram/interface/HTuple.h"

using namespace std;

ClassImp(HTuple)

HTuple::HTuple() 
{ 
    fMap   = new THashList(); 
}

// Constructor to create a tuple with name and title:
HTuple::HTuple(const char* name,const char* title) : 
TNamed(name,title)
{
    fMap  = new THashList(); 
    fTree = new TTree(name,title);
}

// Destructor:
HTuple::~HTuple() 
{
    delete fTree;
    delete fMap;
}

// Column booking/filling. All these have the same name - Column(...)

// Specify the data for a Column. The string is to the key to
// the Column,so it must be unique.


// ===== = Bool type ======
// Make/fill Column with a single value
void HTuple::Column(const char* label,
			Bool_t value,
			Bool_t defval,
			const char* block) 
{   
    HColumn* colp = (HColumn*) fMap->FindObject(label);
    if(colp) {
	// Column exists,fill corresponding memory location with value:
	colp->SetValue(&value);
	colp->SetUseDefValue(0);
    }
    else {
	// Create a new Column:
	colp = new BoolColumn(label,value,defval,fTree);
	fMap->Add(colp);
    }
    
}

// Make/fill Column-array. Length is fixed at creation time.
void HTuple::Column(const char* label,
			const HTAbsValVector<Bool_t> &vector,
			Bool_t defval,
			const char* block)
{   
    HColumn* colp = (HColumn*) fMap->FindObject(label);
    if(colp) {
	// Column exists,fill corresponding memory location with value:
	colp->SetValue(&vector);
	colp->SetUseDefValue(kFALSE);
    }
    else {
	// Create a new Column:
	colp = new BoolArrColumn(label,vector,defval,fTree);
	fMap->Add(colp);
    }
    
}

// Make/fill Column-array. Length is variable and is taken from 
// another Column.
void HTuple::Column(const char* label,
			const HTAbsValVector<Bool_t> &vector,
			const char* ilab,
			Bool_t defval,
			const char* block) 
{ 
    HColumn* colp = (HColumn*) fMap->FindObject(label);
    if(colp) {
	// Column exists,fill corresponding memory location with value:
	colp->SetValue(&vector,(HColumn*)fMap->FindObject(ilab));
	colp->SetUseDefValue(kFALSE);
    }
    else {
	// Create a new branch:
	HColumn* indexPtr = (HColumn*) fMap->FindObject(ilab);
	colp = new BoolDynArrColumn(label,vector,defval,indexPtr,fTree);
	fMap->Add(colp);
    }
    
}


// ===== = Int type ======
// Make/fill Column with a single value
void HTuple::Column(const char* label,
			Int_t value,
			Int_t defval,
			const char* block,
			const HTRange<Int_t> &range)
{
    HColumn* colp = (HColumn*) fMap->FindObject(label);
    if(colp) {
	// Column exists,fill corresponding memory location with value:
	colp->SetValue(&value);
	colp->SetUseDefValue(0);
    }
    else {
	// Create a new Column:
	colp = new IntColumn(label,value,defval,fTree);
	fMap->Add(colp);
    }
    
}

// Make/fill Column-array. Length is fixed at creation time.
void HTuple::Column(const char* label,
			const HTAbsValVector<Int_t> &vector,
			Int_t defval,
			const char* block,
			const HTRange<Int_t> &range)
{
    HColumn* colp = (HColumn*) fMap->FindObject(label);
    if(colp) {
	// Column exists,fill corresponding memory location with value:
	colp->SetValue(&vector);
	colp->SetUseDefValue(kFALSE);
    }
    else {
	// Create a new Column:
	colp = new IntArrColumn(label,vector,defval,fTree);
	fMap->Add(colp);
    }
    
}

// Make/fill Column-array. Length is variable and is taken from 
// another Column.
void HTuple::Column(const char* label,
			const HTAbsValVector<Int_t> &vector,
			const char* ilab,
			Int_t defval,
			const char* block,
			const HTRange<Int_t> &range)
{
    HColumn* colp = (HColumn*) fMap->FindObject(label);
    if(colp) {
	// Column exists,fill corresponding memory location with value:
	colp->SetValue(&vector,(HColumn*)fMap->FindObject(ilab));
	colp->SetUseDefValue(kFALSE);
    }
    else {
	// Create a new branch:
	HColumn* indexPtr = (HColumn*) fMap->FindObject(ilab);
	colp = new IntDynArrColumn(label,vector,defval,indexPtr,fTree);
	fMap->Add(colp);
    }
    
}


// ===== = Float type ======
// Make/fill Column with a single value
void HTuple::Column(const char* label,
			Float_t value,
			Float_t defval,
			const char* block,
			const HTRange<Float_t> &range) 
{
    HColumn* colp = (HColumn*) fMap->FindObject(label);
    if(colp) {
	// Column exists,fill corresponding memory location with value:
	colp->SetValue(&value);
	colp->SetUseDefValue(0);
    }
    else {
	// Create a new Column:
	colp = new FloatColumn(label,value,defval,fTree);
	fMap->Add(colp);
    }
    
    
}

// Make/fill Column-array. Length is fixed at creation time.
void HTuple::Column(const char* label,
			const TVector &vector,
			Float_t defval,
			const char* block,
			const HTRange<Float_t> &range)
{
    HColumn* colp = (HColumn*) fMap->FindObject(label);
    if(colp) {
	// Column exists,fill corresponding memory location with value:
	colp->SetValue(&vector);
	colp->SetUseDefValue(0);
    }
    else {
	// Create a new Column:
	colp = new FloatArrColumn(label,vector,defval,fTree);
	fMap->Add(colp);
    }
    
    
}

// Make/fill Column-array. Length is variable and is taken from 
// another Column.
void HTuple::Column(const char* label,
			const TVector &vector,
			const char *ilab,
			Float_t defval,
			const char* block,
			const HTRange<Float_t> &range)
{   
    HColumn* colp = (HColumn*) fMap->FindObject(label);
    if(colp) {
	// Column exists,fill corresponding memory location with value:
	colp->SetValue(&vector,(HColumn*)fMap->FindObject(ilab));
	colp->SetUseDefValue(0);
    }
    else {
	// Create a new branch:
	HColumn* indexPtr = (HColumn*) fMap->FindObject(ilab);
	colp = new FloatDynArrColumn(label,vector,defval,indexPtr,fTree);
	fMap->Add(colp);
    }
    
}

// Make/fill Column-array. Length is fixed at creation time.
void HTuple::Column(const char* label,
			const HTAbsValVector<Float_t> &vector,
			Float_t defval,
			const char* block,
			const HTRange<Float_t> &range) 
{
    HColumn* colp = (HColumn*) fMap->FindObject(label);
    if(colp) {
	// Column exists,fill corresponding memory location with value:
	colp->SetValue(&vector);
	colp->SetUseDefValue(kFALSE);
    }
    else {
	// Create a new Column:
	colp = new FloatArrColumn(label,vector,defval,fTree);
	fMap->Add(colp);
    }
    
    
}

// Make/fill Column-array. Length is variable and is taken from 
// another Column.
void HTuple::Column(const char* label,
			const HTAbsValVector<Float_t> &vector,
			const char *ilab,
			Float_t defval,
			const char* block,
			const HTRange<Float_t> &range) 
{
    HColumn* colp = (HColumn*) fMap->FindObject(label);
    if(colp) {
	// Column exists,fill corresponding memory location with value:
	colp->SetValue(&vector,(HColumn*)fMap->FindObject(ilab));
	colp->SetUseDefValue(kFALSE);
    }
    else {
	// Create a new branch:
	HColumn* indexPtr = (HColumn*) fMap->FindObject(ilab);
	colp = new FloatDynArrColumn(label,vector,defval,indexPtr,fTree);
	fMap->Add(colp);
    }
    
}

// ===== = Double type ======
// Make/fill Column with a single value
void HTuple::Column(const char* label,
			Double_t value,
			Double_t defval,
			const char* block,
			const HTRange<Double_t> &range) 
{
    HColumn* colp = (HColumn*) fMap->FindObject(label);
    if(colp) {
	// Column exists,fill corresponding memory location with value:
	colp->SetValue(&value);
	colp->SetUseDefValue(0);
    }
    else {
	// Create a new Column:
	colp = new DoubleColumn(label,value,defval,fTree);
	fMap->Add(colp);
    }
    
    
}

// Make/fill Column-array. Length is fixed at creation time.
void HTuple::Column(const char* label,
			const HTAbsValVector<Double_t> &vector,
			Double_t defval,
			const char* block,
			const HTRange<Double_t> &range)
{ 
    HColumn* colp = (HColumn*) fMap->FindObject(label);
    if(colp) {
	// Column exists,fill corresponding memory location with value:
	colp->SetValue(&vector);
	colp->SetUseDefValue(kFALSE);
    }
    else {
	// Create a new Column:
	colp = new DoubleArrColumn(label,vector,defval,fTree);
	fMap->Add(colp);
    }
    
    
}

// Make/fill Column-array. Length is variable and is taken from 
// another Column.
void HTuple::Column(const char* label,
			const HTAbsValVector<Double_t> &vector,
			const char *ilab,
			Double_t defval,
			const char* block,
			const HTRange<Double_t> &range)
{
    HColumn* colp = (HColumn*) fMap->FindObject(label);
    if(colp) {
	// Column exists,fill corresponding memory location with value:
	colp->SetValue(&vector,(HColumn*)fMap->FindObject(ilab));
	colp->SetUseDefValue(kFALSE);
    }
    else {
	// Create a new branch:
	HColumn* indexPtr = (HColumn*) fMap->FindObject(ilab);
	colp = new DoubleDynArrColumn(label,vector,defval,indexPtr,fTree);
	fMap->Add(colp);
    }
    
}

// ===== = string Columns ======
// Can actually be variable length in ROOT,N is ignored:
void HTuple::Column(const char* label,
			const char* value,
			Int_t N,
			const char* defval,
			const char* block)
{
    HColumn* colp = (HColumn*) fMap->FindObject(label);
    if(colp) {
	// Branch exists,fill corresponding memory location with value:
	Column(label,value);
    }
    else {
	// Create a new branch:
	colp = new StringColumn(label,value,defval,fTree);
	fMap->Add(colp);
    }
    
}

void HTuple::Column(const char* label,const char* value)
{    
    HColumn* colp = (HColumn*) fMap->FindObject(label);
    if(colp) {
	colp->SetValue(value);
	colp->SetUseDefValue(0);
    }
    else {
	cerr << "HTuple::Column: Column "<< label << " does not exist" << endl;
    }
    
}


// Dump all the data into the ntuple and then clear
void HTuple::DumpData()
{    
    for (Int_t i=0;i<fMap->GetSize();i++)
    {
	HColumn *col = (HColumn*) fMap->At(i);
	if(col->GetUseDefValue()) col->SetDefValue();
	col->SetUseDefValue(1);
    }
    fTree->Fill(); 
    
}

// Set all the data to their default values:
void HTuple::ClearData() 
{   
    for (Int_t i=0;i<fMap->GetSize();i++)
    {
	HColumn *col = (HColumn*) fMap->At(i);
	col->SetDefValue();
	col->SetUseDefValue(1);
    }
    return; 
    
}

// Return title of ntuple:
const char* HTuple::Title() const 
{
    return fTree->GetTitle();
}

// number of Columns
Int_t HTuple::NColumns() const 
{
    return fMap->GetSize();    
}

// Return label for a particular Column with index i:
const char* HTuple::Label(Int_t i) const 
{
    TString str;
    if(i >= 0 && i < fMap->GetSize()) str = ((HColumn*)fMap->At(i))->GetLabel();
    else str = "unknown Column index";
    return str.Data();
    
}

// Print info about ntuple:
void HTuple::PrintOn(ostream & o)  const 
{  
    cout << "HTuple: ntuple " << Title() << " has " << NColumns() 
	<< " Columns." << endl;
    cout << "Complete printout follows: " << endl;
    fTree->Print(); 
}
