#ifndef HTUPLEMANAGER_H
#define HTUPLEMANAGER_H
//////////////////////////////////////////////////////////////////////////
// $Id: HTupleManager.h,v 1.1.1.1 2008/03/26 08:41:07 ferriff Exp $                                                                 //
//                                                                      //
// HTupleManager							//
//                                                                      //
// Persistence manager					    		//
//                                                                      //
// Original Author: Marcel Kunze, Bochum University, Feb. 99		//
// HIGGS version: Sergey Ganzhur, Bochum University, Dec. ++2k		//
// Copyright (C) 1999-2002, Ruhr-University Bochum.			//
//									//
//////////////////////////////////////////////////////////////////////////

#include <iostream>
#include "TFile.h"
#include "TDirectory.h"
#include "TString.h"
#include "THashList.h"
#include "TH1.h"
#include "TH2.h"
//#include "HAF/HafBase/interface/HCandList.h"
#include "SystematicUncertainties/HafHistogram/interface/HHistID.h"
#include "SystematicUncertainties/HafHistogram/interface/HHistogram.h"
#include "SystematicUncertainties/HafHistogram/interface/HTuple.h"

class HTupleManager : public TObject {
    
public:
    //Constructor
    HTupleManager(const char* file= "framework.root",const char* fmode= "RECREATE");
    //Destructor
    virtual ~HTupleManager();
    void SetHepTupleMode(Bool_t yesNo=kTRUE) { fHepTuple = yesNo; }
    Bool_t SetFileName( const char* );
    int Save();
    int Store() { return Save(); }
    Bool_t SetDir( const char* path= "/" );
    const char* GetDir() const;
    size_t GetNumDir() const;
    const char* GetDirName( size_t ) const;
    Bool_t GetDirIndex( const char*, size_t & ) const;
    size_t GetNumHist() const;
    size_t GetNumHist( const char* path ) const;
    void PrintOn ( ostream & out= cout ) const;
    TObject* Add(TObject* item);
    TObject* Put(TObject* item) { return Add(item); } 
    TObject* Get(const char *name);
    void Remove(TObject* item);
    void Remove(const char *name);
    void ClearAll();
    HTupleManager(const HTupleManager &h) : rtfilep(h.rtfilep), olist(0) {}
    HTupleManager& operator=(const HTupleManager &v ) { return *this; }
    // 1D histogram without id
    HHistogram* Histogram( const char*, int, float, float );
    // Create a 1D histogram. Behaves generally like ntuple().
    HHistogram* Histogram( const char*, int, float, float, HHistID ); 
    // 2D histogram without id:
    HHistogram* Histogram( const char*, int, float, float, int, float, float );
    // 2D histogram with ID:
    HHistogram* Histogram( const char*, int, float, float, int, float, float, HHistID );
    // Profile histogram without ID:
    HHistogram* Profile( const char*, int, float, float, float, float );
    // Profile histogram with ID:
    HHistogram* Profile( const char*, int, float, float, float, float, HHistID );
    // Ntuple
    HTuple* Ntuple( const char* nttitle );
    HTuple* Ntuple( const char* nttitle,HHistID hhid );
    void DumpAllData();
protected:        
    TFile* rtfilep;
    THashList *olist;  //! List of external objects
    THashList *hlist;  //! List of internal histograms
    THashList *ntlist; //! List of internal ntuples
    Bool_t fHepTuple;  //! HepTuple mode for names
    
    // Helper function to compose ROOT names:
    const char* makeName( const char*, HHistID );
    
    // Helper function to set directory according to PicoTuple's weird
    // indexing scheme:
    //  void setDirFromIndexList( const THepCList<size_t> & ) const;
    
    // Helper function to reset current ROOT-file to this manager:
    void checkFile( const char* opt= "" ) const;
    
    // Helper function for setFileName, moves directories and contents
    void movedir( TDirectory* olddir, TDirectory* newdir );
    
    HHistogram* getHistByName( const char* );
    HHistogram* getHistByTitle( const char* );

public:
    ClassDef(HTupleManager,1) // Histogram and ntuple manager
};      

// standalone print
ostream&  operator << (ostream& o, const HTupleManager&);

#endif

