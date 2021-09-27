#ifndef VEHICLE_STATUS_INTENT_WORKER_H
#define VEHICLE_STATUS_INTENT_WORKER_H

#include <iostream>

#include <vector>
#include <mutex>

#include "vehicle_status_intent.h"
#include "mobilityoperation.h"
#include "mobilitypath.h"
#include "bsm.h"
#include "kafka_client.h"
#include "base_worker.h"

namespace message_services
{
    namespace workers
    {
        class vehicle_status_intent_worker : public base_worker
        {
        private:
            std::map<std::string, models::vehicle_status_intent> vsi_map_;

        public:
            vehicle_status_intent_worker();
            ~vehicle_status_intent_worker();
            std::mutex worker_mtx;
            std::map<std::string, models::vehicle_status_intent> &get_curr_map();
            void process_incoming_msg(const std::string json_str);
            void pop_element_from_map(std::string key);
            void update_by_incoming_bsm_msg(models::bsm bsm);
            void update_insert_by_incoming_mobilityoperation_msg(models::mobilityoperation mo);
            void update_insert_by_incoming_mobilitypath_msg(models::mobilitypath mp);
        };
    }
}

#endif