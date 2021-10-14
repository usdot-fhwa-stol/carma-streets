#include <lanelet2_core/LaneletMap.h> 
#include <lanelet2_projection/UTM.h>
#include <lanelet2_io/Io.h>
#include <lanelet2_core/LaneletMap.h>

#include <lanelet2_extension/projection/mgrs_projector.h>
#include <lanelet2_extension/projection/local_frame_projector.h>
#include <lanelet2_extension/io/autoware_osm_parser.h>

#include <spdlog/spdlog.h>

namespace intersection_model
{
    class intersection_model
    {
        public:
        /**
         * Constructor for intersection model.
         * 
         * @param filename path to lanelet2 osm map
         */
        intersection_model( std::string filename );
        
        /**
         * Getter for lanelet2 map.
         * 
         * @return lanelet map object
         */ 
        lanelet::LaneletMapPtr get_lanelet2_map();

        /**
         * Retreive lanelet information given lat,long location
         * 
         * @param lat latitude
         */ 
        lanelet::Lanelet get_lanelet(double lat, double lon );


        /**
         * Retreive 
         * 
         */ 


        private:
        lanelet::LaneletMapPtr map;
        lanelet::projection::LocalFrameProjector *projector;

        
        /**
         * Read lanelet2 map.
         * 
         * @param filename path to lanelet2 osm map
         */
        void read_lanelet2_map( std::string filename );


    };
}