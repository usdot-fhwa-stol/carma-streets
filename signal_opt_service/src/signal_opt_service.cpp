#include "signal_opt_service.h"

namespace signal_opt_service
{
    bool signal_opt_service::initialize()
    {
        try
        {
            read_configuration_params();
            
            _so_msgs_worker_ptr = std::make_shared<signal_opt_messages_worker>();
            _so_processing_worker_ptr = std::make_shared<signal_opt_processing_worker>();

            movement_groups_ptr = std::make_shared<streets_signal_optimization::movement_groups>();
            vehicle_list_ptr = std::make_shared<streets_vehicles::vehicle_list>();
            auto processor = std::make_shared<streets_vehicles::signalized_status_intent_processor>();
            processor->set_timeout(_exp_delta);
            vehicle_list_ptr->set_processor(processor);
            intersection_info_ptr = std::make_shared<OpenAPI::OAIIntersection_info>();
            spat_ptr = std::make_shared<signal_phase_and_timing::spat>();
            tsc_configuration_ptr = std::make_shared<streets_tsc_configuration::tsc_configuration_state>();

            // Kafka config
            auto client = std::make_unique<kafka_clients::kafka_client>();
            _spat_consumer = client->create_consumer(_bootstrap_server, _spat_topic_name, _spat_group_id);
            _vsi_consumer = client->create_consumer(_bootstrap_server, _vsi_topic_name, _vsi_group_id);
            _tsc_config_consumer = client->create_consumer(_bootstrap_server, _tsc_config_topic_name, _tsc_config_group_id);
            _dpp_producer = client->create_producer(_bootstrap_server, _dpp_topic_name);


            if (!_spat_consumer->init() || !_vsi_consumer->init() || !_tsc_config_consumer->init() )
            {
                SPDLOG_CRITICAL("kafka consumers ( _spat_consumer, _tsc_config_consumer  or _vsi_consumer ) initialize error!");
                return false;
            }
            else
            {
                _spat_consumer->subscribe();
                _vsi_consumer->subscribe();
                _tsc_config_consumer->subscribe();
                if (!_spat_consumer->is_running() || !_vsi_consumer->is_running() || !_tsc_config_consumer->is_running())
                {
                    SPDLOG_CRITICAL("kafka consumers ( _spat_consumer, _tsc_config_consumer or _vsi_consumer) is not running!");
                    return false;
                }
            }

            if(!_dpp_producer->init())
            {
                SPDLOG_CRITICAL("kafka producer (_dpp_producer) initialize error!");
                return false;
            }

            // Create logger
            if ( enable_so_logging ) {
                configure_csv_logger();
                _so_processing_worker_ptr->set_enable_so_logging(enable_so_logging);
            }

            // Service config
            auto sleep_millisecs = streets_service::streets_configuration::get_int_config("sleep_millisecs");
            auto int_client_request_attempts = streets_service::streets_configuration::get_int_config("int_client_request_attempts");            
            // HTTP request to update intersection information
            if (!update_intersection_info(sleep_millisecs, int_client_request_attempts))
            {
                SPDLOG_CRITICAL("Failed to initialize signal_opt_service due to intersection client request failure!");
                return false;
            }

            //Configure signal_opt_processing_worker
            _so_processing_worker_ptr->configure_signal_opt_processing_worker(dpp_config);

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
        populate_movement_groups(movement_groups_ptr, tsc_configuration_ptr);
        // After movement groups are created, process vehicle status and intent and spat messages to
        // maintain information about current vehicle states and TSC state
        std::thread vsi_t(&signal_opt_service::consume_vsi,  this, this->_vsi_consumer, this->vehicle_list_ptr);
        std::thread spat_t(&signal_opt_service::consume_spat,this, this->_spat_consumer, this->spat_ptr);
        std::thread dpp_t(&signal_opt_service::produce_dpp, this, this->_dpp_producer,
                                                            this->intersection_info_ptr,
                                                            this->vehicle_list_ptr, 
                                                            this->spat_ptr, 
                                                            this->tsc_configuration_ptr,
                                                            this->movement_groups_ptr,
                                                            this->dpp_config,
                                                            this->so_sleep_time);
        // Running in parallel
        spat_t.join();
        vsi_t.join();
        dpp_t.join();
    }

    void signal_opt_service::consume_spat(const std::shared_ptr<kafka_clients::kafka_consumer_worker> spat_consumer, 
                                            const std::shared_ptr<signal_phase_and_timing::spat> _spat_ptr ) const{
        SPDLOG_INFO("Consuming SPat.");
        while( spat_consumer->is_running()) {
            const std::string payload = spat_consumer->consume(1000); 
            if (!payload.empty()) {
                bool update = _so_msgs_worker_ptr->update_spat(payload,_spat_ptr);
                if ( !update ){
                    SPDLOG_CRITICAL("Failed to update SPaT with update {0}!", payload);
                }
            }   
        }
    }

    void signal_opt_service::consume_vsi(const std::shared_ptr<kafka_clients::kafka_consumer_worker> vsi_consumer,
                                         const std::shared_ptr<streets_vehicles::vehicle_list> _vehicle_list_ptr) const{
        SPDLOG_INFO("Consuming VSI.");
        while( vsi_consumer->is_running()) {
            const std::string payload = vsi_consumer->consume(1000); 
            if (!payload.empty()) {
                bool update = _so_msgs_worker_ptr->update_vehicle_list(payload,_vehicle_list_ptr);
                if (!update) {
                    SPDLOG_CRITICAL("Failed to update Vehicle List with update {0}!", payload);
                }
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
                    SPDLOG_INFO("TSC Configuration sucessfully loaded : \n{0}", payload );
                    tsc_config_consumer->stop();
                    break;
                }
            }
        }
    }

    void signal_opt_service::produce_dpp(const std::shared_ptr<kafka_clients::kafka_producer_worker> dpp_producer,
                                const std::shared_ptr<OpenAPI::OAIIntersection_info> _intersection_info_ptr, 
                                const std::shared_ptr<streets_vehicles::vehicle_list> _vehicle_list_ptr, 
                                const std::shared_ptr<signal_phase_and_timing::spat> _spat_ptr, 
                                const std::shared_ptr<streets_tsc_configuration::tsc_configuration_state> _tsc_config_ptr, 
                                const std::shared_ptr<streets_signal_optimization::movement_groups> _movement_groups_ptr,
                                const streets_signal_optimization::streets_desired_phase_plan_generator_configuration &_dpp_config,
                                const int &_so_sleep_time) const
    {
        if (!_so_processing_worker_ptr->get_is_configured()) {
            _so_processing_worker_ptr->configure_signal_opt_processing_worker(_dpp_config);
        }
        
        int prev_future_move_group_count = 0;
        int current_future_move_group_count = 0;
        bool new_dpp_generated = false;

        while ( dpp_producer->is_running() ) {
            SPDLOG_INFO("Signal Optimization iteration!");
            if ( !_vehicle_list_ptr->get_vehicles().empty() ) {
                streets_desired_phase_plan::streets_desired_phase_plan spat_dpp;
                
                try
                {
                    spat_dpp = _so_processing_worker_ptr->convert_spat_to_dpp(_spat_ptr, _movement_groups_ptr);
                }
                catch(const std::runtime_error &ex)
                {
                    SPDLOG_ERROR("Encountered Exception : {0} ", ex.what());
                    break;
                }
                SPDLOG_INFO("Base DPP is {0}", spat_dpp.toJson());
                current_future_move_group_count = static_cast<int>(spat_dpp.desired_phase_plan.size());
                if (new_dpp_generated) {
                    SPDLOG_INFO("Current number of movement groups represented in SPAT : {0}!", prev_future_move_group_count);
                    if (current_future_move_group_count > prev_future_move_group_count) {
                        new_dpp_generated = false;
                    }
                    else {
                        SPDLOG_WARN("Skipping SO iteration. Previously sent DPP is not yet reflected in SPaT!");
                        continue;
                    }
                }
                auto current_timestamp = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
                auto time_to_yellow = spat_dpp.desired_phase_plan.front().end_time - current_timestamp;
                SPDLOG_INFO("Time to yellow is : {0}", time_to_yellow );
                if ( time_to_yellow <= _time_to_yellow ) {
                    SPDLOG_INFO("Checking fixed phases");
                    /**
                     * If the number of future movement groups in the spat has changed compared to the previous step
                     * and it is less than or equal to the desired numNo vehicles presenter of future movement groups, run streets_signal_optimization
                     * libraries to get optimal desired_phase_plan.
                    */
                    if (current_future_move_group_count <= dpp_config.desired_future_move_group_count ) {
                        SPDLOG_INFO("The number of future movement groups in the spat is updated from {0} to {1}.", 
                                                                    prev_future_move_group_count, current_future_move_group_count);
                        // Current movement group count includes current fix momvement group.
                        // Desired future movemement group count only includes future movement groups
                        streets_desired_phase_plan::streets_desired_phase_plan optimal_dpp = 
                                    _so_processing_worker_ptr->select_optimal_dpp(_intersection_info_ptr, 
                                                                                _spat_ptr, 
                                                                                _tsc_config_ptr, 
                                                                                _vehicle_list_ptr, 
                                                                                _movement_groups_ptr, 
                                                                                _dpp_config);
                        if (!optimal_dpp.desired_phase_plan.empty()) {
                            std::string msg_to_send = optimal_dpp.toJson();
                            /* produce the optimal desired phase plan to kafka */
                            dpp_producer->send(msg_to_send);
                            prev_future_move_group_count = current_future_move_group_count;
                            new_dpp_generated = true;
                        }
                        
                    }
                    else {
                        SPDLOG_INFO("The number of future movement groups in the spat did not change from the previous check.");
                    }
                }
                else {
                    SPDLOG_WARN("Currently {0} ms to yellow! Waiting for {1} ms or lower time to yellow!", time_to_yellow, _time_to_yellow);
                }
            }
            else {
                SPDLOG_DEBUG("No vehicles present");
 
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(_so_sleep_time));
        }
        SPDLOG_WARN("Stopping desired phase plan producer thread!");
        dpp_producer->stop();

    }

    
    void signal_opt_service::populate_movement_groups(std::shared_ptr<streets_signal_optimization::movement_groups> _groups, 
                                        const std::shared_ptr<streets_tsc_configuration::tsc_configuration_state> tsc_config) const 
    {
        if (tsc_config) {
            for (const auto &phase_config : tsc_config->tsc_config_list) {
                // If signal group has one or more concurrent phase
                if ( !phase_config.concurrent_signal_groups.empty() ) {
                    // Loop through concurrent phases and create a movement group for each pair
                    for (const auto &concur : phase_config.concurrent_signal_groups) {
                        streets_signal_optimization::movement_group new_group;
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
                    streets_signal_optimization::movement_group new_group;
                    // Add invalid signal group 0 as current signal group to indicate there is none.
                    new_group.signal_groups = { phase_config.signal_group_id, 0};
                    _groups->groups.push_back(new_group);
                }
            }
            SPDLOG_DEBUG("Movement Groups: ");
            for ( const auto &mg : _groups->groups) {
                SPDLOG_DEBUG("Signal Group {0} and Signal Group {1}.", mg.signal_groups.first, mg.signal_groups.second );
            }
            // Remove configured signal groups to ignore them
            auto mg_itr = _groups->groups.begin();
            while ( mg_itr != _groups->groups.end() ) {
                bool erased = false;
                for (const auto &ignore : ignore_signal_groups) {
                    if ( mg_itr->signal_groups.first == ignore && mg_itr->signal_groups.second == 0) {
                        SPDLOG_WARN("Removing movement group with signal groups {0} , {0}", mg_itr->signal_groups.first, mg_itr->signal_groups.second);
                        mg_itr = _groups->groups.erase(mg_itr);
                        erased = true;
                    }
                }
                if (!erased) {
                    mg_itr++;
                }

            }
            
            
            
        }
    }


    bool signal_opt_service::update_intersection_info(unsigned long sleep_millisecs, unsigned long int_client_request_attempts) const
    {
        SPDLOG_INFO("Send client request to update intersection inforamtion every {0} ms for maximum {1} times.", sleep_millisecs, int_client_request_attempts);
        int attempt_count = 0;
        while (attempt_count < int_client_request_attempts)
        {
            // Send HTTP request, and update intersection information. If updated successfully, it returns true and exit the while loop.
            if (_so_msgs_worker_ptr && _so_msgs_worker_ptr->request_intersection_info(intersection_info_ptr))
            {
                return true;
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(sleep_millisecs));
            attempt_count++;
        }
        // If failed to update the intersection information after certain numbers of attempts
        SPDLOG_ERROR("Updating Intersection information failed. ");
        return false;
    }

    void signal_opt_service::configure_csv_logger() const
    {
        try{
            SPDLOG_INFO("csv log path: {0}", _so_log_path + _so_log_filename + ".csv");
            auto csv_logger = spdlog::daily_logger_mt<spdlog::async_factory>(
                "so_csv_logger",  // logger name
                _so_log_path + _so_log_filename +".csv",  // log file name and path
                23, // hours to rotate
                59 // minutes to rotate
                );
            // Only log log statement content
            csv_logger->set_pattern("%v");
            csv_logger->set_level(spdlog::level::info);
        }
        catch (const spdlog::spdlog_ex& ex)
        {
            spdlog::error( "Log initialization failed: {0}!",ex.what());
        }
    }

    void signal_opt_service::read_configuration_params() 
    {
        _exp_delta = streets_service::streets_configuration::get_int_config("exp_delta");
        _bootstrap_server = streets_service::streets_configuration::get_string_config("bootstrap_server");
        _spat_topic_name = streets_service::streets_configuration::get_string_config("spat_consumer_topic");
        _spat_group_id = streets_service::streets_configuration::get_string_config("spat_group_id");
        _vsi_topic_name = streets_service::streets_configuration::get_string_config("vsi_consumer_topic");
        _vsi_group_id = streets_service::streets_configuration::get_string_config("vsi_group_id");
        _tsc_config_topic_name = streets_service::streets_configuration::get_string_config("tsc_config_consumer_topic");
        _tsc_config_group_id = streets_service::streets_configuration::get_string_config("tsc_config_group_id");
        _dpp_topic_name = streets_service::streets_configuration::get_string_config("dpp_producer_topic");
        _so_log_path = streets_service::streets_configuration::get_string_config("so_log_path");
        _so_log_filename = streets_service::streets_configuration::get_string_config("so_log_filename");
        enable_so_logging = streets_service::streets_configuration::get_boolean_config("enable_so_logging");
        so_sleep_time = streets_service::streets_configuration::get_int_config("signal_optimization_frequency");
        _time_to_yellow = streets_service::streets_configuration::get_int_config("time_before_yellow_change");

        dpp_config.initial_green_buffer = streets_service::streets_configuration::get_int_config("initial_green_buffer");
        dpp_config.final_green_buffer = streets_service::streets_configuration::get_int_config("final_green_buffer");
        dpp_config.et_inaccuracy_buffer = streets_service::streets_configuration::get_int_config("et_inaccuracy_buffer");
        dpp_config.queue_max_time_headway = streets_service::streets_configuration::get_int_config("queue_max_time_headway");
        dpp_config.so_radius = streets_service::streets_configuration::get_double_config("so_radius");
        dpp_config.min_green = streets_service::streets_configuration::get_int_config("min_green");
        dpp_config.max_green = streets_service::streets_configuration::get_int_config("max_green");
        dpp_config.desired_future_move_group_count = static_cast<uint8_t>(streets_service::streets_configuration::get_int_config("desired_future_move_group_count"));
        std::stringstream comma_separated_list (streets_service::streets_configuration::get_string_config("ignore_signal_groups"));
        while( comma_separated_list.good() )
        {
            std::string signal_group;
            getline( comma_separated_list, signal_group, ',' );
            SPDLOG_WARN("Adding signal group {0} to the list of ignored signal groups", std::stoi(signal_group));
            ignore_signal_groups.push_back( std::stoi(signal_group) );
        }

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
        if (_dpp_producer) 
        {
            _dpp_producer->stop();
        }
    }
}