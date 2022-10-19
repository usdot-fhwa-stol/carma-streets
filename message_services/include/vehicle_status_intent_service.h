#ifndef VEHICLE_STATUS_INTENT_SERVICE_H
#define VEHICLE_STATUS_INTENT_SERVICE_H

#include <iostream>
#include <thread>
#include <vector>
#include <map>
#include <stdlib.h> /* abs */
#include <lanelet2_core/Exceptions.h>


#include "bsm_worker.h"
#include "mobilitypath_worker.h"
#include "mobilityoperation_worker.h"
#include "vehicle_status_intent_worker.h"
#include "vehicle_status_intent.h"
#include "kafka_client.h"
#include "message_lanelet2_translation.h"

#include "streets_configuration.h"


namespace message_services
{
    namespace services
    {
        typedef struct vsi_message_bucket
        {
            models::mobilityoperation mo;
            models::bsm bsm;
            models::mobilitypath mp;
        } vsi_message_bucket_t;

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
            std::shared_ptr<kafka_clients::kafka_producer_worker> _vsi_producer_worker;
            std::shared_ptr<kafka_clients::kafka_consumer_worker> _mo_consumer_worker;
            std::shared_ptr<kafka_clients::kafka_consumer_worker> _mp_consumer_worker;
            std::shared_ptr<kafka_clients::kafka_consumer_worker> _bsm_consumer_worker;
            std::int64_t vsi_est_path_point_count = 0;
            bool disable_est_path = false; // false: Show est path in the vsi message. true: Not show
            /***
             * False: Each point in est path includes lanelet id (Identify which lanelet this point lands on), distance to end of lanelet and timestamp
             * True: First point in est_path includes distance to current vehicle location, and timestamp. Following points in est_path include distance to previous point and timestamp 
            **/
            bool is_est_path_p2p_distance_only = false; 

            //Mapping MobilityOperation and BSM msg_count maximum allowed differences.
            std::int32_t MOBILITY_OPERATION_BSM_MAX_COUNT_OFFSET = 0;

            //Mapping MobilityOperation and MobilityPath timestamp duration within 1000 ms.
            std::int32_t MOBILITY_OPERATION_PATH_MAX_DURATION = 1000; 

            //The duration between the offset points in mobilitypath message. Default duration is MOBILITY_PATH_TRAJECTORY_OFFSET_DURATION * 100 (milliseconds)
            std::uint32_t MOBILITY_PATH_TRAJECTORY_OFFSET_DURATION = 1;

            //Publish vehicle status intent thread sleep time.
            unsigned int VSI_TH_SLEEP_MILLI_SEC = 100; 

            //Expire BSM message from the queue after duration. Default value is 6 seconds
            unsigned long BSM_MSG_EXPIRE_IN_SEC = 6; 

            //Clean the queue every CLEAN_QUEUE_IN_SECS; 
            std::int32_t CLEAN_QUEUE_IN_SECS = 0;

            //Tracking last message expired timestamp
            std::time_t prev_msg_expired_timestamp_ = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();                      

            //add lanelet2 translation object
            std::shared_ptr<message_translations::message_lanelet2_translation> _msg_lanelet2_translate_ptr;

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
             * @brief Generate the vehicle status and intent message based on the latest bsm , MobilityOperation and MobilityPath objects.
             * @param pointers to object that will store the latest messages
             * **/
            models::vehicle_status_intent compose_vehicle_status_intent(models::bsm &bsm, models::mobilityoperation &mo, models::mobilitypath &mp);

            /**
             * @brief Consume a message from particular topic and convert the message as an object. Then  append the message to its corresponding worker
             * @param pointers to object that will store the message consumed. topic from which the message is from. The group id of the consumer
             * **/
            template <class T>
            void msg_consumer(std::shared_ptr<T> msg_ptr,  std::shared_ptr<kafka_clients::kafka_consumer_worker>  consumer_worker);

            /**
             * @brief Producer a message to a topic
             * @param pointer to object that will be published, and a topic name
             * **/
            template <typename T>
            void publish_msg(T msg,  std::shared_ptr<kafka_clients::kafka_producer_worker>  producer_worker);
        };
    }
}

#endif