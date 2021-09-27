#pragma once
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
            std::deque<models::mobilityoperation>& get_curr_list();
            void process_incoming_msg(const std::string json_str);
            void pop_cur_element_from_list(long element_position);
        };
    }
}