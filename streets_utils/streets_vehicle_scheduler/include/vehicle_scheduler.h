#pragma once

#include <spdlog/spdlog.h>
#include <shared_mutex>
#include <mutex>
#include <math.h>

#include "vehicle.h"
#include "vehicle_list.h"
#include "intersection_schedule.h"
#include "OAIIntersection_info.h"
#include "scheduling_exception.h"


namespace streets_vehicle_scheduler {
    class vehicle_scheduler {
        protected:
            std::shared_ptr<OpenAPI::OAIIntersection_info> intersection_info;

            std::shared_mutex scheduler_lock;

            void estimate_vehicles_at_common_time( std::unordered_map<std::string,streets_vehicles::vehicle> &vehicles, 
                                                    const u_int64_t timestamp);

            OpenAPI::OAILanelet_info get_entry_lanelet_info(const streets_vehicles::vehicle &veh) const;

            OpenAPI::OAILanelet_info get_link_lanelet_info(const streets_vehicles::vehicle &veh) const;

           
        public:

            virtual void schedule_vehicles( std::unordered_map<std::string,streets_vehicles::vehicle> &vehicles, intersection_schedule &schedule) = 0;

            std::shared_ptr<OpenAPI::OAIIntersection_info> get_intersection_info();

            void set_intersection_info(std::shared_ptr<OpenAPI::OAIIntersection_info> &_intersection_info );
            
    };
}
