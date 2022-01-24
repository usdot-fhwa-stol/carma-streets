#ifndef VEHICLE_STATUS_INTENT_LANELET_TRANSLATION_H
#define VEHICLE_STATUS_INTENT_LANELET_TRANSLATION_H

// Lanelet2 libraries
#include <lanelet2_core/LaneletMap.h>
#include <lanelet2_projection/UTM.h>
#include <lanelet2_io/Io.h>
#include <lanelet2_core/geometry/Area.h>
#include <lanelet2_core/geometry/Lanelet.h>
#include <lanelet2_core/primitives/Area.h>
#include <lanelet2_core/primitives/Lanelet.h>
#include <lanelet2_core/primitives/LineString.h>
#include <lanelet2_core/primitives/Point.h>
#include <lanelet2_core/primitives/Polygon.h>
#include <lanelet2_core/utility/Units.h>
#include <lanelet2_core/geometry/BoundingBox.h>
#include <lanelet2_core/primitives/BoundingBox.h>
#include "lanelet2_core/primitives/RegulatoryElement.h"
#include <boost/optional/optional.hpp>
#include "lanelet2_core/primitives/BasicRegulatoryElements.h"
#include <lanelet2_extension/projection/mgrs_projector.h>
#include <lanelet2_extension/projection/local_frame_projector.h>
#include <lanelet2_extension/io/autoware_osm_parser.h>
#include <lanelet2_core/primitives/Lanelet.h>
#include <lanelet2_io/Io.h>
#include <lanelet2_io/io_handlers/Factory.h>
#include <lanelet2_extension/regulatory_elements/DigitalSpeedLimit.h>
#include <lanelet2_matching/LaneletMatching.h>
#include "lanelet2_routing/RoutingGraph.h"
#include "lanelet2_routing/Route.h"
#include <lanelet2_io/io_handlers/Writer.h>
#include <lanelet2_projection/UTM.h>
#include <lanelet2_core/geometry/Point.h>

// Standard library
#include <spdlog/spdlog.h>
#include <iomanip>
#include <utility>
#include <type_traits>
#include <math.h>
#include <string>
#include <regex>
#include <rapidjson/document.h>
#include <rapidjson/istreamwrapper.h>
#include <fstream>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/writer.h>
#include <map>
#include <intersection_lanelet_type.h>
#include <trajectory.h>

namespace message_services
{
    namespace message_translations
    {
        class message_lanelet2_translation
        {
        private:
            lanelet::LaneletMapPtr map_ptr;
            lanelet::projection::LocalFrameProjector *local_projector;
            double laneChangeCost = 2.;
            double participantHeight = 2.;
            double minLaneChangeLength = 0.;
            int projector_type = 0;

            //Assume that within intersection radius, the vehicle trajectory maximum cover 4 lanelets.
            const std::uint16_t _maximum_num_lanelets_per_trajectory = 4;

            // Routing graph is used to store the possible routing set
            lanelet::routing::RoutingGraphPtr vehicleGraph_ptr;

        public:
            message_lanelet2_translation(/* args */);
            message_lanelet2_translation(std::string filename);
            ~message_lanelet2_translation();

            /**
             * @brief Read lanelet2 map.
             * @param filename path to lanelet2 osm map.
             * @return true if no exception and map pointer is updated.
             */
            bool read_lanelet2_map(std::string filename);

            /***
             * @brief Identify the current lanelet with the given vehicle geo-loc and vehicle turn direction.
             * @param latitude 
             * @param longtitude
             * @param elevation
             * @param turn_direction (Optional if position is not in intersection bridge/link lanelet).
             * @return Signed Integer lanelet id; \n Return 0 if cannot find the current lanelet.
             **/
            lanelet::Lanelet get_cur_lanelet_by_loc_and_direction(double lat, double lon, double elev, std::string turn_direction, models::trajectory& trajectory) const;

            /***
             * @brief Identify the current lanelet with the given vehicle geo-loc (convert into map point) and vehicle turn direction.
             * @param BasicPoint3d 
             * @param turn_direction (Optional if position is not in intersection bridge/link lanelet).
             * @return Signed Integer lanelet id; \n Return 0 if cannot find the current lanelet.
             **/
             lanelet::Lanelet get_cur_lanelet_by_point_and_direction(lanelet::BasicPoint3d subj_point3d, std::string turn_direction, models::trajectory& trajectory) const;
            
             /***
             * @brief Identify the current lanelet with the given vehicle geo-loc (convert into map point).
             * @param BasicPoint3d 
             * @return a vector of lanelets; \n Return empty vetor if cannot find the current lanelets.
             **/
            std::vector<lanelet::Lanelet> get_cur_lanelets_by_point(lanelet::BasicPoint3d subj_point3d) const;

            /***
             * @brief The distance between the vehicle’s current position and the end of its current lane with the given vehicle geo-loc and vehicle turn direction.
             * @param latitude 
             * @param longtitude
             * @param elevation
             * @param turn_direction (Optional if position is not in intersection bridge/link lanelet).
             * @return Decimal distance to the end of the current lanelet (unit of meters). 
             * \n Return -1 if cannot determine the current lanelet
             **/
            double distance2_cur_lanelet_end(double lat, double lon, double elev, lanelet::Lanelet subj_lanelet, std::string turn_direction, models::trajectory& trajectory) const;
            
            /***
             * @brief The distance between the vehicle’s current position and the end of its current lane with the given vehicle geo-loc and vehicle turn direction.
             * @param BasicPoint3d
             * @param turn_direction (Optional if position is not in intersection bridge/link lanelet).
             * @return Decimal distance to the end of the current lanelet (unit of meters). 
             * \n Return -1 if cannot determine the current lanelet
             **/
            double distance2_cur_lanelet_end(lanelet::BasicPoint3d point,lanelet::Lanelet subj_lanelet, std::string turn_direction, models::trajectory& trajectory) const;
            
            /**
             * @brief Initialize vehicle routing graph.
             * @return true if the routing graph for vehicle participant  is updated, otherwise false.
             */
            bool update_vehicle_routing_graph();

            /***
             * @brief Vehicle broadcast mobilitypath message that contains current vehicle trajectory within the next 6 secs.
             * Determine the subject vehicle future lanelet ids along the vehicle route with the start point to the end point of the trajectory. 
             * @param subj_vehicle_trajectory
             * @param offset_size allow to choose the number of points from mobilitypath messages
             * @param turn_direction
             * @return A map of lanelet_id and intersection lanelet type (entry, departure, link or unknown)
             * **/
            std::map<int64_t, models::intersection_lanelet_type> get_lanelet_types_ids_by_vehicle_trajectory(models::trajectory& subj_vehicle_trajectory, std::uint64_t offset_size, std::string turn_direction) const;

            /***
             * @brief Vehicle broadcast mobilitypath message that contains current vehicle trajectory within the next 6 secs.
             * Determine the subject vehicle future lanelet ids along the vehicle route with the start point to the end point of the trajectory. 
             * @param start_lanelet
             * @param dest_lanelet
             * @return A map of lanelet_id and intersection lanelet type (entry, departure, link or unknown)
             * **/
            std::map<int64_t, models::intersection_lanelet_type> get_lanelet_types_ids_by_route(lanelet::Lanelet start_lanelet, lanelet::Lanelet dest_lanelet,  std::string turn_direction) const;

            /**
            * @brief Function to convert ecef location to a 2d point in map frame
            * @param ecef_point ecef location point
            * @return 2d point in map frame
            */
            lanelet::BasicPoint3d ecef_2_map_point(std::int32_t ecef_x, std::int32_t ecef_y, std::int32_t ecef_z) const;
            
        };
    }
}

#endif
