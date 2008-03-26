#ifndef HTUPLE_H
#define HTUPLE_H
//////////////////////////////////////////////////////////////////////////
// $Id: HTuple.h,v 1.1 2006/12/13 14:10:16 cmkuo Exp $                                                                 //
//                                                                      //
// HTuple		                                                //
//                                                                      //
// Nested class hierarchy to hold information about HTuple columns.	//
//									//
// Author List:								//
// Marcel Kunze,  RUB, Mar. 99						//
// Apr.2001 (MK), Faster implementation based on THashList		//
// Copyright (C) 1999-2001, Ruhr-University Bochum.			//
//									//
//////////////////////////////////////////////////////////////////////////
// Root implementation of a Tuple.
//
// The member function "Column" provides the data for a Column of the ntuple.
// The string is the label of the Column as well as being a unique identifier
// of the Column. The second argument provides the data (Float_t or int) for
// one row in the Column. Note that only one line of code is needed to
// define the Column (if it has not been seen previously) and provide the
// data for each "event". 
//
// The third argument of "Column()" provides the default value for that
// Column (if it not provided, it defaults to 0.0). On a particular "event",
// if no call is made to "Column" for a particular Column, that Column's
// default value is used when filling the ntuple. Therefore, the default
// value should be set to an "unphysical" number.
//
// At the end of an "event", a call should be made to either "dumpData()" or
// "clearData()". "dumpData()" dumps all the data it has stored internally
// into the ntuple and then calls "clearData()".
// "clearData()" sets all the internal Column values to their defaults,
// without changing the ntuple. Therefore, if you want to keep the data
// that is presently in an NTuple, call "dumpData()"; else, call
// "clearData()". 

#include <iostream>
#include "Cosmics/HafHistogram/interface/HColumn.h"
#include "THashList.h"

class TTree;
class TBranch;

class HTuple : public TNamed {
    
public:
    
    HTuple();
    
    // Constructor to create a ROOT tuple with name and title:
    HTuple(const char*, const char*);
    
    // Destructor:
    virtual ~HTuple();
    
    // Column booking/filling. All these have the same name - Column(...)
    // Specify the data for a Column. The string is to the key to
    // the Column, so it must be unique. If an existing Column with the given
    // label is not found, a new one is created. The third, optional, argument
    // is the value to use if this Column is not otherwise filled in for a
    // given row of the tuple.
    
    
    // ====== Bool type ======
    // Make/fill Column with a single value
    void Column(const char* label, 
	Bool_t value,
	Bool_t defval = 0,
	const char* block = 0);
    // Make/fill Column-array. Length is fixed at creation time.
    virtual void Column(const char* label, 
	const HTAbsValVector<Bool_t> &vector, 
	Bool_t defval = kFALSE,
	const char* block = 0);
    // Make/fill Column-array. Length is variable and is taken from 
    // another Column.
    virtual void Column(const char* label, 
	const HTAbsValVector<Bool_t> &vector, 
	const char* ilab,
	Bool_t defval = kFALSE,
	const char* block = 0);
    
    
    // ====== Int type ======
    // Make/fill Column with a single value
    void Column(const char* label, 
	Int_t value, 
	Int_t defval = 0,
	const char* block = 0,
	const HTRange<Int_t>& range = HTRange<Int_t>());
    // Make/fill Column-array. Length is fixed at creation time.
    virtual void Column(const char* label, 
	const HTAbsValVector<Int_t> &vector, 
	Int_t defval = 0,
	const char* block = 0,
	const HTRange<Int_t>& range = HTRange<Int_t>());
    // Make/fill Column-array. Length is variable and is taken from 
    // another Column.
    virtual void Column(const char* label, 
	const HTAbsValVector<Int_t> &vector, 
	const char* ilab,
	Int_t defval = 0,
	const char* block = 0,
	const HTRange<Int_t> &range = HTRange<Int_t>());
    
    
    // ====== Float type ======
    // Make/fill Column with a single value
    void Column(const char* label, 
	Float_t value,
	Float_t defval = 0.0f,
	const char* block = 0,
	const HTRange<Float_t> &range = HTRange<Float_t>());
    // Make/fill Column-array. Length is fixed at creation time.
    void Column(const char* label, 
	const TVector& vec, 
	Float_t defval = 0.0f,
	const char* block = 0,
	const HTRange<Float_t> &range = HTRange<Float_t>());
    // Make/fill Column-array. Length is variable and is taken from 
    // another Column.
    void Column(const char* label, 
	const TVector& vec, 
	const char* ilab,
	Float_t defval = 0.0f,
	const char* block = 0,
	const HTRange<Float_t> &range = HTRange<Float_t>());
    // Make/fill Column-array. Length is fixed at creation time.
    virtual void Column(const char* label, 
	const HTAbsValVector<Float_t> &vector, 
	Float_t defval = 0.0f,
	const char* block = 0,
	const HTRange<Float_t> &range = HTRange<Float_t>());
    // Make/fill Column-array. Length is variable and is taken from 
    // another Column.
    virtual void Column(const char* label, 
	const HTAbsValVector<Float_t> &vector, 
	const char* ilab,
	Float_t defval = 0.0f,
	const char* block = 0,
	const HTRange<Float_t> &range = HTRange<Float_t>());
    
    
    // ====== Double type ======
    // Make/fill Column with a single value
    void Column(const char* label, 
	Double_t value,
	Double_t defval = 0.0,
	const char* block = 0,
	const HTRange<Double_t> &range = HTRange<Double_t>());
    // Make/fill Column-array. Length is fixed at creation time.
    virtual void Column(const char *label, 
		      const HTAbsValVector<Double_t> &vector, 
		      Double_t defval = 0.0,
		      const char* block = 0,
		      const HTRange<Double_t> &range = HTRange<Double_t>());
    // Make/fill Column-array. Length is variable and is taken from 
    // another Column.
    virtual void Column(const char* label, 
	const HTAbsValVector<Double_t> &vector, 
	const char* ilab,
	Double_t defval = 0.0,
	const char* block = 0,
	const HTRange<Double_t> &range = HTRange<Double_t>());
    
    // ====== fixed-length string Columns ======
    // ROOT ntuples allow variable length strings, thus N is ignored
    void Column(const char* label, 
	const char* value, 
	Int_t N,
	const char* defval = 0,
	const char* block = 0);
    void Column(const char* label,
	const char* value);
    
    
    // Dump all the data into the ntuple and then clear:
    void DumpData();
    
    // Set all the data to their default values:
    void ClearData();
    
    // Return the title of the ntuple:
    const char* Title() const;
    
    // Number of Columns:
    Int_t NColumns() const ;
    
    // Label for a particular Column
    const char* Label(Int_t) const ;
    
    // Print info about ntuple:
    virtual void PrintOn(ostream &) const;
    
    HTuple& operator=(const HTuple &v) { return *this; }
    
private:
    
    // Data members of HTuple:
    THashList* fMap; //!Do not stream
    TTree* fTree; //!Do not stream
public:
    ClassDef(HTuple,1) // NTuple
};



#endif
