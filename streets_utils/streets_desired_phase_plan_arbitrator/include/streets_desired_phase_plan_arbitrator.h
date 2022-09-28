#pragma once
#include "streets_desired_phase_plan_arbitrator_exception.h"
#include "streets_desired_phase_plan.h"
#include "intersection_client_api_lib/OAIIntersection_info.h"
#include "signalized_intersection_schedule.h"
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
    struct ddp_delay_measures_mapping
    {
        int dpp_index = 0;
        float delay_measure = 0.0;
    };

    class streets_desired_phase_plan_arbitrator
    {
    public:
        streets_desired_phase_plan_arbitrator() = default;
        ~streets_desired_phase_plan_arbitrator() = default;

        streets_desired_phase_plan::streets_desired_phase_plan filter_dpp_list_by_optimization_algorithm(
            const std::vector<streets_desired_phase_plan::streets_desired_phase_plan> &dpp_list,
            const OpenAPI::OAIIntersection_info &intersection_info,
            std::shared_ptr<signal_phase_and_timing::spat> spat_ptr,
            const streets_vehicles::vehicle_list &veh_list);

        void update_spat_with_proposed_dpp(std::shared_ptr<signal_phase_and_timing::spat> spat, streets_desired_phase_plan::streets_desired_phase_plan dpp);
        streets_vehicle_scheduler::signalized_intersection_schedule calculate_vehicle_schedules(std::shared_ptr<signal_phase_and_timing::spat> spat_ptr, const streets_vehicles::vehicle_list &veh_list, const OpenAPI::OAIIntersection_info &intersection_info);
        ddp_delay_measures_mapping calculate_delay_measures(const streets_vehicle_scheduler::signalized_intersection_schedule &shcedule, streets_desired_phase_plan::streets_desired_phase_plan dpp);
        streets_desired_phase_plan::streets_desired_phase_plan identify_ddp_by_delay_measures(const std::vector<streets_desired_phase_plan::streets_desired_phase_plan> &dpp_list, const ddp_delay_measures_mapping &ddp_delay_measures);
    };
}