#pragma once

#include <spdlog/spdlog.h>
#include <rapidjson/rapidjson.h>
#include <rapidjson/writer.h>
#include <rapidjson/stringbuffer.h>
#include "vehicle.h"

namespace streets_vehicle_scheduler {
    /**
     * @brief Object to represent schedule for single vehicle.
     */
    struct vehicle_schedule {
        
        /**
         * @brief Construct a new vehicle schedule object
         * 
         */
        vehicle_schedule() = default;
        /**
         * @brief Destroy the vehicle schedule object
         * 
         */
        virtual ~vehicle_schedule() = default;
        /**
         * @brief string unique static vehicle id.
         */
        std::string v_id;
        /**
         * @brief Entry lanelet id of vehicle 
         */
        int entry_lane =  0;
        /**
         * @brief Intended lanelet id of link lanelet used to travel through intersection.
         */
        int link_id = 0;
        /**
         * @brief Write vehicle schedule as rapidjson::Value.
         * @param allocator json document allocator.
         * @return rapidjson::Value 
         */
        virtual rapidjson::Value toJson(rapidjson::Document::AllocatorType& allocator) const = 0;
    };

    
    /**
     * @brief Struct to store vehicle schedules for all vehicle in an intersection. Includes a timestamp in milliseconds
     * since epoch. Also includes a get_delay method that will get each vehicle schedules delay and sum them for a total
     * intersection delay. Delay is the difference between scheduled Entering Time (et) and scheduled Stopping time (st)
     * and represents the time a vehicle would be waiting at the stop bar before gaining access to the intersection.
     * 
     */
    struct intersection_schedule {
        /**
         * @brief Construct a new intersection schedule object
         * 
         */
        intersection_schedule() = default;
        /**
         * @brief Destroy the intersection schedule object
         * 
         */
        virtual ~intersection_schedule() = default;
        /**
         * @brief Time since epoch in milliseconds timestamp for intersection schedule.
         */
        u_int64_t timestamp = 0;
        /**
         * @brief Method to turn schedule into CSV string. Each vehicle schedule is represented by a single row and all rows for 
         * a given schedule share a timestamp. The values in each row represent the following properties:
         * 
         *  timestamp ,v_id ,entry_lane ,link_id 
         * 
         * @return std::string CSV entry
         */
        virtual std::string toCSV() const = 0;
        /**
         * @brief Method to write intersection schedule as JSON scheduling message.
         * 
         * @return std::string& reference.
         */
        virtual std::string toJson() const = 0;

    };  
}