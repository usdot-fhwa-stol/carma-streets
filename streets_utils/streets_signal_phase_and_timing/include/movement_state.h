#pragma once
#define RAPIDJSON_HAS_STDSTRING 1

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
        uint8_t signal_group_id = 0;
        /**
         * @brief Consisting of sets of movement data with:
         *  SignalPhaseState
         *  TimeChangeDetails
         *  Advisory Speeds  (optional)
         * Note one or more of the movement events may be for a future time and that this allows conveying
         * multiple predictive phase and movement timing for various uses for the current signal group.
         */
        std::list<movement_event> movement_event_list;
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

        bool operator==(const movement_state &other) const;

        bool operator!=(const movement_state &other) const;
    
    };

}