#pragma once


#include "intersection_state.h"
#include "streets_desired_phase_plan.h"
#include "tsc_configuration_state.h"
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
#include <algorithm>

namespace signal_phase_and_timing{
    struct spat{
        /**
         * @brief Timestamp in minutes of the UTC year (see J2735 SPaT message documentation for reference).
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
         * @brief Update spat object data using ntcip_1202_ext data received via UDP socket. 
         * Bool flag to control whether to use ntcip_1202_ext message provided timestamp or 
         * host unix timestamp information.
         * 
         * @param ntcip_data bytes from UDP socket, read into a struct
         * @param use_ntcip_timestamp Bool flag to control whether to use ntcip_1202_ext message provided timestamp or 
         * host unix timestamp information. If true will use message timestamp. 
         */
        void update(ntcip::ntcip_1202_ext &ntcip_data, bool use_ntcip_timestamp );

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
         * @param second_of_day NTCIP message timestamp information (seconds of the current UTC day).
         * @param millisecond_of_second NTCIP message timestamp information (millisecond of the current second).
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
         * @param ntcip_data 
         */
        void update_intersection_state( ntcip::ntcip_1202_ext &ntcip_data );

         // Conversion constants
        /**
         * @brief Process first desired green signal group and duration in desired phase plan. Populate 
         * movement_event list of current movement_state with movement_events for first entry in desired 
         * phase plan.
         * 
         * @param cur_movement_state_ref reference to movement_state inside SPaT for which to populate
         * movement_event list.
         * @param desired_sg_green_timing first entry in desired phase plan which contains information 
         * about a green duration and the signal groups to which it applies.
         * @param sg_yellow_duration_red_clearnace_map_ptr tsc_state information about traffic signal controller configuration including
         * red clearance and yellow change duraction.
         */
        void process_first_desired_green(movement_state &cur_movement_state_ref, 
                                        const streets_desired_phase_plan::signal_group2green_phase_timing &desired_sg_green_timing,
                                        const std::shared_ptr<std::unordered_map<int,streets_tsc_configuration::signal_group_configuration>> sg_yellow_duration_red_clearnace_map_ptr
                                        );
        /**
         * @brief Process all desired green signal group and duration in desired phase plan that are not the first entry.
         * Populate movement_event list of current movement_state with movement_events for all entries after first entry
         * in desired phase plan.
         * 
         * @param cur_movement_state_ref reference to movement_state inside SPaT for which to populate 
         * movement_event list.
         * @param desired_sg_green_timing entry in desired phase plan which contains information 
         * about a green duration and the signal groups to which it applies.
         * @param sg_yellow_duration_red_clearnace_map_ptr tsc_state information about traffic signal controller configuration including
         * red clearance and yellow change duraction.
         */
        void process_second_onward_desired_green(signal_phase_and_timing::movement_state &cur_movement_state_ref, 
                                                const streets_desired_phase_plan::signal_group2green_phase_timing &desired_sg_green_timing,
                                                const std::shared_ptr<std::unordered_map<int,streets_tsc_configuration::signal_group_configuration>> sg_yellow_duration_red_clearnace_map_ptr
                                                ) ;
        /**
         * @brief Populate movement_event with movement_phase_state, start_time and end_time(min_end_time).
         * 
         * @param movement_event_to_populate reference to movement_event to populate.
         * @param phase_state enumeration to describe phase state.
         * @param start_time_epoch start time of movement event in milliseconds epoch time.
         * @param end_time_epoch end time of movement event in milliseconds epoch time.
         */
        void populate_movement_event(
                                    signal_phase_and_timing::movement_event &movement_event_to_populate, 
                                    const signal_phase_and_timing::movement_phase_state &phase_state, 
                                    const uint64_t start_time_epoch, 
                                    const uint64_t end_time_epoch);
        /**
         * @brief Populate movement_event list for given movement_state with green event, yellow change event, and red clearance 
         * event.
         * 
         * @param cur_movement_state_ref reference to movement_state.
         * @param desired_sg_green_timing desired phase plan entry for which to populate green, yellow and red clearance events.
         * @param desired_yellow_duration yellow duration configured for signal group of movement_state.
         * @param desired_red_clearance red clearance configured for signal group of movement_state.
         */
        void append_full_green_yellow_red_phases_by_desired_green(signal_phase_and_timing::movement_state &cur_movement_state_ref, 
                                                                const streets_desired_phase_plan::signal_group2green_phase_timing &desired_sg_green_timing, 
                                                                const int desired_yellow_duration,
                                                                const int desired_red_clearance);
        /**
         * @brief Find the signal group id for the signal group in a pair (movement group) that will have the largest yellow change plus red clearance time 
         * interval. This is useful since the largest yellow change plus red clearance will control how long all red events during this time interval will last.
         * 
         * @param desired_signal_groups a pair of signal groups that make up a movement group.
         * @param sg_yellow_duration_red_clearnace_map_ptr A pointer for traffic signal controller configuration informaton including yellow change and red intervals for all
         * signal groups.
         * @return int the signal group id which has the largest combination of yellow change and red clearance time interval.
         */
        int find_max_desired_yellow_duration_red_clearance_pair(std::vector<int> desired_signal_groups, const std::shared_ptr<std::unordered_map<int, streets_tsc_configuration::signal_group_configuration>> sg_yellow_duration_red_clearnace_map_ptr) const;    

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