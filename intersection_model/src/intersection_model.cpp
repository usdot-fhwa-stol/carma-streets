#include "intersection_model.h"


namespace intersection_model
{

    intersection_model::intersection_model()
    { 
        initialization();
    }

    intersection_model::intersection_model(std::string filename){
        read_lanelet2_map(filename);
    }

    void intersection_model::initialization()
    {
        std::string manifest_json_file_name = "../manifest.json";
        rapidjson::Document doc = read_json_file(manifest_json_file_name);

        if(doc.HasParseError())
        {
            spdlog::error("{0}: Initialization Failure! Cannot read {1} file.", __FILE__ , manifest_json_file_name);
            exit(1);
        }

        //Update the intersection name based on the value set in the manifest configuration file
        if(doc.HasMember(this->intersection_name_key))
        {
            int_info.intersection_name = doc[this->intersection_name_key].GetString();
        }
        
        //Update the intersection id based on the value set in the manifest configuration file
        if(doc.HasMember(this->intersection_id_key))
        {
           int_info.intersection_id = doc[this->intersection_id_key].GetInt();
        }

        //Read the osm file path from the manifest json configuration file
        if(doc.HasMember(this->osm_file_path_key))
        {
            std::string osm_file_path = doc[this->osm_file_path_key].GetString();

            //Read the osm file and initialize the map object.
            read_lanelet2_map(osm_file_path);
        }
        if(doc.HasMember("log_level") ) {
            std::string loglevel = doc["log_level"].GetString();
            spdlog::init_thread_pool(8192, 1);
            try {
                
                auto file_sink = std::make_shared<spdlog::sinks::daily_file_sink_mt>("/home/carma-streets/intersection_model/logs/intersection_model.log", 23, 3);
                auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
                console_sink->set_level(spdlog::level::from_str(loglevel));
                file_sink->set_level( spdlog::level::from_str(loglevel ) );

                auto logger = std::make_shared<spdlog::async_logger>("main",  spdlog::sinks_init_list({console_sink, file_sink}),spdlog::thread_pool());
                spdlog::register_logger(logger);
                spdlog::set_default_logger(logger);
                spdlog::info("Default Logger initialized!");
            }   
            catch (const spdlog::spdlog_ex& ex)
            {
                spdlog::error( "Log initialization failed: {0}!",ex.what());
            }
        }
        
        if (map && !map->empty())
        {
            spdlog::info("{0} Map is loaded ...", __FILE__);

            if(update_intersection_info())
            {
                spdlog::info("{0}: Intersection information is initialized ...", __FILE__);
            }
            else
            {
                spdlog::error("{0}:  Cannot initialize intersection information.", __FILE__);
            }
        }
        else
        {
            spdlog::error("{0}: Loading Map Failed.", __FILE__);
        }

        spdlog::info("{0}: Initialization Done!", __FILE__);  
    }

    rapidjson::Document intersection_model::read_json_file(const std::string &json_file_name) const
    {     
        rapidjson::Document doc{};
        try
        { 
            std::ifstream ifs {json_file_name};
            if(!ifs.is_open())
            {
                spdlog::error("{0}: Could not open file: {1}" , __FILE__, json_file_name.c_str());
                // exit(1);
            }
            rapidjson::IStreamWrapper isw{ifs};
            doc.ParseStream(isw);
            rapidjson::StringBuffer buffer{};
            rapidjson::Writer<rapidjson::StringBuffer>  writer {buffer};
            doc.Accept(writer);
            if(doc.HasParseError())
            {
                spdlog::error("Read JSON file error  : {0} Offset: {1} ", doc.GetParseError() ,doc.GetErrorOffset());
            }          
        }        
        catch(std::exception ex)
        {         
            spdlog::critical("Read JSON file failure: {0}", ex.what());
            // exit(1);
        }        
        return doc;
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
        catch(std::exception ex)
        {         
            spdlog::error("{0}: Cannot read osm file {1}. Error message: {2} " , __FILE__, filename,ex.what());
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
        spdlog::debug("Getting lanelet for ({0},{1}", lat, lon);
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
                         spdlog::critical("{0}: Failed to load intersection information based on all_way_stop regultory element. ",__FILE__);
                    };
                }
            }
        }
        catch (...)
        {
            spdlog::error("{0}: Failed to call get_routing_graph(). ",__FILE__);
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
        catch (...)
        {
            spdlog::error("{0}: Failed to call load intersection information. ",__FILE__);
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

    uint32_t intersection_model::get_speed_limit_by_lanelet(const lanelet::ConstLanelet &subj_lanelet)
    {
        uint32_t speed_limit_result = 0;
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
            spdlog::error("{0}: Not a valid input link lanelet id. {0}", __FILE__, sub_link_lanelet_id);
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
                    lanelet_info_t lanelet_info_conflict;
                    lanelet_info_conflict.id = link_lanelet.id;
                    conflict_lanelets.push_back(lanelet_info_conflict);
                }
                intersectionPts.clear();
            }
        }
        catch (...)
        {
            spdlog::debug("{0}: Failed to lookup element with id {1} . " ,__FILE__ ,sub_link_lanelet_id);
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
            spdlog::error("{0} Intersection information contains zero number of link lanelets." , __FILE__);
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