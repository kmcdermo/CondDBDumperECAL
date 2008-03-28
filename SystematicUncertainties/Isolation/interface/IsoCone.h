#ifndef ISO_CONE_hh
#define ISO_CONE_hh

#include <DataFormats/Math/interface/Vector3D.h>
#include <DataFormats/Common/interface/View.h>

#include <DataFormats/TrackReco/interface/Track.h>

#include <Math/VectorUtil.h>
#include <vector>

namespace Iso {

        typedef math::XYZVector Vector;
        typedef math::XYZPoint Point;

        struct Results {
                public:
                        // number of tracks in the cone
                        size_t nTracks;
                        // sum of track pt's in the cone
                        float ptSum;
                        // deltaR with ALL the other tracks (even outside the cone)
                        std::vector<float> deltaRTracks;
                        // deltaZ with ALL the other tracks (even outside the cone)
                        std::vector<float> deltaZ;

                        // print values
                        void print() const {
                                std::cout << "ptSum   = " << ptSum << std::endl;
                                std::cout << "nTracks = " << nTracks << std::endl;
                                std::cout << "deltaRTracks: " << std::endl;
                                for (size_t i = 0; i < deltaRTracks.size(); ++i) {
                                        std::cout << "   " << deltaRTracks[i] << std::endl;
                                }
                                for (size_t i = 0; i < deltaZ.size(); ++i) {
                                        std::cout << "   " << deltaZ[i] << std::endl;
                                }
                        }
        };

        template < typename T >
        class Cone {
                public:

                        // default constructor
                        Cone() : iradius_(0.015), oradius_(0.25), dz_(0.1), dRt_(0.01), ptThreshold_(1.5), pThreshold_(0.8), direction_(0,1,0), position_(0,0,0) {};
                        // default destructor
                        ~Cone() {};

                        // set direction of the cone
                        void setDirection( Vector direction ) { direction_ = direction; };

                        // set position of the cone vertex
                        void setPosition( Point position ) { position_ = position; };

                        // get direction of the cone
                        Vector const & getDirection() const { return direction_; };

                        // get direction of the cone vertex
                        Point const & getPosition() const { return position_; };

                        // get the isolation as the sum of the tracks pt
                        Results const getIsolation( const edm::Handle< edm::View<T> > &tracks ) const {
                                Iso::Results ir;
                                ir.nTracks = 0;
                                ir.ptSum = 0.;
                                reco::TrackCollection temp;
                                for (size_t i = 0; i < tracks->size(); ++i) {
                                        float pt = (*tracks)[i].pt();
                                        float p  = (*tracks)[i].p();
                                        float dr = ROOT::Math::VectorUtil::DeltaR( (*tracks)[i].momentum(), direction_ );
                                        float dz = fabs( position_.z() - (*tracks)[i].vz() );
                                        //float vx = position_.x() - (*tracks)[i].vx();
                                        //float vy = position_.y() - (*tracks)[i].vy();

                                        //float dRt = sqrt( vx*vx + vy*vy );
                                        float dRt = (*tracks)[i].innerPosition().R();
                                        
                                        ir.deltaRTracks.push_back( dr );
                                        ir.deltaZ.push_back( dz );

                                        //fprintf(stdout,"%f\n",dr);
                                        //fprintf(stderr,"---------\n");
                                        //std::cerr << "tk eta = " << (*tracks)[i].eta() << " phi = " << (*tracks)[i].phi() << " p = |" << (*tracks)[i].momentum() << "| = " << (*tracks)[i].p() << std::endl;
                                        //fprintf(stderr,"cuts  : %f %f %f %f %f %f\n",pThreshold_,ptThreshold_,iradius_,oradius_,dz_,dRt_);
                                        //fprintf(stderr,"values: %f %f %f %f %f %f\n",p,pt,dr,dr,dz,dRt);

                                        if ( p   < pThreshold_) continue;
                                        if ( pt  < ptThreshold_) continue;
                                        if ( dr  < iradius_ || dr > oradius_ ) continue;
                                        if ( dz  > dz_ ) continue;
                                        if ( dRt > dRt_ ) continue;
                                        //fprintf(stderr,".....accepted\n");
                                        temp.push_back( (*tracks)[i] );
                                        ir.ptSum += pt;
                                        ir.nTracks += 1;
                                }
                                return ir;
                        };

                        // reset to default values
                        void reset() { setValues( 0.015, 0.25, 0.1, 1.5, Vector(0,1,0), Vector(0,0,0) ); };

                        // set all values at a time
                        void setValues( float iradius, float oradius, float dz, float dRt, float ptThreshold, float pThreshold, Vector direction, Point position ) {
                                iradius_ = iradius;
                                oradius_ = oradius;
                                dz_ = dz;
                                dRt_ = dRt;
                                ptThreshold_ = ptThreshold;
                                pThreshold_ = pThreshold;
                                direction_ = direction;
                                position_ = position;
                        };

                private:
                        // exclude tracks inside this radius
                        float iradius_;
                        // accept tracks up to this radius
                        float oradius_;
                        // accept tracks within dz in z direction
                        float dz_;
                        // accept tracks with the vertex within a radius in the transverse plain
                        float dRt_;
                        // exclude tracks with pT/p less than ptThreshold_
                        float ptThreshold_, pThreshold_;
                        // direction of the cone
                        Vector direction_;
                        // position of the cone
                        Point position_;
        };
}

#endif
