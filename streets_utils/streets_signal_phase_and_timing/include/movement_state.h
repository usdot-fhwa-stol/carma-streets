#pragma once


#include "movement_event.h"
#include "connection_maneuver_assist.h"
#include "signal_phase_and_timing_exception.h"

#include <rapidjson/rapidjson.h>
#include <rapidjson/document.h>
#include <spdlog/spdlog.h>


namespace signal_phase_and_timing{
    struct movement_state{
        /**
         * @brief Uniquely defines movement by name human readable for intersection to used only 
         * in debug mode.
         */
        std::string movement_name;
        /**
         * @brief The group id is used to map to list of lanes (and their descriptions) which this
         * MovementState data applies to.
         */
        uint8_t signal_group = 0;
        /**
         * @brief Consisting of sets of movement data with:
         *  SignalPhaseState
         *  TimeChangeDetails
         *  Advisory Speeds  (optional)
         * Note one or more of the movement events may be for a future time and that this allows conveying
         * multiple predictive phase and movement timing for various uses for the current signal group.
         */
        std::list<movement_event> state_time_speed;
        /**
         * @brief This information may also be placed in the IntersectionState when common information applies
         * to different lanes in the same way.
         */
        std::list<connection_maneuver_assist> maneuver_assist_list;

        /**
         * @brief Serialize Movement State object to rapidjson::Value for writing as JSON string
         * 
         * @return rapidjson::Value serialize SPaT object
         */
        rapidjson::Value toJson(rapidjson::Document::AllocatorType &allocator) const;
        /**
         * @brief Deserialize Movement State JSON into Movement State object.
         * 
         * @param val  Movement State JSON.
         */
        void fromJson(const rapidjson::Value &val);
        /**
         * @brief Equals operator to asses whether two objects contain equivalent data.
         * 
         * @param compare second object to compare current object with.
         * @return true if both object contain equivalent data.
         * @return false if not.
         */
        bool operator==(const movement_state &other) const;
         /**
         * @brief Returns the inverse of equals operator.
         * 
         * @param other second object to compare current object with.
         * @return true if both objects do not contain equivalent data.
         * @return false if both objects do contain equivalent data.
         */
        bool operator!=(const movement_state &other) const;
    
    };

}