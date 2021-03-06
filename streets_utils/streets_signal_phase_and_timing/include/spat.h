#pragma once


#include "intersection_state.h"
#include "signal_phase_and_timing_exception.h"
#include <rapidjson/rapidjson.h>
#include <rapidjson/document.h>
#include <spdlog/spdlog.h>
#include <rapidjson/writer.h>
#include <rapidjson/stringbuffer.h>
#include <list>


namespace signal_phase_and_timing{
    struct spat{
        /**
         * @brief Timestamp Minute of the UTC year
         */
        u_int64_t timestamp = 0;
        /**
         * @brief Descriptive name for this collection.
         */
        std::string name;
        /**
         * @brief Sets of SPaT data ( one per intersection).
         */
        std::list<intersection_state> intersections;
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