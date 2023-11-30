#pragma once 

#include <kafka_client.h>
#include <streets_configuration.h>
#include <streets_configuration_exception.h>
#include <streets_service.h>
#include <streets_clock_singleton.h>
#include <string>
#include <streets_utils/streets_messages_lib/sensor_data_sharing_msg/sensor_data_sharing_msg.hpp>
#include <streets_utils/streets_messages_lib/serializers/sensor_data_sharing_msg_json_serializer.hpp>
#include <streets_utils/streets_messages_lib/detected_object_msg/detected_object_msg.hpp>
#include <streets_utils/streets_messages_lib/deserializers/detected_obj_msg_deserializer.hpp>
#include <map>
#include <shared_mutex>


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
            /**
             * @brief Map of detected objects. New detections of existing objects will replace old detections.
             */
            std::map<std::string,streets_utils::messages::detected_objects_msg::detected_objects_msg> detected_objects;
            /**
             * @brief Mutex for thread safe operations on detected objects map.
             */
            std::shared_mutex detected_objects_lock;
            /**
             * @brief Initialize Kafka consumers and producers for sensor data sharing service.
             * @return true if successful and false if unsuccessful.
             */
            bool initialize_kafka_consumers_producers( );
            /**
             * @brief Loop to consume detections from kafka detection consumer. Will terminate if kafka consumer
             * is no longer running
             * 
             * @throws std::runtime exception if detection_consumer == nullptr
             */
            void consume_detections();
            /**
             * @brief Loop to produce sensor data sharing messages from detected_objects. Loop will populate sensor data sharing message with 
             * most recent detection information, publish message and clear detected object map. Will terminate if kafka producer is no longer
             * running.
             * 
             * @throws std::runtime exception if sdsm_producer == nullptr
             */
            void produce_sdsms();


        public:
            sds_service() = default;

            ~sds_service();
            
            /**
             * @brief Method to initialize the sds_service.
             * 
             * @return true if successful.
             * @return false if not successful.
             */
            bool initialize() override;

            /**
             * @brief Method to start all threads included in the tsc_service.
             */
            void start() override;

    };
}