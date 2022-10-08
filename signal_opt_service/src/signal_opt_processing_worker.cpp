#include "signal_opt_processing_worker.h"
namespace signal_opt_service
{
    streets_desired_phase_plan::streets_desired_phase_plan signal_opt_processing_worker::select_optimal_dpp(
        const std::vector<streets_desired_phase_plan::streets_desired_phase_plan> &dpp_list,
        const std::shared_ptr<OpenAPI::OAIIntersection_info> intersection_info_ptr,
        const std::shared_ptr<signal_phase_and_timing::spat> spat_ptr,
        const std::shared_ptr<streets_tsc_configuration::tsc_configuration_state> tsc_config_state,
        const std::shared_ptr<streets_vehicles::vehicle_list> veh_list_ptr,
        uint64_t initial_green_buffer,
        uint64_t final_green_buffer)
    {
        streets_signal_optimization::streets_desired_phase_plan_arbitrator arbitrator;
        auto chosen_dpp = arbitrator.select_optimal_dpp(dpp_list, intersection_info_ptr, spat_ptr, tsc_config_state, veh_list_ptr, initial_green_buffer, final_green_buffer);
        return chosen_dpp;
    }
}