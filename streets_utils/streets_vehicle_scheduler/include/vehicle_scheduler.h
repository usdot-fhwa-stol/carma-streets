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
    /**
     * @brief Abstract Class for extend to implement scheduling business logic.
     * 
     */
    class vehicle_scheduler {
        protected:
            /**
             * @brief shared pointer to intersection information required for scheduling.
             * 
             */
            std::shared_ptr<OpenAPI::OAIIntersection_info> intersection_info;

            /**
             * @brief Limits how much departure position for a given vehicle can change from current reported departure position.
             */
            int flexibility_limit = 5;
            /**
             * @brief Method to estimate all vehicle's speed and location at a common future time. The time needs to be time since epoch in milliseconds.
             * To predict vehicle location and speed at future time, current kinematic information is used including speed and current
             * acceleration.
             * 
             * @param vehicles Vehicle for which to calculate future position and speed.
             * @param timestamp Current or future time in milliseconds since epoch
             * @throw scheduling_exception if vehicle update time for any vehicle is more recent than timestamp.
             */
            void estimate_vehicles_at_common_time( std::unordered_map<std::string,streets_vehicles::vehicle> &vehicles, 
                                                    const u_int64_t timestamp);

            /**
             * @brief Helper method to get entry lane OAILanelet_info from intersection information given a vehicle.
             * 
             * @param veh Vehicle for which to find entry lane OAILanlet_info.
             * @return OpenAPI::OAILanelet_info for entry lane.
             */
            OpenAPI::OAILanelet_info get_entry_lanelet_info(const streets_vehicles::vehicle &veh) const;
            /**
             * @brief Helper method to get link lane OAILanelet_info from intersection information given a vehicle.
             * 
             * @param veh Vehicle for which to find link lane OAILanlet_info.
             * @return OpenAPI::OAILanelet_info for link lane.
             */
            OpenAPI::OAILanelet_info get_link_lanelet_info(const streets_vehicles::vehicle &veh) const;

           
        public:
            /**
             * @brief Construct a new vehicle scheduler object
             * 
             */
            vehicle_scheduler() = default;
            /**
             * @brief Destroy the vehicle scheduler object
             * 
             */
            virtual ~vehicle_scheduler() = default;
            /**
             * @brief Virtual method to schedule vehicle. Different implementations of vehicle scheduler can implement custom
             * business logic for scheduling vehicle while taking advantage of polymorphism to dynamically assign class at runtime.
             * 
             * @param vehicles unordered map of vehicles to schedule.
             * @param schedule empty intersection_schedule object which is returned by reference to provide calculated scheduling information.
             */
            virtual void schedule_vehicles( std::unordered_map<std::string,streets_vehicles::vehicle> &vehicles, intersection_schedule &schedule) = 0;
            /**
             * @brief Get the intersection info object
             * 
             * @return std::shared_ptr<OpenAPI::OAIIntersection_info> 
             */
            std::shared_ptr<OpenAPI::OAIIntersection_info> get_intersection_info() const;
            /**
             * @brief Set the intersection info object
             * 
             * @param _intersection_info 
             */
            void set_intersection_info(std::shared_ptr<OpenAPI::OAIIntersection_info> _intersection_info );
            /**
             * @brief Set the flexibility limit. This limit controls how much departure position can change for any given vehicle between
             * successive schedules. A value of 1 represents that vehicles departure position can change by one between two schedules.
             * 
             * @param limit How much can departure position change between schedules for any vehicle.
             */
            void set_flexibility_limit( const int limit );
            
    };
}
