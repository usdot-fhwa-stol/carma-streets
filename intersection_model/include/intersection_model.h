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
#include <map_msg_worker.h>

// Standard library
#include <algorithm>
#include <iterator>
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
        int64_t id                      = 0;    //Uniquely identify the lanelet id within an OSM file
        double speed_limit               = 0;    //Unit of measure m/s
        double length                   = 0.0;  //Unit of meter
        int32_t signal_group_id         = 0;    //0 = Not available
        std::string turn_direction      = "NA"; //Turn direction of current lanelet
        std::vector<int64_t> connecting_lanelet_ids; // Following lanelet ids

        //Override comparison operator
        friend bool operator<(const lanelet_info_t &l, const lanelet_info_t &r)
        {
            return std::tie(l.id, l.speed_limit) < std::tie(r.id, r.speed_limit);
        }
    };

    struct intersection_info_t
    {
        std::string intersection_name = "";
        std::int64_t intersection_id  = 0;

        //In the context of an intersection and OSM map, the link/bridge lanelet within an intersection is to connect the entry lanelet and departure lanelet.
        std::vector<lanelet_info_t> link_lanelets_info;

        //The entering lanelet is an atomic lane segment where vehicles need to take, stop and wait for infrastructure signal before entering the intersection.         
        std::vector<lanelet_info_t> entering_lanelets_info;

        /**
        * The departure lanelet is an atomic lane segment where vehicles have already safely exited the intersection, 
        * and is allowed to ignore any further signals from infrastructure.         
        **/
        std::set<lanelet_info_t>    departure_lanelets_info;
    };

    struct signalized_intersection_lanelets
    {
        lanelet::Id enter_lanelet_id;
        lanelet::Id link_lanelet_id;
        lanelet::Id depart_lanelet_id;
        int32_t signal_group_id = 0;
    };

    class intersection_model
    {
        public:
            intersection_model() = default;
            intersection_model(const std::string &intersection_name, const int intersection_id, const std::string &osm_file_path ) ;
            ~intersection_model();
            
            /**
            * @brief Read the manifest.json configuration file and initialize intersection information.
            */ 
            void initialization(const std::string &intersection_name, const int intersection_id,const std::string &osm_file_path);            
            /**
             * @brief Getter for lanelet2 map.
             * @return lanelet map object
             */ 
            lanelet::LaneletMapPtr get_lanelet2_map();

            /**
             * @brief Retreive lanelet information given lat,long location
             * @param lat latitude
             * @return Lanelet2 primitives. The atomic section of a lane.  
             */ 
            lanelet::Lanelet get_lanelet(double lat, double lon );

            /**
             * @brief Read the OSM map, project the map, and initialize all the intersection related info including: intersection id, intersection name, 
             * entry_lanelet_ids, link_lanelet_ids and departure_lanelet_ids
             * @return if the intersection information is updated successfully, return true. Otherwise false.
            **/
            bool update_intersection_info();

            /**
             * @brief Get intersection metadata
             * @return intersection information struct 
             * **/
            const  intersection_info_t& get_intersection_info() const ;

            /**
             * @brief Get list of entry lanelet ids
             * @return A vector of lanelet information struct
             * **/
            const std::vector<lanelet_info_t>& get_entry_lanelets_info() const ;

            /**
             * @brief Get list of link lanelet ids
             * @return A vector of lanelet information struct
             * **/
            const std::vector<lanelet_info_t>& get_link_lanelets_info() const ;

            /**
             * @brief Get list of conflict lanelet ids relative to the given link lanelet id
             * @param sub_link_lanelet_id is validated, and has to be link lanelet within an intersection
             * @return A vector of lanelet information struct
             * **/
            const std::vector<lanelet_info_t> get_conflict_lanelets_info(int64_t sub_link_lanelet_id);

            /**
             * @brief Get list of departure lanelet ids
             * @return A vector of lanelet information struct
             * **/
            const std::set<lanelet_info_t>& get_departure_lanelets_info() const;

            /**
             * @brief Comparing the given lanelet id with all link lanelet ids in the current intersection information 
             * to determin if a lanelet id is a link lanelet id.
             * @param lanelet_id
             * @return true if the lanelet id belongs to a link lanelet at the intersection
             **/
            bool is_valid_link_lanelet_id(int64_t lanelet_id);

            /**
             * @brief Retrieve the speed_limit of the current lanelet
             * @return speed_limit regulatory element that is attached to this lanelet
             **/
            double get_speed_limit_by_lanelet(const lanelet::ConstLanelet& subj_lanelet) const;

            /**
             * @brief Get the list of link lanelets that has the same entry lanelet as the subject link lanelet
             * @return A vector of link lanelet ids
             **/
            const std::vector<int> get_shared_entry_link_lanelet_ids(int64_t subject_link_lanelet_id);

            /**
             * @brief Based on the all_way_stop regulatory element and routing graph, it is able to retrieve all the entry lanelets, 
             * link lanelets and depature lanelets.
             * If the method call is successful, return true; otherwise return false
             * @param all_way_stop_reg_elem_ptr a pointer to an all way stop regulatory element
             * @return true if the intersection information is updated, otherwise false.
             **/
            bool update_intersection_info_by_all_way_stop_reg(const lanelet::RegulatoryElement * all_way_stop_reg_elem_ptr);

            /**
             * @brief Get the following link lanelets information from the entering lanelet. 
             * Based on the link lanelets, it retrieves the following depature lanelets
             * @return true if the update for intersection model info (link lanelet info and departure lanelet info) is successful. Otherwise, not updated 
             * */
            bool update_link_departure_lanelets_info_by_entry_lanelet(const lanelet::Lanelet &entry_lanelet);

            /**
             * @brief Read the manifest.json configuration file.
             * @param json_file_name provide the path to the configuration file.
             * @return An object of the document that contains the information from the configuration file.
             * */
            rapidjson::Document read_json_file(const std::string &json_file_name) const;
            
            /**
             * @brief Read lanelet2 map.
             * @param filename path to lanelet2 osm map
             */
            void read_lanelet2_map( std::string filename );

            const std::shared_ptr<lanelet::routing::RoutingGraph> get_vehicleGraph_ptr() const ;

            /**
            * @brief Initialize vehicle routing graph
            * @return true if the routing graph for vehicle participant  is updated, otherwise false.
            */
            bool update_vehicle_routing_graph();
            
            /***
             * @brief Update intersection info structure with incoming map msg consumed by kafka map consumer
             * @param intersection_map populated with MAP message published by v2xhub. This map message describes intersection geometry and signal group ids
             * @return True if the intersection info is updated, otherwise false
             **/
            bool update_intersecion_info_by_map_msg(const std::shared_ptr<intersection_map> int_map_msg);
            /**
             * @brief Comparing the geometry between lane in the MAP message and geometry of the list of lanelets to determine which lanelet this lane path fall into.
             * @param ref_point Reference location (latitude, longitude, elevation) of the subsequent data points
             * @param lane MAP message lane definition includes the lane geometry, unique lane id, and connections if entry lane.
             * @param lane_width MAP message lane definition includes the lane width.
             * @param subj_lanelets List of lanelets
             * @param lane2lanelet_m A mapping reference to update with lane id and its relevant lanelet id
             */
            void mapping_lane_id_2_lanelet_id(const map_referencepoint& ref_point, const map_lane& lane, const long lane_width, const std::vector<lanelet::ConstLanelet>& subj_lanelets, std::unordered_map<long,lanelet::ConstLanelet>& lane2lanelet_m) const;

            /**
             * @brief Convert the centerline geometry of lane defined in J2735 MAP to a list of lanelet2 points in the OSM map.
             * @param ref_point GPS location for a reference point inside an intersection
             * @param lane defind in the J2735 MAP message 
             * @return The list of lanelet2 map points
             */
            std::vector<lanelet::BasicPoint3d>  convert_lane_path_2_basic_points(const map_referencepoint& ref_point, const map_lane& lane) const;

            /**
             * @brief Compute the average distance between a linestring and lanelet
             * @param basic_points a list of points that form a line
             * @param subj_lanelet the lanelet geometry
             * @return The average distance between line and lanelet
             */
            double compute_points_2_lanelet_avg_distance(const std::vector<lanelet::BasicPoint3d>&  basic_points, lanelet::ConstLanelet subj_lanelet) const;

            /**
             * @brief Transform the GPS location into map point using porjector
             * @param lat decimal degree
             * @param lon decimal degree
             * @param elev decimal degree
             * @return lanelet::BasicPoint3d 
             */
            lanelet::BasicPoint3d gps_2_map_point(double lat, double lon, double elev ) const;

            /**
             * @brief Transform the map point to gps location
             * @param x coordinate (meter)
             * @param y coordinate (meter)
             * @param z coordinate (meter)
             * @return GPS location
             */
            lanelet::GPSPoint map_point2_gps(double x, double y, double z) const;

            /**
             * @brief Determine the connection lanelet for a pair of enter and departure lanelet at the intersection
             * @param enter_lanelet_id Entry lanelet id at the intersection
             * @param depart_lanelet_id Exit lanelet id at the intersection
             * @return  The connection/link lanelet id if found. Otherwise return an invalid lanelet id
             */
            lanelet::Id find_link_lanelet_id_by_enter_depart_lanelet_ids(const lanelet::Id enter_lanelet_id, const lanelet::Id depart_lanelet_id ) const;

            /**
             * @brief Get the enter lanelet by id object from the list of intersection entering lanelets
             * @param id Lanelet id 
             * @return lanelet::ConstLanelet . If not matching, the it returns invalid lanelet
             */
            lanelet::ConstLanelet get_enter_lanelet_by_id(lanelet::Id id) const;

            /**
             * @brief Get the link lanelet by id object from the list of intersection link lanelets
             * @param id Lanelet id 
             * @return lanelet::ConstLanelet . If not matching, the it returns invalid lanelet
             */
            lanelet::ConstLanelet get_link_lanelet_by_id(lanelet::Id id) const;

            /**
             * @brief Get the deaparture lanelet by id object from the list of intersection deaparture lanelets
             * @param id Lanelet id 
             * @return lanelet::ConstLanelet. If not matching, the it returns invalid lanelet
             */
            lanelet::ConstLanelet get_departure_lanelet_by_id(lanelet::Id id) const;   

        private:
            lanelet::LaneletMapPtr map;
            lanelet::projection::LocalFrameProjector *projector;
            double laneChangeCost               = 2.;
            double participantHeight            = 2.;
            double minLaneChangeLength          = 0.;
            int projector_type                  = 0;
            const char* osm_file_path_key       = "osm_file_path";      //The osm_file_path should match configuration name in the manifest.json
            const char* intersection_name_key   = "intersection_name";  //The intersection_name should match configuration name in the manifest.json
            const char* intersection_id_key     = "intersection_id";    //The intersection_id should match configuration name in the manifest.json
            const double MPH_TO_MS              = 0.44704;              //Miles per hour to meters per seconds conversion
            const double CM_TO_M                = 0.01;                 //Centimiter to meter conversion
            const double DM_TO_M                = 0.1;                  //Decimeter to meter conversion
            const double MICRO_DEGREE_TO_DEGREE = 0.000001;             //Microdegree to degree conversion
            const double TENTH_TO_ONE           = 0.1;                  //One-tenth of a unit to one

            //Routing graph is used to store the possible routing set
            lanelet::routing::RoutingGraphPtr  vehicleGraph_ptr;

            //Define intersection information
            intersection_info_t int_info;   

            //In the context of an intersection and OSM map, the link/bridge lanelet within an intersection is to connect the entry lanelet and departure lanelet.
            //Keep a private copy of link lanelets geometry for intersection
            std::vector<lanelet::ConstLanelet> link_lanelets;

            //The entering lanelet is an atomic lane segment where vehicles need to take, stop and wait for infrastructure signal before entering the intersection.         
            //Keep a private copy of entering lanelets geometry for intersection
            std::vector<lanelet::ConstLanelet> entering_lanelets;

            /**
            * The departure lanelet is an atomic lane segment where vehicles have already safely exited the intersection, 
            * and is allowed to ignore any further signals from infrastructure.         
            **/
            //Keep a private copy of departure lanelets geometry for intersection
            std::vector<lanelet::ConstLanelet> departure_lanelets;    
    };
}
#endif