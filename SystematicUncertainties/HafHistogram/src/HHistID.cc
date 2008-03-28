//////////////////////////////////////////////////////////////////////////
// $Id: HHistID.cc,v 1.1.1.1 2008/03/26 08:41:07 ferriff Exp $                                                                 //
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

#include "SystematicUncertainties/HafHistogram/interface/HHistID.h"

HHistID::HHistID(int theNumber) : 
_numSet(1), 
_stringSet(0),
_idNumber(theNumber),
_idString() {}

HHistID::HHistID(const char * theString) :
_numSet(0), 
_stringSet(1),
_idNumber(0),
_idString(theString) {}

HHistID::HHistID(int theNumber,const char * theString) :
_numSet(1), 
_stringSet(1),
_idNumber(theNumber),
_idString(theString) {}

HHistID::HHistID(const HHistID & other) :
_numSet(other._numSet), 
_stringSet(other._stringSet),
_idNumber(other._idNumber),
_idString(other._idString) {}

// prefix increment number ID
HHistID & HHistID::operator++() { 
    if ( _numSet ) {
	++_idNumber;
    };
    // return this
    return *this; 
}

// postfix increment number ID
const HHistID HHistID::operator++(int) { 
    // initialize result
    HHistID result(*this);
    // increment this 
    ++(*this);
    // return result
    return result; 
}

// prefix decrement number ID
HHistID & HHistID::operator--() { 
    if ( _numSet ) {
	--_idNumber;
    };
    // return this
    return *this; 
}

// postfix decrement number ID
const HHistID HHistID::operator--(int) { 
    // initialize result
    HHistID result(*this);
    // increment this 
    --(*this);
    // return result
    return result; 
}
