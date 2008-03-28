#ifndef HHISTID_H
#define HHISTID_H
//////////////////////////////////////////////////////////////////////////
// $Id: HHistID.h,v 1.1.1.1 2008/03/26 08:41:07 ferriff Exp $                                                                 //
//                                                                      //
// HHistID				    				//
//                                                                      //
// This class allows ID numbers, ID strings or both to be used		//
// to uniquely identify histograms.  The actual use of this		//
// class depends on the concrete package that implements HepTuple	//
//                                                                      //
// Author: Marcel Kunze, RUB, Feb. 99					//
// Copyright (C) 1999-2001, Ruhr-University Bochum.			//
//                                                                      //
//////////////////////////////////////////////////////////////////////////

#include "TString.h"

class HHistID {
public: 
    // constructors
    HHistID(int theNumber);
    // constructor with number
    HHistID(const char * theString);
    // constructor with string
    HHistID(int theNumber,const char * theString);
    // constructor with both number and string
    HHistID(const HHistID & other);
    // copy constructor
    
    // accessors
    inline Bool_t isIDnumberSet() const { return _numSet; };
    // number used in constructor
    inline Bool_t isIDstringSet() const { return _stringSet; };
    // string used in constructor
    inline int getIDnumber() const { return _idNumber; };
    // the id number
    const char* getIDstring() const { return _idString.Data(); };
    // the id string
    
    // operations
    HHistID & operator++(); // prefix ++
    const HHistID operator++(int); // postfix ++
    
    HHistID & operator--(); // prefix -- 
    const HHistID operator--(int); // postfix -- 
    
private:
    Bool_t _numSet;
    Bool_t _stringSet;
    int _idNumber;
    TString _idString;
};

#endif
