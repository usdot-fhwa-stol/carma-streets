#ifndef VEHICLE_STATUS_INTENT_SERVICE_H
#define VEHICLE_STATUS_INTENT_SERVICE_H

#include <iostream>
#include <thread>
#include <vector>
#include <map>

#include "bsm_worker.h"
#include "mobilitypath_worker.h"
#include "mobilityoperation_worker.h"
#include "vehicle_status_intent_worker.h"
#include "vehicle_status_intent.h"
#include "kafka_client.h"

namespace message_services
{
    namespace services
    {
        class vehicle_status_intent_service
        {
        private:
            std::string bootstrap_server;
            std::string bsm_group_id;
            std::string bsm_topic_name;
            std::string mo_group_id;
            std::string mo_topic_name;
            std::string mp_group_id;
            std::string mp_topic_name;
            std::string vsi_topic_name;

        public:
            vehicle_status_intent_service();
            ~vehicle_status_intent_service();

            /**
             * @brief configuration file name
             * **/
            const std::string MANIFEST_CONFIG_FILE_PATH = "../manifest.json";

            /**
             * @brief read configuration file and determine the producer and consumer topics, bootstrap server
             * **/
            bool initialize();

            /**
             * @brief initialize variables and call run() methods to spin up threads.
             * **/
            void start();

            /**
             * @brief Creating and running threads. Each thread is acting as consumer or producer to consume or produce messages to topics
             * @param pointers to workers that will be used to work on messages that are consumed
             * **/
            void run(std::shared_ptr<message_services::workers::bsm_worker> bsm_w_ptr,
                     std::shared_ptr<message_services::workers::mobilitypath_worker> mp_w_ptr,
                     std::shared_ptr<message_services::workers::mobilityoperation_worker> mo_w_ptr);

            /**
             * @brief Creating and running threads. Each thread is acting as consumer or producer to consume or produce messages to topics
             * @param pointers to workers that will be used to work on messages that are consumed
             * **/
            void run(std::shared_ptr<workers::vehicle_status_intent_worker> vsi_w_ptr,
                     std::shared_ptr<message_services::workers::bsm_worker> bsm_w_ptr,
                     std::shared_ptr<message_services::workers::mobilitypath_worker> mp_w_ptr,
                     std::shared_ptr<message_services::workers::mobilityoperation_worker> mo_w_ptr);

            /**
             * @brief Identify the latest bsm , MobilityOperation and MobilityPath messages from the workers. Mapping those messages based on 
             * vehicle id, bsm_id and timestamp is less than 100 ms.
             * @param pointers to object that will store the latest messages
             * **/
            void identify_latest_mapping_bsm_mo_mp(std::shared_ptr<workers::bsm_worker> bsm_w_ptr, std::shared_ptr<workers::mobilityoperation_worker> mo_w_ptr,
                                                   std::shared_ptr<workers::mobilitypath_worker> mp_w_ptr, std::shared_ptr<models::bsm> bsm_ptr,
                                                   std::shared_ptr<models::mobilityoperation> mo_ptr, std::shared_ptr<models::mobilitypath> mp_ptr);
            /**
             * @brief Generate the vehicle status and intent message based on the latest bsm , MobilityOperation and MobilityPath objects.
             * @param pointers to object that will store the latest messages
             * **/
            models::vehicle_status_intent compose_vehicle_status_intent(models::bsm &bsm, models::mobilityoperation &mo, models::mobilitypath &mp);

            /**
             * @brief Consume a message from particular topic and convert the message as an object. Then  append the message to its corresponding worker
             * @param pointers to object that will store the message consumed. topic from which the message is from. The group id of the consumer
             * **/
            template <class T>
            void msg_consumer(std::shared_ptr<T> msg_ptr, std::string topic, std::string group_id);

            /**
             * @brief Producer a message to a topic
             * @param pointer to object that will be published, and a topic name
             * **/
            template <typename T>
            void publish_msg(T msg, std::string topic);
        };
    }
}

#endif