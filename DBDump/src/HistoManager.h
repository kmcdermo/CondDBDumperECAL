#ifndef HistoManager_hh
#define HistoManager_hh

#include <TROOT.h>
#include <TFile.h>

#include <map>
#include <string>

#include <assert.h>

class HistoManager {
        public:
                typedef std::map< std::string, TObject *> Map;

                HistoManager() {};
                ~HistoManager() {};

                template < class T > void addTemplate( const char * type, T * templ )
                {
                        assert( m_templates.find( type ) == m_templates.end() );
                        m_templates[ type ] = templ;
                }

                template < class T > T * h( const char * type, const char * name )
                {
                        assert( m_templates.find( type ) != m_templates.end() );
                        std::string id(type);
                        id += std::string("_") + name;
                        if ( m_histos.find( id ) != m_histos.end() ) {
                                return (T*)m_histos[id];
                        } else {
                                m_histos[id] = (T*)m_templates[type]->Clone(id.c_str());
                                return (T*)(m_histos[id]);
                        }
                }

                void save( const char * fileName )
                {
                        TFile f( fileName, "RECREATE" );
                        f.cd();
                        Map::const_iterator it;
                        for ( it = m_histos.begin(); it != m_histos.end(); ++it ) {
                                it->second->Write();
                        }
                        f.Close();
                }

        private:
                Map m_templates;
                Map m_histos;
};


#endif
