#ifndef HistoManager_hh
#define HistoManager_hh

//
// Author: Federico Ferri (Saclay) 2009
//

// 26/08/09 -- GHM: Directory management
// 29/09/09 -- GHM: pass strings instead of const char*

// HistoManager histos;
// add a template histogram called ZZmass
// histos.addTemplate<TH1F>( "ZZmass", new TH1F( "ZZmass", "ZZmass", 100, 50., 150.) );
// [...]
// histos.h<TH1F>( "ZZmass", "mass_after_eleID" )->Fill( 4lMass );>
// [..]
// histos.h<TH1F>( "ZZmass", "mass_after_isolation" )->Fill( 4lMass );
// [...]
// std::string outPlotFile_ = "toto.root";
// histos.save( outPlotFile_.c_str() );

#include <TROOT.h>
#include <TH1.h>
#include <TFile.h>

#include <map>
#include <string>
#include <iostream>

#include <cassert>

class HistoManager 
{
public:

  typedef std::map< std::string, TH1* > Map;  //ghm: TObject* -> TH1*

  HistoManager() : _f(0) {}

  void setFile( TFile* f ) { _f=f; }

  ~HistoManager()  {}

  template < class T > void addTemplate( std::string type, T* templ )
  {
    assert( m_templates.find( type ) == m_templates.end() );
    m_templates[ type ] = templ;
  }

  template < class T > T* h( std::string type, std::string name, 
                             std::string dir="", std::string prefix="" )
  {
    if( m_templates.find( type ) == m_templates.end() ) 
      { std::cout << type << std::endl; assert(0); }
    std::string hname;
    if( prefix!="" ) hname += ( prefix + "__" );
    hname += ( type + "__" );
    hname += name;
    std::string id(hname);
    if( dir!="" ) id += ( std::string("__") + dir );
    if ( m_histos.find( id ) != m_histos.end() ) 
      {
        return (T*)m_histos[id];
      } 
    else 
      {
        m_histos[id] = (T*)m_templates[type]->Clone(hname.c_str());

        TDirectory* tdir_(_f);
        if( _f!=0 && dir!="" )
          {
            tdir_ = _f->GetDirectory( dir.c_str(), false );
            if( tdir_==0 ) 
              {
                tdir_ = _f->mkdir( dir.c_str() );
                std::cout << "create directory with path " << tdir_->GetPath() << std::endl;
              }
          }
        m_histos[id] -> DirectoryAutoAdd( tdir_ ); 

        return (T*)(m_histos[id]);
      }
  }

  void save()
  {
    Map::const_iterator it;
    for ( it = m_histos.begin(); it != m_histos.end(); ++it ) 
      {
        TH1* h_ = it->second;
        TDirectory* tdir_ = h_->GetDirectory();
        if ( tdir_ ) tdir_->cd();
        h_->Write();
      }
  }

private:

  TFile* _f;
  Map m_templates;
  Map m_histos;

};


#endif
