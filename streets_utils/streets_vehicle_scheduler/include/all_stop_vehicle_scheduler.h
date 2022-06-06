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
             * @brief Method to determine whether vehicle was granted access to the intersection in previous schedules. To avoid granting
             * conflicting vehicles intersection access simultaneously, the scheduling service maintains a list of vehicles it has granted
             * access to in the past. Vehicles are removed from this list once their update reflects that they have received and processed
             * this access.
             * 
             * @param veh 
             * @return true if vehicle is in list of vehicles previously granted access.
             * @return false if vehicle is not in list of vehicles previously granted access.
             */
            bool is_rdv_previously_granted_access( const streets_vehicles::vehicle &veh) const;
            /**
             * @brief This method will remove a vehicle (DV) once it from the list of previously granted access RDVs. This method should be
             * called once a RDV received the schedule update that grants it access to the intersection. This is indicated by the vehicle 
             * no longer being a RDV and instead being a DV ( @see streets_vehicle_list library).
             * 
             * @param veh DV to be removed from list
             */
            void remove_rdv_previously_granted_access( const streets_vehicles::vehicle &veh);
            /**
             * @brief Given a list of vehicles and a vehicle id, this method returns the vehicle with a matching vehicle id if there is one.
             * This method will throw a scheduling_exception if no vehicle with the given id is found.
             * 
             * @param veh_list list to search.
             * @param veh_id vehicle id to search for.
             * @return streets_vehicles::vehicle vehicle with matching vehicle id
             * @throw scheduling_exception if vehicle is not in list.
             */
            streets_vehicles::vehicle get_vehicle_with_id( const std::list<streets_vehicles::vehicle> &veh_list, const std::string &veh_id ) const;

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
             * @param scheduled_dvs a vector of scheduled dvs, This includes dvs already schedules and any RDVs scheduled in this permutation.
             *         This is necessary for calculating the current RDVs entering time based on already scheduled DVs and RDVs conflicts.
             * @return true if departure order is valid and does not violate flexibility_limit for any vehicle.
             * @return false if departure order is invalid and does violate flexibility_limit for any vehicle.
             */
            bool consider_departure_position_permutation( 
                                                    const std::list<streets_vehicles::vehicle> &rdvs, 
                                                    intersection_schedule &option,
                                                    int starting_departure_position ) const;

            /**
             * @brief Estimate clearance time any vehicle given it's link lanelet information.  
             * 
             * @param veh vehicle to estimate clearance time for. 
             * @param link_lane_info link lanelet vehicle is attempting to clear. 
             * @return uint64_t clearance time in milliseconds. 
             */
            uint64_t estimate_clearance_time(const streets_vehicles::vehicle &veh, const OpenAPI::OAILanelet_info &link_lane_info) const;

            /**
             * @brief Given a link lane and a vector of vehicle schedules, this method returns a pointer to a vehicle schedule with a 
             * conflicting direction and the latest departure time. If no vehicle schedules have conflicting directions it returns 
             * nullptr. This method is meant to return the limiting conflicting vehicle for scheduling new RDVs in a link lane.
             * 
             * @param link_lane the linking lane to be traveled
             * @param schedules Already scheduled vehicles
             * @return std::shared_ptr<vehicle_schedule> pointer to vehicle schedule with conflicting direction and latest departure time.
             * 
             */
            std::shared_ptr<vehicle_schedule> get_latest_conflicting(const OpenAPI::OAILanelet_info &link_lane,
                                        const std::vector<vehicle_schedule> &schedules) const;

            /**
             * @brief Method to use vehicle kinematic information to estimate earliest possible time stopping time for a vehicle.
             * Does not consider any other vehicles. Limiting factors are the vehicles current kinematic information, lane speed
             * limits and vehicle acceleration and deceleration limits.
             * 
             * @param veh vehicle for which to calculate trajectory
             * @return uint64_t time in milliseconds.
             */
            uint64_t estimate_earliest_time_to_stop_bar(const streets_vehicles::vehicle &veh) const;
            /**
             * @brief Calculate minumum distance required to speed up to lane speed limit with maximum acceleration, cruise at
             * lane speed limit for some time, then decelerate with maximum deceleration. This is used to determine the 
             * trajectory to provide the vehicle. 
             * 
             * @param veh vehicle for which to calculate distance.
             * @param entry_lane entry lane information.
             * @return double distance in meters
             */
            double estimate_delta_x_prime( const streets_vehicles::vehicle &veh, const OpenAPI::OAILanelet_info &entry_lane ) const;

            /**
             * @brief Method to calculate v_hat. This is the maximum speed reached between maximum speed reached between acceleration
             * and deceleration periods for a vehicle trajectory towards stopping at a stop bar.
             * 
             * @param veh vehicle to calculate trajectory for.
             * @return double v_hat in m/s
             */
            double calculate_v_hat(const streets_vehicles::vehicle &veh) const;
            /**
             * @brief Method to calculate time interval necessary for accelerating to speed v_hat for a given vehicle. Will return 0
             * if v_hat is less than vehicle speed. 
             * 
             * @param veh vehicle for which to calculate trajectory.
             * @param v_hat maximum speed to reach during acceleration in m/s.
             * @return double time interval necessary for acceleration in seconds.
             */
            double calculate_acceleration_time( const streets_vehicles::vehicle &veh, const double v_hat) const;
            /**
             * @brief Method to calculate time interval necessary for deceleration from v_hat to 0 to stop at stop bar. Assumes trajectory
             * with constant maximum deceleration.
             * 
             * @param veh vehicle for which to calculate trajectory.
             * @param v_hat max speed from which to decelerate from in m/s
             * @return double time interval necessary for acceleration in seconds.
             */
            double calculate_deceleration_time( const streets_vehicles::vehicle &veh, const double v_hat) const;
            /**
             * @brief Method to calculate time interval necessary for cruising at a constant speed v_hat (speed limit) greater than 
             * current vehicle speed.If vehicle speed is greater that v_hat, this method will return 0. 
             * 
             * @param veh vehicle for which to calculate trajectory for.
             * @param v_hat cruising speed, usually lane speed limit, in m/s.
             * @param delta_x_prime the distance necessary for vehicle to accelerate to speed limit with maximum acceleration 
             *      and then decelerate back to a stop in meters.The difference the distance to the stop bar and this distance 
             *      is the part of the trajectory for which the vehicle can cruise at the speed limit.
             * @return double time interval necessary for cruising at speed limit in seconds. 
             */
            double calculate_cruising_time( const streets_vehicles::vehicle &veh, const double v_hat, const double delta_x_prime) const;



        public:
            /**
             * @brief Construct a new all stop vehicle scheduler object
             * 
             */
            all_stop_vehicle_scheduler() = default ;
            /**
             * @brief Destroy the all stop vehicle scheduler object
             * 
             */
            ~all_stop_vehicle_scheduler() override = default ;
            /**
             * @brief Method to schedule vehicles. Given and empty intersection_schedule object and a map of vehicle, this method
             * will populate the intersection schedule with a schedule for each EV,RDV and DV in the vehicle map based on UC 1
             * scheduling logic to minimize delay for a all way stop intersection.
             * 
             * @param vehicles A map of the vehicles to schedule, with vehicle id as keys
             * @param schedule A intersection schedule object populated with a vehicle schedule for each EV,RDV and DV in the map.
             */
            void schedule_vehicles( std::unordered_map<std::string,streets_vehicles::vehicle> &vehicles, intersection_schedule &schedule) override;
            /**
             * @brief Set the entering time buffer (ms). This value is used to account for the time it takes a vehicle stopped before 
             * the stop bar at an entry lane to exit the entry lane and enter the link lane once given access.
             * 
             * @param buffer entering time buff in milliseconds.
             */
            void set_entering_time_buffer( const uint64_t buffer);



            
    };
}
