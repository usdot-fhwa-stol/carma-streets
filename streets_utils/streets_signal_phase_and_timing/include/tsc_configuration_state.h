#pragma once

#include <rapidjson/rapidjson.h>
#include <rapidjson/document.h>
#include <rapidjson/writer.h>
#include <rapidjson/stringbuffer.h>
#include <spdlog/spdlog.h>
#include "signal_phase_and_timing_exception.h"

namespace signal_phase_and_timing
{
    struct signal_group_configuration
    {
        /**
        * @brief Signal Group ID
        */
        uint8_t signal_group_id = 0;

        /**
        * @brief Yellow change duration for the signal_group_id, from the traffic signal controller, in milliseconds.
        */
        uint16_t yellow_change_duration = -1;

        /**
        * @brief Red Clearance duration for the signal_group_id, from the traffic signal controller. In milliseconds.
        */
        uint16_t red_clearance = -1;

        /**
        * @brief List of signal groups which can be green concurrently with the signal group in message.
        * These represent phases in the same barrier but on a different ring than the signal group for which it is defined.
        */
        std::vector<uint8_t> concurrent_signal_groups;

        /**
         * @brief Serialize signal_group_configuration object to rapidjson::Value for writing as JSON string
         * 
         * @return rapidjson::Value serialize signal_group_configuration object
        */
        rapidjson::Value toJson(rapidjson::Document::AllocatorType &allocator) const;
        
        /**
         * @brief Deserialize signal_group_configuration JSON into tsc_configuration_state object.
         * 
         * @param val signal_group_configuration JSON.
         */
        void fromJson(const rapidjson::Value &val);
    };

    struct tsc_configuration_state
    {
        std::vector<signal_group_configuration> tsc_config_list;

        /**
         * @brief Serialize tsc_configuration_state object to rapidjson::Value for writing as JSON string
         * 
         * @return rapidjson::Value serialize tsc_configuration_state object
        */
        std::string toJson() const;
        
        /**
         * @brief Deserialize tsc_configuration_state JSON into tsc_configuration_state object.
         * 
         * @param json tsc_configuration_state JSON string.
         */
        void fromJson(const std::string &json);


    };
}