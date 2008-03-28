#include <SystematicUncertainties/Isolation/interface/ConeGenerator.h>
#include <CLHEP/Random/RandFlat.h>
#include <CLHEP/Random/RandGaussQ.h>
#include <CLHEP/Random/RanecuEngine.h>

namespace {
        namespace {
                ConeGenerator c;
                std::pair< math::RhoEtaPhiVector, math::XYZPoint > p;
                std::vector< std::pair< math::RhoEtaPhiVector, math::XYZPoint > > v;
                CLHEP::RandGaussQ *rndGaussQ;
                CLHEP::RandFlat *rndFlat;
                CLHEP::RanecuEngine *engine;
        }
}
