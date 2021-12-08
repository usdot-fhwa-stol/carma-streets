#include "mobilitypath_worker.h"
namespace message_services
{
    namespace workers
    {
        mobilitypath_worker::mobilitypath_worker()
        {
        }

        mobilitypath_worker::~mobilitypath_worker()
        {
            this->mobilitypath_v.clear();
        }

        std::deque<models::mobilitypath> &mobilitypath_worker::get_curr_list()
        {
            return this->mobilitypath_v;
        }
        std::map<std::string, message_services::models::mobilitypath> &mobilitypath_worker::get_curr_map()
        {
            return this->mobilitypath_m;
        }
        void mobilitypath_worker::process_incoming_msg(const std::string json_str)
        {
            message_services::models::mobilitypath mobilitypath_obj;
            if (mobilitypath_obj.fromJson(json_str.c_str()))
            {
                std::unique_lock<std::mutex> lck(worker_mtx);
                // this->mobilitypath_v.push_back(mobilitypath_obj);
                spdlog::info("MP hash id = {0}",mobilitypath_obj.generate_hash_sender_timestamp_id(mobilitypath_obj.getHeader().sender_id, mobilitypath_obj.getHeader().timestamp/this->MOBILITY_OPERATION_PATH_MAX_DURATION));
                this->mobilitypath_m.insert({mobilitypath_obj.generate_hash_sender_timestamp_id(mobilitypath_obj.getHeader().sender_id, mobilitypath_obj.getHeader().timestamp/this->MOBILITY_OPERATION_PATH_MAX_DURATION), mobilitypath_obj});
            }
            else
            {
                spdlog::critical("mobilitypath_worker: Document parse error");
            }
        }

        void mobilitypath_worker::pop_cur_element_from_list(long element_position)
        {
            if (this->mobilitypath_v.size() > 0)
            {
                std::unique_lock<std::mutex> lck(worker_mtx);
                if (element_position == 0)
                {
                    this->mobilitypath_v.pop_front();
                }
                else
                {
                    this->mobilitypath_v.erase(this->mobilitypath_v.begin() + element_position);
                }
            }
        }
    }
}