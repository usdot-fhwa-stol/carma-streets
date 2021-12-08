#ifndef BSM_WORKER_H
#define BSM_WORKER_H

#include <iostream>

#include <vector>
#include <mutex>

#include "bsm.h"
#include "kafka_client.h"
#include "base_worker.h"

namespace message_services
{
    namespace workers
    {
        class bsm_worker : public base_worker
        {
        private:
            std::deque<message_services::models::bsm> bsm_v;
            std::map<std::string ,message_services::models::bsm> bsm_m;

        public:
            bsm_worker();
            ~bsm_worker();
            /**
             * @brief Return the vector of bsm stored in the bsm_worker
             * ***/
            std::deque<models::bsm>& get_curr_list();
            std::map<std::string ,message_services::models::bsm> &get_curr_map();
            
            std::mutex worker_mtx;
            /***
            * @brief process incoming bsm json string and create bsm object.
              Appending the bsm object to the bsm_list
              @param std::string json_string
            */
            void process_incoming_msg(const std::string json_str);
            
            /**
             * @brief Remove an element from bsm vector based on the element position.
             * If the position is greater than the deque current size, the last element is removed
             * @param long Current element position in the bsm deque. 
             * 
             * ***/
            void pop_cur_element_from_list(long element_position);
        };
    }
}

#endif