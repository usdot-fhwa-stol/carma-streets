
#pragma once

#include <thread>    
#include <chrono>  
#include <spdlog/spdlog.h>
#include <QTimer>
#include <QEventLoop>
#include <iostream>
#include <string>
#include <unordered_map>
#include <intersection_client_api_lib/OAIHelpers.h>
#include "intersection_client_api_lib/OAIDefaultApi.h"
#include "intersection_client_api_lib/OAIIntersection_info.h"

#include "kafka_client.h"
#include "streets_configuration.h"
#include "intersection_client.h"
#include "vehicle_list.h"
#include "all_stop_vehicle_scheduler.h"
#include "all_stop_status_intent_processor.h"
#include "all_stop_scheduling_worker.h"


namespace scheduling_service{

	class all_stop_scheduling_service
	{
	private:

		std::string bootstrap_server;
        std::string group_id;
        std::string consumer_topic;
        std::string producer_topic;

        std::shared_ptr<OpenAPI::OAIIntersection_info> intersection_info_ptr;
        std::shared_ptr<streets_vehicles::vehicle_list> vehicle_list_ptr;
		std::shared_ptr<streets_vehicle_scheduler::all_stop_vehicle_scheduler> scheduler_ptr;

		std::shared_ptr<kafka_clients::kafka_consumer_worker> consumer_worker;
        std::shared_ptr<kafka_clients::kafka_producer_worker> producer_worker;
		std::shared_ptr<all_stop_scheduling_worker> scheduling_worker;

	public:

		
		/**
         * @brief Initialize 
         */
        all_stop_scheduling_service() = default;

        /**
         * @brief stop the consumer and producer
         */
        ~all_stop_scheduling_service();

		/**
         * @brief Initialize the consumer, producer, and scheduling workers.
		 * Create a vehicle list and scheduler objects and configure them.
         */
        bool initialize(const int sleep_millisecs, const int int_client_request_attempts);

        /**
         * @brief Create 2 threads:
		 * The first thread consumes status and intent message and updates the vehicle list.
		 * The second thread schedule vehicles and produce the schedule plan.
         */
        void start();

		/**
         * @brief Create the vehicle list processor and configure it.
         */
		bool config_vehicle_list() const;

		/**
         * @brief Configure the scheduler object.
         */
		bool config_scheduler() const;

        /**
         * @brief Consume the status and intent messages via kafka consumer.
		 * @param consumer_worker The consumer worker.
		 * @param vehicle_list_ptr The vehicle list object.
         */
        void consume_msg(std::shared_ptr<kafka_clients::kafka_consumer_worker> _consumer_worker, std::shared_ptr<streets_vehicles::vehicle_list> _vehicle_list_ptr) const;

        /**
         * @brief Schedule vehicles and produce the schedule plan.
		 * @param producer_worker The kafka producer worker.
		 * @param scheduling_worker The scheduling worker.
		 * @param vehicle_list_ptr The vehicle list object.
		 * @param scheduler_ptr The scheduler object.
         */
        void schedule_veh(std::shared_ptr<kafka_clients::kafka_producer_worker> _producer_worker, std::shared_ptr<all_stop_scheduling_worker> _scheduling_worker, std::shared_ptr<streets_vehicles::vehicle_list> _vehicle_list_ptr, std::shared_ptr<streets_vehicle_scheduler::all_stop_vehicle_scheduler> _scheduler_ptr) const;

		/**
		 * @brief Method to configure spdlog::logger for logging scheduling metrics into daily rotating csv file.
		 */
		void configure_csv_logger() const;

	};

}

