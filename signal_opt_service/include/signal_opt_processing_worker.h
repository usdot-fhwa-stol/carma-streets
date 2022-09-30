#pragma once
#include "streets_desired_phase_plan_arbitrator_exception.h"
#include "streets_desired_phase_plan_arbitrator.h"
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

namespace signal_opt_service
{
    class signal_opt_processing_worker
    {
    public:
        signal_opt_processing_worker() = default;
        ~signal_opt_processing_worker() = default;

        /**
         * @brief Call streets_desired_phase_plan_arbitrator library to choose the optimized desired phase plan.
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
        void filter_desired_phase_plan_by_algorithm(
            const std::shared_ptr<streets_desired_phase_plan_arbitrator::streets_desired_phase_plan_arbitrator> arbitrator_ptr,
            const std::vector<streets_desired_phase_plan::streets_desired_phase_plan> &dpp_list,
            const std::shared_ptr<OpenAPI::OAIIntersection_info> &intersection_info_ptr,
            std::shared_ptr<signal_phase_and_timing::spat> spat_ptr,
            std::shared_ptr<std::unordered_map<int, streets_tsc_configuration::signal_group_configuration>> sg_yellow_duration_red_clearnace_map_ptr,
            streets_vehicles::vehicle_list &veh_list,
            uint64_t initial_green_buffer,
            uint64_t final_green_buffer);
    };

}
