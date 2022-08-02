#pragma once


#include "intersection_state.h"
#include "signal_phase_and_timing_exception.h"
#include "ntcip_1202_ext.h"
#include <rapidjson/rapidjson.h>
#include <rapidjson/document.h>
#include <spdlog/spdlog.h>
#include <rapidjson/writer.h>
#include <rapidjson/stringbuffer.h>
#include <list>
#include <math.h>
#include <chrono>


namespace signal_phase_and_timing{
    struct spat{
        /**
         * @brief Timestamp Minute of the UTC year
         */
        u_int32_t timestamp = 0;
        /**
         * @brief Descriptive name for this collection.
         */
        std::string name;
        /**
         * @brief Sets of SPaT data ( one per intersection).
         */
        std::list<intersection_state> intersections;
        
        /**
         * @brief Map of phase number(NTCIP) to signal group(J2735).
         */
        std::unordered_map<int,int> phase_to_signal_group;

        /**
         * @brief Serialize SPaT object to rapidjson::Value for writing as JSON string
         * 
         * @return rapidjson::Value serialize SPaT object
         */
        std::string toJson() const;
        /**
         * @brief Deserialize SPaT JSON into SPaT object.
         * 
         * @param val SPaT JSON.
         */
        void fromJson(const std::string &json);
        /**
         * @brief Update spat object data using ntcip_1202_ext spat data received via UDP socket. 
         * Bool flag to control whether to use message provided timestamp or system timestamp.
         * 
         * @param spat_data bytes from UDP socket, read into a struct
         * @param use_spat_timestamp bool flag to control whether to use message timestamp or system 
         * time stamp. If true will use message timestamp. 
         */
        void update(ntcip::ntcip_1202_ext &spat_data, bool use_spat_timestamp );

        /**
         * @brief Method to initialize intersection information not provided in the ntcip SPaT UDP
         * message but necessary for the J2735 SPaT message. This method must be executed prior to 
         * using the update method.
         * 
         * @param intersection_name name of intersection
         * @param intersection_id J2735 intersection ID
         * @param phase_number_to_signal_group a map of phase numbers (NTCIP) to signal groups (J2735)
         */
        void initialize_intersection(const std::string &intersection_name, const int intersection_id, const std::unordered_map<int,int> &phase_number_to_signal_group);

        /**
         * @brief Method to set SPaT timestamp based on NTCIP message timestamp. The NTCIP message timestamp consists
         * of a seconds_of_day and a milliseconds of second field. This method will get the current day from system time
         * and then apply seconds and milliseconds informat as a offset for the current day.
         * 
         * @param second_of_day the message time of day in seconds
         * @param millisecond_of_second the message millisecond of second timestamp
         */
        void set_timestamp_ntcip(const uint32_t second_of_day , const uint16_t millisecond_of_second );

        /**
         * @brief Method to set SPaT timestamp based on system time
         * 
         */
        void set_timestamp_local();

        /**
         * @brief Method to update the front entry in the intersections list of
         * intersection_state(s) with the provided NTCIP SPaT data.
         * 
         * @param spat_data 
         */
        void update_intersection_state( ntcip::ntcip_1202_ext &spat_data );

        /**
         * @brief Equals operator to asses whether two objects contain equivalent data.
         * 
         * @param compare second object to compare current object with.
         * @return true if both object contain equivalent data.
         * @return false if not.
         */
        bool operator==(const spat &other) const;
        /**
         * @brief Returns the inverse of equals operator.
         * 
         * @param other second object to compare current object with.
         * @return true if both objects do not contain equivalent data.
         * @return false if both objects do contain equivalent data.
         */
        bool operator!=(const spat &other) const;
    
    };

}