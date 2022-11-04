#pragma once
#include "streets_desired_phase_plan_arbitrator_exception.h"
#include "streets_desired_phase_plan_arbitrator.h"
#include "streets_desired_phase_plan_generator.h"
#include "streets_desired_phase_plan.h"
#include "intersection_client_api_lib/OAIIntersection_info.h"
#include "signalized_intersection_schedule.h"
#include "signalized_vehicle_scheduler.h"
#include "vehicle_list.h"
#include "spat.h"
#include <spdlog/spdlog.h>
#include <math.h>
#include <chrono>

namespace signal_opt_service
{
    class signal_opt_processing_worker
    {
    private:
        /**
         * @brief a shared pointer to desired phase plan generator object.
        */
        std::shared_ptr<streets_signal_optimization::streets_desired_phase_plan_generator> dpp_generator_ptr;

        /**
         * @brief a shared pointer to desired phase plan arbitrator object.
        */
        std::shared_ptr<streets_signal_optimization::streets_desired_phase_plan_arbitrator> dpp_arbitrator_ptr;

    public:

        signal_opt_processing_worker() = default;
        

        ~signal_opt_processing_worker() = default;

        /**
         * @brief Configure streets_signal_optimization library.
         * 
         * @param dpp_config desired_phase_plan_generator_configuration object
         */
        void configure_dpp_optimizer(const streets_signal_optimization::streets_desired_phase_plan_generator_configuration &dpp_config);

        /**
         * @brief Convert the provided intersection_state to a desired phase plan. The number of fixed future movement groups
         * shall be at least 1.
         * 
         * @param spat_ptr The spat pointer that points to the latest spat information received from the Kafka stream.
         * @param move_groups The pointer to the list of candidate movement groups.
         * @return streets_desired_phase_plan::streets_desired_phase_plan desired phase plan converted from spat.
         */
        streets_desired_phase_plan::streets_desired_phase_plan convert_spat_to_dpp(
                                const std::shared_ptr<signal_phase_and_timing::spat> &spat_ptr,
                                const std::shared_ptr<streets_signal_optimization::movement_groups> &move_groups) const;

        /**
         * @brief Call streets_signal_optimization library to choose the optimized desired phase plan.
         *
         * @param intersection_info_ptr The current intersection model intersection information
         * @param spat_ptr The spat pointer that points to the latest spat information received from the Kafka stream.
         * @param tsc_config_state The pointer to tsc configuration state object.
         * @param veh_list_ptr The list of vehicles within the intersection communication radius.
         * @param move_groups The pointer to the list of candidate movement groups.
         * @param dpp_config desired_phase_plan_generator_configuration object 
         * @return Return the chosen desired phase plan
         */
        streets_desired_phase_plan::streets_desired_phase_plan select_optimal_dpp(
            const std::shared_ptr<OpenAPI::OAIIntersection_info> &intersection_info_ptr,
            const std::shared_ptr<signal_phase_and_timing::spat> &spat_ptr, 
            const std::shared_ptr<streets_tsc_configuration::tsc_configuration_state> &tsc_config_state, 
            const std::shared_ptr<streets_vehicles::vehicle_list> &veh_list_ptr,
            const std::shared_ptr<streets_signal_optimization::movement_groups> &move_groups,
            const streets_signal_optimization::streets_desired_phase_plan_generator_configuration &dpp_config) const;
        
        /**
         * @brief Method to get the pointer to dpp_generator.
         * 
         * @return A pointer to dpp_generator.
        */
        std::shared_ptr<streets_signal_optimization::streets_desired_phase_plan_generator> get_dpp_generator() const;
        
    };

}
