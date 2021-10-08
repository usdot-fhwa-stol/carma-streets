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

            /**
             * @brief Return the vector of vehicle_status_intent stored in the vehicle_status_intent_worker
             * ***/
            std::map<std::string, models::vehicle_status_intent> &get_curr_map();

            /***
            * @brief process incoming vehicle_status_intent json string and create vehicle_status_intent object.
              Appending the vehicle_status_intent object to the vehicle_status_intent_list
              @param std::string json_string
            */
            void process_incoming_msg(const std::string json_str);

            /**
             * @brief Remove an element from vehicle_status_intent vector based on the element position.
             * If the position is greater than the deque current size, the last element is removed
             * @param long Current element position in the vehicle_status_intent deque. 
             * 
             * ***/
            void pop_element_from_map(std::string key);

            /**
             * @brief Update the vehicle status and intent base on the bsm info.
             * @param bsm_obj an instance of bsm 
             * ***/
            void update_by_incoming_bsm_msg(models::bsm bsm);

            /**
             * @brief If the vehicle id from the MobilityOperation message exist in vehicle status and intent map, update the vehicle status and intent base on the MobilityOperation info.
             * If the vehicle id from the MobilityOperation message does not exist in vehicle status and intent map, insert a new the vehicle status and intent object to the map base on the MobilityOperation info.
             * @param mo_obj an instance of mobilityoperation 
             * ***/
            void update_insert_by_incoming_mobilityoperation_msg(models::mobilityoperation mo);

            /**
             * @brief If the vehicle id from the mobilitypath message exist in vehicle status and intent map, update the vehicle status and intent base on the mobilitypath info.
             * If the vehicle id from the mobilitypath message does not exist in vehicle status and intent map, insert a new the vehicle status and intent object to the map base on the mobilitypath info.
             * @param mp_obj an instance of mobilitypath 
             * ***/
            void update_insert_by_incoming_mobilitypath_msg(models::mobilitypath mp);
        };
    }
}

#endif