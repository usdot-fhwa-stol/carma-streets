#pragma once
#define RAPIDJSON_HAS_STDSTRING 1


#include "intersection_state.h"
#include <rapidjson/rapidjson.h>
#include <rapidjson/document.h>
#include <spdlog/spdlog.h>
#include <list>


namespace signal_phase_and_timing{
    struct spat{
        /**
         * @brief Timestamp Minute of the year
         */
        u_int64_t timestamp = 0;
        /**
         * @brief Descriptive name for this collection.
         */
        std::string name;
        /**
         * @brief Sets of SPaT data ( one per intersection).
         */
        std::list<intersection_state> intersection_state_list;
        /**
         * @brief Serialize SPaT object to rapidjson::Value for writing as JSON string
         * 
         * @return rapidjson::Value serialize SPaT object
         */
        rapidjson::Value toJson(rapidjson::Document::AllocatorType &allocator) const;
        /**
         * @brief Deserialize SPaT JSON into SPaT object.
         * 
         * @param val SPaT JSON.
         */
        void fromJson(const rapidjson::Value &val);

        bool operator==(const spat &other) const;

        bool operator!=(const spat &other) const;
    
    };

}