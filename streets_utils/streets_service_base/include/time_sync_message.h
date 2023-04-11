#pragma once

#include <rapidjson/rapidjson.h>
#include <rapidjson/document.h>
#include <spdlog/spdlog.h>
#include <rapidjson/writer.h>
#include <rapidjson/stringbuffer.h>

namespace streets_service::simulation {
    struct time_sync_message{
        uint64_t timestep;
        uint64_t seq;

        std::string toJson() const;

        void fromJson(const std::string &json );

    };
}