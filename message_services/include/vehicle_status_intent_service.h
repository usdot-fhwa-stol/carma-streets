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

            const std::string MOBILITYOPERATION_WORKER_TYPE_NAME = "mobilityoperation_worker";
            const std::string MOBILITYPATH_WORKER_TYPE_NAME = "mobilitypath_worker";
            const std::string BSM_WORKER_TYPE_NAME = "bsm_worker";
            const std::string MANIFEST_CONFIG_FILE_PATH = "../manifest.json";

            bool initialize();

            void start();

            void run(std::shared_ptr<message_services::workers::bsm_worker> bsm_w_ptr,
                     std::shared_ptr<message_services::workers::mobilitypath_worker> mp_w_ptr,
                     std::shared_ptr<message_services::workers::mobilityoperation_worker> mo_w_ptr);

            void run(std::shared_ptr<workers::vehicle_status_intent_worker> vsi_w_ptr,
                     std::shared_ptr<message_services::workers::bsm_worker> bsm_w_ptr,
                     std::shared_ptr<message_services::workers::mobilitypath_worker> mp_w_ptr,
                     std::shared_ptr<message_services::workers::mobilityoperation_worker> mo_w_ptr);

            void identify_latest_mapping_bsm_mo_mp(std::shared_ptr<workers::bsm_worker> bsm_w_ptr, std::shared_ptr<workers::mobilityoperation_worker> mo_w_ptr,
                                                   std::shared_ptr<workers::mobilitypath_worker> mp_w_ptr, std::shared_ptr<models::bsm> bsm_ptr,
                                                   std::shared_ptr<models::mobilityoperation> mo_ptr, std::shared_ptr<models::mobilitypath> mp_ptr);

            models::vehicle_status_intent compose_vehicle_status_intent(models::bsm &bsm, models::mobilityoperation &mo, models::mobilitypath &mp);

            template <class T>
            void msg_consumer(std::shared_ptr<T> msg_ptr, std::string topic, std::string group_id);

            template <typename T>
            void publish_msg(T msg, std::string topic);
        };
    }
}

#endif