#pragma once

#include <spdlog/spdlog.h>
#include <vector>
#include <set>
#include "vehicle.h"
#include "vehicle_scheduler.h"
#include "intersection_schedule.h"
#include "signalized_intersection_schedule.h"
#include "scheduling_exception.h"
#include "vehicle_sorting.h"
#include "spat.h"


namespace streets_vehicle_scheduler {
    class signalized_vehicle_scheduler : public vehicle_scheduler {
        private:
            
            /**
             * @brief A shared pointer to the modified_spat.
             * 
             */
            std::shared_ptr<signal_phase_and_timing::spat> spat_ptr;

            /**
             * @brief The configurable time interval at the beginning of a green phase in milliseconds that is considered for estimating 
             * vehicles' entering times. The earliest possible entering time during a phase is set to the beginning of the green phase plus 
             * the initial_green_buffer.
             * 
             */
            uint64_t initial_green_buffer;

            /**
             * @brief The configurable time interval at the end of a green phase in milliseconds that is considered for estimating vehicles' 
             * entering times. The latest possible entering time during a phase is set to the end of the green phase minus the final_green_buffer.
             *
             */
            uint64_t final_green_buffer;

            /**
             * @brief Estimate the intersection departure times (dt's) for all currently Departing Vehicle (DVs) based on kinematic vehicle 
             * information and intersection geometry. Method assumes empty intersection_schedule is passed in. Method will add vehicle_schedule(s) 
             * for each DV in the vector. 
             * 
             * @param dvs vector of unscheduled DVs.
             * @param schedule signalized_intersection_schedule to add DV scheduling information to.
             */
            void schedule_dvs( const std::list<streets_vehicles::vehicle> &dvs, const std::shared_ptr<signalized_intersection_schedule> &schedule ) const;
            /**
             * @brief Schedule all Entering Vehicles (EVs). Method first organizes all EVs into lists based on their entry
             * lane. These list are ordered in ascending order with the vehicle closest to the intersection in a given lane 
             * being the first vehicle in that entry lanes vehicle list. Then, for each entry lane, an entering time (ET)
             * for each EV in the entry lane is calculated sequentially starting from the first vehicle until in the list.
             * ET is estimated based on the EV's earliest entering time (EET), its preceding vehicle's estimated ET, and 
             * the modifed spat.
             * 
             * @param evs list of all EVs.
             * @param schedule signalized_intersection_schedule to add EV scheduling information to.
             */
            void schedule_evs( std::list<streets_vehicles::vehicle> &evs, const std::shared_ptr<signalized_intersection_schedule> &schedule ) const;
            /**
             * @brief Estimate an entering time (ET) for a given Entering Vehicle (EV). ET is estimated based on the EV's 
             * earliest entering time (EET), its preceding vehicle's estimated ET, and the modifed spat.
             * 
             * @param veh vehicle for which to estimate ET.
             * @param preceding_veh shared pointer of the preceding vehicle of the subject vehicle (veh)
             * @param sched signalized_intersection_schedule to add EV scheduling information to.
             * @param move_state movement state information that includes a list of current and pending phases.
             * @param schedule_timestamp timestamp of the current schedule in milliseconds. 
             */
            void estimate_et(const streets_vehicles::vehicle &veh, const std::shared_ptr<signalized_vehicle_schedule> &preceding_veh, signalized_vehicle_schedule &sched, const signal_phase_and_timing::movement_state &move_state, const uint64_t schedule_timestamp) const;
            /**
             * @brief Method to use vehicle kinematic information to estimate earliest possible entering time (EET) for an entering vehicle (EV).
             * Does not consider any other vehicles. Limiting factors are the vehicles current kinematic information, lane speed
             * limits and vehicle acceleration and deceleration limits.
             * 
             * @param veh vehicle for which to estimate EET.
             * @return uint64_t time in milliseconds.
             */
            uint64_t calculate_earliest_entering_time(const streets_vehicles::vehicle &veh) const;
            /**
             * @brief Calculate the distance required to speed up to lane speed limit with maximum acceleration and then decelerate 
             * to departure speed with maximum deceleration. This is used to calculate the earliest entering time (EET) for the vehicle. 
             * 
             * @param veh vehicle for which to calculate distance.
             * @param max_speed speed limit of the vehicle's entry lane.
             * @param departure_speed departure speed = speed limit of the vehicle's link lane.
             * @return double distance in meters
             */
            double calculate_distance_accel_and_decel( const streets_vehicles::vehicle &veh, const double max_speed, const double departure_speed ) const;
            /**
             * @brief Calculate the distance required to speed up (slow down) to departure speed with maximum acceleration (maximum deceleration). 
             * If the departure speed is greater than the vehicle's current speed, the vehicle has to speed up with maximum acceleration. Otherwise, 
             * the vehicle has to slow down with maximum deceleration. This is used to calculate the earliest entering time (EET) for the vehicle. 
             * 
             * @param veh vehicle for which to calculate distance.
             * @param departure_speed departure speed = speed limit of the vehicle's link lane.
             * @return double distance in meters
             */
            double calculate_distance_accel_or_decel( const streets_vehicles::vehicle &veh, const double departure_speed ) const;
            /**
             * @brief Method to calculate v_hat. This is the joint speed between the acceleration and deceleration periods of a vehicle 
             * trajectory that is calculated to estimate the vehicle's earliest entering time (EET).
             * 
             * @param veh vehicle for which to calculate the joint speed.
             * @param max_speed speed limit of the vehicle's entry lane.
             * @param departure_speed departure speed = speed limit of the vehicle's link lane.
             * @param delta_x distance between the vehicle and the stop bar at its entry lane
             * @param delta_x_prime distance required to speed up to lane speed limit with maximum acceleration and then decelerate 
             *      to departure speed with maximum deceleration.
             * @param delta_x_zegond distance required to speed up (slow down) to departure speed with maximum acceleration (maximum deceleration).
             * @return double v_hat in m/s
             */
            double calculate_v_hat( const streets_vehicles::vehicle &veh, const double max_speed, const double departure_speed, const double delta_x, const double delta_x_prime, const double delta_x_zegond) const;
            /**
             * @brief Method to calculate time interval necessary for accelerating to speed v_hat for a given vehicle. Will return 0
             * if there is no acceleration period.
             * 
             * @param veh vehicle for which to calculate the joint speed.
             * @param v_hat joint speed between the acceleration and deceleration periods.
             * @param departure_speed departure speed = speed limit of the vehicle's link lane.
             * @param delta_x distance between the vehicle and the stop bar at its entry lane
             * @param delta_x_zegond distance required to speed up (slow down) to departure speed with maximum acceleration (maximum deceleration).
             * @return double acceleration time interval in seconds.
             */
            double calculate_acceleration_time(const streets_vehicles::vehicle &veh, const double v_hat, const double departure_speed, const double delta_x, const double delta_x_zegond) const;
            /**
             * @brief Method to calculate time interval necessary for deceleration from v_hat to departure speed. Will return 0
             * if there is no deceleration period.
             * 
             * @param veh vehicle for which to calculate trajectory.
             * @param v_hat joint speed between the acceleration and deceleration periods.
             * @param departure_speed departure speed = speed limit of the vehicle's link lane.
             * @param delta_x distance between the vehicle and the stop bar at its entry lane
             * @param delta_x_zegond distance required to speed up (slow down) to departure speed with maximum acceleration (maximum deceleration).
             * @return double deceleration time interval in seconds.
             */
            double calculate_deceleration_time(const streets_vehicles::vehicle &veh, const double v_hat, const double departure_speed, const double delta_x, const double delta_x_zegond) const;
            /**
             * @brief Method to calculate time interval necessary for cruising at a constant speed v_hat (speed limit) greater than 
             * current vehicle speed. Will return 0 if there is no cruising period. 
             * 
             * @param v_hat cruising speed\joint speed between the acceleration and deceleration periods. Usually lane speed limit, in m/s.
             * @param delta_x distance between the vehicle and the stop bar at its entry lane
             * @param delta_x_prime distance required to speed up to lane speed limit with maximum acceleration and then decelerate 
             *      to departure speed with maximum deceleration. The difference between the distance to the stop bar and this distance 
             *      is the part of the trajectory for which the vehicle can cruise at v_hat.
             * @return double cruising time interval in seconds.
             */
            double calculate_cruising_time(const double v_hat, const double delta_x, const double delta_x_prime) const;
            /**
             * @brief Calculate the minimum safety time headway required by the vehicle at a given speed.
             * 
             * @param veh vehicle for which to calculate minimum safety time headway.
             * @param speed the assumed speed at which to calculate minimum safety time headway.
             * @return uint64_t minimum safety time headway in milliseconds.
             */
            uint64_t calculate_min_headway(const streets_vehicles::vehicle &veh, const double speed) const;
            /**
             * @brief Estimate clearance time of a vehicle given it's current speed.  
             * 
             * @param veh vehicle for which to estimate clearance time. 
             * @return uint64_t clearance time in milliseconds. 
             */
            uint64_t estimate_clearance_time( const streets_vehicles::vehicle &veh ) const;
            /**
             * @brief find the movement state info from the modified spat for vehicles from a given entry lane.
             * Note: The signalized_vehicle_scheduler is only capable of understanding intersection where all connection lanes 
             * from a single entry lane share a signal_group_id. Therefore, vehicles from an entry lane with different directions
             * at the intersection box shall be able to receive protected green at the same time.
             * 
             * @param entry_lane_info entry lanelet lane. 
             * @return signal_phase_and_timing::movement_state movement stat object.
             * @throws if two or more connection link lanelets from a single entry lane have different signal_group_id, then the design
             * does not satisfy the requirement of the signalized_vehicle_scheduler and thus, this method throws exception. 
             */
            signal_phase_and_timing::movement_state find_movement_state_for_lane(const OpenAPI::OAILanelet_info &entry_lane_info) const;

        public:
            /**
             * @brief Construct a new signalized vehicle scheduler object
             * 
             */
            signalized_vehicle_scheduler() = default ;
            /**
             * @brief Destroy the signalized vehicle scheduler object
             * 
             */
            ~signalized_vehicle_scheduler() override = default ;

            /**
             * @brief Method to schedule vehicles. Given and empty intersection_schedule shared pointer and a map of vehicles and vehicle ids. 
             * This method will populate the intersection schedule with a schedule for all vehicles in the vehicle map based on UC 3
             * scheduling logic to estimate vehicles' entering times to the intersection box based on the modified spat.
             * 
             * @param vehicles A map of the vehicles to schedule, with vehicle id as keys
             * @param schedule A signalize_intersection schedule shared pointer populated with a vehicle schedule for all EVs and DVs in the map.
             */
            void schedule_vehicles( std::unordered_map<std::string,streets_vehicles::vehicle> &vehicles, std::shared_ptr<intersection_schedule> &schedule) override;
            
            /**
             * @brief Set the initial green buffer (ms). This value is used to account for the time it takes a vehicle before 
             * entering the intersection box at a green phase.
             * 
             * @param buffer initial green buffer in milliseconds.
             */
            void set_initial_green_buffer(const uint64_t buffer);
            /**
             * @brief Get the initial green buffer (ms).
             * 
             * @return uint64_t initial green buffer.
             */
            uint64_t get_initial_green_buffer() const;
            /**
             * @brief Set the final green buffer (ms). This value is used to account for safety. The assumption is that no vehicle shall 
             * enter the intersection box after the end of a green phase minus the final green buffer.
             * entering the intersection box at a green phase.
             * 
             * @param buffer final green buffer in milliseconds.
             */
            void set_final_green_buffer(const uint64_t buffer);
            /**
             * @brief Get the final green buffer (ms).
             * 
             * @return uint64_t final green buffer.
             */
            uint64_t get_final_green_buffer() const;   
            /**
             * @brief Get the spat object
             * 
             * @return std::shared_ptr<signal_phase_and_timing::spat> spat object
             */
            std::shared_ptr<signal_phase_and_timing::spat> get_spat() const;
            /**
             * @brief Set the spat object
             * 
             * @param spat_info signal_phase_and_timing::spat object.
             */
            void set_spat(std::shared_ptr<signal_phase_and_timing::spat> spat_info);
    };
}
