#pragma once

#include <rapidjson/rapidjson.h>
#include <rapidjson/document.h>
#include <spdlog/spdlog.h>
#include <rapidjson/writer.h>
#include <rapidjson/stringbuffer.h>

namespace streets_service::simulation {
    /**
     * @brief Data object for Time Synchronization message sent by CDASim
     * during simulation. This message is sent to all CARMA Streets services
     * via kafka and consumed to manage simulation time.
    */
    struct time_sync_message{
        /**
         * @brief Current time in ms.
         */
        uint64_t timestep;
        /**
         * @brief Sequential number of timestep.
         */
        uint64_t seq;
        /**
         * @brief Convert data object to json string message.
         * @return json string representation of time sync message data.
         */
        std::string toJson() const;
        /**
         * @brief Method to convert JSON time sync message in to data object.
         * @param json time sync message.
         * @throw std::runtime_error json is misformatted or is missing either required property.
         */
        void fromJson(const std::string &json );

    };
}