#ifndef CONE_GENERATOR_hh
#define CONE_GENERATOR_hh

#include <DataFormats/Math/interface/Vector3D.h>
#include <DataFormats/Math/interface/Point3D.h>
#include <vector>
#include <cmath>

namespace CLHEP {
        class RandGaussQ;
        class RandFlat;
        class RanecuEngine;
        class HepRandomEngine;
}

namespace edm {
        class ParameterSet;
}

class ConeGenerator {
        public:
                typedef math::RhoEtaPhiVector Vector;
                typedef math::XYZPoint Point;

                // default constructor
                ConeGenerator();
                // default destructor
                ~ConeGenerator();
                // single random cone generation flat in eta [-2.5,2.5] and phi [0,2pi)
                std::pair<Vector, Point> randomCone();
                // single cone generation according to histogram-based pdf
                //std::pair<Vector,Point> randomConePdf( TH1F const &h);
                // multiple random cone generation
                std::vector< std::pair<Vector, Point> > multipleRandomCones( int nCones=2);
                // multiple random cone generation according to histogram-based pdf
                //std::vector< std::pair<Vector,Point> > multipleRandomConesPdf();
                // 4 cone generation according to Higgs or ZZ event kinematics
                std::vector< std::pair<Vector, Point> > fourKinematicCones();
                // cone generation according to electron kinematics --
                // ... it throws as many cone as MC electrons it finds
                std::vector< std::pair<Vector, Point> > kinematicCones();
                //
                std::vector< std::pair<Vector, Point> > ZKinematicCones();
                std::vector< std::pair<Vector, Point> > ZZKinematicCones();
                void init();

                // vertex generation: set sigma [cm]
                void setVertexSigma( double sx=1., double sy=1., double sz=1. ) { sigmaX_ = sx; sigmaY_ = sy; sigmaZ_ = sz; };
                // vertex generation: set mean [cm]
                void setVertexMean( double mx=0., double my=0., double mz=0. ) { meanX_ = mx; meanY_ = my; meanZ_ = mz; };
                // direction generation: set eta range
                void setEtaRange( double minEta=0., double maxEta=2.5 ) { minEta_ = minEta; maxEta_ = maxEta; };
                // direction generation: set phi range
                void setPhiRange( double minPhi=0., double maxPhi=M_PI ) { minPhi_ = minPhi; maxPhi_ = maxPhi; };
                // set pre-defined directions and vertices sample to randomly choose them
                void setKineCones( const std::vector< std::vector< Vector > > kineDirections, const std::vector< std::vector< Point > > kineVertices) { kineDirections_ = kineDirections; kineVertices_ = kineVertices; };
                void setZKineCones( const std::vector< std::vector< Vector > > kineDirections, const std::vector< std::vector< Point > > kineVertices) { kineZDirections_ = kineDirections; kineZVertices_ = kineVertices; };
                void setZZKineCones( const std::vector< std::vector< Vector > > kineDirections, const std::vector< std::vector< Point > > kineVertices) { kineZZDirections_ = kineDirections; kineZZVertices_ = kineVertices; };
                // load pre-defined directions and vertices from a file
                void loadKineCones( const std::string inputFile );
                void loadZKineCones( const std::string inputFile );
                void loadZZKineCones( const std::string inputFile );
        private:
                // to handle the vertex generation
                double sigmaX_, sigmaY_, sigmaZ_;
                double meanX_,  meanY_,  meanZ_;
                CLHEP::RandGaussQ * rndGaussQ_;
                // to handle the direction generation
                double minEta_, maxEta_;
                double minPhi_, maxPhi_;
                CLHEP::RandFlat * rndFlatEta_;
                CLHEP::RandFlat * rndFlatPhi_;
                // vector of pre-computed directions and vertices according to the kinematics
                std::vector< std::vector< Vector > > kineDirections_;
                std::vector< std::vector< Point  > > kineVertices_;
                // vector of pre-computed directions and vertices according to the kinematics of Z2e event
                std::vector< std::vector< Vector > > kineZDirections_;
                std::vector< std::vector< Point  > > kineZVertices_;
                // vector of pre-computed directions and vertices according to the kinematics of ZZ4e event
                std::vector< std::vector< Vector > > kineZZDirections_;
                std::vector< std::vector< Point  > > kineZZVertices_;
                CLHEP::RandFlat * rndFlatKine_;
                CLHEP::RandFlat * rndFlatKineZ_;
                CLHEP::RandFlat * rndFlatKineZZ_;
                //CLHEP::HepRandomEngine *engine_;
                CLHEP::RanecuEngine *engine_;
};

#endif
