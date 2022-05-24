#pragma once

#include <spdlog/spdlog.h>
#include <vector>
#include <set>
#include "vehicle.h"
#include "vehicle_scheduler.h"
#include "intersection_schedule.h"
#include "scheduling_exception.h"
#include "vehicle_sorting.h"


namespace streets_vehicle_scheduler {
    class all_stop_vehicle_scheduler : public vehicle_scheduler {
        private:

            u_int64_t scheduling_delta;

            int flexibility_rating = 5;
            
            void estimate_st( const std::vector<streets_vehicles::vehicle> &evs, intersection_schedule &schedule ) const;

            void estimate_est( const std::vector<streets_vehicles::vehicle> &evs, intersection_schedule &schedule ) const;

            void schedule_dvs( const std::vector<streets_vehicles::vehicle> &dvs, intersection_schedule &schedule) const;

            void schedule_rdvs( std::vector<streets_vehicles::vehicle> &rdvs, intersection_schedule &schedule ) const;

            bool consider_departure_position_permutation( 
                                                    const std::vector<streets_vehicles::vehicle> &rdvs, 
                                                    intersection_schedule &option,
                                                    int starting_departure_position,
                                                    std::shared_ptr<vehicle_schedule> previously_scheduled_vehicle ) const;

            uint64_t estimate_clearance_time(const streets_vehicles::vehicle &veh, const OpenAPI::OAILanelet_info &link_lane_info);
            


            void estimate_et( const std::vector <streets_vehicles::vehicle> &rdvs, intersection_schedule &schedule) const;

            double estimate_delta_x_prime( const streets_vehicles::vehicle &veh, const OpenAPI::OAILanelet_info &entry_lane ) const;
            // TODO move to parent class if possible 
            OpenAPI::OAILanelet_info get_lanelet_info(const streets_vehicles::vehicle &veh) const;

            double calculate_v_hat(const streets_vehicles::vehicle &veh) const;

            double calculate_acceleration_time( const streets_vehicles::vehicle &veh, const double v_hat) const;

            double calculate_deceleration_time( const streets_vehicles::vehicle &veh, const double v_hat) const;

            double calculate_cruising_time( const streets_vehicles::vehicle &veh, const double v_hat, const double delta_x_prime) const;



        public:

            void schedule_vehicles( streets_vehicles::vehicle_list &list_veh, intersection_schedule &schedule) override;
            
            void set_scheduling_delta( const u_int64_t _scheduling_delta);

            void set_flexibility_rating( const int flexibility );



            
    };
}
