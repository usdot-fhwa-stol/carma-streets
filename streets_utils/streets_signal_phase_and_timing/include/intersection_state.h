#pragma once

#include "movement_state.h"
#include "connection_maneuver_assist.h"
#include "signal_phase_and_timing_exception.h"
#include "ntcip_1202_ext.h"
#include <rapidjson/rapidjson.h>
#include <rapidjson/document.h>
#include <spdlog/spdlog.h>
#include <list>
#include <chrono>


namespace signal_phase_and_timing
{
    struct intersection_state
    {
        int MIN_TO_SECONDS = 60;
        int SECOND_TO_MILLISECONDS = 1000;

        /**
         * @brief Descriptive name for this collection.
         */
        std::string name;

        /**
         * @brief A globally unique value set and consisting of regionID and intersection ID assignment. Provides
         * unique mapping to the intersection MAP in question which provides complete location and approach/move/lane
         * data
         */
        uint16_t id = 0;

        uint8_t revision = 0;

        /**
         * @brief BIT String of IntersectionStatusObject:
         * 
         * manualControlIsEnabled                (0),
         *   -- Timing reported is per programmed values, etc. but person
         *   -- at cabinet can manually request that certain intervals are
         *   -- terminated early (e.g. green).
         * stopTimeIsActivated                   (1),
         *   -- And all counting/timing has stopped.
         * failureFlash                          (2), 
         *   -- Above to be used for any detected hardware failures, 
         *   -- e.g. conflict monitor as well as for police flash 
         * preemptIsActive                       (3),
         * signalPriorityIsActive                (4),   
	     *   -- Additional states
         * fixedTimeOperation                    (5),
         *   -- Schedule of signals is based on time only 
         *   -- (i.e. the state can be calculated)
         * trafficDependentOperation             (6),
         *   -- Operation is based on different levels of traffic parameters 
         *   -- (requests, duration of gaps or more complex parameters)
         * standbyOperation                      (7),
         *   -- Controller: partially switched off or partially amber flashing
         * failureMode                           (8),
         *   -- Controller has a problem or failure in operation
         * off                                   (9),
         *   -- Controller is switched off 
         *   -- Related to MAP and SPAT bindings
         * recentMAPmessageUpdate                (10),
         *   -- Map revision with content changes
         * recentChangeInMAPassignedLanesIDsUsed (11),
         *   -- Change in MAP's assigned lanes used (lane changes)
         *   -- Changes in the active lane list description
         * noValidMAPisAvailableAtThisTime       (12),
         *   -- MAP (and various lanes indexes) not available
         * noValidSPATisAvailableAtThisTime      (13)
         *   -- SPAT system is not working at this time  
         *   -- Bits 14,15 reserved at this time and shall be zero
         * } (SIZE(16))
         *
         */
        uint8_t status;
        /**
         * @brief Minute of current UTC year.
         */
        uint32_t moy = 0;
        /**
        *  @brief Millisecond point in the current UTC minute that this message was created
        **/
        uint16_t time_stamp = 0;
        /**
         * @brief A list of unique lane ID for each lane object which is active. Refers to dynamic MAP contents.
         */
        std::list<int> enabled_lanes;
        /**
         * @brief Each Movement is given in turn and contains its signal phase state, mapping to the lanes it applies
         * to, and point in time it will and. It may contain both active and future states 
         * 
         */
        std::list<movement_state> states;
        /**
         * @brief Assist Data
         * 
         */
        std::list<connection_maneuver_assist> maneuver_assist_list;

        /**
         * @brief Serialize Intersection State object to rapidjson::Value for writing as JSON string
         * 
         * @return rapidjson::Value serialize Intersection State object
         */
        rapidjson::Value toJson(rapidjson::Document::AllocatorType &allocator) const;
        /**
         * @brief Deserialize Intersection State JSON into Intersection State object.
         * 
         * @param val Intersection State JSON.
         */
        void fromJson(const rapidjson::Value &val);
        /**
         * @brief Equals operator to asses whether two objects contain equivalent data.
         * 
         * @param compare second object to compare current object with.
         * @return true if both object contain equivalent data.
         * @return false if not.
         */
        bool operator==(const intersection_state &other) const;
         /**
         * @brief Returns the inverse of equals operator.
         * 
         * @param other second object to compare current object with.
         * @return true if both objects do not contain equivalent data.
         * @return false if both objects do contain equivalent data.
         */
        bool operator!=(const intersection_state &other) const;
        /**
         * @brief Convert current mimutes of the year and milliseconds of the current minute into epoch timestamp in milliseconds
         * 
         * @param moy_ll The current minutes of the year
         * @param min_mills_timestamp  The milliseconds of the current minute
         * @return uint64_t epoch timestamp in milliseconds
         */
        uint64_t convert_min_mills2epoch_ts(uint32_t moy_ll ,uint16_t min_mills_timestamp) const;

        /**
         * @brief Get the epoch timestamp of the intersection state timestamp
         * 
         * @return ** uint64_t epoch timestamp in unit of milliseconds
         */
        uint64_t get_epoch_timestamp() const;

        /**
         * @brief Set timestamp for intersection state based on NTCIP SPaT UDP message timestamp which is
         * transmitted as second of the UTC day and millisecond of the UTC second. This information can be
         * combined with the unix host time day of the year to get minute of the UTC year and millisecond of 
         * the UTC minute (J2735 Intersection State timeStamp units).
         * 
         * @param second_of_day second of the UTC year.
         * @param millisecond_of_second millisecond of the UTC second.
         */
        void set_timestamp_ntcip(const uint32_t second_of_day , const uint16_t millisecond_of_second );

        /**
         * @brief Set timestamp for intersection state based on host machine unix time.
         * 
         */
        void set_timestamp_local();

        /**
         * @brief Method to update movement_state with signal_group_id using ntcip spat data.
         * 
         * @param spat_data NTCIP SPaT data.
         * @param signal_group_id signal group id of movement to update. 
         */
        void update_movement_state( ntcip::ntcip_1202_ext &spat_data, const int signal_group_id, const int phase_number);

        /**
         * @brief Clears old movement event data for all movement_states. Adds one movement_event to the state_time_speed
         * list as the current event for each movement state. Then calls update_movement_state on every phase number/signal
         * group mapping included in the phase_number to signal group map. This method is meant to be executed each time a
         * nticp_1202_ext UDP packet is received to update the current movement event data of each phase with the timing information
         * received.
         * 
         * @param spat_data UDP TSC information.
         * @param phase_number_to_signal_group phase number to signal group map. 
         */
        void update_movements( ntcip::ntcip_1202_ext &spat_data,const std::unordered_map<int,int> &phase_number_to_signal_group );
        /**
         * @brief Return reference to movement_state for signal group
         * 
         * @param signal_group_id 
         * @return movement_state& 
         */
        movement_state& get_movement(const int signal_group_id);
        /**
         * @brief Initialize list<movement_state> to include a movement for each phase_numer/signal_group entry in the phase number
         * to signal group map. This method is meant to be run once, while initializing the spat object to contain movement states
         * for all relevant signal groups.
         * 
         * @param phase_number_to_signal_group std::unordered_map<int,int> of phase number (keys) to signal group (values). Map used to translate 
         * phase_number (NTCIP) to signal group (J2735).
         */
        void initialize_movement_states(const std::unordered_map<int,int> &phase_number_to_signal_group);
        /**
         * @brief Convert time offset (in tenths of seconds) from current time (NTCIP SPaT timing information units) to
         * tenths of seconds from current hour (J2735 SPaT timing information units). For reference please see J2735 and 
         * NTCIP documentation.
         * 
         * @param offset_tenths_of_sec tenths of seconds from current time.
         * @return uint16_t tenths of seconds from current hour.
         */
        uint16_t convert_offset(const uint16_t offset_tenths_of_sec ) const;
    };

}