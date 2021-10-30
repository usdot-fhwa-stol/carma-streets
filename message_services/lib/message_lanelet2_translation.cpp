#include "message_lanelet2_translation.h"

namespace message_services
{
    namespace message_translations
    {
        message_lanelet2_translation::message_lanelet2_translation(/* args */) {}

        message_lanelet2_translation::message_lanelet2_translation(std::string filename)
        {
            read_lanelet2_map(filename);
        }

        message_lanelet2_translation::~message_lanelet2_translation()
        {
        }

        bool message_lanelet2_translation::read_lanelet2_map(std::string filename)
        {
            try
            {
                int projector_type = 1;
                std::string target_frame;
                lanelet::ErrorMessages errors;

                // Parse geo reference info from the lanelet map (.osm)
                lanelet::io_handlers::AutowareOsmParser::parseMapParams(filename, &projector_type, &target_frame);
                local_projector = new lanelet::projection::LocalFrameProjector(target_frame.c_str());
                this->map_ptr = lanelet::load(filename, *local_projector, &errors);
                if (!this->map_ptr->empty())
                {
                    return true;
                }
            }
            catch (std::exception ex)
            {
                spdlog::error("{0}: Cannot read osm file {1}. Error message: {2} ", __FILE__, filename, ex.what());
            }
            return false;
        }

        bool message_lanelet2_translation::update_vehicle_routing_graph()
        {
            // get routingGraph from map
            lanelet::traffic_rules::TrafficRulesPtr trafficRules{
                lanelet::traffic_rules::TrafficRulesFactory::create(lanelet::Locations::Germany,
                                                                    lanelet::Participants::Vehicle,
                                                                    lanelet::traffic_rules::TrafficRules::Configuration())};
            lanelet::routing::RoutingCostPtrs costPtrs{
                std::make_shared<lanelet::routing::RoutingCostDistance>(this->laneChangeCost, this->minLaneChangeLength),
                std::make_shared<lanelet::routing::RoutingCostTravelTime>(this->laneChangeCost)};
            lanelet::routing::RoutingGraph::Configuration configuration;
            configuration.insert(std::make_pair(lanelet::routing::RoutingGraph::ParticipantHeight, this->participantHeight));
            this->vehicleGraph_ptr = lanelet::routing::RoutingGraph::build(*this->map_ptr, *trafficRules, costPtrs, configuration);
            if (!this->vehicleGraph_ptr)
            {
                return false;
            }
            return true;
        }

        std::int64_t message_lanelet2_translation::get_cur_lanelet_id_by_pos(double lat, double lon, double elev, std::string turn_direction) const
        {          
            lanelet::BasicPoint3d subj_point3d;
            try
            {
                // construct a GPS point
                lanelet::GPSPoint subj_gps_pos;
                subj_gps_pos.lat = lat;
                subj_gps_pos.lon = lon;
                subj_gps_pos.ele = elev;
                subj_point3d = local_projector->forward(subj_gps_pos);
            }
            catch (...)
            {
                spdlog::error("Cannot project the GPS position: Latitude: {0} , Longitude: {1}, Elevation: {2}", lat, lon, elev);
                return 0;
            }
            
            // project the GPS point to (x,y,z)
            lanelet::BasicPoint2d subj_point2d = lanelet::utils::to2D(subj_point3d);

            // Find the nearest lanelets with maximum number (=3) of return lanelets
            auto nearest_lanelets = lanelet::geometry::findNearest(this->map_ptr->laneletLayer, subj_point2d, 3);

            if (nearest_lanelets.size() == 0)
            {
                spdlog::error("No nearest lanelet to responder vehicle in map point: {0} {1}", lat, lon);
                return 0;
            }

            // Only interested in the nearest lanelets with first = 0
            std::vector<lanelet::Lanelet> current_total_lanelets;
            for (auto itr = nearest_lanelets.begin(); itr != nearest_lanelets.end(); itr++)
            {
                if (itr->first == 0)
                {
                    current_total_lanelets.push_back(itr->second);
                }
            }

            /****
             * Check the lanelet turn direction to help identify the current lanelet if current geo-loc return more than 1 current lanelets.
             * If there is only one element in the current total lanelets, ignore the turn direction
             * ****/
            for (auto itr = current_total_lanelets.begin(); itr != current_total_lanelets.end(); itr++)
            {
                const lanelet::Lanelet cur_lanelet = *itr;

                if (current_total_lanelets.size() > 1 && cur_lanelet.hasAttribute("turn_direction"))
                {
                    const lanelet::Attribute attr = cur_lanelet.attribute("turn_direction");
                    if (attr.value() == turn_direction)
                    {
                        return cur_lanelet.id();
                    }
                    else
                    {
                        spdlog::error("{0}: Cannot determine the current lanelet with this turn direction = {1} for position = ({2} , {3})", __FILE__, turn_direction, lat, lon);
                    }
                }
                else
                {
                    return cur_lanelet.id();
                }
            }

            return 0;
        }

        double message_lanelet2_translation::distance2_cur_lanelet_end(double lat, double lon, double elev, std::string turn_direction) const
        {
            double total_length = 0.0;
            bool start_accumulate = false;
            std::int64_t id = get_cur_lanelet_id_by_pos(lat, lon, elev, turn_direction);
            if(id == 0)
            {
                spdlog::error("{0}: Get invalid lanelet id = {1} from position: ({2}, {3} , {4}) and turn direction: {5}", __FILE__,id, lat, lon, elev, turn_direction);
                return -1;
            }

            lanelet::Lanelet subj_lanelet = this->map_ptr->laneletLayer.get(id);
            auto sub_lanelet_centerline = subj_lanelet.centerline2d();
            lanelet::ConstHybridLineString2d lsHybrid = lanelet::utils::toHybrid(sub_lanelet_centerline);

            // construct a GPS point
            lanelet::GPSPoint subj_gps_pos;
            subj_gps_pos.lat = lat;
            subj_gps_pos.lon = lon;
            subj_gps_pos.ele = elev;

            // project the GPS point to (x,y,z)
            lanelet::BasicPoint3d subj_point3d = local_projector->forward(subj_gps_pos);
            lanelet::BasicPoint2d subj_point2d = lanelet::utils::to2D(subj_point3d);

            //Get the distance between the subject point to the centerline.
            auto dP2Line2d = lanelet::geometry::distance(subj_point2d, lsHybrid);

            for (size_t i = 0; i < sub_lanelet_centerline.numSegments(); i++)
            {
                //Get the distance btween the subject point to a point on the centerline
                double p2p_d = lanelet::geometry::distance2d(subj_point2d, sub_lanelet_centerline.segment(i).first);

                //Start from the closest point on the centerline to the subject point, and accumulate the closest point to the end of centerline
                if (p2p_d <= (dP2Line2d + 0.1))
                {
                    start_accumulate = true;
                }

                if (start_accumulate)
                {
                    double segment_distance = lanelet::geometry::distance2d(sub_lanelet_centerline.segment(i).first, sub_lanelet_centerline.segment(i).second);
                    total_length += segment_distance;
                }
            }
            return total_length;
        }

    }
}
