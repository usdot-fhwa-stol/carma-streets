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

        streets_desired_phase_plan::streets_desired_phase_plan filter_dpp_list_by_optimization_algorithm(
            const std::vector<streets_desired_phase_plan::streets_desired_phase_plan> &dpp_list,
            const std::shared_ptr<OpenAPI::OAIIntersection_info> &intersection_info_ptr,
            std::shared_ptr<signal_phase_and_timing::spat> spat_ptr,
            std::shared_ptr<std::unordered_map<int, streets_tsc_configuration::signal_group_configuration>> sg_yellow_duration_red_clearnace_map_ptr,
            streets_vehicles::vehicle_list &veh_list,
            uint64_t initial_green_buffer,
            uint64_t final_green_buffer);

        void update_spat_with_proposed_dpp(signal_phase_and_timing::spat &local_spat, const streets_desired_phase_plan::streets_desired_phase_plan& proposed_dpp, std::shared_ptr<std::unordered_map<int, streets_tsc_configuration::signal_group_configuration>> sg_yellow_duration_red_clearnace_map_ptr);
        void calculate_vehicle_schedules(std::shared_ptr<streets_vehicle_scheduler::intersection_schedule> schedule_ptr, signal_phase_and_timing::spat &local_spat, streets_vehicles::vehicle_list &veh_list, const std::shared_ptr<OpenAPI::OAIIntersection_info> &intersection_info_ptr, uint64_t initial_green_buffer, uint64_t final_green_buffer);
        float calculate_delay_measure(std::shared_ptr<streets_vehicle_scheduler::signalized_intersection_schedule> schedule_ptr, streets_desired_phase_plan::streets_desired_phase_plan proposed_dpp);
        streets_desired_phase_plan::streets_desired_phase_plan identify_ddp_by_delay_measures(const std::vector<streets_desired_phase_plan::streets_desired_phase_plan> &dpp_list, const std::unordered_map<int, float> &ddp_index_delay_measure_mappings);
    };
}