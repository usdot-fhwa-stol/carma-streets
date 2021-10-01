#ifndef MOBILITYOPERATION_WORKER_H
#define MOBILITYOPERATION_WORKER_H

#include <iostream>
#include "mobilityoperation.h"
#include "kafka_client.h"
#include "base_worker.h"

namespace message_services
{
    namespace workers
    {
        class mobilityoperation_worker : public base_worker
        {
        private:
            std::deque<message_services::models::mobilityoperation> mobilityoperation_v;

        public:
            mobilityoperation_worker();
            ~mobilityoperation_worker();

            std::mutex worker_mtx;
            /**
             * @brief Return the vector of mobilityoperation stored in the mobilityoperation_worker
             * ***/

            std::deque<models::mobilityoperation> &get_curr_list();
            /***
            * @brief process incoming mobilityoperation json string and create mobilityoperation object.
              Appending the mobilityoperation object to the mobilityoperation_list
              @param std::string json_string
            */
            void process_incoming_msg(const std::string json_str);

            /**
             * @brief Remove an element from mobilityoperation vector based on the element position.
             * If the position is greater than the deque current size, the last element is removed
             * @param long Current element position in the mobilityoperation deque. 
             * 
             * ***/
            void pop_cur_element_from_list(long element_position);
        };
    }
}

#endif