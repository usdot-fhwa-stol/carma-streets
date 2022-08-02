#include "tsc_service.h"

namespace traffic_signal_controller_service {
    

    bool tsc_service::initialize() {
        try
        {
            // Intialize spat kafka producer
            auto client = std::make_unique<kafka_clients::kafka_client>();
            _bootstrap_server = streets_service::streets_configuration::get_string_config("bootstrap_server");
            _spat_topic_name = streets_service::streets_configuration::get_string_config("spat_producer_topic");
            spat_producer = client->create_producer(_bootstrap_server, _spat_topic_name);
            if (!spat_producer->init())
            {
                SPDLOG_CRITICAL("Kafka spat producer initialize error");
                return false;
            }
            SPDLOG_DEBUG("Initialized SPAT Kafka producer!");
            
            // Initialize SNMP Client
            std::string target_ip = streets_service::streets_configuration::get_string_config("target_ip");
            int target_port = streets_service::streets_configuration::get_int_config("target_port");
            std::string community = streets_service::streets_configuration::get_string_config("community");
            int snmp_version = streets_service::streets_configuration::get_int_config("snmp_version");
            int timeout = streets_service::streets_configuration::get_int_config("timeout");
            snmp_client_ptr = std::make_shared<snmp_client>(target_ip, target_port, community, snmp_version, timeout);
            SPDLOG_DEBUG("SNMP Client initialized!");
            //Initialize TSC State
            tsc_state_ptr = std::make_shared<tsc_state>(snmp_client_ptr );
            if ( !tsc_state_ptr->initialize() ){
                SPDLOG_ERROR("Failed to initialize tsc_state!");
                return false;
            }
            SPDLOG_DEBUG("TSC State initialized!");
            // Enable SPaT 
            snmp_response_obj enable_spat;
            enable_spat.type = snmp_response_obj::response_type::INTEGER;
            enable_spat.val_int = 2;
            if (!snmp_client_ptr->process_snmp_request(ntcip_oids::ENABLE_SPAT_OID, request_type::SET, enable_spat)){
                SPDLOG_ERROR("Failed to enable SPaT broadcasting on Traffic Signal Controller!");
                return false;
            }


            // Initialize spat_worker
            std::string socket_ip = streets_service::streets_configuration::get_string_config("udp_socket_ip");
            int socket_port = streets_service::streets_configuration::get_int_config("udp_socket_port");
            int socket_timeout = streets_service::streets_configuration::get_int_config("socket_timeout");            

            spat_worker_ptr = std::make_shared<spat_worker>(socket_ip, socket_port, socket_timeout);
            // HTTP request to update intersection information
            if (!spat_worker_ptr->initialize())
            {
                SPDLOG_ERROR("Failed to initialize spat worker!");
                return false;
            }

            intersection_client_ptr = std::make_shared<intersection_client>();
            if ( !intersection_client_ptr->request_intersection_info() ) {
                SPDLOG_ERROR("Failed to retrieve intersection information from intersection model!");
                return false;
            }
            spat_ptr =  std::make_shared<signal_phase_and_timing::spat>();
            spat_ptr->initialize_intersection( 
                    intersection_client_ptr->get_intersection_name(), 
                    intersection_client_ptr->get_intersection_id(), 
                    tsc_state_ptr->get_phase_num_map());

            SPDLOG_INFO("Traffic Signal Controller Service initialized successfully!");
            return true;
        }
        catch (const streets_service::streets_configuration_exception &ex)
        {
            SPDLOG_ERROR("Signal Optimization Service Initialization failure: {0} ", ex.what());
            return false;
        }
    }

    void tsc_service::produce_spat_json() const {
        try {
            while(true) {
                spat_worker_ptr->receive_spat(spat_ptr);
                spat_producer->send(spat_ptr->toJson());
            }
        }
        catch( const udp_socket_listener_exception &e) {
            SPDLOG_ERROR("Encounted exception : \n {0}", e.what());
        }
        catch( const signal_phase_and_timing::signal_phase_and_timing_exception &e ) {
            SPDLOG_ERROR("Encounted exception : \n {0}", e.what());
        }
    }

    void tsc_service::start() {
        std::thread spat_t(&tsc_service::produce_spat_json, this);
        spat_t.join();
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