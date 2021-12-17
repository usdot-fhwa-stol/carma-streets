#include "bsm_worker.h"

namespace message_services
{
    namespace workers
    {
        bsm_worker::bsm_worker()
        {
        }

        bsm_worker::~bsm_worker()
        {
            this->bsm_v.clear();
        }

        std::deque<models::bsm> &bsm_worker::get_curr_list()
        {
            return this->bsm_v;
        }

        std::map<std::string, message_services::models::bsm> &bsm_worker::get_curr_map()
        {
            return this->bsm_m;
        }
        void bsm_worker::process_incoming_msg(const std::string json_str)
        {
            message_services::models::bsm bsm_obj;
            if (bsm_obj.fromJson(json_str.c_str()))
            {
                std::unique_lock<std::mutex> lck(worker_mtx);
                std::string bsm_msg_id = bsm_obj.generate_hash_bsm_msg_id(bsm_obj.getCore_data().temprary_id, bsm_obj.getCore_data().msg_count, bsm_obj.getCore_data().sec_mark);
                if(!this->bsm_m.empty() &&  this->bsm_m.find(bsm_msg_id) != this->bsm_m.end())
                {
                    this->bsm_m.erase(bsm_msg_id);
                }
                this->bsm_m.insert({bsm_msg_id, bsm_obj});
            }
            else
            {
                spdlog::critical("bsm_worker: Document parse error");
            }
        }

        void bsm_worker::pop_cur_element_from_list(long element_position)
        {
            if (this->bsm_v.size() > 0)
            {
                std::unique_lock<std::mutex> lck(worker_mtx);
                if (element_position == 0)
                {
                    this->bsm_v.pop_front();
                }
                else
                {
                    this->bsm_v.erase(this->bsm_v.begin() + element_position);
                }
            }
        }
    }
}