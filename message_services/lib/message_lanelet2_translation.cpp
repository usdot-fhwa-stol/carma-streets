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
                SPDLOG_INFO("Map is initialied. ");
                if (!this->vehicleGraph_ptr)
                {
                    SPDLOG_INFO("Updating vehicle routing graph ... ");
                    if (this->update_vehicle_routing_graph())
                    {
                        SPDLOG_INFO("Updated vehicle routing graph. ");
                    }
                    else
                    {
                        SPDLOG_ERROR("Failed to update vehicle routing graph. ");
                        exit(-1);
                    }
                }
            }
            else
            {
                SPDLOG_ERROR("Failed to initialzie map. ");
                exit(-1);
            }
            SPDLOG_INFO("Finished initializing message_lanelet2_translation. ");
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
            catch (const std::exception &ex)
            {
                SPDLOG_ERROR("Cannot read osm file {0}. Error message: {1} ", filename, ex.what());
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

        lanelet::Lanelet message_lanelet2_translation::get_cur_lanelet_by_loc_and_direction(double lat, double lon, double elev, std::string turn_direction, models::trajectory &trajectory) const
        {
            lanelet::BasicPoint3d subj_point3d = gps_2_map_point(lat, lon, elev);
            return get_cur_lanelet_by_point_and_direction(subj_point3d, turn_direction, trajectory);
        }

        std::vector<lanelet::Lanelet> message_lanelet2_translation::get_cur_lanelets_by_point(lanelet::BasicPoint3d subj_point3d) const
        {
            std::vector<lanelet::Lanelet> current_total_lanelets;           
            lanelet::BasicPoint2d subj_point2d = lanelet::utils::to2D(subj_point3d);

            // Find the nearest lanelets with maximum number (=3) of return lanelets because a point in intersection may return maximum three link/bridge lanelets
            auto nearest_lanelets = lanelet::geometry::findNearest(this->map_ptr->laneletLayer, subj_point2d, 3);

            if (nearest_lanelets.size() == 0)
            {
                SPDLOG_ERROR("No nearest lanelet to the vehicle in map point: x = {0} y = {1}, z = {2}", subj_point3d.x(), subj_point3d.y(), subj_point3d.z());
                return current_total_lanelets;
            }

            // Only interested in the nearest lanelets with first = 0
            for (auto itr = nearest_lanelets.begin(); itr != nearest_lanelets.end(); itr++)
            {
                SPDLOG_DEBUG("Nearest lanelet to the vehicle in map point: x = {0} y = {1}, z = {2}. Nearest lanelet id = {3}", subj_point3d.x(), subj_point3d.y(), subj_point3d.z(), itr->second.id());

                if (itr->first == 0)
                {
                    current_total_lanelets.push_back(itr->second);
                }
            }
            return current_total_lanelets;
        }

        lanelet::Lanelet message_lanelet2_translation::get_cur_lanelet_by_point_and_direction(lanelet::BasicPoint3d subj_point3d, std::string turn_direction, models::trajectory &trajectory) const
        {                  
            std::vector<lanelet::Lanelet> current_total_lanelets = get_cur_lanelets_by_point(subj_point3d);   
            std::vector<lanelet::Lanelet> result_lanelets;

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
                            result_lanelets.push_back(cur_lanelet);
                        }
                    }
                }

                if (result_lanelets.size() == 1)
                {
                    return result_lanelets.front();
                }

                // If there are more than two current lanelets return, check the trajectory
                if (result_lanelets.size() > 1)
                {
                    lanelet::BasicPoint3d basic_point3d_start = this->ecef_2_map_point(trajectory.location.ecef_x, trajectory.location.ecef_y, trajectory.location.ecef_z);
                    if (trajectory.offsets.empty())
                    {
                        SPDLOG_ERROR("Cannot determine current lanelet and ids with vehicle trajectory offset size = 0. ");
                        return lanelet::Lanelet();
                    }

                    std::int32_t dest_x = trajectory.location.ecef_x;
                    std::int32_t dest_y = trajectory.location.ecef_y;
                    std::int32_t dest_z = trajectory.location.ecef_z;
                    for (auto offset_itr = trajectory.offsets.begin(); offset_itr != trajectory.offsets.end(); offset_itr++)
                    {
                        dest_x += offset_itr->offset_x;
                        dest_y += offset_itr->offset_y;
                        dest_z += offset_itr->offset_z;
                        SPDLOG_TRACE("offset_x = {0},offset_y = {1},offset_z = {2}", offset_itr->offset_x, offset_itr->offset_y, offset_itr->offset_z);
                        SPDLOG_TRACE("dest_x = {0},dest_y = {1},dest_z = {2}", dest_x, dest_y, dest_z);
                    }

                    lanelet::BasicPoint3d basic_point3d_dest = ecef_2_map_point(dest_x, dest_y, dest_z);
                    lanelet::Lanelet result_lanelet = lanelet::Lanelet();
                    double smaller_distance_sum = 0;
                    for (auto itr = result_lanelets.begin(); itr != result_lanelets.end(); itr++)
                    {
                        const lanelet::Lanelet cur_lanelet = *itr;
                        double start_distance2_ctl = lanelet::geometry::distance2d(lanelet::utils::to2D(basic_point3d_start), lanelet::utils::toHybrid(cur_lanelet.centerline2d()));
                        SPDLOG_DEBUG("cur_lanelet = {0} to trajectory start point distance = {1}", cur_lanelet.id(), start_distance2_ctl);

                        double dest_distance2_ctl = lanelet::geometry::distance2d(lanelet::utils::to2D(basic_point3d_dest), lanelet::utils::toHybrid(cur_lanelet.centerline2d()));
                        SPDLOG_DEBUG("cur_lanelet = {0} to trajectory dest point distance = {1}", cur_lanelet.id(), dest_distance2_ctl);

                        double cur_distance_sum = start_distance2_ctl + dest_distance2_ctl;

                        if (smaller_distance_sum >= cur_distance_sum || smaller_distance_sum == 0)
                        {
                            smaller_distance_sum = cur_distance_sum;
                            result_lanelet = cur_lanelet;
                        }
                    }
                    return result_lanelet;
                }
                else {
                    SPDLOG_ERROR("Cannot determine the current lanelet with this turn direction : {0} for point = (x : {1} , y : {2}, z=:{3})", turn_direction, subj_point3d.x(), subj_point3d.y(), subj_point3d.z());
                    return lanelet::Lanelet();
                }
            }
            else if (current_total_lanelets.size() == 1)
            {           
                return current_total_lanelets.front();
            }
            else {
                SPDLOG_ERROR("No current lanelets to the vehicle in map point: x = {0}, y = {1}, z = {2}, and turn direction = {3}", subj_point3d.x(), subj_point3d.y(), subj_point3d.z(), turn_direction);
                return lanelet::Lanelet();
            }
        }

        double message_lanelet2_translation::distance2_cur_lanelet_end(double lat, double lon, double elev, lanelet::Lanelet subj_lanelet, std::string turn_direction, models::trajectory &trajectory) const
        {   
            lanelet::BasicPoint3d subj_point3d = gps_2_map_point(lat, lon, elev);
            return distance2_cur_lanelet_end(subj_point3d,subj_lanelet, turn_direction, trajectory);
        }

        double message_lanelet2_translation::distance2_cur_lanelet_end(lanelet::BasicPoint3d subj_point3d, lanelet::Lanelet subj_lanelet,std::string turn_direction, models::trajectory &trajectory) const
        {
            double total_length = 0.0;
            double p2p_d = -1;
            bool start_accumulate = false;
            if (subj_lanelet.id() == lanelet::InvalId)
            {
                SPDLOG_ERROR("Get invalid lanelet id = {0} from position: ({1}, {2} , {3}) and turn direction: {4}", subj_point3d.x(), subj_point3d.y(), subj_point3d.z(), turn_direction);
                return -1;
            }

            auto sub_lanelet_centerline = subj_lanelet.centerline2d();
            lanelet::ConstHybridLineString2d lsHybrid = lanelet::utils::toHybrid(sub_lanelet_centerline);
            lanelet::BasicPoint2d subj_point2d = lanelet::utils::to2D(subj_point3d);

            // Get the distance between the subject point to the centerline.
            auto dP2Line2d = lanelet::geometry::distance(subj_point2d, lsHybrid);
            for (size_t i = 0; i < sub_lanelet_centerline.numSegments(); i++)
            {
                // Project the subject point to a point on the centerline                
                if(!start_accumulate)
                {
                    p2p_d = lanelet::geometry::distance2d(subj_point2d, sub_lanelet_centerline.segment(i).first);
                }
                // Start from the closest point on the centerline to the subject point, and accumulate the closest point to the end of centerline
                if (p2p_d != -1 && p2p_d <= (dP2Line2d + 0.2))
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

        lanelet::BasicPoint3d message_lanelet2_translation::ecef_2_map_point(std::int32_t ecef_x, std::int32_t ecef_y, std::int32_t ecef_z) const
        {
            lanelet::BasicPoint3d basic_point3d = this->local_projector->projectECEF({((double)ecef_x) / 100, ((double)ecef_y) / 100, ((double)ecef_z) / 100}, -1);
            return basic_point3d;
        }

        lanelet::BasicPoint3d message_lanelet2_translation::gps_2_ecef(double lat, double lon, double elev) const {
            lanelet::BasicPoint3d basic_point3d = this->local_projector->projectECEF({lat, lon, elev}, 1);
            return basic_point3d;
        }

        lanelet::BasicPoint3d message_lanelet2_translation::gps_2_map_point(double lat, double lon, double elev) const
        {
            lanelet::BasicPoint3d basic_point3d;
            try
            {
                // construct a GPS point
                lanelet::GPSPoint subj_gps_pos;
                subj_gps_pos.lat = lat;
                subj_gps_pos.lon = lon;
                subj_gps_pos.ele = elev;

                // project the GPS point to (x,y,z)
                basic_point3d = local_projector->forward(subj_gps_pos);
            }
            catch (...)
            {
                SPDLOG_ERROR("Cannot project the GPS position: Latitude: {0} , Longitude: {1}, Elevation: {2}", lat, lon, elev);
            }
            return basic_point3d;            
        }

        std::map<int64_t, models::intersection_lanelet_type> message_lanelet2_translation::get_lanelet_types_ids(lanelet::Lanelet subj_lanelet, std::string turn_direction) const
        {
            std::map<int64_t, models::intersection_lanelet_type> lanelet_id_type_m;
            if (subj_lanelet.id() == 0 )
            {
                SPDLOG_ERROR("Invalid start or end lanelet id. ");
                return lanelet_id_type_m;
            }

                lanelet::ConstLanelet entry_lanelet;
                lanelet::ConstLanelet link_lanelet;
                lanelet::ConstLanelet departure_lanelet;
                lanelet::Lanelet* ll_itr = &subj_lanelet;
            try
            {
                    /** Checking whether the current lanelet is link lanelet.
                     * The link lanelet's previous lanelet is entry lanelet, and entry lanelet has the all_way_stop regulatory element
                     * **/
                    lanelet::ConstLanelet local_ll = vehicleGraph_ptr->previous(*ll_itr).front();
                    if (local_ll.regulatoryElements().size() > 0)
                    {
                        lanelet::RegulatoryElementConstPtrs reg_ptrs = local_ll.regulatoryElements();
                        for (auto reg_ptrs_itr = reg_ptrs.begin(); reg_ptrs_itr != reg_ptrs.end(); reg_ptrs_itr++)
                        {
                            const lanelet::RegulatoryElement *reg = reg_ptrs_itr->get();
                            if (reg->attribute(lanelet::AttributeName::Subtype).value() == lanelet::AttributeValueString::AllWayStop)
                            {
                                SPDLOG_DEBUG("Found link lanelet id :{0}  ", ll_itr->id());
                                entry_lanelet = local_ll;
                                link_lanelet = *ll_itr;
                                departure_lanelet = vehicleGraph_ptr->following(link_lanelet).front();
                                break;
                            }
                        }
                    }

                    /***
                     * Checking whether the current lanelet is entry lanelet, and entry lanelet has the all_way_stop regulatory element
                     * **/
                    if (!ll_itr->regulatoryElements().empty())
                    {
                        lanelet::RegulatoryElementPtrs reg_ptrs = ll_itr->regulatoryElements();
                        for (auto reg_ptrs_itr = reg_ptrs.begin(); reg_ptrs_itr != reg_ptrs.end(); reg_ptrs_itr++)
                        {
                            const lanelet::RegulatoryElement *reg = reg_ptrs_itr->get();
                            if (reg->attribute(lanelet::AttributeName::Subtype).value() == lanelet::AttributeValueString::AllWayStop)
                            {
                                SPDLOG_DEBUG("Found entry lanelet id :{0}  ", ll_itr->id());
                                entry_lanelet = *ll_itr;
                                lanelet::ConstLanelets possible_link_lanelets = vehicleGraph_ptr->following(*ll_itr);
                                // Check turn_direction to determine the link lanelet for subject vehicle
                                // If turn direction is "NA" or empty, it cannot determine which link lanelet inside the intersection
                                for (auto itr = possible_link_lanelets.begin(); itr != possible_link_lanelets.end(); itr++)
                                {
                                    if (itr->hasAttribute("turn_direction") && itr->attribute("turn_direction").value() == turn_direction)
                                    {
                                        link_lanelet = *itr;
                                        SPDLOG_DEBUG("Found link lanelet id :{0}  ", itr->id());
                                        departure_lanelet = vehicleGraph_ptr->following(link_lanelet).front();
                                        break;
                                    }
                                }
                            }
                        }
                    }
                    /**
                     * Checking whether current lanelet is departure lanelet, previous lanelet will be link lanlet and the link lanelets
                     * previous lanelet will be an entry lanelet that has the all_way_stop regulatory element
                     * 
                     */
                    // Get possible link lanelets
                    lanelet::ConstLanelets possible_links = vehicleGraph_ptr->previous(*ll_itr);
                    lanelet::ConstLanelet possible_link;
                    for ( const auto &lane : possible_links ) {
                        if ( lane.hasAttribute("turn_direction") && lane.attribute("turn_direction").value() == turn_direction ) {
                            possible_link = lane;
                            break;
                        }
                    }
                    if ( possible_link.id() != 0 ) {
                        lanelet::ConstLanelet possible_entry = vehicleGraph_ptr->previous(possible_link).front();
                        if (possible_entry.regulatoryElements().size() > 0)
                        {
                            lanelet::RegulatoryElementConstPtrs reg_ptrs = possible_entry.regulatoryElements();
                            for (auto reg_ptrs_itr = reg_ptrs.begin(); reg_ptrs_itr != reg_ptrs.end(); reg_ptrs_itr++)
                            {
                                const lanelet::RegulatoryElement *reg = reg_ptrs_itr->get();
                                if (reg->attribute(lanelet::AttributeName::Subtype).value() == lanelet::AttributeValueString::AllWayStop)
                                {
                                    entry_lanelet = possible_entry;
                                    link_lanelet = possible_link;
                                    departure_lanelet = *ll_itr;
                                    break;
                                }
                            }
                        }
                    }


                // insert the type for each lanelet id in the list of lanelet ids
                if (entry_lanelet.id() != lanelet::InvalId)
                {
                    lanelet_id_type_m.insert(std::make_pair(entry_lanelet.id(), models::intersection_lanelet_type::entry));
                }

                if (link_lanelet.id() != lanelet::InvalId)
                {
                    lanelet_id_type_m.insert(std::make_pair(link_lanelet.id(), models::intersection_lanelet_type::link));
                }

                if (departure_lanelet.id() != lanelet::InvalId)
                {
                    lanelet_id_type_m.insert(std::make_pair(departure_lanelet.id(), models::intersection_lanelet_type::departure));
                }
            return lanelet_id_type_m;
            }
            catch(const lanelet::LaneletError &e)
            {
                SPDLOG_ERROR("Cannot determine lanelet type and ids with vehicle current lanelet. \n {0}", e.what());
                lanelet_id_type_m.clear();
                return lanelet_id_type_m;
            }
        }


        lanelet::Lanelet message_lanelet2_translation::get_lanelet_by_id( const int lanelet_id)  const {
        return map_ptr->laneletLayer.get(lanelet_id);
    }
    }

   
}
