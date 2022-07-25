
#pragma once

#include <thread>    
#include <chrono>  
#include <spdlog/spdlog.h>
#include <QTimer>
#include <QEventLoop>
#include <iostream>
#include <string>
#include <unordered_map>

#include "intersection_client_api_lib/OAIHelpers.h"
#include "intersection_client_api_lib/OAIDefaultApi.h"
#include "intersection_client_api_lib/OAIIntersection_info.h"
#include "kafka_client.h"
#include "streets_configuration.h"
#include "intersection_client.h"
#include "vehicle_list.h"
#include "scheduling_worker.h"
#include "spat.h"


namespace scheduling_service{

    class scheduling_service
    {
    private:

        std::string bootstrap_server;
        std::string group_id;
        std::string consumer_topic;
        std::string producer_topic;
        std::string spat_topic;

        std::shared_ptr<OpenAPI::OAIIntersection_info> intersection_info_ptr;
        std::shared_ptr<streets_vehicles::vehicle_list> vehicle_list_ptr;
        std::shared_ptr<streets_vehicle_scheduler::vehicle_scheduler> scheduler_ptr;
        std::shared_ptr<signal_phase_and_timing::spat> spat_ptr;

        std::shared_ptr<kafka_clients::kafka_consumer_worker> consumer_worker;
        std::shared_ptr<kafka_clients::kafka_consumer_worker> spat_consumer_worker;
        std::shared_ptr<kafka_clients::kafka_producer_worker> producer_worker;
        std::shared_ptr<scheduling_worker> _scheduling_worker;

    public:

        
        /**
         * @brief Initialize 
         */
        scheduling_service() = default;

        /**
         * @brief stop the consumer and producer
         */
        ~scheduling_service();

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
        bool config_vehicle_list();

        /**
         * @brief Configure the scheduler object.
         */
        bool config_scheduler();

        /**
         * @brief Consume the status and intent messages via kafka consumer.
         */
        void consume_msg() const;

        /**
         * @brief Consume the modified spat via kafka consumer.
         */
        void consume_spat() const;

        /**
         * @brief Schedule vehicles and produce the schedule plan.
         */
        void schedule_veh() const;

        /**
         * @brief Method to configure spdlog::logger for logging scheduling metrics into daily rotating csv file.
         */
        void configure_csv_logger() const;

        /**
         * @brief Set the scheduling worker object for unit testing
         * 
         * @param sched_worker 
         */
        void set_scheduling_worker(std::shared_ptr<scheduling_worker> sched_worker);

        /**
         * @brief Set the vehicle list object for unit testing
         * 
         * @param veh_list 
         */
        void set_vehicle_list(std::shared_ptr<streets_vehicles::vehicle_list> veh_list);

        /**
         * @brief Set the vehicle scheduler object for unit testing
         * 
         * @param scheduler 
         */
        void set_vehicle_scheduler(std::shared_ptr<streets_vehicle_scheduler::vehicle_scheduler> scheduler);

        /**
         * @brief Set the intersection info object for unit testing
         * 
         * @param int_info 
         */
        void set_intersection_info(std::shared_ptr<OpenAPI::OAIIntersection_info> int_info);

        /**
         * @brief Set the consumer worker object for unit testing
         * 
         * @param worker 
         */
        void set_consumer_worker( std::shared_ptr<kafka_clients::kafka_consumer_worker> worker );

        /**
         * @brief Set the producer worker object for unit testing
         * 
         * @param worker 
         */
        void set_producer_worker( std::shared_ptr<kafka_clients::kafka_producer_worker> worker );

        /**
         * @brief Set the spat consumer worker object for unit testing
         * 
         * @param worker 
         */
        void set_spat_consumer_worker( std::shared_ptr<kafka_clients::kafka_consumer_worker> worker );
    };

}

