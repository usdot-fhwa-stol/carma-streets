#include "scheduling_service.h"

namespace scheduling_service{

    bool scheduling_service::initialize(const int sleep_millisecs, const int int_client_request_attempts)
    {
        try
        {
            auto client = std::make_shared<kafka_clients::kafka_client>();
            
            this -> bootstrap_server = streets_service::streets_configuration::get_string_config("bootstrap_server");
            this -> group_id = streets_service::streets_configuration::get_string_config("group_id");
            this -> consumer_topic = streets_service::streets_configuration::get_string_config("consumer_topic");
            this -> producer_topic = streets_service::streets_configuration::get_string_config("producer_topic");

            consumer_worker = client->create_consumer(bootstrap_server, consumer_topic, group_id);
            producer_worker  = client->create_producer(bootstrap_server, producer_topic);

            if(!consumer_worker->init())
            {
                SPDLOG_CRITICAL("kafka consumer initialize error");
                exit(EXIT_FAILURE);
                return false;
            }
            else
            {
                consumer_worker->subscribe();
                if(!consumer_worker->is_running())
                {
                    SPDLOG_CRITICAL("consumer_worker is not running");
                    exit(EXIT_FAILURE);
                    return false;
                }
            }

            // Create logger
            if ( streets_service::streets_configuration::get_boolean_config("enable_schedule_logging") ) {
                configure_csv_logger();
            }
            
            if(!producer_worker->init())
            {
                SPDLOG_CRITICAL("kafka producer initialize error");
                exit(EXIT_FAILURE);
                return false;
            }
            
            if ( streets_service::streets_configuration::get_string_config("intersection_type").compare("signalized_intersection") == 0 ) {
                this -> spat_topic = streets_service::streets_configuration::get_string_config("spat_topic");
                spat_consumer_worker = client->create_consumer(bootstrap_server, spat_topic, group_id);
                if(!spat_consumer_worker->init())
                {
                    SPDLOG_CRITICAL("kafka consumer initialize error");
                    exit(EXIT_FAILURE);
                    return false;
                }
                else
                {
                    spat_consumer_worker->subscribe();
                    if(!spat_consumer_worker->is_running())
                    {
                        SPDLOG_CRITICAL("spat_consumer_worker is not running");
                        exit(EXIT_FAILURE);
                        return false;
                    }
                }
            }

            config_vehicle_list();

            // HTTP request to update intersection information
            auto int_client = std::make_shared<intersection_client>();
            if (int_client->update_intersection_info(sleep_millisecs, int_client_request_attempts))
            {
                config_scheduler(int_client->get_intersection_info());
            }
            else
            {
                return false;
            }

            _scheduling_worker = std::make_shared<scheduling_worker>();

            SPDLOG_INFO("all stop scheduling service initialized successfully!!!");
            return true;
        }
        catch ( const streets_service::streets_configuration_exception &ex ) {
            SPDLOG_ERROR("all stop scheduling service Initialization failure: {0} ", ex.what());
            return false;
        }
    }


    void scheduling_service::start()
    {
        std::thread consumer_thread(&scheduling_service::consume_msg, this );
        std::thread scheduling_thread(&scheduling_service::schedule_veh, this);
        
        if ( streets_service::streets_configuration::get_string_config("intersection_type").compare("signalized_intersection") == 0 ) {
            std::thread spat_consumer_thread(&scheduling_service::consume_spat, this );
            spat_consumer_thread.join();
        }

        consumer_thread.join();
        scheduling_thread.join();
    }


    bool scheduling_service::config_vehicle_list()
    {
        vehicle_list_ptr = std::make_shared<streets_vehicles::vehicle_list>();
        std::string intersection_type =  streets_service::streets_configuration::get_string_config("intersection_type");
        if ( intersection_type.compare("stop_controlled_intersection") == 0) {
            vehicle_list_ptr->set_processor(std::make_shared<streets_vehicles::all_stop_status_intent_processor>());
            auto processor = std::dynamic_pointer_cast<streets_vehicles::all_stop_status_intent_processor>(vehicle_list_ptr->get_processor());
            processor->set_stopping_distance(streets_service::streets_configuration::get_double_config("stop_distance"));
            processor->set_stopping_speed(streets_service::streets_configuration::get_double_config("stop_speed"));
            processor->set_timeout(streets_service::streets_configuration::get_int_config("exp_delta"));
            
            SPDLOG_INFO("Vehicle list is configured successfully! ");
            return true;
        }
        else if (intersection_type.compare("signalized_intersection") == 0) {
            vehicle_list_ptr->set_processor(std::make_shared<streets_vehicles::signalized_status_intent_processor>());
            std::dynamic_pointer_cast<streets_vehicles::signalized_status_intent_processor>(vehicle_list_ptr->get_processor())->set_timeout(streets_service::streets_configuration::get_int_config("exp_delta"));

            SPDLOG_INFO("Vehicle list is configured successfully! ");
            return true;
        }
        else {
            SPDLOG_ERROR("Failed configuring Vehicle List. Scheduling Service does not support intersection_type : {0}!", intersection_type);
            return false;
        }
        
    }


    bool scheduling_service::config_scheduler(std::shared_ptr<OpenAPI::OAIIntersection_info> intersection_info_ptr) 
    {
        std::string intersection_type =  streets_service::streets_configuration::get_string_config("intersection_type");
        if ( intersection_type.compare("stop_controlled_intersection") == 0) {
            scheduler_ptr = std::make_shared<streets_vehicle_scheduler::all_stop_vehicle_scheduler>();
            scheduler_ptr->set_intersection_info(intersection_info_ptr);
            std::dynamic_pointer_cast<streets_vehicle_scheduler::all_stop_vehicle_scheduler>(scheduler_ptr)->set_flexibility_limit(
                    streets_service::streets_configuration::get_int_config("flexibility_limit"));
            
            SPDLOG_INFO("Scheduler is configured successfully! ");
            return true;
        }
        else if ( intersection_type.compare("signalized_intersection") == 0 ) {
            scheduler_ptr = std::make_shared<streets_vehicle_scheduler::signalized_vehicle_scheduler>();
            scheduler_ptr->set_intersection_info(intersection_info_ptr);
            
            auto processor = std::dynamic_pointer_cast<streets_vehicle_scheduler::signalized_vehicle_scheduler>(scheduler_ptr);
            processor->set_initial_green_buffer(streets_service::streets_configuration::get_int_config("initial_green_buffer"));
            processor->set_final_green_buffer(streets_service::streets_configuration::get_int_config("final_green_buffer"));
            
            SPDLOG_INFO("Scheduler is configured successfully! ");
            return true;
        }
        else {
            SPDLOG_ERROR("Failed configuring Vehicle Scheduler. Scheduling Service does not support intersection_type : {0}!", intersection_type);
            return false;
        }

    }


    void scheduling_service::consume_msg() const
    {
        SPDLOG_INFO("Starting status and intent consumer thread.");
        while (consumer_worker->is_running()) 
        {
            
            const std::string payload = consumer_worker->consume(1000);

            if(payload.length() != 0 && vehicle_list_ptr)
            {                

                vehicle_list_ptr->process_update(payload);
    
            }
        }
        SPDLOG_WARN("Stopping status and intent consumer thread!");
        consumer_worker->stop();
    }


    void scheduling_service::consume_spat() const
    {
        SPDLOG_INFO("Starting spat consumer thread.");
        while (spat_consumer_worker->is_running()) 
        {
            
            const std::string spat_msg = spat_consumer_worker->consume(1000);

            if(spat_msg.length() != 0 && spat_ptr)
            {                

                spat_ptr->fromJson(spat_msg);
    
            }
        }
        SPDLOG_WARN("Stopping spat consumer thread!");
        spat_consumer_worker->stop();
    }


    void scheduling_service::schedule_veh() const
    {
        SPDLOG_INFO("Starting scheduling thread.");
        if (!_scheduling_worker)
        {
            SPDLOG_CRITICAL("scheduling worker is not initialized");
            exit(EXIT_FAILURE);
        }
        
        auto scheduling_delta = u_int64_t(streets_service::streets_configuration::get_double_config("scheduling_delta") * 1000);
        int sch_count = 0;
        std::unordered_map<std::string, streets_vehicles::vehicle> veh_map;

        while (true)
        {
            
            SPDLOG_DEBUG("schedule number #{0}", sch_count);      
            auto next_schedule_time_epoch = std::chrono::system_clock::now() + std::chrono::milliseconds(scheduling_delta);

            
            veh_map = vehicle_list_ptr -> get_vehicles();
            auto int_schedule = _scheduling_worker->schedule_vehicles(veh_map, scheduler_ptr);

            std::string msg_to_send = int_schedule->toJson();

            SPDLOG_DEBUG("schedule plan: {0}", msg_to_send);

            /* produce the scheduling plan to kafka */
            producer_worker->send(msg_to_send);

            sch_count += 1;

            // sleep until next schedule
            if (std::chrono::system_clock::now() < next_schedule_time_epoch){
                std::this_thread::sleep_until(next_schedule_time_epoch);
            }
            
        }
        SPDLOG_WARN("Stopping scheduling thread!");
        producer_worker->stop();

    }

 
    void scheduling_service::configure_csv_logger() const
    {
        try{
            auto csv_logger = spdlog::daily_logger_mt<spdlog::async_factory>(
                "csv_logger",  // logger name
                streets_service::streets_configuration::get_string_config("schedule_log_path")+
                    streets_service::streets_configuration::get_string_config("schedule_log_filename") +".csv",  // log file name and path
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


    scheduling_service::~scheduling_service()
    {
        if (consumer_worker)
        {
            SPDLOG_WARN("Stopping consumer worker!");
            consumer_worker->stop();
        }

        if (producer_worker)
        {
            SPDLOG_WARN("Stopping producer worker");
            producer_worker->stop();
        }

    }

    void scheduling_service::set_scheduling_worker(std::shared_ptr<scheduling_worker> sched_worker) {
        _scheduling_worker = sched_worker;
    }

    void scheduling_service::set_vehicle_list(std::shared_ptr<streets_vehicles::vehicle_list> veh_list) {
        vehicle_list_ptr = veh_list;
    }

    void scheduling_service::set_vehicle_scheduler(std::shared_ptr<streets_vehicle_scheduler::vehicle_scheduler> scheduler) {
        scheduler_ptr = scheduler;
    }

    void scheduling_service::set_consumer_worker(std::shared_ptr<kafka_clients::kafka_consumer_worker> worker) {
        consumer_worker = worker;
    }

    void scheduling_service::set_producer_worker(std::shared_ptr<kafka_clients::kafka_producer_worker> worker) {
        producer_worker = worker;
    }

    void scheduling_service::set_spat_consumer_worker(std::shared_ptr<kafka_clients::kafka_consumer_worker> worker) {
        spat_consumer_worker = worker;
    }
}

