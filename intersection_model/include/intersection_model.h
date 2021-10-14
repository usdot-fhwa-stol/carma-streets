#ifndef INTERSECTION_MODEL_H
#define INTERSECTION_MODEL_H

//Lanelet2 libraries
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

namespace intersection_model
{
    struct lanelet_info_t
    {
        int64_t id           = 0;
        uint32_t speed_limit = 0.0;  //Unit of measure MPH
        friend bool operator<(const lanelet_info_t &l, const lanelet_info_t &r)
        {
            return std::tie(l.id, l.speed_limit) < std::tie(r.id, r.speed_limit);
        }
    };

    struct intersection_info_t
    {
        std::string intersection_name = "";
        std::int64_t intersection_id = 0;

        //In the context of OSM map, the link/bridge lanelet within an intersection is to connect the entry lanelet and departure lanelet.
        std::vector<lanelet_info_t> link_lanelets;
        std::vector<lanelet_info_t> entering_lanelets;
        std::set<lanelet_info_t> departure_lanelets;
    };
    class intersection_model
    {
        public:

        intersection_model();
        intersection_model(std::string filename);
        ~intersection_model();

        void initialization();
        /**
         * Getter for lanelet2 map.
         * 
         * @return lanelet map object
         */ 
        lanelet::LaneletMapPtr get_lanelet2_map();

        /**
         * @brief Retreive lanelet information given lat,long location
         * 
         * @param lat latitude
         * @return Lanelet2 primitives. The atomic section of a lane.  
         */ 
        lanelet::Lanelet get_lanelet(double lat, double lon );


        /**
         * Retreive 
         * 
         */ 
        /**
         * @brief Read the OSM map, project the map, and initialize all the intersection related info including metadata, 
         * entry_lanelet_ids, link_lanelet_ids and departure_lanelet_ids
         * @return routing graph for vehicle participant 
        **/
        bool load_intersection_info();

        /**
         * @brief Get intersection metadata
         * @return intersection information struct 
         * **/
        intersection_info_t get_intersection_info();

        /**
         * @brief Get list of entry lanelet ids
         * @return A vector of lanelet information struct
         * **/
        std::vector<lanelet_info_t> get_entry_lanelets();

        /**
         * @brief Get list of link lanelet ids
         * @return A vector of lanelet information struct
         * **/
        std::vector<lanelet_info_t> get_link_lanelets();

        /**
         * @brief Get list of conflict lanelet ids relative to the given link lanelet id
         * @param Input parameter is validated, and has to be link lanelet within an intersection
         * @return A vector of lanelet information struct
         * **/
        std::vector<lanelet_info_t> get_conflict_lanelets(int64_t sub_link_lanelet_id);

        /**
         * @brief Get list of departure lanelet ids
         * @return A vector of lanelet information struct
         * **/
        std::set<lanelet_info_t> get_departure_lanelets();

        /**
         * Determin if a lanelet id is a link lanelet id
         * @return true if the lanelet id belongs to a link lanelet at the intersection
         **/
        bool is_valid_link_lanelet_id(int64_t lanelet_id);

        /**
         * @brief Retrieve the speed_limit of the current lanelet
         * @return uint32_t speed_limit regulatory element that is attached to this lanelet
         **/
        uint32_t get_speed_limit_by_lanelet(const lanelet::ConstLanelet& subj_lanelet);

        /**
         * @brief Retrieve the previous lanelet ids of the current lanelet
         * @return A vector of lanelet ids
         **/
        std::vector<int> get_previous_lanelet_ids(int64_t subject_link_lanelet_id);

        /**
         * Based on the all_way_stop regulatory element and routing graph, it is able to retrieve all the entry lanelets, 
         * link lanelets and depature lanelets.
         * If the method call is successful, retunr true; otherwise return false
         **/
        bool load_intersection_info_by_all_way_stop_reg(const lanelet::RegulatoryElement * all_way_stop_reg_elem_ptr);

        /**
         * @brief Get the following lanelets from the entering lanelet. The result following lanelets are link lanelets.
         * Based on the link lanelets, it updates the depature lanelet list
         * @return true if the update for link lanelet and departure lanelet is successful.
         * */
        bool update_link_departure_lanelets_by_entry_lanelet(const lanelet::Lanelet &entry_lanelet);

         /**
         * @brief Read the manifest.json configuration file.
         * @param json_file_name provide the path to the configuration file.
         * @return An object of the document that contains the information from the configuration file.
         * */
        rapidjson::Document read_json_file(const std::string &json_file_name) const;
        
        /**
         * @brief Read lanelet2 map.
         * 
         * @param filename path to lanelet2 osm map
         */
        void read_lanelet2_map( std::string filename );

        private:
        lanelet::LaneletMapPtr map;
        lanelet::projection::LocalFrameProjector *projector;
        double laneChangeCost       = 2.;
        double participantHeight    = 2.;
        double minLaneChangeLength  = 0.;
        int projector_type          = 0;
        const char* osm_file_path_key       = "osm_file_path";
        const char* intersection_name_key   = "intersection_name";
        const char* intersection_id_key     = "intersection_id";

        //routing graph is used to store the possible routing set
        lanelet::routing::RoutingGraphPtr vehicleGraph_ptr;

        //Define intersection information
        intersection_info_t int_info;
        
    };
}
#endif