#include "intersection_model.h"

namespace intersection_model
{

    intersection_model::intersection_model(const std::string &intersection_name, const int intersection_id, const std::string &osm_file_path ){
        initialization(intersection_name, intersection_id, osm_file_path);
    }

    void intersection_model::initialization(const std::string &intersection_name, const int intersection_id,const std::string &osm_file_path)
    {
        
        int_info.intersection_name = intersection_name;
        int_info.intersection_id = intersection_id; 
        read_lanelet2_map(osm_file_path);
        if (map && !map->empty())
        {
            SPDLOG_INFO("Map is loaded ...");

            if(update_intersection_info())
            {
                SPDLOG_INFO("Intersection information is initialized ...");
            }
            else
            {
                SPDLOG_INFO("Cannot initialize intersection information.");
            }
        }
        else
        {
            SPDLOG_ERROR("Loading Map Failed.");
        }

        SPDLOG_INFO("Initialization Done!");  
    }


    void intersection_model::read_lanelet2_map( std::string filename ) 
    {
        try
        {
            int projector_type = 1;
            std::string target_frame;
            lanelet::ErrorMessages errors;

            // Parse geo reference info from the lanelet map (.osm)
            lanelet::io_handlers::AutowareOsmParser::parseMapParams(filename, &projector_type, &target_frame);
            projector = new lanelet::projection::LocalFrameProjector(target_frame.c_str());
            map = lanelet::load(filename, *projector, &errors);
        }
        catch(const lanelet::LaneletError &ex)
        {         
            SPDLOG_ERROR("Cannot read osm file {0}. Error message: {1} ", filename, ex.what());
            exit(1);
        }
    }

    lanelet::LaneletMapPtr intersection_model::get_lanelet2_map() {
        return map;
    }

    lanelet::Lanelet intersection_model::get_lanelet( double lat, double lon ) 
    {
        SPDLOG_DEBUG("Getting lanelet for ({0},{1}", lat, lon);
        lanelet::GPSPoint gps;
        gps.ele = 0;
        gps.lat = lat;
        gps.lon = lon;
        lanelet::BasicPoint3d point = projector->forward(gps);

        lanelet::BasicPoint2d point2d = lanelet::utils::to2D(point );
        
        std::vector<lanelet::Lanelet> lanelets = map->laneletLayer.nearest(point2d,1);
        return lanelets.front();
    }

    bool intersection_model::update_vehicle_routing_graph()
    {
        //get routingGraph from map
        lanelet::traffic_rules::TrafficRulesPtr trafficRules{
            lanelet::traffic_rules::TrafficRulesFactory::create(lanelet::Locations::Germany,
                                                        lanelet::Participants::Vehicle,
                                                        lanelet::traffic_rules::TrafficRules::Configuration())};
        lanelet::routing::RoutingCostPtrs costPtrs{
            std::make_shared<lanelet::routing::RoutingCostDistance>(this->laneChangeCost, this->minLaneChangeLength),
            std::make_shared<lanelet::routing::RoutingCostTravelTime>(this->laneChangeCost)};
        lanelet::routing::RoutingGraph::Configuration configuration;
        configuration.insert(std::make_pair(lanelet::routing::RoutingGraph::ParticipantHeight, this->participantHeight));
        this->vehicleGraph_ptr = lanelet::routing::RoutingGraph::build(*this->map, *trafficRules, costPtrs, configuration);
        if ( !this->vehicleGraph_ptr )
        {
            return false;
        }
        return true;
    }

    bool intersection_model::update_intersection_info()
    {
        try
        {         
            //Find the all_way_stop regulatory element
            lanelet::RegulatoryElementLayer &reg_layer = this->map->regulatoryElementLayer;
            for (auto reg_element_itr = reg_layer.begin(); reg_element_itr != reg_layer.end(); reg_element_itr++)
            {
                lanelet::RegulatoryElement *reg_element_ptr = reg_element_itr->get();

                if (reg_element_ptr->attribute(lanelet::AttributeName::Subtype) == lanelet::AttributeValueString::AllWayStop)
                {
                    if (!update_intersection_info_by_all_way_stop_reg(reg_element_ptr))
                    {
                         SPDLOG_CRITICAL("Failed to load intersection information based on all_way_stop regulatory element.");
                    };
                }
            }
        }
        catch (const lanelet::LaneletError &ex)
        {
            SPDLOG_ERROR("Failed to call get_routing_graph() : {0}", ex.what());
            return false;
        }

        return true;
    }

    bool intersection_model::update_intersection_info_by_all_way_stop_reg(const lanelet::RegulatoryElement *all_way_stop_reg_elem_ptr)
    {
        try
        {
            //get all entering lanelet based on all_way_stop regulatory elements
            lanelet::RegulatoryElementDataConstPtr coreData = all_way_stop_reg_elem_ptr->constData();
            lanelet::RuleParameterMap rule_map = coreData->parameters;
            lanelet::RegulatoryElement::iterator rule_map_itr = rule_map.begin();

            while (rule_map_itr != rule_map.end())
            {

                //Find all entry lanelets participating in the 4-way stop by role='yield'
                if (rule_map_itr->first == lanelet::RoleNameString::Yield)
                {
                    lanelet::RuleParameters::iterator rule_params_itr = rule_map_itr->second.begin();
                    while (rule_params_itr != rule_map_itr->second.end())
                    {
                        auto lanelet_apply_rule = boost::get<lanelet::WeakLanelet>(*rule_params_itr);
                        if (!lanelet_apply_rule.expired())
                        {
                            auto entry_lanelet = lanelet_apply_rule.lock();
                            lanelet_info_t lanelet_info_entry;
                            lanelet_info_entry.id           = entry_lanelet.id();
                            lanelet_info_entry.speed_limit  = get_speed_limit_by_lanelet(entry_lanelet);
                            lanelet_info_entry.length       = lanelet::geometry::length3d(entry_lanelet);
                            this->int_info.entering_lanelets_info.push_back(lanelet_info_entry);

                            update_link_departure_lanelets_info_by_entry_lanelet(entry_lanelet);
                            entering_lanelets.push_back(entry_lanelet);                           
                        }
                        rule_params_itr++;
                    }
                }
                rule_map_itr++;
            }
        }
        catch (const lanelet::LaneletError &ex)
        {
            SPDLOG_ERROR("Failed to call load intersection information: {0}", ex.what());
            return false;
        }
        return true;
    }

    bool intersection_model::update_link_departure_lanelets_info_by_entry_lanelet(const lanelet::Lanelet &entry_lanelet)
    {    
        //making sure the vehicle routing graph is not null
        if( !this->vehicleGraph_ptr )
        { 
            if( !update_vehicle_routing_graph() ) 
            { 
                return false;
            }
        }

        //Retrieve all the possible link lanelets for the entry lanelet
        lanelet::ConstLanelets link_lanelets_per_entering_lanelet = this->vehicleGraph_ptr->following(entry_lanelet, false);

        //Every entry lanelet at the intersection has tt most three link lanelets
        for (auto &link_lanelet : link_lanelets_per_entering_lanelet)
        {
            lanelet_info_t lanelet_info_link;
            lanelet_info_link.id = link_lanelet.id();
            lanelet_info_link.speed_limit = get_speed_limit_by_lanelet(link_lanelet);
            lanelet_info_link.length      = lanelet::geometry::length3d(link_lanelet);
            this->int_info.link_lanelets_info.push_back(lanelet_info_link);
            link_lanelets.push_back(link_lanelet);
            
            //Updating connecting lanelet ids for entering lanelet
            for(auto itr = this->int_info.entering_lanelets_info.begin(); itr!=this->int_info.entering_lanelets_info.end(); itr++ )
            {
                if(itr->id == entry_lanelet.id())
                {
                    itr->connecting_lanelet_ids.push_back(link_lanelet.id());
                }
            }

            //Only one departure lanelet per link lanelet
            lanelet_info_t lanelet_info_depart;
            lanelet::ConstLanelet depart_lanelet = this->vehicleGraph_ptr->following(link_lanelet, false).front();
            lanelet_info_depart.id = depart_lanelet.id();
            lanelet_info_depart.speed_limit = get_speed_limit_by_lanelet(depart_lanelet);
            lanelet_info_depart.length      = lanelet::geometry::length3d(depart_lanelet);
            this->int_info.departure_lanelets_info.insert(lanelet_info_depart);
           
            //If the departure lanelet with the same lanelet id already exist in the list, do not add this lanelet into the list
            bool is_exist = false;
            for(const auto& subj_dpl: departure_lanelets)
            {
                if(subj_dpl.id() == depart_lanelet.id())
                {
                    is_exist = true;
                }
            }

            if(!is_exist)
            {
                departure_lanelets.push_back(depart_lanelet);
            }
        }
    }

    const intersection_info_t intersection_model::get_intersection_info() const 
    {
        return this->int_info;
    }

   const  std::vector<lanelet_info_t> intersection_model::get_entry_lanelets_info() const 
    {
        return this->int_info.entering_lanelets_info;
    }

    double intersection_model::get_speed_limit_by_lanelet(const lanelet::ConstLanelet &subj_lanelet) const
    {
        double speed_limit_result = 0;
        std::regex r("[0-9]+");
        lanelet::RegulatoryElementConstPtrs reg_elements = subj_lanelet.regulatoryElements();

        /***
         * Identify the speed limit regulatory element by using an attribute with a key value that equals to 'limit'.
         * The value of this attribute stores the speed limit value in unit of measure MPH
        **/
        for (auto itr = reg_elements.begin(); itr != reg_elements.end(); itr++)
        {
            lanelet::AttributeMap attr_map = itr->get()->attributes();
            for (auto inner_itr = attr_map.begin(); inner_itr != attr_map.end(); inner_itr++)
            {
                if (inner_itr->first == lanelet::DigitalSpeedLimit::Limit)
                {
                    std::string speed_str = inner_itr->second.value();
                    if (std::regex_search(speed_str, r))
                    {
                        speed_limit_result = std::stoi(std::regex_replace(speed_str, std::regex("\\s"), std::string("")));
                        speed_limit_result *= MPH_TO_MS; //MPH to M/S conversion
                    };
                }
            }
        }
        return speed_limit_result;
    }

   const  std::vector<lanelet_info_t> intersection_model::get_link_lanelets_info() const 
    {
        return this->int_info.link_lanelets_info;
    }

    const std::vector<lanelet_info_t> intersection_model::get_conflict_lanelets_info(int64_t sub_link_lanelet_id)
    {       
        //Check the subject link_lanelet conflict by comparing it to all link lanelets at the intersection
        lanelet::Id subject_link_lanelet_id{sub_link_lanelet_id};
        lanelet::Points2d intersectionPts;
        std::vector<lanelet_info_t> conflict_lanelets;

        if ( !is_valid_link_lanelet_id(sub_link_lanelet_id) )
        {
            SPDLOG_ERROR("Not a valid input link lanelet id {0}.", sub_link_lanelet_id);
            return conflict_lanelets;
        }

        //Get the vector of link lanelets at the intersection that share the same entry lanelet. 
        //The vector of link lanelets can be skipped.
        std::vector<int> lanelet_ids_to_skip = get_shared_entry_link_lanelet_ids(sub_link_lanelet_id);

        //get lanelets from map
        lanelet::LaneletLayer &lanelets = this->map->laneletLayer;
        try
        {
            //comparing centerline between subject link lanelet and all other link lanelets at the intersection
            lanelet::ConstLineString2d subject_centerline_b = lanelets.get(subject_link_lanelet_id).centerline2d();
            for (const auto& link_lanelet : this->int_info.link_lanelets_info)
            {
                if (std::find(lanelet_ids_to_skip.begin(), lanelet_ids_to_skip.end(), link_lanelet.id) != lanelet_ids_to_skip.end())
                {
                    continue;
                }
                lanelet::ConstLineString2d centerline_link_lanelet_b = lanelets.get(link_lanelet.id).centerline2d();
                lanelet::ConstHybridLineString2d c_sub(centerline_link_lanelet_b);
                lanelet::ConstHybridLineString2d c(subject_centerline_b);

                boost::geometry::intersection(c, c_sub, intersectionPts);
                if (intersectionPts.size() > 0)
                {
                    conflict_lanelets.push_back(link_lanelet);
                }
                intersectionPts.clear();
            }
        }
        catch (const lanelet::LaneletError &ex)
        {
            SPDLOG_ERROR("Failed to lookup element with id {0} : {1} " , ex.what() ,sub_link_lanelet_id);
            conflict_lanelets.clear();
        }
        return conflict_lanelets;
    }

    std::vector<int> intersection_model::get_shared_entry_link_lanelet_ids(int64_t subject_link_lanelet_id)
    {
        std::vector<int> lanelet_ids;

        //If routing graph is not initialized, it will return zero number of lanelet ids.
        if( !this->vehicleGraph_ptr )
        {
            if( !update_vehicle_routing_graph() )
            {
                return lanelet_ids;
            };
        }

        lanelet::ConstLanelet  lanelet_entry = this->vehicleGraph_ptr->previous(this->map->laneletLayer.get(subject_link_lanelet_id)).front();
        lanelet::ConstLanelets shared_entry_link_lanelets = vehicleGraph_ptr->following(lanelet_entry);
        
        for (auto ele_ptr = shared_entry_link_lanelets.begin(); ele_ptr != shared_entry_link_lanelets.end(); ele_ptr++)
        {
            lanelet_ids.push_back(ele_ptr->id());
        }
        return lanelet_ids;
    }

    bool intersection_model::is_valid_link_lanelet_id(int64_t lanelet_id)
    {
        bool is_link_lanelet_id = false;

        if(this->int_info.link_lanelets_info.size() == 0)
        {
            SPDLOG_ERROR("Intersection information contains zero number of link lanelets.");
            return is_link_lanelet_id;
        }

        for (const auto& link_lanelet_info : this->int_info.link_lanelets_info)
        {
            if (lanelet_id == link_lanelet_info.id)
            {
                is_link_lanelet_id = true;
            }
        }

        return is_link_lanelet_id;
    }

    const  std::set<lanelet_info_t> intersection_model::get_departure_lanelets_info() const 
    {
        return this->int_info.departure_lanelets_info;
    }
     
    const std::shared_ptr<lanelet::routing::RoutingGraph> intersection_model::get_vehicleGraph_ptr()  const
    {
        return this->vehicleGraph_ptr;
    }

    bool intersection_model::update_intersecion_info_by_map_msg(const std::shared_ptr<intersection_map> int_map_msg)
    {
        bool is_updated = false;
        SPDLOG_INFO("Intersection id {0} has {1} number of geometries.", int_map_msg->intersectionid, int_map_msg->geometries.size());
        if(int_map_msg->geometries.size() == 0)
        {
            return false;
        }  
        // Assuming the map msg only has one geometry object
        const auto& map_msg_geometry = int_map_msg->geometries.front();
        std::unordered_map<long, lanelet::ConstLanelet> entry_lane2lanelet_m;
        std::unordered_map<long, std::vector<map_connection>> entry_lane2connections_m;
        std::unordered_map<long, map_lane> link_departure_lanes_m; 
        for(const auto& map_msg_lane: map_msg_geometry.approach.lanes)
        {    
            // Lane geometry has to have at least 2 points to form a path/line
            if(map_msg_lane.nodes.size() < 2)
            {
                SPDLOG_CRITICAL("Skip processing MAP message lane as lane id: {0} has less than 2 nodes. Connection size = {1}. Node size = {2}.", map_msg_lane.lane_id, map_msg_lane.connection.size(), map_msg_lane.nodes.size());
                continue;
            }
            //Entry lane includes connection in MAP message. 
            if(map_msg_lane.connection.size() != 0 )
            {
                //Mapping MAP message lane id to lanelet id from OSM map
                mapping_lanelet_id_2_lane_id(map_msg_geometry.refpoint, map_msg_lane, this->entering_lanelets, entry_lane2lanelet_m);
                entry_lane2connections_m.insert({map_msg_lane.lane_id,  map_msg_lane.connection});
                continue;
            }
            link_departure_lanes_m.insert({map_msg_lane.lane_id, map_msg_lane});
        }     
        
        //Matching enter lanelet, departure lanelet to signal group id
        std::vector<signalized_intersection_lanelets> enter_departure_lanelets2SG_id_v;    
        for (const auto& lane: entry_lane2connections_m)
        {
            long lane_id = lane.first;
            const auto& entry_lanelet = entry_lane2lanelet_m[lane_id];
            //Retrieve all connections for the entry lane
            const auto& connections = entry_lane2connections_m[lane.first];
            for(const auto& conn : connections)
            {
                //Find each departure lane for the entry lanelet using connections, and mapping the departure lane id to the above departure lanelet id
                const auto&  depart_lane = link_departure_lanes_m[conn.lane_id];
                std::unordered_map<long, lanelet::ConstLanelet> depart_lane2lanelet_m;                
                mapping_lanelet_id_2_lane_id(map_msg_geometry.refpoint, depart_lane, this->departure_lanelets ,depart_lane2lanelet_m); 
                const auto&  depart_lanelet = depart_lane2lanelet_m[depart_lane.lane_id];
                int32_t signal_group_id = conn.signalGroup;

                signalized_intersection_lanelets temp_sil; 
                temp_sil.enter_lanelet_id = entry_lanelet.id();
                temp_sil.depart_lanelet_id = depart_lanelet.id();
                temp_sil.signal_group_id = signal_group_id;
                enter_departure_lanelets2SG_id_v.push_back(temp_sil);
                SPDLOG_DEBUG("Enter_lanelet_id = {0}, depart_lanelet_id = {1}, signal group_id = {2}.", temp_sil.enter_lanelet_id, temp_sil.depart_lanelet_id, temp_sil.signal_group_id);
            }
        }

        //Matching intersection link lanelet_id with signal group id
        std::unordered_map<lanelet::Id, int32_t> link_lanelet2signal_group_id_m;
        for(auto  itr = enter_departure_lanelets2SG_id_v.begin(); itr != enter_departure_lanelets2SG_id_v.end(); itr++)
        {
            const auto&  link_lanelet_id = find_link_lanelet_id_by_enter_depart_lanelet_ids(itr->enter_lanelet_id, itr->depart_lanelet_id);    
            if(link_lanelet_id != lanelet::InvalId)
            {
                itr->link_lanelet_id = link_lanelet_id;
                link_lanelet2signal_group_id_m.insert({link_lanelet_id, itr->signal_group_id});
            }
            else
            {
                continue;
            }
        }     

        //Update intersection link lanelet info with signal group id
        bool is_signal_group_updated = false;        
        for(auto itr = this->int_info.link_lanelets_info.begin(); itr != this->int_info.link_lanelets_info.end(); itr++)
        {
            auto  find_itr = link_lanelet2signal_group_id_m.find(itr->id);
            if(find_itr != link_lanelet2signal_group_id_m.end())
            {
                itr->signal_group_id = link_lanelet2signal_group_id_m[itr->id];
                is_signal_group_updated = true; 
            }
        }
        is_signal_group_updated ? SPDLOG_INFO("Intersection info is updated with signal group ids.") : SPDLOG_INFO("Intersection_map message is processed, but intersection information  signal group ids are not updated.");
        is_updated = is_updated || is_signal_group_updated; 
        return is_updated;
    }

    lanelet::Id intersection_model::find_link_lanelet_id_by_enter_depart_lanelet_ids(const lanelet::Id enter_lanelet_id, const lanelet::Id depart_lanelet_id ) const
    {
        const auto&  following_enter_lanelets = this->vehicleGraph_ptr->following(get_enter_lanelet_by_id(enter_lanelet_id), true);
        const auto&  prev_depart_lanelets =  this->vehicleGraph_ptr->previous(get_departure_lanelet_by_id(depart_lanelet_id), true);
        std::vector<lanelet::Id> fel_ids;
        std::vector<lanelet::Id> pdl_ids;
        std::vector<lanelet::Id> link_lanelet_id_intersect_v;
        for(const auto&  temp_lanelet: following_enter_lanelets)
        {
            fel_ids.push_back(temp_lanelet.id());
        }
        std::sort(fel_ids.begin(), fel_ids.end());

        for(const auto&  temp_lanelet: prev_depart_lanelets)
        {
            pdl_ids.push_back(temp_lanelet.id());
        }
        std::sort(pdl_ids.begin(), pdl_ids.end());
        std::set_intersection(fel_ids.begin(), fel_ids.end(), pdl_ids.begin(), pdl_ids.end(), std::back_inserter(link_lanelet_id_intersect_v));
        if(link_lanelet_id_intersect_v.size() > 0)
        {
            SPDLOG_DEBUG("Found matching link lanelet id = {0} for enter lanelet id = {1} and departure lanelet id = {2}", link_lanelet_id_intersect_v.front(),enter_lanelet_id, depart_lanelet_id);
            return link_lanelet_id_intersect_v.front();
        }  
        SPDLOG_DEBUG("NO matching link lanelet for enter lanelet id = {1} and departure lanelet id = {2}", link_lanelet_id_intersect_v.front(),enter_lanelet_id, depart_lanelet_id);
        return lanelet::InvalId;
    }

    void intersection_model::mapping_lanelet_id_2_lane_id(const map_referencepoint& ref_point, const map_lane& lane, const std::vector<lanelet::ConstLanelet>& subj_lanelets, std::unordered_map<long , lanelet::ConstLanelet>& lane2lanelet_m) const
    {    
        std::vector<lanelet::BasicPoint3d> basic_points = convert_lane_path_2_basic_points(ref_point, lane);
        std::unordered_map<lanelet::Id, double> lanelet2lane_path_distance_m;
        for(const auto&  subj_l: subj_lanelets)
        {
            double avg_distance = compute_points_2_lanelet_avg_distance(basic_points, subj_l);
            SPDLOG_DEBUG("Lanelet id {0} to lane id = {1} path points average distance {2}", subj_l.id(), lane.lane_id, avg_distance);
            lanelet2lane_path_distance_m.insert({subj_l.id(), avg_distance});   
        }
        //Find the nearest lanelet from the lane path by shortest average distance
        const auto&  min_distance_pair = std::min_element(lanelet2lane_path_distance_m.begin(), lanelet2lane_path_distance_m.end(), [](const auto& l, const auto& r){return l.second < r.second; });
        for(const auto& subj_l: subj_lanelets)
        {
            if(subj_l.id() == min_distance_pair->first)
            {
                lane2lanelet_m.insert({lane.lane_id, subj_l});     
                SPDLOG_DEBUG("min_distance_pair lanelet id = {0}, distance = {1}", min_distance_pair->first, min_distance_pair->second);   
            }
        }        
    }

    std::vector<lanelet::BasicPoint3d> intersection_model::convert_lane_path_2_basic_points(const map_referencepoint& ref_point, const map_lane& lane) const
    {
        std::vector<lanelet::BasicPoint3d> basic_point_v;
        const auto&  ref_point3d = gps_2_map_point(ref_point.latitude/10000000.0, ref_point.longitude/10000000.0, ref_point.elevation);
        double cur_x = ref_point3d.x();
        double cur_y = ref_point3d.y();
        double cur_z = ref_point3d.z();
        for(const auto&  node : lane.nodes)
        {
            cur_x += node.x/100.0;
            cur_y += node.y/100.0;

            lanelet::BasicPoint3d basic_point;
            basic_point.x() = cur_x;
            basic_point.y() = cur_y;
            basic_point.z() = cur_z;
            basic_point_v.push_back(basic_point);
        }
        return basic_point_v;
    }

    double intersection_model::compute_points_2_lanelet_avg_distance(const std::vector<lanelet::BasicPoint3d>&  basic_points, lanelet::ConstLanelet subj_lanelet) const
    {
        auto points_num = basic_points.size();
        double distance_sum = 0;
        lanelet::ConstLineString2d centerline = subj_lanelet.centerline2d();
        for(const auto&  bp3D: basic_points)
        {
            const auto&  bp2d = lanelet::utils::to2D(bp3D);
            double distance = lanelet::geometry::distance2d(bp2d, centerline);
            SPDLOG_DEBUG("Point to lanelet id {0} 2D distance {1}", subj_lanelet.id(), distance);
            distance_sum += distance;
        }
        double distance_avg = distance_sum/points_num;
        return distance_avg;
    }

    lanelet::BasicPoint3d intersection_model::gps_2_map_point(double lat, double lon, double elev ) const
    {
        lanelet::GPSPoint sub_gps;
        sub_gps.ele = elev;
        sub_gps.lat = lat;
        sub_gps.lon = lon;
        lanelet::BasicPoint3d basic_point;
        try
        {
            basic_point =  projector->forward(sub_gps); 
        }
        catch (lanelet::ForwardProjectionError &ex)
        {
            SPDLOG_ERROR("Cannot project the GPS position: Latitude: {0} , Longitude: {1}, Elevation: {2}", lat, lon, elev);
        }
        return basic_point;    
    }

    lanelet::GPSPoint intersection_model::map_point2_gps(double x, double y, double z) const
    {
        lanelet::BasicPoint3d sub_map_point;
        sub_map_point.x() = x;
        sub_map_point.y() = y;
        sub_map_point.z() = z;
        lanelet::GPSPoint gps_point;
        try
        {
            gps_point =  projector->reverse(sub_map_point); 
        }
        catch (lanelet::ReverseProjectionError &ex)
        {
            SPDLOG_ERROR("Cannot project (x, y,z) point to geolocation: ({0} ,{1}, {2}). Error {3}", x, y, z);
        }
        return gps_point;  
    }

    lanelet::ConstLanelet intersection_model::get_enter_lanelet_by_id(lanelet::Id id) const
    {
        lanelet::ConstLanelet result;
        for(const auto&  ll : this->entering_lanelets)
        {
            if(ll.id() == id)
            {
                return ll;
            }
        }
        return result;
    }

    lanelet::ConstLanelet intersection_model::get_link_lanelet_by_id(lanelet::Id id) const
    {
        lanelet::ConstLanelet result;
        for(const auto&  ll : this->link_lanelets)
        {
            if(ll.id() == id)
            {
                return ll;
            }
        }
        return result;
    }

    lanelet::ConstLanelet intersection_model::get_departure_lanelet_by_id(lanelet::Id id) const
    {
        lanelet::ConstLanelet result;
        for(const auto&  ll : this->departure_lanelets)
        {
            if(ll.id() == id)
            {
                return ll;
            }
        }
        return result;
    }

    intersection_model::~intersection_model()
    {
        int_info.link_lanelets_info.clear();
        int_info.entering_lanelets_info.clear();
        int_info.departure_lanelets_info.clear();
    }
}