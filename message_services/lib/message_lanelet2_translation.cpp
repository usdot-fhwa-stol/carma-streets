#include "message_lanelet2_translation.h"

namespace message_services
{
    namespace message_translations
    {
        message_lanelet2_translation::message_lanelet2_translation(/* args */) {}

        message_lanelet2_translation::message_lanelet2_translation(std::string filename)
        {
            if (read_lanelet2_map(filename))
            {
                spdlog::info("{0}: Map is initialied. ", __FILE__);
                if (!this->vehicleGraph_ptr)
                {
                    spdlog::info("{0}: Updating vehicle routing graph ... ", __FILE__);
                    if (this->update_vehicle_routing_graph())
                    {
                        std::cout << "Updated vehicle routing graph. " << std::endl;
                        spdlog::info("{0}: Updated vehicle routing graph. ", __FILE__);
                    }
                    else
                    {
                        spdlog::error("{0}: Failed to update vehicle routing graph. ", __FILE__);
                        exit(-1);
                    }
                }
            }
            else
            {
                spdlog::error("{0}: Failed to initialzie map. ", __FILE__);
                exit(-1);
            }
            spdlog::info("{0}: Finished initializing message_lanelet2_translation. ", __FILE__);
        }

        message_lanelet2_translation::~message_lanelet2_translation() {}

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

        std::int64_t message_lanelet2_translation::get_cur_lanelet_id_by_loc_and_direction(double lat, double lon, double elev, std::string turn_direction) const
        {
            lanelet::BasicPoint3d subj_point3d;
            try
            {
                // construct a GPS point
                lanelet::GPSPoint subj_gps_pos;
                subj_gps_pos.lat = lat;
                subj_gps_pos.lon = lon;
                subj_gps_pos.ele = elev;

                // project the GPS point to (x,y,z)
                subj_point3d = local_projector->forward(subj_gps_pos);
            }
            catch (...)
            {
                spdlog::error("Cannot project the GPS position: Latitude: {0} , Longitude: {1}, Elevation: {2}", lat, lon, elev);
                return lanelet::InvalId;
            }
            return get_cur_lanelet_id_by_point_and_direction(subj_point3d, turn_direction);
        }

        std::vector<lanelet::Lanelet> message_lanelet2_translation::get_cur_lanelets_by_point(lanelet::BasicPoint3d subj_point3d) const
        {
            std::vector<lanelet::Lanelet> current_total_lanelets;
            lanelet::BasicPoint2d subj_point2d = lanelet::utils::to2D(subj_point3d);

            // Find the nearest lanelets with maximum number (=3) of return lanelets because a point in intersection may return maximum three link/bridge lanelets
            auto nearest_lanelets = lanelet::geometry::findNearest(this->map_ptr->laneletLayer, subj_point2d, 3);

            if (nearest_lanelets.size() == 0)
            {
                spdlog::error("No nearest lanelet to the vehicle in map point: x = {0} y = {1}, z = {2}", subj_point3d.x(), subj_point3d.y(), subj_point3d.z());
                return current_total_lanelets;
            }

            // Only interested in the nearest lanelets with first = 0
            for (auto itr = nearest_lanelets.begin(); itr != nearest_lanelets.end(); itr++)
            {
                if (itr->first == 0)
                {
                    current_total_lanelets.push_back(itr->second);
                }
            }
            return current_total_lanelets;
        }

        std::int64_t message_lanelet2_translation::get_cur_lanelet_id_by_point_and_direction(lanelet::BasicPoint3d subj_point3d, std::string turn_direction) const
        {
            std::vector<lanelet::Lanelet> current_total_lanelets = get_cur_lanelets_by_point(subj_point3d);
            if (current_total_lanelets.empty())
            {
                spdlog::error("No current lanelets to the vehicle in map point: x = {0}, y = {1}, z = {2}, and turn direction = {3}", subj_point3d.x(), subj_point3d.y(), subj_point3d.z(), turn_direction);
                return lanelet::InvalId;
            }

            /****
             * Check the lanelet turn direction to help identify the current lanelet if current geo-loc return more than 1 current lanelets.
             * If there is only one element in the current total lanelets, ignore the turn direction
             * ****/
            if (current_total_lanelets.size() > 1)
            {
                for (auto itr = current_total_lanelets.begin(); itr != current_total_lanelets.end(); itr++)
                {
                    const lanelet::Lanelet cur_lanelet = *itr;

                    if (cur_lanelet.hasAttribute("turn_direction"))
                    {
                        const lanelet::Attribute attr = cur_lanelet.attribute("turn_direction");
                        if (attr.value() == turn_direction)
                        {
                            return cur_lanelet.id();
                        }
                    }
                    else if (!cur_lanelet.hasAttribute("turn_direction") || cur_lanelet.attribute("turn_direction").value() != turn_direction)
                    {
                        spdlog::error("{0}: Cannot determine the current lanelet with this turn direction = {1} for point = (x = {2} , y = {3}, z= {4})", __FILE__, turn_direction, subj_point3d.x(), subj_point3d.y(), subj_point3d.z());
                    }
                }
            }
            else if (current_total_lanelets.size() == 1)
            {
                return current_total_lanelets.front().id();
            }

            return lanelet::InvalId;
        }

        double message_lanelet2_translation::distance2_cur_lanelet_end(double lat, double lon, double elev, std::string turn_direction) const
        {
            double total_length = 0.0;
            bool start_accumulate = false;
            std::int64_t id = get_cur_lanelet_id_by_loc_and_direction(lat, lon, elev, turn_direction);
            if (id == lanelet::InvalId)
            {
                spdlog::error("{0}: Get invalid lanelet id = {1} from position: ({2}, {3} , {4}) and turn direction: {5}", __FILE__, id, lat, lon, elev, turn_direction);
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

        std::map<int64_t, models::intersection_lanelet_type> message_lanelet2_translation::get_lanelet_types_ids_by_vehicle_trajectory(models::trajectory trajectory, std::uint64_t offset_size, std::string turn_direction)
        {
            std::map<int64_t, models::intersection_lanelet_type> lanelet_id_type_m;

            lanelet::BasicPoint3d basic_point3d_start = ecef_2_map_point(trajectory.location.ecef_x, trajectory.location.ecef_y, trajectory.location.ecef_z);
            lanelet::BasicPoint3d basic_point3d_dest  = ecef_2_map_point((trajectory.location.ecef_x + trajectory.offsets.back().offset_x),
                                                                        (trajectory.location.ecef_y + trajectory.offsets.back().offset_y),
                                                                        (trajectory.location.ecef_z + trajectory.offsets.back().offset_z));
            // lanelet::BasicPoint3d basic_point3d_start_2 = this->local_projector->projectECEF(this->local_projector->projectECEF(point3d.basicPoint(), 1), -1);
            int64_t start_lanelet_id = get_cur_lanelet_id_by_point_and_direction(basic_point3d_start, turn_direction);
            spdlog::info("start lanelet id = {0}", start_lanelet_id);

            int64_t dest_lanelet_id = get_cur_lanelet_id_by_point_and_direction(basic_point3d_dest, turn_direction);
            spdlog::info("dest lanelet id = {0}", dest_lanelet_id);

            if (start_lanelet_id == 0L || dest_lanelet_id == 0L)
            {
                spdlog::error("{0}: Empty start or end lanelets. ", __FILE__);
                return lanelet_id_type_m;
            }

            lanelet_id_type_m = get_lanelet_types_ids_by_route(start_lanelet_id, dest_lanelet_id);

            return lanelet_id_type_m;
        }

        lanelet::BasicPoint3d message_lanelet2_translation::ecef_2_map_point(std::int32_t ecef_x, std::int32_t ecef_y, std::int32_t ecef_z)
        {
            lanelet::BasicPoint3d basic_point3d = this->local_projector->projectECEF({((double)ecef_x) / 100, ((double)ecef_y) / 100, ((double)ecef_z) / 100}, -1);
            return basic_point3d;
        }
        std::map<int64_t, models::intersection_lanelet_type> message_lanelet2_translation::get_lanelet_types_ids_by_route(int64_t start_lanelet_id, int64_t dest_lanelet_id) const
        {
            std::map<int64_t, models::intersection_lanelet_type> lanelet_id_type_m;

            if (start_lanelet_id == 0 || dest_lanelet_id == 0)
            {
                spdlog::error("{0}: Invalid start or end lanelet id. ", __FILE__);
                return lanelet_id_type_m;
            }

            lanelet::Optional<lanelet::routing::Route> route = this->vehicleGraph_ptr->getRoute(this->map_ptr->laneletLayer.get(start_lanelet_id), this->map_ptr->laneletLayer.get(dest_lanelet_id));
            if (route)
            {
                const lanelet::routing::LaneletPath &lp = route->shortestPath();

                //Assume that within intersection radius, the vehicle trajectory maximum cover 4 lanelets (entry. link, departure lanelets etc).
                if (lp.size() > _maximum_num_lanelets_per_trajectory)
                {
                    spdlog::error("{0}: Cannot identify the lanelets along the subject vehicle route. The subject vehicle trajectory should cover maximum {1} number of lanelets. ", __FILE__, _maximum_num_lanelets_per_trajectory);
                    lanelet_id_type_m.clear();
                    return lanelet_id_type_m;
                }

                lanelet::ConstLanelet entry_lanelet;
                lanelet::ConstLanelet link_lanelet;
                lanelet::ConstLanelet departure_lanelet;

                //Get the list of lanelet ids along the route and identify the intersection lanelet types using the all_way_stop regulatory element
                for (auto ll_itr = lp.begin(); ll_itr != lp.end(); ll_itr++)
                {
                    lanelet_id_type_m.insert(std::make_pair(ll_itr->id(), models::intersection_lanelet_type::unknown));
                    // spdlog::error("insert lanelet id {0}:  ", ll_itr->id());

                    /***
                     * Checking whether the current lanelet is link lanelet. 
                     * The link lanelet's previous lanelet is entry lanelet, and entry lanelet has the all_way_stop regulatory element
                     * **/
                    if (vehicleGraph_ptr->previous(*ll_itr).front().regulatoryElements().size() > 0)
                    {
                        lanelet::RegulatoryElementConstPtrs reg_ptrs = vehicleGraph_ptr->previous(*ll_itr).front().regulatoryElements();
                        for (auto reg_ptrs_itr = reg_ptrs.begin(); reg_ptrs_itr != reg_ptrs.end(); reg_ptrs_itr++)
                        {
                            const lanelet::RegulatoryElement *reg = reg_ptrs_itr->get();
                            if (reg->attribute(lanelet::AttributeName::Subtype).value() == lanelet::AttributeValueString::AllWayStop)
                            {
                                entry_lanelet = vehicleGraph_ptr->previous(*ll_itr).front();
                                link_lanelet = *ll_itr;
                                departure_lanelet = vehicleGraph_ptr->following(link_lanelet).front();
                            }
                        }
                    }
                }

                //update the type for each lanelet id in the list of lanelet ids
                for (auto itr = lanelet_id_type_m.begin(); itr != lanelet_id_type_m.end(); itr++)
                {
                    if (itr->first == entry_lanelet.id())
                    {
                        itr->second = models::intersection_lanelet_type::entry;
                    }
                    else if (itr->first == link_lanelet.id())
                    {
                        itr->second = models::intersection_lanelet_type::link;
                    }
                    else if (itr->first == departure_lanelet.id())
                    {
                        itr->second = models::intersection_lanelet_type::departure;
                    }
                }
            }
            else
            {
                spdlog::error("{0}: Cannot find a route. ", __FILE__);
            }

            return lanelet_id_type_m;
        }
    }
}
