#include "mobilityoperation_worker.h"
namespace message_services
{
    namespace workers
    {
        mobilityoperation_worker::mobilityoperation_worker(/* args */)
        {
        }

        mobilityoperation_worker::~mobilityoperation_worker()
        {
            this->mobilityoperation_v.clear();
        }

        std::deque<models::mobilityoperation> &mobilityoperation_worker::get_curr_list()
        {
            return this->mobilityoperation_v;
        }

        void mobilityoperation_worker::process_incoming_msg(const std::string json_str)
        {
            message_services::models::mobilityoperation mobilityoperation_obj;
            if (mobilityoperation_obj.fromJson(json_str.c_str()))
            {
                std::unique_lock<std::mutex> lck(worker_mtx);
                this->mobilityoperation_v.push_back(mobilityoperation_obj);
            }
            else
            {
                spdlog::critical("mobilityoperation_worker: Document parse error");
            }

        }
        void mobilityoperation_worker::pop_cur_element_from_list(long element_position)
        {
            if (this->mobilityoperation_v.size() > 0)
            {
                std::unique_lock<std::mutex> lck(worker_mtx);
                if (element_position == 0)
                {
                    this->mobilityoperation_v.pop_front();
                }
                else
                {
                    this->mobilityoperation_v.erase(this->mobilityoperation_v.begin() + element_position);
                }
            }
        }
    }
}