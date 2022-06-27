#include "all_stop_scheduling_service.h"

namespace scheduling_service{

	bool all_stop_scheduling_service::initialize(OpenAPI::OAIIntersection_info intersection_info)
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
				exit(-1);
			}
			else
			{
				consumer_worker->subscribe();
				if(!consumer_worker->is_running())
				{
					SPDLOG_CRITICAL("consumer_worker is not running");
					exit(-1);
				}
			}

			// Create logger
			if ( streets_service::streets_configuration::get_boolean_config("enable_schedule_logging") ) {
				configure_csv_logger();
			}
			if(!producer_worker->init())
			{
				SPDLOG_CRITICAL("kafka producer initialize error");
				exit(-1);
			}
			

			intersection_info_ptr = std::make_shared<OpenAPI::OAIIntersection_info>(intersection_info);

			vehicle_list_ptr = std::make_shared<streets_vehicles::vehicle_list>();
			config_vehicle_list();

			scheduler_ptr = std::make_shared<streets_vehicle_scheduler::all_stop_vehicle_scheduler>();
			config_scheduler();

			scheduling_worker = std::make_shared<all_stop_scheduling_worker>();

			SPDLOG_INFO("all stop scheduling service initialized successfully!!!");
            return true;
		}
		catch ( const streets_service::streets_configuration_exception &ex ) {
			SPDLOG_ERROR("all stop scheduling service Initialization failure: {0} ", ex.what());
			return false;
		}
	}


	void all_stop_scheduling_service::start()
	{
		std::thread consumer_thread(&all_stop_scheduling_service::consume_msg, this, std::ref(this->consumer_worker), vehicle_list_ptr);
        std::thread scheduling_thread(&all_stop_scheduling_service::schedule_veh, this, std::ref(this->producer_worker), std::ref(this->scheduling_worker), vehicle_list_ptr, scheduler_ptr);
        consumer_thread.join();
        scheduling_thread.join();
	}


	void all_stop_scheduling_service::config_vehicle_list()
    {
		if (vehicle_list_ptr)
		{
			vehicle_list_ptr->set_processor(std::make_shared<streets_vehicles::all_stop_status_intent_processor>());
			auto processor = std::dynamic_pointer_cast<streets_vehicles::all_stop_status_intent_processor>(vehicle_list_ptr->get_processor());
			processor->set_stopping_distance(streets_service::streets_configuration::get_double_config("stop_distance"));
			processor->set_stopping_speed(streets_service::streets_configuration::get_double_config("stop_speed"));
			processor->set_timeout(streets_service::streets_configuration::get_int_config("exp_delta"));
			
			SPDLOG_INFO("Vehicle list is configured successfully! ");
		}
    }


	void all_stop_scheduling_service::config_scheduler()
    {
		if (scheduler_ptr && intersection_info_ptr)
		{
			scheduler_ptr->set_intersection_info(intersection_info_ptr);
			scheduler_ptr->set_flexibility_limit(streets_service::streets_configuration::get_int_config("flexibility_limit"));
			
			SPDLOG_INFO("Scheduler is configured successfully! ");
		}
    }


	void all_stop_scheduling_service::consume_msg(std::shared_ptr<kafka_clients::kafka_consumer_worker> consumer_worker, std::shared_ptr<streets_vehicles::vehicle_list> vehicle_list_ptr){
		
		while (consumer_worker->is_running()) 
        {
            
            const std::string payload = consumer_worker->consume(1000);

            if(payload.length() != 0 && vehicle_list_ptr)
            {                

                vehicle_list_ptr->process_update(payload);
    
            }
        }
	}


	void all_stop_scheduling_service::schedule_veh(std::shared_ptr<kafka_clients::kafka_producer_worker> producer_worker, std::shared_ptr<all_stop_scheduling_worker> scheduling_worker, std::shared_ptr<streets_vehicles::vehicle_list> vehicle_list_ptr, std::shared_ptr<streets_vehicle_scheduler::all_stop_vehicle_scheduler> scheduler_ptr){
		

		if (this->scheduling_worker)
		{
			
			u_int64_t last_schedule_timestamp = 0;
			u_int64_t scheduling_delta = u_int64_t(streets_service::streets_configuration::get_double_config("scheduling_delta") * 1000);
			int sch_count = 0;

			while (true)
			{
				if (scheduling_worker->start_next_schedule(last_schedule_timestamp, scheduling_delta))
				{
					SPDLOG_INFO("schedule number #{0}", sch_count);      
					auto next_schedule_time_epoch = std::chrono::system_clock::now() + std::chrono::milliseconds(scheduling_delta);

					streets_vehicle_scheduler::intersection_schedule int_schedule = scheduling_worker->schedule_vehicles(vehicle_list_ptr, scheduler_ptr);

					std::string msg_to_send = scheduling_worker->create_schedule_plan(int_schedule);

					/* produce the scheduling plan to kafka */
                	producer_worker->send(msg_to_send);

					last_schedule_timestamp = int_schedule.timestamp;
					sch_count += 1;

					// sleep until next schedule
					if (std::chrono::system_clock::now() < next_schedule_time_epoch){
						std::this_thread::sleep_until(next_schedule_time_epoch);
					}
				}
			}
			producer_worker->stop();
		}
		else
		{
			SPDLOG_CRITICAL("scheduling worker is not initialized");
			exit(-1);
		}

	}

 
	void all_stop_scheduling_service::configure_csv_logger() {
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


	all_stop_scheduling_service::~all_stop_scheduling_service()
    {
        if (consumer_worker)
        {
            consumer_worker->stop();
        }

        if (producer_worker)
        {
            producer_worker->stop();
        }

    }

}

