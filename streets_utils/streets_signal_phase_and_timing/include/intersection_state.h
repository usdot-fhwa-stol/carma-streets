#pragma once
#define RAPIDJSON_HAS_STDSTRING 1

#include "movement_state.h"
#include "connection_maneuver_assist.h"
#include "signal_phase_and_timing_exception.h"
#include <rapidjson/rapidjson.h>
#include <rapidjson/document.h>
#include <spdlog/spdlog.h>
#include <list>

namespace signal_phase_and_timing
{
    struct intersection_state
    {

        /**
         * @brief Descriptive name for this collection.
         */
        std::string name;

        /**
         * @brief A globally unique value set and consisting of regionID and intersection ID assignment. Provides
         * unique mapping to the intersection MAP in question which provides complete location and approach/move/lane
         * data
         */
        int id;

        int message_count;

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
        std::string status;
        /**
         * @brief Minute of current UTC year
         */
        uint64_t minute_of_the_year = 0;
        /**
        *  @brief LSB units of of 10 mSec, 
        * with a range of 0.01 seconds to 10 minutes and 55.34 seconds
        * a value of 65534 to be used for 655.34 seconds or greater
        **/
        uint8_t second = 0;
        /**
         * @brief A list of unique lane ID for each lane object which is active. Refers to dynamic MAP contents.
         */
        std::list<int> enabled_lane_list;

        std::list<movement_state> movement_states;

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

        bool operator==(const intersection_state &other) const;

        bool operator!=(const intersection_state &other) const;

    };

}