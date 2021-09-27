
#ifndef VEHICLE_STATUS_INTENT_H
#define VEHICLE_STATUS_INTENT_H

#include <iostream>
#include <vector>

#include "baseMessage.h"

namespace message_services
{

    namespace models
    {
        typedef struct est_path
        {
            uint32_t lanelet_id = 0;
            double distance_to_end_of_lanelet = 0.0;
            long timestamp = 0;
        } est_path_t;

        class vehicle_status_intent : public baseMessage
        {
            friend std::ostream &operator<<(std::ostream &out, vehicle_status_intent &vehicle_status_intent_obj);

        private:
            std::string vehicle_id;

            /***
             *  bsm_id is not serialized as json since it is not published as part of vehicle status and intent message.
             * It is used to connect BSM with MobilityPath and MobilityOperation message
            */
            std::string bsm_id;
            double cur_speed;
            double cur_accel;
            long cur_timestamp;
            long react_timestamp;
            double max_accel;
            double max_decel;
            uint32_t vehicle_length;
            double minimum_gap;
            long depart_position;
            bool is_allowed;
            uint32_t cur_lanelet_id;
            double cur_distance;
            uint32_t enter_lanelet_id;
            uint32_t dest_lanelet_id;
            uint32_t link_lanelet_id;
            long earliest_stop_timestamp;
            long actual_stop_timestamp;
            long actual_enter_timestamp;
            long actual_depart_timestamp;
            std::vector<est_path_t> est_path_v;

        public:
            vehicle_status_intent();
            virtual ~vehicle_status_intent();

            virtual void fromJsonObject(const rapidjson::Value &obj);
            virtual bool asJsonObject(rapidjson::Writer<rapidjson::StringBuffer> *writer) const;

            //getters and setters
            std::string getVehicle_id() const;
            void setVehicle_id(std::string vehicle_id);
            double getCur_speed() const;
            void setCur_speed(double cur_speed);
            double getCur_accel() const;
            void setCur_accel(double cur_accel);
            std::vector<est_path_t> getEst_path_v() const;
            void setEst_path_v(std::vector<est_path_t> est_path_v);
            long getActual_enter_timestamp() const;
            void setActual_enter_timestamp(long actual_enter_timestamp);
            long getActual_stop_timestamp() const;
            void setActual_stop_timestamp(long actual_stop_timestamp);
            long getEarliest_stop_timestamp() const;
            void setEarliest_stop_timestamp(long earliest_stop_timestamp);
            uint32_t getLink_lanelet_id() const;
            void setLink_lanelet_id(uint32_t link_lanelet_id);
            uint32_t getDest_lanelet_id() const;
            void setDest_lanelet_id(uint32_t dest_lanelet_id);
            uint32_t getEnter_lanelet_id() const;
            void setEnter_lanelet_id(uint32_t enter_lanelet_id);
            double getCur_distance() const;
            void setCur_distance(double cur_distance);
            uint32_t getCur_lanelet_id() const;
            void setCur_lanelet_id(uint32_t cur_lanelet_id);
            bool getIs_allowed() const;
            void setIs_allowed(bool is_allowed);
            long getDepart_position() const;
            void setDepart_position(long depart_position);
            double getMinimum_gap() const;
            void setMinimum_gap(double minimum_gap);
            uint32_t getVehicle_length() const;
            void setVehicle_length(uint32_t vehicle_length);
            double getMax_decel() const;
            void setMax_decel(double max_decel);
            double getMax_accel() const;
            void setMax_accel(double max_accel);
            long getReact_timestamp() const;
            void setReact_timestamp(long react_timestamp);
            long getActual_depart_timestamp() const;
            void setActual_depart_timestamp(long actual_depart_timestamp);
            long getCur_timestamp() const;
            void setCur_timestamp(long cur_timestamp);
            std::string getBsm_id();
            void setBsm_id(std::string bsm_id);
        };
    }

}

#endif