#pragma once

#include <spdlog/spdlog.h>
#include <rapidjson/rapidjson.h>
#include <rapidjson/writer.h>
#include <rapidjson/stringbuffer.h>
#include "intersection_schedule.h"

namespace streets_vehicle_scheduler {
    /**
     * @brief Object to represent schedule for single vehicle.
     */
    struct signalized_vehicle_schedule : vehicle_schedule {
        /**
         * @brief Construct a new signalized vehicle schedule object
         * 
         */
        signalized_vehicle_schedule() = default;
        /**
         * @brief Destroy the signalized vehicle schedule object
         * 
         */
        ~signalized_vehicle_schedule() override = default;
        /**
         * @brief Earliest possible entering time based on kinematic information and 
         * lanelet speed limit and length. entering time is the time since epoch in 
         * milliseconds when the vehicle will arrive at the stop bar.
         */
        u_int64_t eet = 0;
        /**
         * @brief Entering time considering the preceding vehicles and the modified spat. Entering time is the
         * time since epoch in milliseconds when the vehicle will enter the intersection box.
         */
        u_int64_t et = 0;
        /**
         * @brief Departure time considering the preceding vehicles and the modified spat. Departure time is the
         * time since epoch in milliseconds when the vehicle will depart from the intersection
         * box.
         */
        u_int64_t dt = 0;
        /**
         * @brief Vehicle State (@see @link streets_vehicles::vehicle @endlink)
         */
        streets_vehicles::vehicle_state state = streets_vehicles::vehicle_state::ND;
        /**
         * @brief Write vehicle schedule as rapidjson::Value.
         * @param allocator json document allocator.
         * @return rapidjson::Value 
         */
        rapidjson::Value toJson(rapidjson::Document::AllocatorType& allocator) const override;
    };

    
    /**
     * @brief Struct to store vehicle schedules for all vehicle in an intersection. Includes a timestamp in milliseconds
     * since epoch.
     */
    struct signalized_intersection_schedule : intersection_schedule {
        /**
         * @brief Construct a new signalized intersection schedule object
         * 
         */
        signalized_intersection_schedule() = default;
        /**
         * @brief Destroy the signalized intersection schedule object
         * 
         */
        ~signalized_intersection_schedule() override = default;
        /**
         * @brief Vector of all scheduled vehicles
         */
        std::vector<signalized_vehicle_schedule> vehicle_schedules;
        /**
         * @brief Method to turn schedule into CSV string. Each vehicle schedule is represented by a single row and all rows for 
         * a given schedule share a timestamp. The values in each row represent the following properties:
         * 
         *  timestamp ,v_id ,entry_lane ,link_id ,eet ,et, state
         * 
         * @return std::string CSV entry
         */
        std::string toCSV() const override;
        /**
         * @brief Method to write intersection schedule as JSON scheduling message.
         * 
         * @return std::string& reference.
         */
        std::string toJson() const override;

    };  
}