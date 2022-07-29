#include "tsc_service.h"

namespace traffic_signal_controller_service {
    

    bool tsc_service::initialize() {
        try
        {
            spat_worker_ptr = std::make_shared<spat_worker>();

            // Kafka config
            auto client = std::make_unique<kafka_clients::kafka_client>();
            _bootstrap_server = streets_service::streets_configuration::get_string_config("bootstrap_server");
            _spat_topic_name = streets_service::streets_configuration::get_string_config("spat_producer_topic");
            _spat_group_id = streets_service::streets_configuration::get_string_config("spat_group_id");
            spat_producer = client->create_producer(_bootstrap_server, _spat_topic_name, _spat_group_id);
        

            if (!spat_producer->init())
            {
                SPDLOG_CRITICAL("Kafka spat producer initialize error");
                exit(EXIT_FAILURE);
            }
            else
            {
                _spat_consumer->subscribe();
                if (!_spat_consumer->is_running() )
                {
                    SPDLOG_CRITICAL("Kafka spat producer is not running!");
                    exit(EXIT_FAILURE);
                }
            }

            // Serice config
            std::string socket_ip = streets_service::streets_configuration::get_string_config("udp_socket_ip");
            int socket_port = streets_service::streets_configuration::get_int_config("udp_socket_port");
            int socket_timeout = streets_service::streets_configuration::get_int_config("socket_timeout");            

            spat_worker_ptr = std::make_shared<spat_worker>( new spat_worker( socket_ip, socket_port, socket_timeout ));
            // HTTP request to update intersection information
            if (!spat_worker_ptr->initialize())
            {
                SPDLOG_ERROR("Failed to initialize spat worker!");
                return false;
            }
            SPDLOG_INFO("Traffic Signal Controller Service initialized successfully!");
            return true;
        }
        catch (const streets_service::streets_configuration_exception &ex)
        {
            SPDLOG_ERROR("Signal Optimization Service Initialization failure: {0} ", ex.what());
            return false;
        }
    }
}