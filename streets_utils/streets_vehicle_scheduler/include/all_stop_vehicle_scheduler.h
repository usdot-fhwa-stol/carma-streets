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
            
            std::list<streets_vehicles::vehicle> rdvs_previously_granted_access;
            /**
             * @brief Configurable time buffer in milliseconds for estimating the time it takes a vehicle to leave its entry lane 
             * and enter the intersection box once granted access to the intersection.
             */
            uint64_t entering_time_buffer = 0;
            /**
             * @brief Schedule all currently Departing Vehicle (DVs). Estimate intersection departure times (dt's) for each vehicle
             * based on kinematic vehicle information and intersection geometry. Method assumes empty intersection_schedule is passed in.
             * Method will add vehicle_schedule(s) for each DV in the vector. 
             * 
             * @param dvs vector of unscheduled DVs
             * @param schedule intersection_schedule to add DV scheduling information to.
             */
            void schedule_dvs( std::list<streets_vehicles::vehicle> &dvs, intersection_schedule &schedule) const;
            /**
             * @brief Schedule all currently Ready to Depart Vehicles (RDVs). Consider all possible RDV departure orders. Select
             * the one with the least calculated delay. Delay for a given vehicle is the difference between Entering Time and Stopping 
             * Time. Method assumes intersection_schedule only populated with DV(s) vehicle_schedules. Method will add vehicle_schedule(s)
             * for each RDV in vector.
             * 
             * @param rdvs vector of unscheduled RDVs
             * @param schedule intersection_schedule to add RDV scheduling information to.
             */
            void schedule_rdvs( std::list<streets_vehicles::vehicle> &rdvs, intersection_schedule &schedule );
            /**
             * @brief 
             * 
             */
            bool is_rdv_previously_granted_access( const streets_vehicles::vehicle &veh);

            void remove_rdv_previously_granted_access( const streets_vehicles::vehicle &veh);

            streets_vehicles::vehicle get_vehicle_with_id( const std::list<streets_vehicles::vehicle> &veh_list, const std::string veh_id ) const;

            /**
             * @brief Schedule all Entering Vehicles (EVs). Method first organizes all EVs into lists based on their entry
             * lane. These list are order in ascending order with the vehicle closest to the intersection in a given lane 
             * being the first vehicle in that entry lanes vehicle list. Then stopping time (ST) for the unscheduled front EVs 
             * from every approach are calculated and the vehicle with the lowest (earliest) stopping times is scheduled. This
             * is repeated until all evs from all approach lanelets are scheduled 
             * 
             * @param vehicles 
             * @param evs 
             * @param schedule 
             */
            void schedule_evs( std::list<streets_vehicles::vehicle> &evs, intersection_schedule &schedule ) const;

            /**
             * @brief Creates and intersection schedule for a given RDV departure order. RDV departure order is based on 
             * rdvs order. Returns reference to intersection_schedule option/possibility with given departure order. Assumes
             * intersection_schedule option only contains schedules for DVs. Populates intersection_schedule with RDV
             * vehicle_schedules.
             * 
             * @param rdvs vector of unscheduled RDVs
             * @param option intersection_schedule possibility.
             * @param starting_departure_position first available departure position given already scheduled vehicle (e.i. DVs)
             * @param previously_scheduled_vehicle pointer to previously scheduled DV. If null_ptr, method assumes no DVs.
             * @return true if departure order is valid and does not violate flexibility_limit for any vehicle.
             * @return false if departure order is invalid and does violate flexibility_limit for any vehicle.
             */
            bool consider_departure_position_permutation( 
                                                    const std::list<streets_vehicles::vehicle> &rdvs, 
                                                    intersection_schedule &option,
                                                    int starting_departure_position,
                                                    std::shared_ptr<vehicle_schedule> previously_scheduled_vehicle ) const;

            /**
             * @brief Estimate clearance time any vehicle given it's link lanelet information.  
             * 
             * @param veh vehicle to estimate clearance time for. 
             * @param link_lane_info link lanelet vehicle is attempting to clear. 
             * @return uint64_t clearance time in milliseconds. 
             */
            uint64_t estimate_clearance_time(const streets_vehicles::vehicle &veh, const OpenAPI::OAILanelet_info &link_lane_info) const;

            uint64_t estimate_earliest_time_to_stop_bar(const streets_vehicles::vehicle &veh) const;
            double estimate_delta_x_prime( const streets_vehicles::vehicle &veh, const OpenAPI::OAILanelet_info &entry_lane ) const;

            double calculate_v_hat(const streets_vehicles::vehicle &veh) const;

            double calculate_acceleration_time( const streets_vehicles::vehicle &veh, const double v_hat) const;

            double calculate_deceleration_time( const streets_vehicles::vehicle &veh, const double v_hat) const;

            double calculate_cruising_time( const streets_vehicles::vehicle &veh, const double v_hat, const double delta_x_prime) const;



        public:

            all_stop_vehicle_scheduler() = default;
             
            ~all_stop_vehicle_scheduler() = default;

            void schedule_vehicles( std::unordered_map<std::string,streets_vehicles::vehicle> &vehicles, intersection_schedule &schedule) override;

            void set_entering_time_buffer( const uint64_t buffer);



            
    };
}
