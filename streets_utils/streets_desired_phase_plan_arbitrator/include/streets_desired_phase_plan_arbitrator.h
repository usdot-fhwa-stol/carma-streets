#pragma once
#include "streets_desired_phase_plan_arbitrator_exception.h"
#include "streets_desired_phase_plan.h"
#include "intersection_client_api_lib/OAIIntersection_info.h"
#include "signalized_intersection_schedule.h"
#include "signalized_vehicle_scheduler.h"
#include "vehicle_list.h"
#include "spat.h"
#include <rapidjson/rapidjson.h>
#include <rapidjson/document.h>
#include <spdlog/spdlog.h>
#include <rapidjson/writer.h>
#include <rapidjson/stringbuffer.h>
#include <math.h>
#include <chrono>

namespace streets_desired_phase_plan_arbitrator
{
    class streets_desired_phase_plan_arbitrator
    {
    public:
        streets_desired_phase_plan_arbitrator() = default;
        ~streets_desired_phase_plan_arbitrator() = default;
        /**
         * @brief Iterate movement groups and list of proposed desired phase plan, and return the optimized desired phase plan.
         *
         * @param dpp_list List of proposed desired phase plan.
         * @param intersection_info_ptr The current intersection model intersection information
         * @param spat_ptr The spat pointer that points to the latest spat information received from the Kafka stream.
         * @param sg_yellow_duration_red_clearnace_map_ptr The Map of signal group and yellow change and red clearance duration values
         * @param veh_list The list of vehicles within the intersection communication radius.
         * @param initial_green_buffer A configuration parameter for green phase
         * @param final_green_buffer A configuration parameter for green phase
         * @return ** streets_desired_phase_plan::streets_desired_phase_plan The final desired phase plan
         */
        streets_desired_phase_plan::streets_desired_phase_plan filter_dpp_list_by_optimization_algorithm(
            const std::vector<streets_desired_phase_plan::streets_desired_phase_plan> &dpp_list,
            const std::shared_ptr<OpenAPI::OAIIntersection_info> &intersection_info_ptr,
            std::shared_ptr<signal_phase_and_timing::spat> spat_ptr,
            std::shared_ptr<std::unordered_map<int, streets_tsc_configuration::signal_group_configuration>> sg_yellow_duration_red_clearnace_map_ptr,
            streets_vehicles::vehicle_list &veh_list,
            uint64_t initial_green_buffer,
            uint64_t final_green_buffer);
            
        /**
         * @brief Update the local copy of spat object with the desired phase plan.
         *
         * @param local_spat A local copy of spat object.
         * @param proposed_dpp The current proposed desired phase plan.
         * @param sg_yellow_duration_red_clearnace_map_ptr  The Map of signal group and yellow change and red clearance duration values.
         */
        void update_spat_with_proposed_dpp(signal_phase_and_timing::spat &local_spat, const streets_desired_phase_plan::streets_desired_phase_plan &proposed_dpp, std::shared_ptr<std::unordered_map<int, streets_tsc_configuration::signal_group_configuration>> sg_yellow_duration_red_clearnace_map_ptr);

        /**
         * @brief Update vehicles schedules with the signalized scheduler and updated local spat object.
         *
         * @param schedule_ptr An intialized schedule pointer to be updated by this function.
         * @param local_spat A local copy of spat object.
         * @param intersection_info_ptr The current intersection model intersection information.
         * @param veh_list The list of vehicles within the intersection communication radius.
         * @param initial_green_buffer A configuration parameter for green phase.
         * @param final_green_buffer A configuration parameter for green phase.
         */
        void calculate_vehicle_schedules(std::shared_ptr<streets_vehicle_scheduler::intersection_schedule> schedule_ptr, signal_phase_and_timing::spat &local_spat, streets_vehicles::vehicle_list &veh_list, const std::shared_ptr<OpenAPI::OAIIntersection_info> &intersection_info_ptr, uint64_t initial_green_buffer, uint64_t final_green_buffer);
        
        /**
         * @brief Calculate delay measure for each proposed desired phase plan choice.
         * 
         * @param schedule_ptr A schedule pointer that points to schedule object with list of vehicle schedules.
         * @param proposed_dpp  The current proposed desired phase plan.
         * @return float The calculated delay measure.
         */
        float calculate_delay_measure(std::shared_ptr<streets_vehicle_scheduler::signalized_intersection_schedule> schedule_ptr, streets_desired_phase_plan::streets_desired_phase_plan proposed_dpp);
        
        /**
         * @brief Find the desired phase plan based on the highest delay measure.
         * 
         * @param dpp_list A list of desired phase plan.
         * @param ddp_index_delay_measure_mappings A Map of desired phase plan index and delay measure values.
         * @return streets_desired_phase_plan::streets_desired_phase_plan.
         */
        streets_desired_phase_plan::streets_desired_phase_plan identify_ddp_by_delay_measures(const std::vector<streets_desired_phase_plan::streets_desired_phase_plan> &dpp_list, const std::unordered_map<int, float> &ddp_index_delay_measure_mappings);
    };
}