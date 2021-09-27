#pragma once
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
            std::deque<models::mobilitypath>& get_curr_list();
            void process_incoming_msg(const std::string json_str);
            void pop_cur_element_from_list(long element_position);
        };
    }
}