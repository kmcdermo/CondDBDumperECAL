#include <SystematicUncertainties/Isolation/interface/ConeGenerator.h>
#include <FWCore/ServiceRegistry/interface/Service.h>
#include <FWCore/Utilities/interface/RandomNumberGenerator.h>
#include <FWCore/Utilities/interface/Exception.h>
#include <CLHEP/Random/RandFlat.h>
#include <CLHEP/Random/RandGaussQ.h>
#include <CLHEP/Random/RanecuEngine.h>

#include <TBranch.h>
#include <TFile.h>
#include <TTree.h>
#include <string>

ConeGenerator::ConeGenerator() : 
        sigmaX_(1.), sigmaY_(1.), sigmaZ_(1.),
        meanX_(0.), meanY_(0.), meanZ_(0.),
        rndGaussQ_(0),
        minEta_(0.), maxEta_(2.5),
        minPhi_(0.), maxPhi_(0.),
        rndFlatEta_(0), rndFlatPhi_(0),
        kineDirections_(0), kineVertices_(0),
        kineZDirections_(0), kineZVertices_(0),
        kineZZDirections_(0), kineZZVertices_(0),
        rndFlatKine_(0),
        rndFlatKineZ_(0),
        rndFlatKineZZ_(0),
        engine_(0)
{
}

ConeGenerator::~ConeGenerator()
{
        // to avoid FWLite crashes :-S
        //delete rndGaussQ_;
        //delete rndFlatEta_;
        //delete rndFlatPhi_;
        //delete engine_;
}

void ConeGenerator::init()
{
        //edm::Service<edm::RandomNumberGenerator> rng;
        //if ( ! rng.isAvailable()) {
        //        throw cms::Exception("Configuration")
        //        << "ConeGenerator::ConeGenerator requires the RandomNumberGeneratorService\n"
        //        "which is not present in the configuration file.  You must add the service\n"
        //        "in the configuration file or remove the modules that require it.";
        //}
        //CLHEP::HepRandomEngine & engine = rng->getEngine();
        //rndGaussQ_ = new CLHEP::RandGaussQ( engine );
        //rndFlatEta_   = new CLHEP::RandFlat( engine, minEta_, maxEta_);
        //rndFlatPhi_   = new CLHEP::RandFlat( engine, minPhi_, maxPhi_);
        //rndFlatKine_   = new CLHEP::RandFlat( engine, 0, kineDirections_.size());
        //
        engine_ = new CLHEP::RanecuEngine();
        rndGaussQ_ = new CLHEP::RandGaussQ( engine_ );
        rndFlatEta_   = new CLHEP::RandFlat( engine_, minEta_, maxEta_);
        rndFlatPhi_   = new CLHEP::RandFlat( engine_, minPhi_, maxPhi_);
        rndFlatKine_   = new CLHEP::RandFlat( engine_, 0, kineDirections_.size());
        rndFlatKineZ_   = new CLHEP::RandFlat( engine_, 0, kineZDirections_.size());
        rndFlatKineZZ_   = new CLHEP::RandFlat( engine_, 0, kineZZDirections_.size());
}

std::pair<ConeGenerator::Vector, ConeGenerator::Point> ConeGenerator::randomCone()
{
        double x = meanX_ + sigmaX_ * rndGaussQ_->fire();
        double y = meanY_ + sigmaY_ * rndGaussQ_->fire();
        double z = meanZ_ + sigmaZ_ * rndGaussQ_->fire();
        ConeGenerator::Point p(x,y,z); 
        double eta = rndFlatEta_->fire();
        double phi = rndFlatPhi_->fire();
        ConeGenerator::Vector v(1.,eta,phi);
        return std::pair<Vector,Point>(v, p);
}

std::vector< std::pair<ConeGenerator::Vector, ConeGenerator::Point> > ConeGenerator::multipleRandomCones( int nCones )
{
        std::vector< std::pair<ConeGenerator::Vector, ConeGenerator::Point> > v;
        for (int i = 0; i < nCones; ++i) {
                v.push_back( randomCone() );
        }
        return v;
}

std::vector< std::pair<ConeGenerator::Vector, ConeGenerator::Point> > ConeGenerator::fourKinematicCones()
{
        std::vector< std::pair<ConeGenerator::Vector, ConeGenerator::Point> > v;
        int i = (int)floor( rndFlatKine_->fire() );
        for (size_t j = 0; j < kineDirections_[i].size(); ++j) {
                v.push_back( std::pair<ConeGenerator::Vector,ConeGenerator::Point>( kineDirections_[i][j], kineVertices_[i][j] ));
        }
        return v;
}

std::vector< std::pair<ConeGenerator::Vector, ConeGenerator::Point> > ConeGenerator::ZKinematicCones()
{
        std::vector< std::pair<ConeGenerator::Vector, ConeGenerator::Point> > v;
        int i = (int)floor( rndFlatKineZ_->fire() );
        for (size_t j = 0; j < kineZDirections_[i].size(); ++j) {
                v.push_back( std::pair<ConeGenerator::Vector,ConeGenerator::Point>( kineZDirections_[i][j], kineZVertices_[i][j] ));
        }
        return v;
}

std::vector< std::pair<ConeGenerator::Vector, ConeGenerator::Point> > ConeGenerator::ZZKinematicCones()
{
        std::vector< std::pair<ConeGenerator::Vector, ConeGenerator::Point> > v;
        int i = (int)floor( rndFlatKineZZ_->fire() );
        for (size_t j = 0; j < kineZZDirections_[i].size(); ++j) {
                v.push_back( std::pair<ConeGenerator::Vector,ConeGenerator::Point>( kineZZDirections_[i][j], kineZZVertices_[i][j] ));
        }
        return v;
}

std::vector< std::pair<ConeGenerator::Vector, ConeGenerator::Point> > ConeGenerator::kinematicCones()
{
        std::vector< std::pair<ConeGenerator::Vector, ConeGenerator::Point> > v;
        int i = (int)floor( rndFlatKine_->fire() );
        for (size_t j = 0; j < kineDirections_[i].size(); ++j) {
                v.push_back( std::pair<ConeGenerator::Vector,ConeGenerator::Point>( kineDirections_[i][j], kineVertices_[i][j] ));
        }
        return v;
}

void ConeGenerator::loadKineCones( const std::string inputFile )
{
        std::cout << "reading particle vectors file..." << std::endl;
        TFile *iFile = new TFile( inputFile.c_str() );
        TTree *tree = (TTree*)iFile->Get("Events");
        tree->SetAlias("particlesMomenta" ,"doubleROOTMathCylindricalEta3DROOTMathDefaultCoordinateSystemTagROOTMathDisplacementVector3Ds_dumper_particleMomenta_kineDumper.obj");
        tree->SetAlias("particlesVertices","doubleROOTMathCartesian3DROOTMathDefaultCoordinateSystemTagROOTMathPositionVector3Ds_dumper_particleVertices_kineDumper.obj");
        std::vector< std::vector< ConeGenerator::Vector > > kineDirections;
        std::vector< std::vector< ConeGenerator::Point  > > kineVertices;
        std::vector< ConeGenerator::Vector > directions;
        std::vector< ConeGenerator::Point > vertices;
        TBranch *b_directions = tree->GetBranch(tree->GetAlias("particlesMomenta"));
        b_directions->SetAddress(&directions);
        TBranch *b_vertices = tree->GetBranch(tree->GetAlias("particlesVertices"));
        b_vertices->SetAddress(&vertices);
        for (int iev = 0; iev < tree->GetEntries(); ++iev) {
                b_directions->GetEntry(iev);
                b_vertices->GetEntry(iev);
                if (directions.size() < 4) continue;
                kineDirections.push_back( directions );
                kineVertices.push_back( vertices );
        }
        std::cout << "size : " << kineDirections.size() << std::endl;
        iFile->Close();
        std::cout << "done." << std::endl;
        setKineCones( kineDirections, kineVertices );
        //coneGen_.init();
}


void ConeGenerator::loadZKineCones( const std::string inputFile )
{
        std::cout << "reading Z particle vectors file..." << std::endl;
        TFile *iFile = new TFile( inputFile.c_str() );
        TTree *tree = (TTree*)iFile->Get("Events");
        tree->SetAlias("particlesMomenta" ,"doubleROOTMathCylindricalEta3DROOTMathDefaultCoordinateSystemTagROOTMathDisplacementVector3Ds_dumper_particleMomenta_kineDumper.obj");
        tree->SetAlias("particlesVertices","doubleROOTMathCartesian3DROOTMathDefaultCoordinateSystemTagROOTMathPositionVector3Ds_dumper_particleVertices_kineDumper.obj");
        std::vector< std::vector< ConeGenerator::Vector > > kineDirections;
        std::vector< std::vector< ConeGenerator::Point  > > kineVertices;
        std::vector< ConeGenerator::Vector > directions;
        std::vector< ConeGenerator::Point > vertices;
        TBranch *b_directions = tree->GetBranch(tree->GetAlias("particlesMomenta"));
        b_directions->SetAddress(&directions);
        TBranch *b_vertices = tree->GetBranch(tree->GetAlias("particlesVertices"));
        b_vertices->SetAddress(&vertices);
        for (int iev = 0; iev < tree->GetEntries(); ++iev) {
                b_directions->GetEntry(iev);
                b_vertices->GetEntry(iev);
                if (directions.size() < 2) continue;
                kineDirections.push_back( directions );
                kineVertices.push_back( vertices );
        }
        std::cout << "size : " << kineDirections.size() << std::endl;
        iFile->Close();
        std::cout << "done." << std::endl;
        setZKineCones( kineDirections, kineVertices );
}


void ConeGenerator::loadZZKineCones( const std::string inputFile )
{
        std::cout << "reading ZZ particle vectors file..." << std::endl;
        TFile *iFile = new TFile( inputFile.c_str() );
        TTree *tree = (TTree*)iFile->Get("Events");
        tree->SetAlias("particlesMomenta" ,"doubleROOTMathCylindricalEta3DROOTMathDefaultCoordinateSystemTagROOTMathDisplacementVector3Ds_dumper_particleMomenta_kineDumper.obj");
        tree->SetAlias("particlesVertices","doubleROOTMathCartesian3DROOTMathDefaultCoordinateSystemTagROOTMathPositionVector3Ds_dumper_particleVertices_kineDumper.obj");
        std::vector< std::vector< ConeGenerator::Vector > > kineDirections;
        std::vector< std::vector< ConeGenerator::Point  > > kineVertices;
        std::vector< ConeGenerator::Vector > directions;
        std::vector< ConeGenerator::Point > vertices;
        TBranch *b_directions = tree->GetBranch(tree->GetAlias("particlesMomenta"));
        b_directions->SetAddress(&directions);
        TBranch *b_vertices = tree->GetBranch(tree->GetAlias("particlesVertices"));
        b_vertices->SetAddress(&vertices);
        for (int iev = 0; iev < tree->GetEntries(); ++iev) {
                b_directions->GetEntry(iev);
                b_vertices->GetEntry(iev);
                if (directions.size() < 4) continue;
                kineDirections.push_back( directions );
                kineVertices.push_back( vertices );
        }
        std::cout << "size : " << kineDirections.size() << std::endl;
        iFile->Close();
        std::cout << "done." << std::endl;
        setZZKineCones( kineDirections, kineVertices );
}
