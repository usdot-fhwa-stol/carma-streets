#pragma once

#include <spdlog/spdlog.h>
#include <shared_mutex>
#include <mutex>
#include <math.h>

#include "vehicle.h"
#include "vehicle_list.h"
#include "intersection_schedule.h"
#include "OAIIntersection_info.h"


namespace streets_vehicle_scheduler {
    class vehicle_scheduler {
        protected:
            std::shared_ptr<OpenAPI::OAIIntersection_info> intersection_info;

            std::shared_mutex scheduler_lock;

            void estimate_vehicles_at_common_time( std::unordered_map<std::string,streets_vehicles::vehicle> &vehicles, 
                                                    const u_int64_t timestamp);

           
        public:

            virtual void schedule_vehicles( streets_vehicles::vehicle_list &list_veh, intersection_schedule &schedule) = 0;

            std::shared_ptr<OpenAPI::OAIIntersection_info> get_intersection_info();

            void set_intersection_info(std::shared_ptr<OpenAPI::OAIIntersection_info> &_intersection_info );
            
    };
}
