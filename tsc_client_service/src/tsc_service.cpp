#include "tsc_service.h"

namespace traffic_signal_controller_service {
    
    std::mutex dpp_mtx;

    bool tsc_service::initialize() {
        try
        {
            // Intialize spat kafka producer
            std::string bootstrap_server = streets_service::streets_configuration::get_string_config("bootstrap_server");
            std::string spat_topic_name = streets_service::streets_configuration::get_string_config("spat_producer_topic");
            std::string dpp_consumer_topic = streets_service::streets_configuration::get_string_config("desired_phase_plan_consumer_topic");
            std::string dpp_consumer_group = streets_service::streets_configuration::get_string_config("desired_phase_plan_consumer_group");
            if (!initialize_kafka_producer(bootstrap_server, spat_topic_name)) {
                return false;
            }

            if (!initialize_kafka_consumer(bootstrap_server, dpp_consumer_topic, dpp_consumer_group)) {
                return false;
            }            
            // Initialize SNMP Client
            std::string target_ip = streets_service::streets_configuration::get_string_config("target_ip");
            int target_port = streets_service::streets_configuration::get_int_config("target_port");
            std::string community = streets_service::streets_configuration::get_string_config("community");
            int snmp_version = streets_service::streets_configuration::get_int_config("snmp_version");
            int timeout = streets_service::streets_configuration::get_int_config("timeout");
            if (!initialize_snmp_client(target_ip, target_port, community, snmp_version, timeout)) {
                return false;
            }
            //Initialize TSC State
            use_tsc_state_spat_update_ = streets_service::streets_configuration::get_boolean_config("use_tsc_state_spat_update");
            use_desired_phase_plan_update_ = streets_service::streets_configuration::get_boolean_config("use_desired_phase_plan_update");            
            if (!initialize_tsc_state(snmp_client_ptr)){
                return false;
            }
            // Enable SPaT
            if (!enable_spat()) {
                return false;
            }
            // Initialize spat_worker
            std::string socket_ip = streets_service::streets_configuration::get_string_config("udp_socket_ip");
            int socket_port = streets_service::streets_configuration::get_int_config("udp_socket_port");
            int socket_timeout = streets_service::streets_configuration::get_int_config("socket_timeout");
            bool use_msg_timestamp =  streets_service::streets_configuration::get_boolean_config("use_tsc_timestamp");         
            if (!initialize_spat_worker(socket_ip, socket_port, socket_timeout, use_msg_timestamp)) {
                return false;
            }
            if (!initialize_intersection_client()) {
                return false;
            }
            // Add all phases to a single map
            auto all_phases = tsc_state_ptr->get_vehicle_phase_map();
            auto ped_phases = tsc_state_ptr->get_ped_phase_map();
            // Insert pedestrian phases into map of vehicle phases.
            all_phases.insert(ped_phases.begin(), ped_phases.end());
            initialize_spat(intersection_client_ptr->get_intersection_name(), intersection_client_ptr->get_intersection_id(), 
                                all_phases);
            
            // Initialize spat ptr
            SPDLOG_INFO("Traffic Signal Controller Service initialized successfully!");
            return true;
        }
        catch (const streets_service::streets_configuration_exception &ex)
        {
            SPDLOG_ERROR("Signal Optimization Service Initialization failure: {0} ", ex.what());
            return false;
        }
    }

    bool tsc_service::initialize_kafka_producer(const std::string &bootstrap_server, const std::string &spat_producer_topic) {
        auto client = std::make_unique<kafka_clients::kafka_client>();
        spat_producer = client->create_producer(bootstrap_server, spat_producer_topic);
        if (!spat_producer->init())
        {
            SPDLOG_CRITICAL("Kafka spat producer initialize error");
            return false;
        }
        SPDLOG_DEBUG("Initialized SPAT Kafka producer!");
        return true;
    }

    bool tsc_service::initialize_kafka_consumer(const std::string &bootstrap_server, const std::string &desired_phase_plan_consumer_topic,  std::string &consumer_group) {
        auto client = std::make_unique<kafka_clients::kafka_client>();
        desired_phase_plan_consumer = client->create_consumer(bootstrap_server, desired_phase_plan_consumer_topic, consumer_group);
        if (!desired_phase_plan_consumer->init())
        {
            SPDLOG_CRITICAL("Kafka desired phase plan initialize error");
            return false;
        }
        SPDLOG_DEBUG("Initialized desired phase plan Kafka consumer!");
        return true;
    }

    bool tsc_service::initialize_snmp_client(const std::string &server_ip, const int server_port, const std::string &community,
                                        const int snmp_version, const int timeout) {
        try {
            snmp_client_ptr = std::make_shared<snmp_client>(server_ip, server_port, community, snmp_version, timeout);
            SPDLOG_DEBUG("SNMP Client initialized!");
            return true;
        }
        catch (const snmp_client_exception &e) {
            SPDLOG_ERROR("Exception encountered initializing snmp client : \n {0}", e.what());
            return false;
        }
    }

    bool tsc_service::initialize_tsc_state(const std::shared_ptr<snmp_client> _snmp_client_ptr ) {
        tsc_state_ptr = std::make_shared<tsc_state>(_snmp_client_ptr );
        if ( !tsc_state_ptr->initialize() ){
            SPDLOG_ERROR("Failed to initialize tsc_state!");
            return false;
        }
        SPDLOG_DEBUG("TSC State initialized!");
        return true;
    }
    bool tsc_service::enable_spat() const{
        // Enable SPaT 
        snmp_response_obj enable_spat;
        enable_spat.type = snmp_response_obj::response_type::INTEGER;
        enable_spat.val_int = 2;
        if (!snmp_client_ptr->process_snmp_request(ntcip_oids::ENABLE_SPAT_OID, request_type::SET, enable_spat)){
            SPDLOG_ERROR("Failed to enable SPaT broadcasting on Traffic Signal Controller!");
            return false;
        }
        SPDLOG_DEBUG("Enabled UDP broadcasting of NTCIP SPaT data from TSC!");
        return true;
    }

    bool tsc_service::initialize_spat_worker(const std::string &udp_socket_ip, const int udp_socket_port, 
                                        const int timeout, const bool use_tsc_timestamp) {
        spat_worker_ptr = std::make_shared<spat_worker>(udp_socket_ip, udp_socket_port, timeout, use_tsc_timestamp);
        // HTTP request to update intersection information
        if (!spat_worker_ptr->initialize())
        {
            SPDLOG_ERROR("Failed to initialize spat worker!");
            return false;
        }
        SPDLOG_DEBUG("Spat worker initialized successfully!");
        return true;
    }

    bool tsc_service::initialize_intersection_client() {
        intersection_client_ptr = std::make_shared<intersection_client>();
        if ( !intersection_client_ptr->request_intersection_info() ) {
            SPDLOG_ERROR("Failed to retrieve intersection information from intersection model!");
            return false;
        }
        SPDLOG_DEBUG("Intersection client initialized successfully!");
        return true;
    }
    
    void tsc_service::initialize_spat(const std::string &intersection_name, const int intersection_id, 
                                const std::unordered_map<int,int> &phase_number_to_signal_group) {
        spat_ptr =  std::make_shared<signal_phase_and_timing::spat>();
        spat_ptr->initialize_intersection( intersection_name, intersection_id, phase_number_to_signal_group);
    }


    void tsc_service::produce_spat_json() const {
        try {
            while(true) {
                try {
                    spat_worker_ptr->receive_spat(spat_ptr);
                    
                    if(use_tsc_state_spat_update_){
                        try{
                            tsc_state_ptr->add_future_movement_events(spat_ptr);
                        }
                        catch(const traffic_signal_controller_service::monitor_states_exception &e){
                            SPDLOG_ERROR("Could not update movement events, spat not published. Encounted exception : \n {0}", e.what());
                        }
                    }else if(use_desired_phase_plan_update_){
                        try {
                            std::scoped_lock<std::mutex> lck{dpp_mtx};
                            monitor_dpp_ptr->update_spat_future_movement_events(spat_ptr, tsc_state_ptr);
                        }
                        catch( const traffic_signal_controller_service::monitor_desired_phase_plan_exception &e) {
                            SPDLOG_ERROR("Could not update movement events, spat not published. Encounted exception : \n {0}", e.what());
                        }
                    }
                    
                    spat_producer->send(spat_ptr->toJson());
                    
                }
                catch( const signal_phase_and_timing::signal_phase_and_timing_exception &e ) {
                    SPDLOG_ERROR("Encounted exception : \n {0}", e.what());
                }   
            }
        }
        catch( const udp_socket_listener_exception &e) {
            SPDLOG_ERROR("Encounted exception : \n {0}", e.what());
        }
        
    }

    void tsc_service::consume_desired_phase_plan() const {
       while (desired_phase_plan_consumer->is_running())
        {
            const std::string payload = desired_phase_plan_consumer->consume(1000);
            if (payload.length() != 0)
            {
                SPDLOG_DEBUG("Consumed: {0}", payload);
                std::scoped_lock<std::mutex> lck{dpp_mtx};
                monitor_dpp_ptr->update_desired_phase_plan(payload);
            }
        }        
    }
    

    void tsc_service::start() {
        std::thread spat_t(&tsc_service::produce_spat_json, this);
        std::thread desired_phase_plan_t(&tsc_service::consume_desired_phase_plan, this);
        spat_t.join();
        desired_phase_plan_t.join();
    }

    tsc_service::~tsc_service()
    {
        if (spat_producer)
        {
            SPDLOG_WARN("Stopping spat producer!");
            spat_producer->stop();
        }
    }
}