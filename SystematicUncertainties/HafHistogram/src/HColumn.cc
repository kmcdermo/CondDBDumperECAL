//////////////////////////////////////////////////////////////////////////
// $Id: HColumn.cc,v 1.1.1.1 2008/03/26 08:41:07 ferriff Exp $                                                                 //
//                                                                      //
// HColumn		                                                //
//                                                                      //
// Nested class hierarchy to hold information about HTuple columns.	//
//									//
// Author List:								//
// Marcel Kunze, RUB,Mar. 99						//
// Apr.2001 (MK),Inherit from TNamed to support THashList		//
// Copyright (C) 1999-2001,Ruhr-University Bochum.			//
//									//
//////////////////////////////////////////////////////////////////////////

#include "TTree.h"
#include "TBranch.h"

#include <iostream>

using namespace std;

#include "SystematicUncertainties/HafHistogram/interface/HColumn.h"

// Bool columns,stored as char (= 8 bit unsigned int):
BoolColumn::BoolColumn(const char* l,const Bool_t & v,const Bool_t & d,TTree* tp) : 
HColumn(l),fDefValue(d) 
{    
    // Create a new branch:
    fPointer = new Char_t;
    *(Char_t*)fPointer = v;
    TString leafs(l) ;
    leafs += "/b";
    fBranch = tp->Branch(fLabel,fPointer,&*leafs,8000);
}

BoolArrColumn::BoolArrColumn(const char* l,
			     const HTAbsValVector<Bool_t> & v,
			     const Bool_t & d,TTree* tp) :
HColumn(l),fDefValue(d) 
{    
    // Create a new branch:
    fMax = v.length();
    Char_t* bp = new Char_t[fMax];
    fPointer = bp;
    for (Int_t i = 0; i < fMax; ++i) bp[i] = v(i);
    TString leafs(fLabel);
    leafs += "[";
    char buf[33];
    sprintf(buf,"%i",fMax);
    leafs += buf;
    leafs += "]/b";
    fBranch = tp->Branch(fLabel,(void*) &bp[0],&*leafs,8000);
}

void BoolArrColumn::SetValue(const void* p,HColumn* cp) 
{    
    const HTAbsValVector<Bool_t>* vp = (const HTAbsValVector<Bool_t>*) p;
    if( vp->length() <  (UInt_t) fMax) {
	cerr << "BoolArrColumn::SetValue: input vector too short,"
	    << "use default values" << endl;
	SetDefValue();
    }
    else {
	for (Int_t i = 0; i < fMax; ++i) ((Char_t*)fPointer)[i] = (*vp)(i);
    }
}

BoolDynArrColumn::BoolDynArrColumn(const char* l,
				   const HTAbsValVector<Bool_t> & v,
				   const Bool_t & d,HColumn* inp,
				   TTree* tp) :
HColumn(l),fDefValue(d),fIndexPtr(inp) 
{   
    // Make a new branch:
    Int_t* np = (Int_t*) fIndexPtr->GetPointer();
    Char_t* bp = new Char_t[*np];
    fPointer = bp;
    for (Int_t i = 0; i < *np; ++i) bp[i] = v(i);
    TString leafs(fLabel);
    leafs += "[";
    leafs += fIndexPtr->GetLabel();
    leafs += "]/b";
    fBranch = tp->Branch(fLabel,(void*) &bp[0],&*leafs,8000);
}

void BoolDynArrColumn::SetDefValue() 
{    
    if(fPointer) delete [] (Bool_t*)fPointer;
    Int_t fMax = *((Int_t*)(fIndexPtr->GetPointer()));
    Char_t* bp = new Char_t[fMax];
    fPointer = bp;
    for (Int_t i = 0; i < fMax; ++i) bp[i] = fDefValue; 
    fBranch->SetAddress(&bp[0]);
}

void BoolDynArrColumn::SetValue(const void* p,HColumn* cp) 
{   
    const HTAbsValVector<Bool_t>* vp = (const HTAbsValVector<Bool_t>*) p;
    UInt_t* np = (UInt_t*) cp->GetPointer();
    if( *np > vp->length()) {
	cerr << "BoolDynArrColumn::SetValue: input vector too short,"
	    << "use default values" << endl;
	SetDefValue();
    }
    else {
	Char_t* bp = new Char_t[*np];
	if(fPointer) delete[] (Bool_t*)fPointer;
	fPointer = bp;
	for (UInt_t i = 0; i < *np; ++i) bp[i] = (*vp)(i);
	fBranch->SetAddress( &bp[0]);
    }
}


// Int columns:
IntColumn::IntColumn(const char* l,const Int_t & v,const Int_t & d,TTree* tp) : 
HColumn(l),fDefValue(d) 
{    
    // Create a new branch:
    fPointer = new Int_t;
    *(Int_t*)fPointer = v;
    TString leafs(l) ;
    leafs += "/I";
    fBranch = tp->Branch(fLabel,fPointer,&*leafs,8000);
}

IntArrColumn::IntArrColumn(const char* l,
			   const HTAbsValVector<Int_t> & v,
			   const Int_t & d,TTree* tp) :
HColumn(l),fDefValue(d) 
{   
    // Create a new branch:
    fMax = v.length();
    Int_t* ip = new Int_t[fMax];
    fPointer = ip;
    for (Int_t i = 0; i < fMax; ++i) ip[i] = v(i);
    TString leafs(fLabel);
    leafs += "[";
    char buf[33];
    sprintf(buf,"%i",fMax);
    leafs += buf;
    leafs += "]/I";
    fBranch = tp->Branch(fLabel,&ip[0],&*leafs,8000);
}

void IntArrColumn::SetValue(const void* p,HColumn* cp) 
{    
    const HTAbsValVector<Int_t>* vp = (const HTAbsValVector<Int_t>*) p;
    if( vp->length() <  (UInt_t) fMax) {
	cerr << "IntArrColumn::SetValue: input vector too short,"
	    << "use default values" << endl;
	SetDefValue();
    }
    else {
	for (Int_t i = 0; i < fMax; ++i) ((Int_t*)fPointer)[i] = (*vp)(i);
    }
}

IntDynArrColumn::IntDynArrColumn(const char* l,
				 const HTAbsValVector<Int_t> & v,
				 const Int_t & d,HColumn* inp,
				 TTree* tp) :
HColumn(l),fDefValue(d),fIndexPtr(inp) 
{   
    // Make a new branch:
    Int_t* np = (Int_t*) fIndexPtr->GetPointer();
    Int_t* ip = new Int_t[*np];
    fPointer = ip;
    for (Int_t i = 0; i < *np; ++i) ip[i] = v(i);
    TString leafs(fLabel);
    leafs += "[";
    leafs += fIndexPtr->GetLabel();
    leafs += "]/I";
    fBranch = tp->Branch(fLabel,&ip[0],&*leafs,8000);
}

void IntDynArrColumn::SetDefValue() 
{    
    if(fPointer) delete [] (Int_t*)fPointer;
    Int_t fMax = *((Int_t*)(fIndexPtr->GetPointer()));
    Int_t* ip = new Int_t[fMax];
    fPointer = ip;
    for (Int_t i = 0; i < fMax; ++i) ip[i] = fDefValue; 
    fBranch->SetAddress(&ip[0]);
}

void IntDynArrColumn::SetValue(const void* p,HColumn* cp) 
{   
    const HTAbsValVector<Int_t>* vp = (const HTAbsValVector<Int_t>*) p;
    UInt_t* np = (UInt_t*) cp->GetPointer();
    if( *np > vp->length()) {
	cerr << "IntDynArrColumn::SetValue: input vector too short,"
	    << "use default values" << endl;
	SetDefValue();
    }
    else {
	Int_t* ip = new Int_t[*np];
	if(fPointer) delete[] (Int_t*)fPointer;
	fPointer = ip;
	for (UInt_t i = 0; i < *np; ++i) ip[i] = (*vp)(i);
	fBranch->SetAddress( &ip[0]);
    }
}

// Float columns:
FloatColumn::FloatColumn(const char* l,const Float_t & v,const Float_t & d,TTree* tp) : 
HColumn(l),fDefValue(d) 
{   
    // Create a new branch:
    fPointer = new Float_t;
    *(Float_t*)fPointer = v;
    TString leafs(l) ;
    leafs += "/F";
    fBranch = tp->Branch(fLabel,fPointer,&*leafs,8000);
}

FloatArrColumn::FloatArrColumn(const char* l,
			       const HTAbsValVector<Float_t> & v,
			       const Float_t & d,TTree* tp) :
HColumn(l),fDefValue(d) 
{    
    // Create a new branch:
    fMax = v.length();
    Float_t* fp = new Float_t[fMax];
    fPointer = fp;
    for (Int_t i = 0; i < fMax; ++i) fp[i] = v(i);
    TString leafs(fLabel);
    leafs += "[";
    char buf[33];
    sprintf(buf,"%i",fMax);
    leafs += buf;
    leafs += "]/F";
    fBranch = tp->Branch(fLabel,&fp[0],&*leafs,8000);
}

void FloatArrColumn::SetValue(const void* p,HColumn* cp)
{    
    const HTAbsValVector<Float_t>* vp = (const HTAbsValVector<Float_t>*) p;
    if( vp->length() <  (UInt_t) fMax) {
	cerr << "FloatArrColumn::SetValue: input vector too short,"
	    << "use default values" << endl;
	SetDefValue();
    }
    else {
	for (Int_t i = 0; i < fMax; ++i) ((Float_t*)fPointer)[i] = (*vp)(i);
    }
}

FloatDynArrColumn::FloatDynArrColumn(const char* l,
				     const HTAbsValVector<Float_t> & v,
				     const Float_t & d,HColumn* ip,
				     TTree* tp) :
HColumn(l),fDefValue(d),fIndexPtr(ip) 
{    
    // Make a new branch:
    Int_t* np = (Int_t*) fIndexPtr->GetPointer();
    Float_t* fp = new Float_t[*np];
    fPointer = fp;
    for (Int_t i = 0; i < *np; ++i) fp[i] = v(i);
    TString leafs(fLabel);
    leafs += "[";
    leafs += fIndexPtr->GetLabel();
    leafs += "]/F";
    fBranch = tp->Branch(fLabel,&fp[0],&*leafs,8000);
}

void FloatDynArrColumn::SetDefValue() 
{    
    if(fPointer) delete [] (Float_t*)fPointer;
    Int_t fMax = *((Int_t*)(fIndexPtr->GetPointer()));
    Float_t* fp = new Float_t[fMax];
    fPointer = fp;
    for (Int_t i = 0; i < fMax; ++i) fp[i] = fDefValue; 
    fBranch->SetAddress(&fp[0]);   
}

void FloatDynArrColumn::SetValue(const void* p,HColumn* cp) 
{   
    const HTAbsValVector<Float_t>* vp = (const HTAbsValVector<Float_t>*) p;
    UInt_t* np = (UInt_t*) cp->GetPointer();
    if( *np >  vp->length()) {
	cerr << "IntDynArrColumn::SetValue: input vector too short,"
	    << "use default values" << endl;
	SetDefValue();
    }
    else {
	Float_t* fp = new Float_t[*np];
	if(fPointer) delete[] (Float_t*)fPointer;
	fPointer = fp;
	for (UInt_t i = 0; i < *np; ++i) fp[i] = (*vp)(i);
	fBranch->SetAddress( &fp[0]);
    }   
}

FloatArrColumn::FloatArrColumn(const char* l,const TVector & v,const Float_t & d,TTree* tp) :
HColumn(l),fDefValue(d) 
{    
    // Create a new branch:
    fMax = v.GetNoElements();
    Real_t* fp = new Real_t[fMax];
    fPointer = fp;
    for (Int_t i = 0; i < fMax; ++i) fp[i] = v(i);
    TString leafs(fLabel);
    leafs += "[";
    char buf[33];
    sprintf(buf,"%i",fMax);
    leafs += buf;
    leafs += "]/F";
    fBranch = tp->Branch(fLabel,&fp[0],&*leafs,8000);   
}

FloatDynArrColumn::FloatDynArrColumn(const char* l,const TVector & v,const Float_t & d,HColumn* ip,TTree* tp) :
HColumn(l),fDefValue(d),fIndexPtr(ip) 
{ 
    // Make a new branch:
    Int_t* np = (Int_t*) fIndexPtr->GetPointer();
    Real_t* fp = new Real_t[*np];
    fPointer = fp;
    for (Int_t i = 0; i < *np; ++i) fp[i] = v(i);
    TString leafs(fLabel);
    leafs += "[";
    leafs += fIndexPtr->GetLabel();
    leafs += "]/F";
    fBranch = tp->Branch(fLabel,&fp[0],&*leafs,8000);    
}


// Double columns:
DoubleColumn::DoubleColumn(const char* l,const Double_t & v,const Double_t & d,TTree* tp) : 
HColumn(l),fDefValue(d) 
{   
    // Create a new branch:
    fPointer = new Double_t;
    *(Double_t*)fPointer = v;
    TString leafs(l) ;
    leafs += "/D";
    fBranch = tp->Branch(fLabel,fPointer,&*leafs,8000);  
}

DoubleArrColumn::DoubleArrColumn(const char* l,
				 const HTAbsValVector<Double_t> & v,
				 const Double_t & d,TTree* tp) :
HColumn(l),fDefValue(d) 
{ 
    // Create a new branch:
    fMax = v.length();
    Double_t* dp = new Double_t[fMax];
    fPointer = dp;
    for (Int_t i = 0; i < fMax; ++i) dp[i] = v(i);
    TString leafs(fLabel);
    leafs += "[";
    char buf[33];
    sprintf(buf,"%i",fMax);
    leafs += buf;
    leafs += "]/D";
    fBranch = tp->Branch(fLabel,&dp[0],&*leafs,8000);   
}

void DoubleArrColumn::SetValue(const void* p,HColumn* cp) 
{   
    const HTAbsValVector<Double_t>* vp = (const HTAbsValVector<Double_t>*) p;
    if(vp->length() <  (UInt_t) fMax) {
	cerr << "DoubleArrColumn::SetValue: input vector too short,"
	    << "use default values" << endl;
	SetDefValue();
    }
    else {
	for (Int_t i = 0; i < fMax; ++i) ((Double_t*)fPointer)[i] = (*vp)(i);
    }   
}

DoubleDynArrColumn::DoubleDynArrColumn(const char* l,
				       const HTAbsValVector<Double_t> & v,
				       const Double_t & d,HColumn* ip,
				       TTree* tp) :
HColumn(l),fDefValue(d),fIndexPtr(ip) 
{
    // Make a new branch:
    Int_t* np = (Int_t*)(fIndexPtr->GetPointer());
    Double_t* dp = new Double_t[*np];
    fPointer = dp;
    for (Int_t i = 0; i < *np; ++i) dp[i] = v(i);
    TString leafs(fLabel);
    leafs += "[";
    leafs += fIndexPtr->GetLabel();
    leafs += "]/D";
    fBranch = tp->Branch(fLabel,&dp[0],&*leafs,8000);
}

void DoubleDynArrColumn::SetDefValue() 
{
    if(fPointer) delete [] (Double_t*)fPointer;
    Int_t fMax = *((Int_t*)(fIndexPtr->GetPointer()));
    Double_t* dp = new Double_t[fMax];
    fPointer = dp;
    for (Int_t i = 0; i < fMax; ++i) dp[i] = fDefValue; 
    fBranch->SetAddress(&dp[0]); 
}

void DoubleDynArrColumn::SetValue(const void* p,HColumn* cp) 
{
    const HTAbsValVector<Double_t>* vp = (const HTAbsValVector<Double_t>*) p;
    UInt_t* np = (UInt_t*) cp->GetPointer();
    if( *np > vp->length()) {
	cerr << "IntDynArrColumn::SetValue: input vector too short,"
	    << "use default values" << endl;
	SetDefValue();
    }
    else {
	Double_t* dp = new Double_t[*np];
	if(fPointer) delete[] (Double_t*)fPointer;
	fPointer = dp;
	for (UInt_t i = 0; i < *np; ++i) dp[i] = (*vp)(i);
	fBranch->SetAddress( &dp[0]);
    }  
}


// String column:
StringColumn::StringColumn(const TString & lb,const TString & v,const TString & d,TTree* tp) :
HColumn(lb),fDefValue(d) 
{
    // Create a new branch:
    Int_t l = v.Length();
    char* cp = new char[l+1];
    fPointer = cp;
    strcpy(cp,&*v);
    TString leafs(lb);
    leafs += "/C";
    fBranch = tp->Branch(lb,(void*)cp,&*leafs,8000); 
}

void StringColumn::SetDefValue() 
{ 
    if(fPointer) delete[] (TString*)fPointer;
    Int_t l = fDefValue.Length();
    char* cp = new char[l+1];
    fPointer = cp;
    strcpy(cp,&*fDefValue);
    fBranch->SetAddress(&cp[0]);
}

void StringColumn::SetValue(const void* p,HColumn* cp) 
{ 
    const char* cpin = (const char*) p;
    if(fPointer) delete[] (TString*)fPointer;
    Int_t l = strlen(cpin);
    char* chp = new char[l+1];
    fPointer = chp;
    strcpy(chp,cpin);
    fBranch->SetAddress(&chp[0]);
}
