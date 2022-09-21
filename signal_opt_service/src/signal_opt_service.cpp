#include "signal_opt_service.h"

namespace signal_opt_service
{
    bool signal_opt_service::initialize()
    {
        try
        {
            _so_msgs_worker_ptr = std::make_shared<signal_opt_messages_worker>();

            // Kafka config
            auto client = std::make_unique<kafka_clients::kafka_client>();
            _bootstrap_server = streets_service::streets_configuration::get_string_config("bootstrap_server");
            _spat_topic_name = streets_service::streets_configuration::get_string_config("spat_consumer_topic");
            _spat_group_id = streets_service::streets_configuration::get_string_config("spat_group_id");
            _vsi_topic_name = streets_service::streets_configuration::get_string_config("vsi_consumer_topic");
            _vsi_group_id = streets_service::streets_configuration::get_string_config("vsi_group_id");
            _tsc_config_topic_name = streets_service::streets_configuration::get_string_config("tsc_config_consumer_topic");
            _tsc_config_group_id = streets_service::streets_configuration::get_string_config("tsc_config_group_id");

            _spat_consumer = client->create_consumer(_bootstrap_server, _spat_topic_name, _spat_group_id);
            _vsi_consumer = client->create_consumer(_bootstrap_server, _vsi_topic_name, _vsi_group_id);
            _tsc_config_consumer = client->create_consumer(_bootstrap_server, _tsc_config_topic_name, _tsc_config_group_id);


            if (!_spat_consumer->init() || !_vsi_consumer->init() || !_tsc_config_consumer->init() )
            {
                SPDLOG_CRITICAL("kafka consumers ( _spat_consumer, _tsc_config_consumer  or _vsi_consumer ) initialize error");
                exit(EXIT_FAILURE);
            }
            else
            {
                _spat_consumer->subscribe();
                _vsi_consumer->subscribe();
                _tsc_config_consumer->subscribe();
                if (!_spat_consumer->is_running() || !_vsi_consumer->is_running() || !_tsc_config_consumer->is_running())
                {
                    SPDLOG_CRITICAL("kafka consumers ( _spat_consumer, _tsc_config_consumer or _vsi_consumer) is not running");
                    exit(EXIT_FAILURE);
                }
            }

            // Serice config
            auto sleep_millisecs = streets_service::streets_configuration::get_int_config("sleep_millisecs");
            auto int_client_request_attempts = streets_service::streets_configuration::get_int_config("int_client_request_attempts");            

            // HTTP request to update intersection information
            if (!update_intersection_info(sleep_millisecs, int_client_request_attempts))
            {
                SPDLOG_CRITICAL("Failed to initialize signal_opt_service due to intersection client request failure!");
                return false;
            }
            SPDLOG_INFO("signal_opt_service initialized successfully!!!");
            return true;
        }
        catch (const streets_service::streets_configuration_exception &ex)
        {
            SPDLOG_ERROR("Signal Optimization Service Initialization failure: {0} ", ex.what());
            return false;
        }
    }

    void signal_opt_service::start() const
    {
        std::thread spat_t(&signal_opt_service::consume_spat, this);
        std::thread vsi_t(&signal_opt_service::consume_vsi, this);
        std::thread tsc_config_t(&signal_opt_service::consume_tsc_config, this);
        tsc_config_t.detach();
        spat_t.detach();
        vsi_t.detach();
    }

    void signal_opt_service::consume_spat() const{
        while( _spat_consumer->is_running()) {
            const std::string payload = _spat_consumer->consume(1000); 
            if (!payload.empty()) {
                if (!_so_msgs_worker_ptr->update_spat(payload)) {
                    SPDLOG_CRITICAL("Failed to update SPaT with update {0}!", payload);
                }
            }
        }
    }

    void signal_opt_service::consume_vsi() const{
        while( _vsi_consumer->is_running()) {
            const std::string payload = _vsi_consumer->consume(1000); 
            if (!payload.empty()) {
                if (!_so_msgs_worker_ptr->update_vehicle_list(payload)) {
                    SPDLOG_CRITICAL("Failed to update Vehicle List with update {0}!", payload);
                }
            }
        }
    }

    void signal_opt_service::consume_tsc_config() const {
        while( _tsc_config_consumer->is_running()) {
            const std::string payload = _tsc_config_consumer->consume(1000); 
            if (!payload.empty()) {
                if (!_so_msgs_worker_ptr->update_tsc_config(payload)) {
                    SPDLOG_CRITICAL("Failed to update TSC Configuration with update {0}!", payload);
                }
            }
        }
    }


    bool signal_opt_service::update_intersection_info(unsigned long sleep_millisecs, unsigned long int_client_request_attempts) const
    {
        auto sleep_secs = static_cast<unsigned int>(sleep_millisecs / 1000);
        SPDLOG_INFO("Send client request to update intersection inforamtion every {0} seconds for maximum {1} times.", sleep_secs, int_client_request_attempts);
        int attempt_count = 0;
        while (attempt_count < int_client_request_attempts)
        {
            // Send HTTP request, and update intersection information. If updated successfully, it returns true and exit the while loop.
            if (_so_msgs_worker_ptr && _so_msgs_worker_ptr->request_intersection_info())
            {
                return true;
            }
            sleep(sleep_secs);
            attempt_count++;
        }
        // If failed to update the intersection information after certain numbers of attempts
        SPDLOG_ERROR("Updating Intersection information failed. ");
        return false;
    }

    signal_opt_service::~signal_opt_service()
    {
        if (_spat_consumer)
        {
            _spat_consumer->stop();
        }

        if (_vsi_consumer)
        {
            _vsi_consumer->stop();
        }
        if (_tsc_config_consumer) 
        {
            _tsc_config_consumer->stop();
        }
    }
}