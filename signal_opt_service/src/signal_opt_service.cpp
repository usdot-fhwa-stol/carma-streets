#include "signal_opt_service.h"

namespace signal_opt_service
{
    bool signal_opt_service::initialize()
    {
        try
        {
            _so_msgs_worker_ptr = std::make_shared<signal_opt_messages_worker>();
            _movement_groups = std::make_shared<movement_groups>();
            intersection_info_ptr = std::make_shared<OpenAPI::OAIIntersection_info>();
            vehicle_list_ptr = std::make_shared<streets_vehicles::vehicle_list>();
            auto processor = std::make_shared<streets_vehicles::signalized_status_intent_processor>();
            processor->set_timeout(streets_service::streets_configuration::get_int_config("exp_delta"));
            vehicle_list_ptr->set_processor(processor);
            spat_ptr = std::make_shared<signal_phase_and_timing::spat>();
            tsc_configuration_ptr = std::make_shared<streets_tsc_configuration::tsc_configuration_state>();

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

    void signal_opt_service::start()
    {
        std::thread tsc_config_t(&signal_opt_service::consume_tsc_config, this, this->_tsc_config_consumer, this->tsc_configuration_ptr);
        // Block until TSC configuration information is received 
        tsc_config_t.join();
        // After TSC configuration information is received, use it to determine movemement groups
        populate_movement_groups(_movement_groups, tsc_configuration_ptr);
        // After movement groups are created, process vehicle status and intent and spat messages to
        // maintain information about current vehicle states and TSC state
        std::thread vsi_t(&signal_opt_service::consume_vsi,  this, this->_vsi_consumer, this->vehicle_list_ptr);
        std::thread spat_t(&signal_opt_service::consume_spat,this, this->_spat_consumer, this->spat_ptr);
        // Running in parallel
        spat_t.join();
        vsi_t.join();
    }

    void signal_opt_service::consume_spat(const std::shared_ptr<kafka_clients::kafka_consumer_worker> spat_consumer, 
                                            const std::shared_ptr<signal_phase_and_timing::spat> _spat_ptr ) const{
        while( spat_consumer->is_running()) {
            const std::string payload = spat_consumer->consume(1000); 
            if (!payload.empty()) {
                bool update = _so_msgs_worker_ptr->update_spat(payload,_spat_ptr);
                if ( !update )
                    SPDLOG_CRITICAL("Failed to update SPaT with update {0}!", payload);
            }
        }
    }

    void signal_opt_service::consume_vsi(const std::shared_ptr<kafka_clients::kafka_consumer_worker> vsi_consumer,
                                         const std::shared_ptr<streets_vehicles::vehicle_list> _vehicle_list_ptr) const{
        while( vsi_consumer->is_running()) {
            const std::string payload = vsi_consumer->consume(1000); 
            if (!payload.empty()) {
                bool update = _so_msgs_worker_ptr->update_vehicle_list(payload,_vehicle_list_ptr);
                if (!update)
                    SPDLOG_CRITICAL("Failed to update Vehicle List with update {0}!", payload);
            }
        }
    }

    void signal_opt_service::consume_tsc_config( const std::shared_ptr<kafka_clients::kafka_consumer_worker> tsc_config_consumer,
                                                const std::shared_ptr<streets_tsc_configuration::tsc_configuration_state> _tsc_config_ptr) const {
        while( tsc_config_consumer->is_running()) {
            const std::string payload = tsc_config_consumer->consume(1000); 
            if (!payload.empty()) {
                bool update = _so_msgs_worker_ptr->update_tsc_config(payload,_tsc_config_ptr);
                if (!update) {
                    SPDLOG_CRITICAL("Failed to update TSC Configuration with update {0}!", payload);
                }else {
                    // Stop consumer after receiving tsc_configuration information
                    tsc_config_consumer->stop();
                }
            }
        }
    }
    
    void signal_opt_service::populate_movement_groups(std::shared_ptr<movement_groups> _groups, 
                                                const std::shared_ptr<streets_tsc_configuration::tsc_configuration_state> tsc_config) const {
        if (tsc_config) {
            for (const auto &phase_config : tsc_config->tsc_config_list) {
                // If signal group has one or more concurrent phase
                if ( !phase_config.concurrent_signal_groups.empty() ) {
                    // Loop through concurrent phases and create a movement group for each pair
                    for (const auto &concur : phase_config.concurrent_signal_groups) {
                        movement_group new_group;
                        new_group.signal_groups = {phase_config.signal_group_id, concur};
                        // Initialize a flag for if this pair already exists in the movement groups in reverse.
                        bool already_added = false;
                        // loop through movement groups and set flag to true if pair already exists in reverse order.
                        for ( const auto &add_group: _groups->groups ) {
                            if ( add_group.signal_groups.second == new_group.signal_groups.first && 
                                    add_group.signal_groups.first == new_group.signal_groups.second) {
                                already_added = true;
                            }
                        }
                        // Only add a new pair if it does not already exist in movement groups in reverse.
                        if (!already_added) {
                            _groups->groups.push_back(new_group);
                        }
                    }
                }
                // If signal group does not have any concurrent phases add it as a movement group pair of between itself and 0 since
                // 0 is an invalid signal group
                else {
                    movement_group new_group;
                    // Add invalid signal group 0 as current signal group to indicate there is none.
                    new_group.signal_groups = { phase_config.signal_group_id, 0};
                    _groups->groups.push_back(new_group);
                }
            }
            SPDLOG_DEBUG("Movement Groups: ");
            for ( const auto &mg : _groups->groups) {
                SPDLOG_DEBUG("Signal Group {0} and Signal Group {1}.", mg.signal_groups.first, mg.signal_groups.second );
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
            if (_so_msgs_worker_ptr && _so_msgs_worker_ptr->request_intersection_info(intersection_info_ptr))
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