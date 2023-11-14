#pragma once 

#include <kafka_client.h>
#include <streets_configuration.h>
#include <streets_configuration_exception.h>
#include <streets_service.h>
#include <streets_clock_singleton.h>
#include <string>

namespace sensor_data_sharing_service {

    class sds_service : public streets_service::streets_service {
        private:
            /**
             * @brief Kafka producer for SDSM JSON
             */
            std::shared_ptr<kafka_clients::kafka_producer_worker> sdsm_producer;
            /*
             * @brief Kafka consumer for consuming Detected Object JSON
             */
            std::shared_ptr<kafka_clients::kafka_consumer_worker> detection_consumer;


        public:
            sds_service() = default;

            ~sds_service();

            // Remove copy constructor
            sds_service(const sds_service &) = delete;
            // Remove copy assignment operator
            sds_service& operator=(const sds_service &) = delete;
            
            /**
             * @brief Method to initialize the sds_service.
             * 
             * @return true if successful.
             * @return false if not successful.
             */
            bool initialize() override;

            bool initialize_kafka_consumers_producers( )

            
            /**
             * @brief Method to start all threads included in the tsc_service.
             */
            void start() override;
        


    };
}