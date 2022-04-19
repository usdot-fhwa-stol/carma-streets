#include <iostream>
#include <string>

#include "vehicle_status_intent.h"

namespace message_services
{

    namespace models
    {

        vehicle_status_intent::vehicle_status_intent() : vehicle_id(""), vehicle_length(0), is_allowed(false), cur_accel(0),
                                                         cur_distance(0), cur_lanelet_id(0), cur_speed(0), react_timestamp(0),
                                                         max_accel(0), max_decel(0), minimum_gap(0), depart_position(0), cur_timestamp(0),
                                                         enter_lanelet_id(0), dest_lanelet_id(0), link_lanelet_id(0), actual_stop_timestamp(0),
                                                         earliest_stop_timestamp(0), actual_enter_timestamp(0), actual_depart_timestamp(0)
        {
        }

        vehicle_status_intent::~vehicle_status_intent() {}

        void vehicle_status_intent::fromJsonObject(const rapidjson::Value &obj)
        {
            if (obj.IsObject())
            {
                for (rapidjson::Value::ConstMemberIterator obj_itr = obj.MemberBegin(); obj_itr != obj.MemberEnd(); obj_itr++)
                {

                    if (obj_itr->value.IsObject() && std::string(obj_itr->name.GetString()) == std::string("temprary_id"))
                    {
                        for (rapidjson::Value::ConstMemberIterator obj_itr = obj_itr->value.MemberBegin(); obj_itr != obj_itr->value.MemberEnd(); obj_itr++)
                        {
                            if (std::string(obj_itr->name.GetString()) == std::string("v_id"))
                            {
                                setVehicle_id(obj_itr->value.GetString());
                            }

                            if (std::string(obj_itr->name.GetString()) == std::string("cur_speed"))
                            {
                                setCur_speed(obj_itr->value.GetDouble());
                            }

                            if (std::string(obj_itr->name.GetString()) == std::string("cur_timestamp"))
                            {
                                setCur_timestamp(obj_itr->value.GetDouble());
                            }

                            if (std::string(obj_itr->name.GetString()) == std::string("is_allowed_int"))
                            {
                                setIs_allowed(obj_itr->value.GetBool());
                            }

                            if (std::string(obj_itr->name.GetString()) == std::string("min_gap"))
                            {
                                setMinimum_gap(obj_itr->value.GetDouble());
                            }

                            if (std::string(obj_itr->name.GetString()) == std::string("v_length"))
                            {
                                setVehicle_length(obj_itr->value.GetUint());
                            }

                            if (std::string(obj_itr->name.GetString()) == std::string("cur_accel"))
                            {
                                setCur_accel(obj_itr->value.GetDouble());
                            }

                            if (std::string(obj_itr->name.GetString()) == std::string("max_decel"))
                            {
                                setMax_decel(obj_itr->value.GetDouble());
                            }

                            if (std::string(obj_itr->name.GetString()) == std::string("max_accel"))
                            {
                                setMax_accel(obj_itr->value.GetDouble());
                            }

                            //Todo: Complete my other fields
                        }
                    }
                }
            }
        }
        bool vehicle_status_intent::asJsonObject(rapidjson::Writer<rapidjson::StringBuffer> *writer) const
        {
            try
            {
                writer->StartObject();
                writer->String("metadata");
                writer->StartObject();
                writer->String("timestamp");
                writer->Uint64(this->getCur_timestamp());
                writer->EndObject();
                writer->String("payload");
                writer->StartObject();
                writer->String("v_id");
                writer->String(this->getVehicle_id().c_str());
                writer->String("v_length");
                writer->Uint64(this->getVehicle_length());
                writer->String("cur_speed");
                writer->Double(this->getCur_speed());
                writer->String("cur_accel");
                writer->Double(this->getCur_accel());
                writer->String("react_t");
                writer->Double(this->getReact_timestamp());
                writer->String("max_accel");
                writer->Double(this->getMax_accel());
                writer->String("max_decel");
                writer->Double(this->getMax_decel());
                writer->String("min_gap");
                writer->Double(this->getMinimum_gap());
                writer->String("depart_pos");
                writer->Uint64(this->getDepart_position());
                writer->String("is_allowed");
                writer->Bool(this->getIs_allowed());
                writer->String("cur_lane_id");
                writer->Int64(this->getCur_lanelet_id());
                writer->String("cur_ds");
                writer->Double(this->getCur_distance());
                writer->String("entry_lane_id");
                writer->Int64(this->getEnter_lanelet_id());
                writer->String("dest_lane_id");
                writer->Int64(this->getDest_lanelet_id());
                writer->String("link_lane_id");
                writer->Int64(this->getLink_lanelet_id());
                //Vehicle turn direction at the intersection
                writer->String("direction");
                writer->String(this->GetTurn_direction().c_str());
                if (this->est_path_v.size() > 0)
                {
                    writer->String("est_paths");
                    writer->StartArray();
                    for (auto est_path_item : this->getEst_path_v())
                    {
                        writer->StartObject();
                        //Lanelet id
                        writer->String("id");
                        writer->Int64(est_path_item.lanelet_id);
                        //Distance to the end of the lanelet where the future position is
                        writer->String("ds");
                        writer->Double(est_path_item.distance);
                        //Timestamp when the future position is
                        writer->String("ts");
                        writer->Uint64(est_path_item.timestamp);
                        writer->EndObject();
                    }
                    writer->EndArray();
                }
                writer->EndObject();
                writer->EndObject();
                return true;
            }
            catch (std::exception &ex)
            {
                SPDLOG_INFO("vehicle_status_intent model: asJsonObject error. ", ex.what());
                return false;
            }
        }

        std::ostream &operator<<(std::ostream &out, vehicle_status_intent &vehicle_status_intent_obj)
        {
            out << "vehicle_status_intent model: vehicle_id  = " << vehicle_status_intent_obj.vehicle_id << ", current_timestamp = " << vehicle_status_intent_obj.cur_timestamp << ", bsm_id =" << vehicle_status_intent_obj.bsm_id << ", current speed= " << vehicle_status_intent_obj.cur_speed << std::endl;
            return out;
        }

        //getters and setters

        std::string vehicle_status_intent::getVehicle_id() const
        {
            return this->vehicle_id;
        }
        void vehicle_status_intent::setVehicle_id(std::string vehicle_id)
        {
            this->vehicle_id = vehicle_id;
        }
        double vehicle_status_intent::getCur_speed() const
        {
            return this->cur_speed;
        }
        void vehicle_status_intent::setCur_speed(double cur_speed)
        {
            this->cur_speed = cur_speed;
        }

        long vehicle_status_intent::getActual_depart_timestamp() const
        {
            return this->actual_depart_timestamp;
        }
        void vehicle_status_intent::setActual_depart_timestamp(long actual_depart_timestamp)
        {
            this->actual_depart_timestamp = actual_depart_timestamp;
        }
        double vehicle_status_intent::getCur_accel() const
        {
            return this->cur_accel;
        }
        void vehicle_status_intent::setCur_accel(double cur_accel)
        {
            this->cur_accel = cur_accel;
        }
        std::vector<est_path_t> vehicle_status_intent::getEst_path_v() const
        {
            return this->est_path_v;
        }
        void vehicle_status_intent::setEst_path_v(std::vector<est_path_t> est_path_v)
        {
            this->est_path_v = est_path_v;
        }

        long vehicle_status_intent::getActual_enter_timestamp() const
        {
            return this->actual_enter_timestamp;
        }
        void vehicle_status_intent::setActual_enter_timestamp(long actual_enter_timestamp)
        {
            this->actual_enter_timestamp = actual_enter_timestamp;
        }

        long vehicle_status_intent::getActual_stop_timestamp() const
        {
            return this->actual_stop_timestamp;
        }
        void vehicle_status_intent::setActual_stop_timestamp(long actual_stop_timestamp)
        {
            this->actual_stop_timestamp = actual_stop_timestamp;
        }

        long vehicle_status_intent::getEarliest_stop_timestamp() const
        {
            return this->earliest_stop_timestamp;
        }
        void vehicle_status_intent::setEarliest_stop_timestamp(long earliest_stop_timestamp)
        {
            this->earliest_stop_timestamp = earliest_stop_timestamp;
        }

        int64_t vehicle_status_intent::getLink_lanelet_id() const
        {
            return this->link_lanelet_id;
        }
        void vehicle_status_intent::setLink_lanelet_id(int64_t link_lanelet_id)
        {
            this->link_lanelet_id = link_lanelet_id;
        }
        long vehicle_status_intent::getCur_timestamp() const
        {
            return this->cur_timestamp;
        }
        void vehicle_status_intent::setCur_timestamp(long cur_timestamp)
        {
            this->cur_timestamp = cur_timestamp;
        }
        int64_t vehicle_status_intent::getDest_lanelet_id() const
        {
            return this->dest_lanelet_id;
        }
        void vehicle_status_intent::setDest_lanelet_id(int64_t dest_lanelet_id)
        {
            this->dest_lanelet_id = dest_lanelet_id;
        }

        int64_t vehicle_status_intent::getEnter_lanelet_id() const
        {
            return this->enter_lanelet_id;
        }
        void vehicle_status_intent::setEnter_lanelet_id(int64_t enter_lanelet_id)
        {
            this->enter_lanelet_id = enter_lanelet_id;
        }

        double vehicle_status_intent::getCur_distance() const
        {
            return this->cur_distance;
        }
        void vehicle_status_intent::setCur_distance(double cur_distance)
        {
            this->cur_distance = cur_distance;
        }

        int64_t vehicle_status_intent::getCur_lanelet_id() const
        {
            return this->cur_lanelet_id;
        }
        void vehicle_status_intent::setCur_lanelet_id(int64_t cur_lanelet_id)
        {
            this->cur_lanelet_id = cur_lanelet_id;
        }

        bool vehicle_status_intent::getIs_allowed() const
        {
            return this->is_allowed;
        }
        void vehicle_status_intent::setIs_allowed(bool is_allowed)
        {
            this->is_allowed = is_allowed;
        }

        long vehicle_status_intent::getDepart_position() const
        {
            return this->depart_position;
        }
        void vehicle_status_intent::setDepart_position(long depart_position)
        {
            this->depart_position = depart_position;
        }

        double vehicle_status_intent::getMinimum_gap() const
        {
            return this->minimum_gap;
        }
        void vehicle_status_intent::setMinimum_gap(double minimum_gap)
        {
            this->minimum_gap = minimum_gap;
        }

        uint32_t vehicle_status_intent::getVehicle_length() const
        {
            return this->vehicle_length;
        }
        void vehicle_status_intent::setVehicle_length(uint32_t vehicle_length)
        {
            this->vehicle_length = vehicle_length;
        }

        double vehicle_status_intent::getMax_decel() const
        {
            return this->max_decel;
        }
        void vehicle_status_intent::setMax_decel(double max_decel)
        {
            this->max_decel = max_decel;
        }

        double vehicle_status_intent::getMax_accel() const
        {
            return this->max_accel;
        }
        void vehicle_status_intent::setMax_accel(double max_accel)
        {
            this->max_accel = max_accel;
        }

        double vehicle_status_intent::getReact_timestamp() const
        {
            return this->react_timestamp;
        }
        void vehicle_status_intent::setReact_timestamp(double react_timestamp)
        {
            this->react_timestamp = react_timestamp;
        }

        std::string vehicle_status_intent::getBsm_id()
        {
            return this->bsm_id;
        }

        void vehicle_status_intent::setBsm_id(std::string bsm_id)
        {
            this->bsm_id = bsm_id;
        }

        std::string vehicle_status_intent::GetTurn_direction() const
        {
            return this->turn_direction;
        }

        void vehicle_status_intent::SetTurn_direction(std::string turn_direction)
        {
            this->turn_direction = turn_direction;
        }
    }
}