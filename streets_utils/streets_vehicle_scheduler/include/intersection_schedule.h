#pragma once

#include <spdlog/spdlog.h>
#include "vehicle.h"

namespace streets_vehicle_scheduler {
    struct vehicle_schedule {
        std::string v_id;
        u_int64_t est = 0;
        u_int64_t st = 0;
        u_int64_t et = 0;
        u_int64_t dt = 0;
        int dp = -1;
        bool access = false;
        streets_vehicles::vehicle_state state = streets_vehicles::vehicle_state::ND;
        int link_id = -1;
    };

    

    struct intersection_schedule {

        u_int64_t timestamp = 0;
        
        std::vector<vehicle_schedule> vehicle_schedules;
       
        u_int64_t get_delay() const;

    };  
}