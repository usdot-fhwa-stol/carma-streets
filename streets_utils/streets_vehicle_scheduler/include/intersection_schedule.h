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
         * @brief string unique static vehicle id.
         */
        std::string v_id;
        /**
         * @brief Earliest possible stopping time based on kinematic information and 
         * lanelet speed limit and length. Stopping time is the time since epoch in 
         * milliseconds when the vehicle will arrive at the stop bar.
         */
        u_int64_t est = 0;
        /**
         * @brief Stopping time considering preceding vehicle limitations. Stopping time 
         * is the time since epoch in milliseconds when the vehicle will arrive at the stop bar. 
         */
        u_int64_t st = 0;
        /**
         * @brief Entering time considering already scheduled vehicles. Entering time is the
         * time since epoch in milliseconds when the vehicle will enter the intersection box.
         */
        u_int64_t et = 0;
        /**
         * @brief Departure time considering already scheduled vehicles. Departure time is the
         * time since epoch in milliseconds when the vehicle will depart from the intersection
         * box.
         */
        u_int64_t dt = 0;
        /**
         * @brief Departure position. This is an index to indicate the order in which vehicles
         * are granted access to the intersection. This index starts at 1 and no two vehicles
         * should ever hold the same index. Negative values are considered invalid but are 
         * initial values set before vehicle is scheduled.
         */
        int dp = -1;
        /**
         * @brief Bool flag indicating whether vehicle has access to enter the intersection.
         */
        bool access = false;
        /**
         * @brief Vehicle State (@see @link streets_vehicles::vehicle @endlink)
         */
        streets_vehicles::vehicle_state state = streets_vehicles::vehicle_state::ND;
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
        rapidjson::Value toJson(rapidjson::Document::AllocatorType& allocator) const;
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
         * @brief Time since epoch in milliseconds timestamp for intersection schedule.
         */
        u_int64_t timestamp = 0;
        /**
         * @brief Vector of all scheduled vehicles
         */
        std::vector<vehicle_schedule> vehicle_schedules;
        /**
         * @brief Calculate intersection schedule delay. This is the sum of ST-ET for each vehicle schedule.
         * This represents the cumulative time spent waiting at the stop bar for all vehicles.
         * 
         * @return u_int64_t 
         */
        u_int64_t get_delay() const;
        /**
         * @brief Method to turn schedule into CSV string. Each vehicle schedule is represented by a single row and all rows for 
         * a given schedule share a timestamp. The values in each row represent the following properties:
         * 
         *  timestamp ,v_id ,entry_lane ,link_id ,dp ,est ,st ,et ,dt ,access ,state
         * 
         * @return std::string CSV entry
         */
        std::string toCSV() const;
        /**
         * @brief Method to write intersection schedule as JSON scheduling message.
         * 
         * @return std::string& reference.
         */
        std::string toJson() const;

    };  
}