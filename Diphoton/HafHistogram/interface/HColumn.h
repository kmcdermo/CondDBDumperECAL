#ifndef HCOLUMN_H
#define HCOLUMN_H
//////////////////////////////////////////////////////////////////////////
// $Id: HColumn.h,v 1.1.1.1 2008/03/28 14:03:25 ferriff Exp $                                                                 //
//                                                                      //
// HColumn		                                                //
//                                                                      //
// Nested class hierarchy to hold information about HTuple columns.	//
//									//
// Author List:								//
// Marcel Kunze,  RUB, Mar. 99						//
// Apr.2001 (MK), Inherit from TNamed to support THashList		//
// Copyright (C) 1999-2001, Ruhr-University Bochum.			//
//									//
//////////////////////////////////////////////////////////////////////////

#include "TNamed.h"
#include "TString.h"
#include "TVector.h"
#include "TTree.h"
#include "TBranch.h"

#ifndef HTRange_HH
#define HTRange_HH

template<class T> 
class HTRange {
        
 public:
    
  HTRange() : 
    fDefined(kFALSE), fLower(0), fUpper(0) 
    {
    }
  
  HTRange(T lower, T upper) : 
    fDefined(kTRUE), fLower(lower), fUpper(upper) 
    {
    }
  
  HTRange(const HTRange<T>& o) :
    fDefined(o.fDefined), fLower(o.fLower), fUpper(o.fUpper)
    {
    }
  
  virtual ~HTRange() {}  
  
  HTRange<T>& operator= (const HTRange<T> &o) 
    {
      if (&o == this) return *this;
      fDefined = o.fDefined ;
      fLower = o.fLower ;
      fUpper = o.fUpper ;
      return *this ;
    }
  
  Bool_t operator()() const { return fDefined ; }
  
  T lower() const { return fLower ; }
  T upper() const { return fUpper ; }
      
private:
    
    Bool_t fDefined ;
    T fLower, fUpper ;
    
};
#endif


#ifndef HTAbsValVector_HH
#define HTAbsValVector_HH

template<class T> 
class HTAbsValVector {
    
 public:
  
  // This must return the number of the stored elements
  virtual size_t length() const = 0 ;
  
  // This provides access to the indifidual elements.
  // Index runs from 0 to length()-1. Here () means that we do not requre 
  // index checking from it, though the real classe may do what they like.
  virtual const T& operator()(size_t i) const = 0;
  virtual T& operator()(size_t i) = 0;
  virtual const T& operator[](size_t i) const = 0;
  virtual T& operator[](size_t i) = 0;
    
};
#endif


#ifndef HTAbsValArray_HH
#define HTAbsValArray_HH

template<class T, size_t N>
class HTValArray : public HTAbsValVector<T> {

 public:

  HTValArray() {}
  HTValArray( const T& v ) ;
  HTValArray( const HTValArray<T,N> &v ) ;

  HTValArray<T,N>& operator=( const HTValArray<T,N> &v ) ;

  virtual size_t length() const ;
  virtual const T& operator()(size_t i) const ;

  T& operator()(size_t i) { return _array[i] ; }
  const T& operator[](size_t i) const {
    assert ( i<N ) ;
    return _array[i] ;
  }
  T& operator[](size_t i) {
    assert ( i<N ) ;
    return _array[i] ;
  }
  
 private:
  
  T _array[N] ;
  
};

template<class T, size_t N>
HTValArray<T,N>::HTValArray( const T& v )
{
  for ( int i = 0 ; i < N ; i ++ ) {
    _array[i] = v ;
  }
}

template<class T, size_t N>
HTValArray<T,N>::HTValArray( const HTValArray<T,N> &v )
{
  for ( int i = 0 ; i < N ; i ++ ) {
    _array[i] = v._array[i] ;
  }
}

template<class T, size_t N>
HTValArray<T,N>&
HTValArray<T,N>::operator=( const HTValArray<T,N> &v )
{
  for ( int i = 0 ; i < N ; i ++ ) {
    _array[i] = v._array[i] ;
  }
  return *this ;
}

template<class T, size_t N>
size_t
HTValArray<T,N>::length() const
{
  return N;
}

template<class T, size_t N>
const T&
HTValArray<T,N>::operator()(size_t i) const
{
  return _array[i] ;
}

#endif  


#ifndef HTValVector_HH
#define HTValVector_HH

using namespace std;
#include <deque>

template<class T>
class HTValVector : public HTAbsValVector<T>
{

 public:
  
  typedef typename deque<T>::value_type value_type;
  typedef typename deque<T>::pointer pointer;
  typedef typename deque<T>::const_pointer conts_pointer;
  typedef typename deque<T>::iterator iterator;
  typedef typename deque<T>::const_iterator const_iterator;
  typedef typename deque<T>::reference reference;
  typedef typename deque<T>::const_reference const_reference;
  typedef typename deque<T>::size_type size_type;
  typedef typename deque<T>::difference_type difference_type;
  
  HTValVector() : _d() {}
  HTValVector( size_type n ) : _d(n) {}
  HTValVector( const HTValVector<T> &d ) : _d(d._d) {}
  HTValVector( const deque<T> &d ) : _d(d) {}

  virtual ~HTValVector() {}

  virtual HTValVector<T>& operator=(const HTValVector<T>& d) { _d=d._d; return *this; }
  virtual bool operator<(const HTValVector<T>& d) { return _d<d._d; }
  virtual bool operator==(const HTValVector<T>& d) { return _d==d._d; }
  virtual const T& operator()(size_type i) const { return _d[i]; }
  virtual T& operator()(size_type i) { return _d[i]; }
  virtual const T& operator[](size_type i) const { return _d[i]; }
  virtual T& operator[](size_type i) { return _d[i]; }

  virtual size_type size() const { return _d.size(); }
  virtual size_type length() const { return _d.size(); }
  virtual size_type entries() const { return _d.size(); }
  virtual void append(const T& t) { _d.push_back(t); }
  virtual void prepend(const T& t) { _d.push_front(t); }
  virtual bool remove(const T& t) {
    for( iterator i=_d.begin(); i!=_d.end(); ++i ) {      
      if( *i == t ) {
        _d.erase(i);
        return true;
      }
    }
    return false;
  }
  virtual bool contains(const T& t) const {
    for( const_iterator i=_d.begin(); i!=_d.end(); ++i ) {
      if( *i == t ) return true;
    }
    return false;
  }
  virtual bool isEmpty() const { return _d.empty(); }
  virtual void reshape(size_type n) { _d.resize(n); }
  virtual iterator begin() { return _d.begin(); }
  virtual iterator   end() { return _d.end(); }
  virtual void clear() { _d.clear(); }
  
 protected:

  deque<T> _d;

};

#endif


#ifndef HTValOrderedVector_HH
#define HTValOrderedVector_HH

using namespace std;
#include <vector>
#include <deque>

template<class T>
class HTValOrderedVector : public HTValVector<T>
{

 public:

  typedef typename deque<T>::size_type size_type;

  HTValOrderedVector() : HTValVector<T>() {}
  HTValOrderedVector( size_type n ) : HTValVector<T>() {}
  HTValOrderedVector( const HTValOrderedVector<T> &v )
    : HTValVector<T>(v) {}
  HTValOrderedVector( const vector<T> &v ) : HTValVector<T>(v) {}

  virtual ~HTValOrderedVector() {}

  void resize(size_type n) {}

};

#endif 


// Parent class (abstract):
class HColumn : public TNamed {

 public:

  HColumn(const char* l) : 
    TNamed(l,l), fLabel(l), fUseDefValue(0), fPointer(0), fBranch(0) {}
  virtual ~HColumn() {}
  const TString &GetLabel() const { return fLabel; }
  TBranch* GetBrPointer() { return fBranch; }
  void* GetPointer() { return fPointer; }
  void SetPointer(void* p) { fPointer = p; }
  void SetUseDefValue(Int_t b) { fUseDefValue = b; }
  const Int_t & GetUseDefValue() const { return fUseDefValue; }
  virtual void SetDefValue() = 0;
  virtual void SetValue(const void*, HColumn* cp=0) = 0;

 protected:

  TString fLabel;
  Int_t fUseDefValue;
  void* fPointer; //!
  TBranch* fBranch;
  
  ClassDef(HColumn,1) // HColumn
    
};


// Classes for Bool_t:
class BoolColumn : public HColumn {
public:
    BoolColumn(const char*, const Bool_t &, const Bool_t &, TTree*);
    virtual ~BoolColumn() { delete (Bool_t*)fPointer; }
    virtual void SetDefValue() { *(Char_t*)fPointer = fDefValue; }
    virtual void SetValue(const void* p, HColumn* cp=0) { 
	*(Char_t*)fPointer = *(const Bool_t*)p;
    }
private:
    Bool_t fDefValue;
};

class BoolArrColumn : public HColumn {
public:
    BoolArrColumn(const char*, const HTAbsValVector<Bool_t> &, const Bool_t &, 
	TTree*);
    virtual ~BoolArrColumn() { delete[] (Bool_t*)fPointer; }
    virtual void SetDefValue() { 
	for(Int_t i = 0; i < fMax; ++i) ((Char_t*)fPointer)[i] = fDefValue; 
    }
    virtual void SetValue(const void*, HColumn* cp=0);
private:
    Bool_t fDefValue;
    Int_t fMax;
};

class BoolDynArrColumn : public HColumn {
public:
    BoolDynArrColumn(const char*, const HTAbsValVector<Bool_t> &,
	const Bool_t &, HColumn*, TTree*);
    virtual ~BoolDynArrColumn() { delete[] (Bool_t*)fPointer; }
    virtual void SetDefValue();
    virtual void SetValue(const void*, HColumn* cp=0);
private:
    Bool_t fDefValue;
    HColumn* fIndexPtr;
};

// Classes for Int_t:
class IntColumn : public HColumn {
public:
    IntColumn(const char*, const Int_t &, const Int_t &, TTree*);
    virtual ~IntColumn() { delete (Int_t*)fPointer; }
    virtual void SetDefValue() { *(Int_t*)fPointer = fDefValue; }
    virtual void SetValue(const void* p, HColumn* cp=0) { 
	*(Int_t*)fPointer = *(const Int_t*)p;
    }
private:
    Int_t fDefValue;
};

class IntArrColumn : public HColumn {
public:
    IntArrColumn(const char*, const HTAbsValVector<Int_t> &, const Int_t &, 
	TTree*);
    virtual ~IntArrColumn() { delete[] (Int_t*)fPointer; }
    virtual void SetDefValue() { 
	for(Int_t i = 0; i < fMax; ++i) ((Int_t*)fPointer)[i] = fDefValue; 
    }
    virtual void SetValue(const void*, HColumn* cp=0);
private:
    Int_t fDefValue;
    Int_t fMax;
};

class IntDynArrColumn : public HColumn {
public:
    IntDynArrColumn(const char*, const HTAbsValVector<Int_t> &,
	const Int_t &, HColumn*, TTree*);
    virtual ~IntDynArrColumn() { delete[] (Int_t*)fPointer; }
    virtual void SetDefValue();
    virtual void SetValue(const void*, HColumn* cp=0);
private:
    Int_t fDefValue;
    HColumn* fIndexPtr;
};

// Classes for Float_t:
class FloatColumn : public HColumn {
public:
    FloatColumn(const char*, const Float_t &, const Float_t &, TTree*);
    virtual ~FloatColumn() { delete (Float_t*)fPointer; }
    virtual void SetDefValue() { *(Float_t*)fPointer = fDefValue; }
    virtual void SetValue(const void* p, HColumn* cp=0) { 
	*(Float_t*)fPointer = *(const Float_t*)p; 
    }
private:
    Float_t fDefValue;
};

class FloatArrColumn : public HColumn {
public:
    FloatArrColumn(const char*, const HTAbsValVector<Float_t> &, const Float_t &, 
	TTree*);
    FloatArrColumn(const char*, const TVector &, const Float_t &, 
	TTree*);
    virtual ~FloatArrColumn() { delete[] (Float_t*)fPointer; }
    virtual void SetDefValue() { 
	for(Int_t i = 0; i < fMax; ++i) ((Float_t*)fPointer)[i] = fDefValue; 
    }
    virtual void SetValue(const void*, HColumn* cp=0);
private:
    Float_t fDefValue;
    Int_t fMax;
};

class FloatDynArrColumn : public HColumn {
public:
    FloatDynArrColumn(const char*, const HTAbsValVector<Float_t> &,
	const Float_t &, HColumn*, TTree*);
    FloatDynArrColumn(const char*, const TVector &,
	const Float_t &, HColumn*, TTree*);
    virtual ~FloatDynArrColumn() { delete[] (Float_t*)fPointer; }
    virtual void SetDefValue();
    virtual void SetValue(const void*, HColumn* cp=0);
private:
    Float_t fDefValue;
    HColumn* fIndexPtr;
};

// Classes for Double_t:
class DoubleColumn : public HColumn {
public:
    DoubleColumn(const char*, const Double_t &, const Double_t &, TTree*);
    virtual ~DoubleColumn() { delete (Double_t*)fPointer; }
    virtual void SetDefValue() { *(Double_t*)fPointer = fDefValue; }
    virtual void SetValue(const void* p, HColumn* cp=0) { 
	*(Double_t*)fPointer = *(const Double_t*)p; 
    }
private:
    Double_t fDefValue;
};

class DoubleArrColumn : public HColumn {
public:
    DoubleArrColumn(const char*, const HTAbsValVector<Double_t> &, 
	const Double_t &, TTree*);
    virtual ~DoubleArrColumn() { delete[] (Double_t*)fPointer; }
    virtual void SetDefValue() { 
	for(Int_t i = 0; i < fMax; ++i) ((Double_t*)fPointer)[i] = fDefValue; 
    }
    virtual void SetValue(const void*, HColumn* cp=0);
private:
    Double_t fDefValue;
    Int_t fMax;
};

class DoubleDynArrColumn : public HColumn {
public:
    DoubleDynArrColumn(const char*, const HTAbsValVector<Double_t> &,
		      const Double_t &, HColumn*, TTree*);
    virtual ~DoubleDynArrColumn() { delete[] (Double_t*)fPointer; }
    virtual void SetDefValue();
    virtual void SetValue(const void*, HColumn* cp=0);
private:
    Double_t fDefValue;
    HColumn* fIndexPtr;
};

// String column:
class StringColumn : public HColumn {
public:
    StringColumn(const TString &, const TString &, 
	const TString &, TTree*);
    virtual ~StringColumn() { delete[] (TString*)fPointer; }
    virtual void SetDefValue();
    virtual void SetValue(const void*, HColumn* cp=0);
private:
    TString fDefValue;
};

#endif
