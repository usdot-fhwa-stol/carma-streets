#pragma once
#include <spdlog/spdlog.h>
#include "streets_desired_phase_plan_arbitrator_exception.h"
#include "streets_desired_phase_plan.h"
#include "intersection_client_api_lib/OAIIntersection_info.h"
#include "signalized_intersection_schedule.h"
#include "signalized_vehicle_scheduler.h"
#include "vehicle_list.h"
#include "spat.h"
#include <math.h>
#include <chrono>

namespace streets_signal_optimization
{
    class streets_desired_phase_plan_arbitrator
    {
    private:
        /**
         * @brief The bool flag to enable signal optimization calculation csv logging.
         */   
        bool enable_so_logging = false;

    public:
        streets_desired_phase_plan_arbitrator() = default;
        ~streets_desired_phase_plan_arbitrator() = default;
        /**
         * @brief Iterate over all movement groups in each candidate desired phase plan and return optimal desired phase plan.
         * Select optimal candidate desired phase plan based on delay measure ( delay served/delay added). Delay is a vehicles EET - ET.
         * @param dpp_list List of candidate  desired phase plan.
         * @param intersection_info_ptr The current intersection model intersection information.
         * @param spat_ptr The spat pointer that points to the latest spat information received from the Kafka stream.
         * @param tsc_state The Map of signal group and yellow change and red clearance duration values.
         * @param veh_list The list of vehicles within the intersection communication radius.
         * @param initial_green_buffer A configuration parameter for green phase.
         * @param final_green_buffer A configuration parameter for green phase.
         * @param so_radius The configurable distance in meters defined as the radius of the signal optimization (SO) area.
         */
        streets_desired_phase_plan::streets_desired_phase_plan select_optimal_dpp(
            const std::vector<streets_desired_phase_plan::streets_desired_phase_plan> &dpp_list,
            const std::shared_ptr<OpenAPI::OAIIntersection_info> intersection_info_ptr,
            const std::shared_ptr<signal_phase_and_timing::spat> spat_ptr,
            const std::shared_ptr<streets_tsc_configuration::tsc_configuration_state> tsc_state,
            const std::shared_ptr<streets_vehicles::vehicle_list> veh_list_ptr,
            uint64_t initial_green_buffer,
            uint64_t final_green_buffer, 
            const double so_radius) const;

        /**
         * @brief Update the local copy of spat object with the desired phase plan.
         *
         * @param local_spat A local copy of spat object.
         * @param candidate_dpp The current candidate  desired phase plan.
         * @param tsc_state  The Map of signal group and yellow change and red clearance duration values.
         */
        void update_spat_with_candidate_dpp(std::shared_ptr<signal_phase_and_timing::spat> local_spat_ptr,
                                            const streets_desired_phase_plan::streets_desired_phase_plan &candidate_dpp,
                                            const std::shared_ptr<streets_tsc_configuration::tsc_configuration_state> tsc_state) const;

        /**
         * @brief Calculate vehicles' schedules with the signalized scheduler and the local spat object.
         * The calculated vehicles' schedules will be stored in an object pointed by the schedule_ptr pointer.
         * @param schedule_ptr An intialized schedule pointer to be updated by this function.
         * @param local_spat A local copy of spat object.
         * @param intersection_info_ptr The current intersection model intersection information.
         * @param veh_list The list of vehicles within the intersection communication radius.
         * @param initial_green_buffer A configuration parameter for green phase.
         * @param final_green_buffer A configuration parameter for green phase.
         */
        void calculate_vehicle_schedules(std::shared_ptr<streets_vehicle_scheduler::intersection_schedule> schedule_ptr,
                                         std::shared_ptr<signal_phase_and_timing::spat> local_spat_ptr,
                                         const std::shared_ptr<streets_vehicles::vehicle_list> veh_list_ptr,
                                         const std::shared_ptr<OpenAPI::OAIIntersection_info> intersection_info_ptr,
                                         uint64_t initial_green_buffer,
                                         uint64_t final_green_buffer) const;

        /**
         * @brief Calculate delay measure for each candidate  desired phase plan choice.
         *
         * @param schedule_ptr A schedule pointer that points to schedule object with list of vehicle schedules.
         * @param candidate_dpp  The current candidate  desired phase plan.
         * @return float The calculated delay measure.
         */
        float calculate_delay_measure(
            const std::shared_ptr<streets_vehicle_scheduler::signalized_intersection_schedule> schedule_ptr,
            const streets_desired_phase_plan::streets_desired_phase_plan &candidate_dpp) const;

        /**
         * @brief Find the desired phase plan based on the highest delay measure.
         *
         * @param dpp_list A list of desired phase plan.
         * @param ddp_index_delay_measure_mappings A Map of desired phase plan index and delay measure values.
         * @return streets_desired_phase_plan::streets_desired_phase_plan.
         */
        streets_desired_phase_plan::streets_desired_phase_plan identify_ddp_by_delay_measures(
            const std::vector<streets_desired_phase_plan::streets_desired_phase_plan> &dpp_list,
            const std::unordered_map<int, float> &ddp_index_delay_measure_mappings) const;

        /**
         * @brief Method to turn the delay calculation for a candidate desired phase plan to CSV string.
         * 
         *  timestamp ,movement group x start time ,movement group x end time, movement group x signal groups, 
         *  delay of vehicles that can enter the intersection box during the last movement group, delay of 
         *  vehicles that cannot enter the intersection box during any movement group (ET within TBD), delay measure
         * 
         * @param candidate_dpp A candidate desired phase plan from the dpp list.
         * @param candidate_vehicle_delay The delay of vehicles that can enter the intersection box during the last movement group.
         * @param TBD_delay The delay of vehicles that cannot enter the intersection box during any movement group (ET within TBD).
         * @param delay_measure candidate_vehicle_delay / TBD_delay
         * @return std::string CSV entry
         */
        std::string dpp_delay_toCSV(const streets_desired_phase_plan::streets_desired_phase_plan &candidate_dpp, 
                                    const u_int64_t candidate_vehicle_delay, 
                                    const u_int64_t TBD_delay, 
                                    const float delay_measure) const;

        /**
         * @brief Method to set enable_so_logging.
        */
        void set_enable_so_logging(const bool _enable_so_logging);
    };
}