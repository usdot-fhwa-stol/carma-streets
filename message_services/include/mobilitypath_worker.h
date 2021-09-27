#ifndef MOBILITYPATH_WORKER_H
#define MOBILITYPATH_WORKER_H

#include <iostream>

#include "kafka_client.h"
#include "mobilitypath.h"
#include "base_worker.h"

namespace message_services
{
    namespace workers
    {
        class mobilitypath_worker : public base_worker
        {
        private:
            std::deque<message_services::models::mobilitypath> mobilitypath_v;

        public:
            mobilitypath_worker();
            ~mobilitypath_worker();

            std::mutex worker_mtx;

            /**
             * @brief Return the vector of mobilitypath stored in the mobilitypath_worker
             * ***/
            std::deque<models::mobilitypath> &get_curr_list();

            /***
            * @brief process incoming mobilitypath json string and create mobilitypath object.
              Appending the mobilitypath object to the mobilitypath_list
              @param std::string json_string
            */
            void process_incoming_msg(const std::string json_str);

            /**
             * @brief Remove an element from mobilitypath vector based on the element position.
             * If the position is greater than the deque current size, the last element is removed
             * @param long Current element position in the mobilitypath deque. 
             * 
             * ***/
            void pop_cur_element_from_list(long element_position);
        };
    }
}

#endif