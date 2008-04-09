#ifndef HHISTOGRAM_H
#define HHISTOGRAM_H
//////////////////////////////////////////////////////////////////////////
// $Id: HHistogram.h,v 1.1.1.1 2008/03/28 14:03:25 ferriff Exp $                                                                 //
//                                                                      //
// HHistogram			    				        //
//                                                                      //
// Histogram class							//
//                                                                      //
// Author: Marcel Kunze, RUB, March 99					//
// Copyright (C) 1999-2001, Ruhr-University Bochum.			//
//                                                                      //
//////////////////////////////////////////////////////////////////////////

#include "TH1F.h"
#include "TH2F.h"

//class HCandList;
//class HCandidate;

class TObject;
class TH1;
class HHistID;

class HHistogram : public TNamed { 
    
public:
    
    // Constructors:
    // 1-D:
    HHistogram( const char*, const char*, Int_t, Axis_t, Axis_t );
    // 2-D:
    HHistogram( const char*, const char*, Int_t, Axis_t, Axis_t, Int_t, Axis_t, Axis_t );
    // Profile:
    HHistogram( const char*, const char*, Int_t, Axis_t, Axis_t, Axis_t, Axis_t );
    
    // Destructor:
    virtual ~HHistogram();
    
    // Functions to fill a histo:
    void Accumulate( Axis_t, Stat_t weight= 1.0 );
    void Accumulate1( Axis_t, Stat_t weight= 1.0 );
    void Accumulate( Axis_t x, Axis_t y, Stat_t weight );
    void Accumulate2( Axis_t, Axis_t, Stat_t weight= 1.0 );
    
    // clear all contents
    void Reset();
    
    // accessors:
    const char* Title() const { return GetTitle(); };
    Float_t GetContents( Int_t, Int_t nbinsY= 0 ) const;
    
    
    // error on a content of a bin
    Float_t GetErrors( Int_t, Int_t nbinsY= 0 ) const;
    
    
    // contents of a bin
    Int_t GetEntries() const;
    // total number of entries
    Float_t GetWtSum() const;
    // weighted sum
    Int_t GetNbins( Int_t ) const;
    // number of bins in a dimension
    Float_t GetLow( Int_t ) const;
    // low edge of a dimension
    Float_t GetHigh( Int_t ) const;
    // high edge of a dimension
    Float_t GetAvg( Int_t ) const; // average X * wtsum
    // average of theDim
    Float_t GetCovar( Int_t, Int_t dim2= 0 ) const;
    // covariance between the two dimensions
    Int_t GetType() const;
    
    // Return the HHistID:
    HHistID GetHistID() const;
    
    // Test if input pointer matches pointer to ROOT histo:
    Bool_t PtrIsEqual( TObject* ptr ) const;
    
    HHistogram& operator<<(Axis_t x);

    // Extra functionality
    void Fill( Axis_t x ) { Accumulate1(x); }
    void Fill( Axis_t x, Axis_t y ) { Accumulate2(x,y); }
    void SetFillColor(Color_t fcolor) { histp->SetFillColor(fcolor); }
    virtual void Draw(Option_t *option="") { histp->Draw(option); }

private:
    
    // Satisfy Scotts weird function
    void setEntries( Int_t ) {}; 
    
    
    // Data membrs:
    TH1* histp;
    
public:
    ClassDef(HHistogram,1) //T histogram
};

#endif

