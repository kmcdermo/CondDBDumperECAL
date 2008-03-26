//////////////////////////////////////////////////////////////////////////
// $Id: HTupleManager.cc,v 1.2 2006/12/20 15:16:07 cmkuo Exp $                                                                 //
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

#include <stdlib.h>
#include <iostream>

using namespace std;

#include "Cosmics/HafHistogram/interface/HTupleManager.h"
//#include "HAF/HafBase/interface/HIGGS.h"

ClassImp(HTupleManager)

//----------------
// Constructors --
//----------------
HTupleManager::HTupleManager( const char* file,const char* fmode ) :
fHepTuple(kFALSE)
{
    rtfilep = new TFile( file, (char*)fmode,"Created for you by HTupleManager" );
    olist   = new THashList();
    hlist   = new THashList();
    ntlist  = new THashList();
}

//--------------
// Destructor --
//--------------

HTupleManager::~HTupleManager() 
{
    olist->Delete(); delete olist;// Clear lists and remove objects
    hlist->Delete(); delete hlist;
    ntlist->Delete(); delete ntlist;
    rtfilep->Close();
    delete rtfilep;
    
}

//----------------------
//-- public Functions --
//----------------------

// Set current directory in memory, create if it does not exist:
Bool_t HTupleManager::SetDir( const char * path ) 
{    
    Bool_t result= kTRUE;
    checkFile();
    gDirectory->cd( "/" );  
    // Now try to change or create directory. ROOT will
    // create only direct subdirectories without "/" in the name. 
    // Go through the path, extract tokens and cd, creating
    // directories along the way as needed:
    TString ch( path );
    int i1= 0;
    int i2= 0;
    if( ch( 0, 1 ) == "/" ) {
	i1= 1;
	i2= 1;
    }
    while( i2 < ch.Length() ) {
	while( ch( i2, 1 ) != "/" && i2 < ch.Length() ) i2++;
	TString tok( ch( i1, i2-i1 ) );
	i1= ++i2;
	TKey* key= gDirectory->GetKey( &*tok );
	if( key != 0 ) {
	    if( !gDirectory->cd( &*tok ) ) {
		cout << "HTupleManager::setDir: can't cd to existing " 
		    << tok << " in " << gDirectory->GetPath() << endl;
		break;
	    }
	}
	else {
	    TDirectory* newdir= gDirectory->mkdir( &*tok );
	    if( newdir != 0 ) {
		newdir->cd();
	    }
	    else {
		result= kFALSE;
		break;
	    }
	}
    }
    return result;
    
}


// Return path of current directory (a la pwd):
const char* HTupleManager::GetDir() const 
{   
    return gDirectory->GetPath();  
}

Bool_t HTupleManager::SetFileName( const char* file ) 
{   
    // Make sure we are at the top of the current file, then save:
    checkFile();
    rtfilep->cd();
    Save();
    
    // Create the new file and transfer all objects from existing to new
    // file. Function movedir will call itself recursively for directories.
    TFile* filep= new TFile( file, "RECREATE", 
	"Created for you again by HTupleManager" );
    movedir( rtfilep, filep );
    
    // Now we can safely get rid of the old file:
    delete rtfilep;
    rtfilep= filep;
    
    // The End:
    return kTRUE;
    
}


// Recursively move contents of olddir to newdir:
void HTupleManager::movedir( TDirectory* olddir, TDirectory* newdir ) 
{   
    TList* list1= olddir->GetList();
    TIter iter( list1 );
    TObject* obj= 0;
    while( (obj= iter.Next()) ) {
	if( obj->InheritsFrom( "TH1" ) ) {
	    // TH1::SetDirectory does all the work for us:
	    ((TH1*)obj)->SetDirectory( newdir );
	}
	else if( strcmp( obj->ClassName(), "TDirectory" ) == 0 ) {
	    // Have to do it by hand recursively for TDirectory:
	    list1->Remove( obj );
	    TDirectory* dir= newdir->mkdir( obj->GetName() );
	    movedir( (TDirectory*)obj, dir );
	}
    }    
    return;
    
}

size_t HTupleManager::GetNumDir() const 
{ 
    checkFile( "w" );
    TIter iter( gDirectory->GetList() );
    TObject* obj;
    int dircount= 0;
    while( (obj= iter.Next()) ) {
	if( !strcmp( obj->ClassName(), "TDirectory" ) ) dircount++;
    }
    return dircount;
    
}

const char* HTupleManager::GetDirName( size_t index ) const 
{
    checkFile( "w" );
    TIter iter( gDirectory->GetList() );
    TObject* obj;
    size_t dircount= 0;
    TString dirName( "Not found" );
    while( (obj= iter.Next()) ) {
	if( !strcmp( obj->ClassName(), "TDirectory" ) ) {
	    dircount++;
	    if( index+1 == dircount ) {
		dirName= obj->GetName();
		break;
	    }
	}
    }
    return dirName;
    
}

Bool_t HTupleManager::GetDirIndex( const char* dirname, 
					    size_t & index ) const 
{
    Bool_t result= kTRUE;
    checkFile( "w" );
    TIter iter( gDirectory->GetList() );
    TObject* obj;
    size_t dircount= 0;
    while( (obj= iter.Next()) ) {
	if( !strcmp( obj->ClassName(), "TDirectory" ) ) {
	    dircount++;
	    if( !strcmp( obj->GetName(), &*dirname ) ) break;
	}
    }
    if( dircount == 0 ) {
	result= kFALSE;
    }
    else {
	index= dircount-1;
    }
    return result;
    
}

void HTupleManager::PrintOn(ostream & o) const 
{
    int i;

    o << endl;
    o << "HTupleManager Directory" << endl;
    o << "---------------------------------" << endl;
    o << "HHistograms: " << hlist->GetSize() << endl;
    for (i=0;i<hlist->GetSize();i++) {
	TH1F *h = (TH1F*)hlist->At(i);
	if (h==0)
	    o << "Reference to non existent object" << endl;
	else
	    o << h->ClassName() << '\t' << h->GetName() << "\t(" << h->GetTitle() << ")" << endl;
    }

    o << endl;
    o << "HTuples: " << ntlist->GetSize() << endl;
    for (i=0;i<ntlist->GetSize();i++) {
	TTree *h = (TTree*)ntlist->At(i);
	if (h==0)
	    o << "Reference to non existent object" << endl;
	else
	    o << h->ClassName() << '\t' << h->GetName() << "\t(" << h->GetTitle() << ")" << endl;
    }

    o << endl;
    o << "Other TObjects: " << olist->GetSize() << endl;
    for (i=0;i<olist->GetSize();i++) {
	TObject *h = (TObject*)olist->At(i);
	if (h==0)
	    o << "Reference to non existent object" << endl;
	else
	    o << h->ClassName() << '\t' << h->GetName() << "\t(" << h->GetTitle() << ")" << endl;
    }

    o << endl;

    return;
}

// Construct names (keys) for histos and ntuples. When hhid contains 0,
// a key using the prefix and a running counter is created:
const char* HTupleManager::makeName( const char* prefix, HHistID hhid ) 
{
    TString name( prefix );
    if( hhid.isIDstringSet() ) {
	name= hhid.getIDstring();
    }
    else if( hhid.isIDnumberSet() ) {
	if( hhid.getIDnumber() != 0 ) {
	    name+= TString( hhid.getIDnumber() );
	}
	else {
	    TIter iter( gDirectory->GetList(), kIterBackward );
	    TObject* obj= 0;
	    int number= 0;
	    if( name( 0, 1 ) == "h" ) {
		while( (obj= iter.Next()) ) if( obj->InheritsFrom( "TH1" ) ) break;
		if( obj != 0 ) {
		    TString hkey( obj->GetName() );
		    TString x(hkey( 3, hkey.Length()-3));
		    number = atol(x);
		}
		char tmp[128];
		sprintf(tmp,"%d\n",number+1);
		name+= TString(tmp);
	    }
	    else if( name( 0, 3 ) == "ntp" ) {
		while( (obj= iter.Next()) ) {
		    if( !strcmp( obj->ClassName(), "TTree" ) && 
			!strncmp( obj->GetName(), "ntp", 3 ) ) break;
		}
		if( obj != 0 ) {
		    TString lastname( obj->GetName() );
		    TString x(lastname( 3, lastname.Length()-3 ));
		    number = atol(x);
		}
		char tmp[128];
		sprintf(tmp,"%d\n",number+1);
		name+= TString(tmp);
	    }
	    else {
		cout << "HTupleManager::makeName: prefix " << prefix 
		    << " not recognised (h1d, h2d, hpr, ntp)" << endl;
	    }
	}
    }
    return name;
}

// Check if current file belongs to this manager, if not change
// directory to ROOT of correct file:
void HTupleManager::checkFile( const char* opt ) const 
{
    if( gDirectory->GetFile() != rtfilep ) {
	if( !strcmp( opt, "" ) ) {
	    rtfilep->cd();
	}
	else {
	    cout << "HTupleManager::checkFile: current file "
		<< gDirectory->GetFile()->GetName() 
		<< " does not belong to this manager ("
		<< rtfilep->GetName() << ")" << endl;
	}
    }
    return; 
}


size_t HTupleManager::GetNumHist() const 
{
    checkFile( "w" );
    TIter iter( gDirectory->GetList() );
    TObject* obj;
    int histcount= 0;
    while( (obj= iter.Next()) ) {
	if( obj->InheritsFrom( "TH1" ) ) histcount++;
    }
    return histcount;
}

size_t HTupleManager::GetNumHist( const char *path ) const 
{
    TDirectory* olddir= gDirectory;
    checkFile();
    gDirectory->cd( path );
    int histcount= GetNumHist();
    olddir->cd();
    return histcount;
    
}

TObject* HTupleManager::Add(TObject* item) 
{
    TDirectory* olddir= gDirectory;
    checkFile();
    olist->Add(item);
    olddir->cd();
    return item;
}

TObject* HTupleManager::Get(const char *name) 
{
    TObject *item;
    if ((item=olist->FindObject(name)) != 0) return item;
    if ((item=hlist->FindObject(name)) != 0) return item;
    if ((item=ntlist->FindObject(name)) != 0) return item;
    return 0; // Not found
}

void HTupleManager::Remove(TObject* item) 
{   
    TDirectory* olddir= gDirectory;
    checkFile();
    olist->Remove(item);
    olddir->cd();
}

void HTupleManager::Remove(const char *name) 
{   
    TDirectory* olddir= gDirectory;
    checkFile();
    TObject* item = Get(name);
    if ((item=olist->FindObject(name)) != 0) olist->Remove(item);
    if ((item=hlist->FindObject(name)) != 0) hlist->Remove(item);
    if ((item=ntlist->FindObject(name)) != 0) ntlist->Remove(item);
    olddir->cd();
}

// 1-D histos:
HHistogram* HTupleManager::Histogram( const char* htitle, int nbin, 
						   float lowX, float highX ) 
{
    return Histogram( htitle, nbin, lowX, highX, HHistID( 0 ) );
}

HHistogram* HTupleManager::Histogram( const char* htitle, int nbin,
						   float lowX, float highX, 
						   HHistID hhid ) 
{
    TDirectory* olddir= gDirectory;
    checkFile();
    HHistogram* histp= getHistByName( htitle );
    if( histp == 0 ) {
	TString hname;
	if (fHepTuple)
	    hname = makeName( "h1d", hhid );
	else
	    hname = TString(htitle);
	histp=new HHistogram( &*hname, htitle, nbin, lowX, highX );
	hlist->Add(histp);
    }
    olddir->cd();
    return histp;
}


// 2-D histos:
HHistogram* 
HTupleManager::Histogram( const char* htitle, 
				   int nbinX, float lowX, float highX,
				   int nbinY, float lowY, float highY ) 
{
    return Histogram( htitle, nbinX, lowX, highX, nbinY, lowY, highY,
	HHistID( 0 ) );
}

HHistogram* HTupleManager::Histogram( const char* htitle, 
						   int nbinX, float lowX, float highX,
						   int nbinY, float lowY, float highY,
						   HHistID hhid ) 
{
    TDirectory* olddir= gDirectory;
    checkFile();
    HHistogram* histp= getHistByName( htitle );
    if( histp == 0 ) {
	TString hname;
	if (fHepTuple)
	    hname = makeName( "h2d", hhid );
	else
	    hname = TString(htitle);
	histp=new HHistogram( &*hname, htitle,nbinX, lowX, highX,nbinY, lowY, highY );
	hlist->Add(histp);
    }
    olddir->cd();
    return histp;    
}


// Profile histos:
HHistogram* HTupleManager::Profile( const char* htitle, int nbin, 
						 float lowX, float highX,
						 float lowY, float highY ) 
{
    return Profile( htitle, nbin, lowX, highX, lowY, highY, HHistID( 0 ) );  
}

HHistogram* 
HTupleManager::Profile( const char* htitle, int nbin, 
				 float lowX, float highX,
				 float lowY, float highY, HHistID hhid ) 
{
    TDirectory* olddir= gDirectory;
    checkFile();
    HHistogram* histp= getHistByName( htitle );
    if( histp == 0 ) {
	TString hname;
	if (fHepTuple)
	    hname = makeName( "hpr", hhid );
	else
	    hname = TString(htitle);
	histp=new HHistogram( &*hname, htitle,nbin, lowX, highX,lowY, highY );
	hlist->Add(histp);
    }
    olddir->cd();
    return histp;
    
}


// Ntuples:
HTuple* HTupleManager::Ntuple( const char* nttitle ) 
{
    // Map onto two-argument function below:
    return Ntuple( nttitle, HHistID( 0 ) );
}

HTuple* HTupleManager::Ntuple( const char* nttitle, 
					    HHistID hhid ) 
{
    // Try to find ntuple by title in list of managed ntuples:
    TDirectory* olddir= gDirectory;
    checkFile();
    HTuple* ntp= 0;
    for( int i= 0; i < ntlist->GetSize(); ++i ) {
	if( ((HTuple*)ntlist->At(i))->Title() == nttitle ) {
	    ntp= (HTuple*)ntlist->At(i);
	    break;
	}
    }
    // If no ntuple is found create one in the current directory:
    if( ntp == 0 ) {
	TString ntname;
	if (fHepTuple)
	    ntname = makeName( "ntp", hhid );
	else
	    ntname = TString(nttitle);
	ntp=new HTuple( &*ntname, nttitle );
	ntlist->Add( ntp );
    }
    olddir->cd();
    return ntp;
    
}

void HTupleManager::DumpAllData() 
{
    for( int i= 0; i < ntlist->GetSize(); ++i ) ((HTuple*)ntlist->At(i))->DumpData();
    return;
}

void HTupleManager::ClearAll() 
{
    olist->Delete(); // This removes the objects and clears the lists
    hlist->Delete();
    ntlist->Delete();
    return;
}

int HTupleManager::Save() 
{
    checkFile();
    rtfilep->Write( rtfilep->GetName(), TObject::kOverwrite );
    for (int i=0;i<olist->GetSize();i++) {
	olist->At(i)->Write();
    }

    return 0;
}

// Helper function for automatic histo creation or returning.
// First check if a hist with this title already exists in 
// the current directory, if so match it to the corresponding
// HHistogram and return its pointer:
HHistogram* HTupleManager::getHistByName( const char* title ) 
{
    HHistogram* histp= 0;
    TIter iter( gDirectory->GetList() );
    TObject* obj;
    while( (obj= iter.Next()) ) {
	if( obj->InheritsFrom( "TH1" ) && 
	    !strcmp( obj->GetName(), title ) ) break;
    }
    for( int i= 0; i < olist->GetSize(); ++i ) {
	if( ((HHistogram*)olist->At(i))->PtrIsEqual( obj ) ) {
	    histp= (HHistogram*)olist->At(i);
	    break;
	}
    }
    return histp;
}

HHistogram* HTupleManager::getHistByTitle( const char* title ) 
{
    HHistogram* histp= 0;
    TIter iter( gDirectory->GetList() );
    TObject* obj;
    while( (obj= iter.Next()) ) {
	if( obj->InheritsFrom( "TH1" ) && 
	    !strcmp( obj->GetTitle(), title ) ) break;
    }
    for( int i= 0; i < olist->GetSize(); ++i ) {
	if( ((HHistogram*)olist->At(i))->PtrIsEqual( obj ) ) {
	    histp= (HHistogram*)olist->At(i);
	    break;
	}
    }
    return histp;
}

// --------------------
// non-member functions
// --------------------

ostream&  operator << (ostream& o, const HTupleManager& a) {a.PrintOn(o); return o;}
