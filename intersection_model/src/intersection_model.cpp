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

    lanelet::Lanelet intersection_model::get_lanelet( double lat, double lon ) {
        // sudo code:
        //  -> Create GPSPoint
        //  -> Use projector->forward() to translate to BasicPoint3D
        //  -> search map for nearest lanelet for BasicPoint3D
        //  -> return lanelet
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
                            this->int_info.entering_lanelets.push_back(lanelet_info_entry);

                            update_link_departure_lanelets_by_entry_lanelet(entry_lanelet);                           
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

    bool intersection_model::update_link_departure_lanelets_by_entry_lanelet(const lanelet::Lanelet &entry_lanelet)
    {    
        //making sure the vehicle routing graph is not null
        if( !this->vehicleGraph_ptr )
        { 
            if( !update_vehicle_routing_graph() ) 
            { 
                return false;
            }
        }

        //Retrieve all the possible link lanelets for the entry lane
        lanelet::ConstLanelets link_lanelets_per_entering_lanelet = this->vehicleGraph_ptr->following(entry_lanelet, false);

        //Every entry lanelet at the intersection has tt most three link lanelets
        for (auto &link_lanelet : link_lanelets_per_entering_lanelet)
        {
            lanelet_info_t lanelet_info_link;
            lanelet_info_link.id = link_lanelet.id();
            lanelet_info_link.speed_limit = get_speed_limit_by_lanelet(link_lanelet);
            lanelet_info_link.length      = lanelet::geometry::length3d(link_lanelet);
            this->int_info.link_lanelets.push_back(lanelet_info_link);

            //Only one departure lanelet per link lanelet
            lanelet_info_t lanelet_info_depart;
            lanelet::ConstLanelet depart_lanelet = this->vehicleGraph_ptr->following(link_lanelet, false).front();
            lanelet_info_depart.id = depart_lanelet.id();
            lanelet_info_depart.speed_limit = get_speed_limit_by_lanelet(depart_lanelet);
            lanelet_info_depart.length      = lanelet::geometry::length3d(depart_lanelet);
            this->int_info.departure_lanelets.insert(lanelet_info_depart);
        }
    }

    const intersection_info_t intersection_model::get_intersection_info() const 
    {
        return this->int_info;
    }

   const  std::vector<lanelet_info_t> intersection_model::get_entry_lanelets() const 
    {
        return this->int_info.entering_lanelets;
    }

    float intersection_model::get_speed_limit_by_lanelet(const lanelet::ConstLanelet &subj_lanelet)
    {
        float speed_limit_result = 0;
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

   const  std::vector<lanelet_info_t> intersection_model::get_link_lanelets() const 
    {
        return this->int_info.link_lanelets;
    }

    const std::vector<lanelet_info_t> intersection_model::get_conflict_lanelets(int64_t sub_link_lanelet_id)
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
            for (auto link_lanelet : this->int_info.link_lanelets)
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

        if(this->int_info.link_lanelets.size() == 0)
        {
            SPDLOG_ERROR("Intersection information contains zero number of link lanelets.");
            return is_link_lanelet_id;
        }

        for (auto link_lanelet_info : this->int_info.link_lanelets)
        {
            if (lanelet_id == link_lanelet_info.id)
            {
                is_link_lanelet_id = true;
            }
        }

        return is_link_lanelet_id;
    }

    const  std::set<lanelet_info_t> intersection_model::get_departure_lanelets() const 
    {
        return this->int_info.departure_lanelets;
    }
     
    const std::shared_ptr<lanelet::routing::RoutingGraph> intersection_model::get_vehicleGraph_ptr()  const
    {
        return this->vehicleGraph_ptr;
    }

    intersection_model::~intersection_model()
    {
        int_info.link_lanelets.clear();
        int_info.entering_lanelets.clear();
        int_info.departure_lanelets.clear();
    }
}