#include "intersection_model.h"

namespace intersection_model
{

    intersection_model::intersection_model( std::string filename ) {
        read_lanelet2_map(filename);
        // TODO :
        // Read manifest.json for coordinates of bounding box
        
    }

    void intersection_model::read_lanelet2_map( std::string filename ) {
        int projector_type = 1;
        std::string target_frame;
        lanelet::ErrorMessages errors;
        // Parse geo reference info from the lanelet map (.osm)
        lanelet::io_handlers::AutowareOsmParser::parseMapParams(filename, &projector_type, &target_frame);
        projector = new lanelet::projection::LocalFrameProjector(target_frame.c_str());
        map = lanelet::load(filename, *projector, &errors);
    }

    lanelet::LaneletMapPtr intersection_model::get_lanelet2_map() {
        return map;
    }

    lanelet::Lanelet intersection_model::get_lanelet( double lat, double lon ) {
        // sudo code:
        //  -> Create GPSPoint
        //  -> Use projector->forward() to translate to BasicPoint3D
        //  -> search map for nearest lanelet for BasicPoint3D
        //  -> return lanelet
        spdlog::debug("Getting lanelet for ({0},{1}", lat, lon);
        lanelet::GPSPoint gps;
        gps.ele = 0;
        gps.lat = lat;
        gps.lon = lon;
        lanelet::BasicPoint3d point = projector->forward(gps);

        lanelet::BasicPoint2d point2d = {point.x(), point.y()};

        std::vector<lanelet::Lanelet> lanelets = map->laneletLayer.nearest(point2d,1);
        return lanelets.front();
    }

    void intersection_model::set_intersection( lanelet::BoundingBox2d box) {
        // sudo code:
        // -> find all regulatory elements and lanelet objects in bounding box
        // -> Save relevant intersection objects
    }
}