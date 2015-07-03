#ifndef ECAL_LASER_DUMPER
#define ECAL_LASER_DUMPER

#include <string>
#include <vector>
#include <ctime>

class DetId;
class EcalLaserAPDPNRatios;

class EcalLaserDumper
{
        public:
                typedef struct Coord {
                        int ix_;
                        int iy_;
                        int iz_;
                } Coord;

                EcalLaserDumper(std::string dir);
                ~EcalLaserDumper();

                void coord(DetId id, Coord * c);
                void dump(time_t t, const EcalLaserAPDPNRatios * apdpn, const EcalLaserAPDPNRatios * apdpn2 = NULL);
                void dumpForDB(time_t iov_begin, time_t iov_end, const EcalLaserAPDPNRatios * apdpn, const char * file_name = "dumped_for_DB.dat");

        private:
                std::string dir_;
                std::vector<DetId> ecalDetIds_;
                FILE * f_;
};

#endif
