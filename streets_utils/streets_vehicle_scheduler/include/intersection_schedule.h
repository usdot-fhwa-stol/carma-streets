#pragma once

#include <spdlog/spdlog.h>
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
         * @brief Vehicle State (@see @link vehicle @endlink)
         */
        streets_vehicles::vehicle_state state = streets_vehicles::vehicle_state::ND;
        /**
         * @brief Entry lanelet id of vehicle 
         */
        int entry_lane =  -1;
        /**
         * @brief Intended lanelet id of link lanelet used to travel through intersection.
         */
        int link_id = -1;
    };

    

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

    };  
}