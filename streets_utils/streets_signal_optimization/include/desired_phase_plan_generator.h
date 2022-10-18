#pragma once

#include <spdlog/spdlog.h>
#include <vector>
#include <list>
#include <set>
#include <math.h>
#include <chrono>
#include <gtest/gtest_prod.h>
#include "vehicle.h"
#include "streets_configuration.h"
#include "signalized_vehicle_scheduler.h"
#include "signalized_intersection_schedule.h"
#include "vehicle_sorting.h"
#include "spat.h"
#include "movement_group.h"
#include "streets_desired_phase_plan.h"
#include "desired_phase_plan_generator_exception.h"
#include "desired_phase_plan_generator_configuration.h"
#include "tsc_configuration_state.h"


namespace streets_signal_optimization {
    
    class desired_phase_plan_generator {
        private:

            /**
             * @brief A shared pointer to the signalized vehicle scheduler.
             */
            std::shared_ptr<streets_vehicle_scheduler::signalized_vehicle_scheduler> scheduler_ptr;
            
            /**
             * @brief A struct containing the configurable parameters for desired_phsae_plan_generator functions.
             */
            desired_phase_plan_generator_configuration config;

            /**
             * @brief The bool parameter defining whether the configurable parameter are set to their desired values or not.
             */            
            bool is_configured = false;

            /**
             * @brief Signal group to list of entry lane ids mapping. Each signal group might be assigned to more than one entry lanes.
             */
            std::unordered_map<uint8_t, std::vector<int>> signal_group_entry_lane_mapping;

            /**
             * @brief Given a map of vehicles and vehicle ids, this method will run the signalized_vehicle_scheduler methods to populate 
             * the signalized_intersection_schedule with a schedule for all vehicles in the vehicle map based on UC 3 scheduling logic 
             * which estimates vehicles' entering times to the intersection box based on the modified spat.
             * 
             * @param vehicles A map of the vehicles to schedule, with vehicle id as keys.
             * @return std::shared_ptr<streets_vehicle_scheduler::intersection_schedule> An intersection_schedule shared pointer.
             */
            std::shared_ptr<streets_vehicle_scheduler::intersection_schedule> get_schedule_plan(
                                                        std::unordered_map<std::string,streets_vehicles::vehicle> &vehicles) const;

            /**
             * @brief Given a populated signalized_intersection_schedule, this method will return a new signalized_intersection_schedule
             * object that includes those EVs that are within the signal optimization area.
             * 
             * @param sched_ptr An intersection_schedule shared pointer populated with vehicle schedules.
             * @param vehicles A map of the vehicles to schedule, with vehicle id as keys.
             * @return streets_vehicle_scheduler::signalized_intersection_schedule A signalized_intersection_schedule object.
             */            
            streets_vehicle_scheduler::signalized_intersection_schedule get_ev_schedules_within_so(
                                    const std::shared_ptr<streets_vehicle_scheduler::intersection_schedule> &sched_ptr, 
                                    const std::unordered_map<std::string,streets_vehicles::vehicle> &vehicles) const;

            /**
             * @brief Given a populated signalized_intersection_schedule object, intersection_info shared pointer, and the start time of 
             * TBD area, this method will return entry lane id to list of schedules that have estimated entering times (ETs) after the 
             * start time of the TBD area for the entry lane mapping.
             * 
             * @param ev_schedules_within_so A populated signalized_intersection_schedule object.
             * @param intersection_info_ptr An intersection_info pointer.
             * @param tbd_start Start time of the TBD area.
             * @return unordered_map<int, vector<streets_vehicle_scheduler::signalized_vehicle_schedule>> entry lane to list of 
             * signalized_vehicle_schedule mapping, where the list of signalized_vehicle_schedule includes the schedules that
             * have ETs after the start time of the TBD area.
             */           
            std::unordered_map<int, std::list<streets_vehicle_scheduler::signalized_vehicle_schedule>> get_schedules_in_tbd_per_lane(
                                    const streets_vehicle_scheduler::signalized_intersection_schedule &ev_schedules_within_so, 
                                    const std::shared_ptr<OpenAPI::OAIIntersection_info> &intersection_info_ptr, 
                                    const uint64_t &tbd_start) const;

            /**
             * @brief Given a list of signalized_vehicle_schedules to entry lane id mapping, for each entry lane, this method will find 
             * the last vehicle in the queue, calculates the queue dissipation time, and finally returns the end time for the required 
             * green duration to dissipate the queue.
             * The last vehicle in the queue is the vehicle that has entering time (ET) equal to its earliest entering time (EET) and the
             * time headway between its ET and its following vehicle's ET is grater than the configurable queue_max_time_headway parameter, 
             * or the last vehicle in the entry lane.
             * 
             * @param schedules_in_tbd A list of signalized_vehicle_schedules to entry lane mapping.
             * @param tbd_start Start time of the TBD area.
             * @return unordered_map<int, uint64_t> End time of required green duration for clearing the queue to entry lane id mapping.
             */    
            std::unordered_map<int, uint64_t> get_green_end_per_entry_lane(
                const std::unordered_map<int, std::list<streets_vehicle_scheduler::signalized_vehicle_schedule>> &schedules_in_tbd, 
                const uint64_t &tbd_start) const;
            
            /**
             * @brief Given a list of signalized_vehicle_schedules for an entry lane, this method will find the last vehicle in the queue.
             * The last vehicle in the queue is the vehicle that has entering time (ET) equal to its earliest entering time (EET) and the
             * time headway between its ET and its following vehicle's ET is grater than the configurable queue_max_time_headway parameter, 
             * or the last vehicle in the entry lane.
             * 
             * @param evs_in_lane A list of signalized_vehicle_schedules for an entry lane.
             * @return streets_vehicle_scheduler::signalized_vehicle_schedule The schedule of the last vehicle in the queue.
             */ 
            streets_vehicle_scheduler::signalized_vehicle_schedule get_last_vehicle_in_queue(
                    const std::list<streets_vehicle_scheduler::signalized_vehicle_schedule> &evs_in_lane) const;

            /**
             * @brief Given a list of movement groups, the end time of required green duration for clearing the queue to entry lane id 
             * mapping, and an updated base_desired_phase_plan, this method with create a list of desired phase plans, where each desired 
             * phase plan in the list includes the existing fixed future movement groups from the received spat and a new movement group 
             * with a given duration at the end. For each movement group, this method first checks if the movement group can be added
             * to the base_desired_phase_plan (i.e., if the movement group has a common signal group with the last movement group in the
             * base_desired_phase_plan). If the movement group can be added, for each entry lane allowed by the subject movement group, 
             * this method will add a new desired phase plan to the list, where the last movement group is the subject movement group and 
             * the end time of the green is the end time of required green duration for clearing the queue of the subject entry lane.
             * 
             * @param move_groups A list of possible movement groups.
             * @param green_end_per_entry_lane End time of required green duration for clearing the queue to entry lane id mapping.
             * @param base_desired_phase_plan The desired_phase_plan converted from the provided intersection_state.
             * @param desired_phase_plan_list An empty desired phase plan list to be populated by this method.
             * @param intersection_info_ptr An intersection_info pointer.
             * @param tbd_start Start time of the TBD area.
             * @throws If the base_desired_phase_plan is empty.
             */     
            void update_desired_phase_plan_list(const streets_signal_optimization::movement_groups &move_groups, 
                                                const std::unordered_map<int, uint64_t> &green_end_per_entry_lane, 
                                                const streets_desired_phase_plan::streets_desired_phase_plan &base_desired_phase_plan, 
                                                std::vector<streets_desired_phase_plan::streets_desired_phase_plan> &desired_phase_plan_list, 
                                                const std::shared_ptr<OpenAPI::OAIIntersection_info> &intersection_info_ptr, 
                                                const uint64_t &tbd_start) ;

            /**
             * @brief Given the entry lane object, this method finds the signal group id.
             * Note: The signalized_vehicle_scheduler is only capable of understanding intersection where all connection lanes 
             * from a single entry lane share a signal_group_id. Therefore, vehicles from an entry lane with different directions
             * at the intersection box shall be able to receive protected green at the same time.
             * 
             * @param intersection_info_ptr An intersection_info pointer.
             * @param entry_lane_info entry lanelet lane. 
             * @return uint8_t signal group id
             * @throws If a connection link lanelet does not have a signal group id.
             * @throws If two or more connection link lanelets from a single entry lane have different signal group ids.
             */
            uint8_t find_signal_group_for_entry_lane(const std::shared_ptr<OpenAPI::OAIIntersection_info> &intersection_info_ptr, 
                                                    const OpenAPI::OAILanelet_info &entry_lane_info) const;


            FRIEND_TEST(test_desired_phase_plan_generator, test_generate_desired_phase_plan_list);

        public:


            /**
             * @brief Construct a new desired phase plan generator object
             * 
             */
            desired_phase_plan_generator() = default ;
            /**
             * @brief Destroy the desired phase plan generator object
             * 
             */
            ~desired_phase_plan_generator() = default;
            
            /**
             * @brief Generate a list of desired phase plans. Each desired phase plan in the list includes the existing fixed future 
             * movement groups from the provided intersection_state and a new movement group with a given duration at the end. 
             * An empty desired phase plan list means that either the spat verification failed or there are no estimated entering time (ET) 
             * within the to-be-determined (tbd) area.
             * 
             * @param intersection_info_ptr An intersection_info pointer.
             * @param vehicles A map of the vehicles to schedule, with vehicle id as keys.
             * @param intersection_state An intersection_state object from the most recent spat.
             * @param move_groups A list of possible movement groups.
             * @return vector<streets_desired_phase_plan::streets_desired_phase_plan> list of desired phase plans.
             * @throws if the provided intersection_state does not have any fixed future movement group.
             */
            std::vector<streets_desired_phase_plan::streets_desired_phase_plan> generate_desire_phase_plan_list(
                                                    const std::shared_ptr<OpenAPI::OAIIntersection_info> &intersection_info_ptr, 
                                                    std::unordered_map<std::string,streets_vehicles::vehicle> &vehicles,
                                                    signal_phase_and_timing::intersection_state &intersection_state,
                                                    const streets_signal_optimization::movement_groups &move_groups);
            

            /**
             * @brief Convert the provided intersection_state to a desired phase plan. The number of fixed future movement groups
             * shall be at least 1 and at most the desired number of fixed future movement groups.
             * 
             * @param intersection_info_ptr An intersection_info pointer.
             * @param move_groups A list of possible movement groups.
             * @return streets_desired_phase_plan::streets_desired_phase_plan desired phase plan converted from intersection_state.
             * @throws If a movement state does not have any movement event in its state_time_speed list.
             * @throws If a signal group has a green timing that partially overlaps with a green timing of another signal group.
             */
            streets_desired_phase_plan::streets_desired_phase_plan convert_spat_to_dpp(
                                                    signal_phase_and_timing::intersection_state &intersection_state, 
                                                    const streets_signal_optimization::movement_groups &move_groups) const;


            /**
             * @brief Find the TBD start time based on the provided modified spat. The start time of the TBD area is equal to
             * the end time of the last movement event of a given signal group\movement event.
             * 
             * @param intersection_info_ptr An intersection_info pointer.
             * @return uint64_t start time of TBD.
             * @throws If a movement state does not have any movement event in its state_time_speed list.
             * @throws If the last movement event of a signal group\movement event has a state different than stop_and_remain.
             */
            uint64_t find_tbd_start_time(const signal_phase_and_timing::intersection_state &intersection_state) const;

            /**
             * @brief Configure the scheduler pointer.
             * 
             * @param intersection_info_ptr An intersection_info pointer.
             */
            void configure_scheduler(const std::shared_ptr<OpenAPI::OAIIntersection_info> &intersection_info_ptr);

            /** 
             * @brief Create signal group id to list of entry lane id mapping. Each signal group might be assigned to more than 
             * one entry lanes.
             * 
             * @param intersection_info_ptr An intersection_info pointer.
             */
            void create_signal_group_entry_lane_mapping(const std::shared_ptr<OpenAPI::OAIIntersection_info> &intersection_info_ptr);

            /**
             * @brief Set the configurable parameters to the desired values.
             * 
             * @param _initial_green_buffer initial green buffer in milliseconds.
             * @param _final_green_buffer final green buffer in milliseconds.
             * @param _et_inaccuracy_buffer ET inaccuracy buffer in milliseconds.
             * @param _queue_max_time_headway queue max time headway in milliseconds.
             * @param _so_radius so radius in meters.
             * @param _min_green maximum green in milliseconds.
             * @param _max_green minimum green in milliseconds.
             * @param _desired_future_move_group_count desired number of future movement group.
             */
            void set_configuration(const uint64_t _initial_green_buffer, 
                                    const uint64_t _final_green_buffer, 
                                    const uint64_t _et_inaccuracy_buffer, 
                                    const uint64_t _queue_max_time_headway,
                                    const double _so_radius,
                                    const uint64_t _min_green,
                                    const uint64_t _max_green,
                                    const uint8_t _desired_future_move_group_count);

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
             * @return uint64_t initial green buffer in milliseconds.
             */
            uint64_t get_initial_green_buffer() const;

            /**
             * @brief Set the final green buffer (ms). This value is used to account for safety. The assumption is that no vehicle shall 
             * enter the intersection box after the end of a green phase minus the final green buffer.
             * 
             * @param buffer final green buffer in milliseconds.
             */
            void set_final_green_buffer(const uint64_t buffer);

            /**
             * @brief Get the final green buffer (ms).
             * 
             * @return uint64_t final green buffer in milliseconds.
             */
            uint64_t get_final_green_buffer() const;  

            /**
             * @brief Set the entering time (ET) inaccuracy_buffer (ms). This value is defined to account for entering time (ET) estimation 
             * inaccuracy. To find the required green duration for dissipating a queue from an entry lane, the end of the green is set to 
             * the estimated ET of the last vehicle in the queue plus this configurable parameter plus final_green_buffer.
             * 
             * @param buffer ET inaccuracy buffer in milliseconds.
             */
            void set_et_inaccuracy_buffer(const uint64_t buffer);

            /**
             * @brief Get the ET inaccuracy buffer (ms).
             * 
             * @return uint64_t ET inaccuracy buffer in milliseconds.
             */
            uint64_t get_et_inaccuracy_buffer() const; 

            /**
             * @brief Set the queue max time headway (ms). This value defines the maximum allowed time headway between the estimated 
             * entering times (ETs) of two consecutive vehicles within a queue.
             * 
             * @param buffer queue max time headway in milliseconds.
             */
            void set_queue_max_time_headway(const uint64_t buffer);

            /**
             * @brief Get the queue max time headway (ms).
             * 
             * @return uint64_t queue max time headway in milliseconds.
             */
            uint64_t get_queue_max_time_headway() const;

            /**
             * @brief Set the so area radius (meter). The signal optimization service only considers vehicles that are within the 
             * signal optimization area.
             * 
             * @param radius so radius in meters.
             */
            void set_so_radius(const double radius);

            /**
             * @brief Get the so area radius (meter).
             * 
             * @return double so radius in meters.
             */
            double get_so_radius() const;

            /**
             * @brief Set the configurable minimum green (ms). 
             * 
             * @param _min_green minimum green in milliseconds.
             */
            void set_min_green(const uint64_t _min_green);

            /**
             * @brief Get the configurable minimum green (ms).
             * 
             * @return uint64_t minimum green in milliseconds.
             */
            uint64_t get_min_green() const;

            /**
             * @brief Set the configurable maximum green (ms). 
             * 
             * @param _max_green maximum green in milliseconds.
             */
            void set_max_green(const uint64_t _max_green);

            /**
             * @brief Get the configurable maximum green (ms).
             * 
             * @return uint64_t maximum green in milliseconds.
             */
            uint64_t get_max_green() const;

            /**
             * @brief Set the desired number of future movement groups.
             * 
             * @param count desired number of future movement group.
             */
            void set_desired_future_move_group_count(const uint8_t count);

            /**
             * @brief Get the desired number of future movement groups.
             * 
             * @return uint8_t desired number of future movement group..
             */
            uint8_t get_desired_future_move_group_count() const;

            /**
             * @brief Get the entry lane to signal group mapping.
             * 
             * @return std::unordered_map<uint8_t, std::vector<int>> entry lane to signal group mapping.
             */
            std::unordered_map<uint8_t, std::vector<int>> get_signal_group_entry_lane_mapping() const;
            
    };
}
