#pragma once

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
        std::string status;
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

    };

}