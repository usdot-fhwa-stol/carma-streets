#include "vehicle_status_intent_worker.h"

namespace message_services
{
    namespace workers
    {
        vehicle_status_intent_worker::vehicle_status_intent_worker() {}

        vehicle_status_intent_worker::~vehicle_status_intent_worker()
        {
            this->vsi_map_.clear();
        }

        std::map<std::string, models::vehicle_status_intent> &vehicle_status_intent_worker::get_curr_map()
        {
            return this->vsi_map_;
        }

        void vehicle_status_intent_worker::pop_element_from_map(std::string key)
        {
            this->vsi_map_.erase(key);
        }

        void vehicle_status_intent_worker::update_by_incoming_bsm_msg(models::bsm bsm)
        {
            std::unique_lock<std::mutex> lck(worker_mtx);
            std::map<std::string, models::vehicle_status_intent>::iterator itr = this->vsi_map_.begin();
            while (itr != this->vsi_map_.end())
            {
                if (itr->second.getBsm_id() == bsm.getCore_data().temprary_id)
                {
                    itr->second.setCur_speed(bsm.getCore_data().speed);
                    itr->second.setVehicle_length(bsm.getCore_data().size.length);
                    itr->second.setBsm_id(bsm.getCore_data().temprary_id);
                    //Todo other fields
                }
                ++itr;
            }
        }

        void vehicle_status_intent_worker::update_insert_by_incoming_mobilityoperation_msg(models::mobilityoperation mo)
        {
            std::unique_lock<std::mutex> lck(worker_mtx);
            std::map<std::string, models::vehicle_status_intent>::iterator itr = this->vsi_map_.begin();
            bool is_update = false;

            while (itr != this->vsi_map_.end())
            {
                if (itr->first == mo.getHeader().sender_id)
                {
                    is_update = true;
                    std::string is_allowed_int_str = mo.get_value_from_strategy_params("is_allowed_int");
                    if (is_allowed_int_str.length() > 0)
                    {
                        std::stringstream is_allowed_ss(is_allowed_int_str);
                        int is_allowed_int;
                        is_allowed_ss >> is_allowed_int;
                        itr->second.setIs_allowed(is_allowed_int);
                    }
                    itr->second.setVehicle_id(mo.getHeader().sender_id);
                    itr->second.setBsm_id(mo.getHeader().sender_bsm_id);
                    itr->second.setCur_timestamp(mo.getHeader().timestamp);
                    //Todo other fields
                    return;
                }
                ++itr;
            }

            if (!is_update)
            {
                models::vehicle_status_intent vsi_obj;
                std::string is_allowed_int_str = mo.get_value_from_strategy_params("is_allowed_int");
                if (is_allowed_int_str.length() > 0)
                {
                    std::stringstream is_allowed_ss(is_allowed_int_str);
                    int is_allowed_int;
                    is_allowed_ss >> is_allowed_int;
                    vsi_obj.setIs_allowed(is_allowed_int);
                }
                vsi_obj.setVehicle_id(mo.getHeader().sender_id);
                vsi_obj.setBsm_id(mo.getHeader().sender_bsm_id);
                vsi_obj.setCur_timestamp(mo.getHeader().timestamp);
                //Todo other fields

                vsi_map_[mo.getHeader().sender_id] = vsi_obj;
            }
        }
        void vehicle_status_intent_worker::update_insert_by_incoming_mobilitypath_msg(models::mobilitypath mp)
        {
            std::unique_lock<std::mutex> lck(worker_mtx);
            std::map<std::string, models::vehicle_status_intent>::iterator itr = this->vsi_map_.begin();
            bool is_update = false;
            while (itr != this->vsi_map_.end())
            {
                if (itr->first == mp.getHeader().sender_id)
                {
                    is_update = true;
                    itr->second.setVehicle_id(mp.getHeader().sender_id);
                    itr->second.setBsm_id(mp.getHeader().sender_bsm_id);
                    itr->second.setCur_timestamp(mp.getHeader().timestamp);
                    //Todo other fields
                }
                ++itr;
            }

            if (!is_update)
            {
                models::vehicle_status_intent vsi_obj;
                vsi_obj.setVehicle_id(mp.getHeader().sender_id);
                vsi_obj.setBsm_id(mp.getHeader().sender_bsm_id);
                vsi_obj.setCur_timestamp(mp.getHeader().timestamp);
                //Todo update other fields

                vsi_map_[mp.getHeader().sender_id] = vsi_obj;
            }
        }

        void vehicle_status_intent_worker::process_incoming_msg(const std::string json_str) {}
    }

}