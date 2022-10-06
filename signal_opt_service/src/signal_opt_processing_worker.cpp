#include "signal_opt_processing_worker.h"
namespace signal_opt_service
{
    void signal_opt_processing_worker::select_optimal_dpp(
        const std::vector<streets_desired_phase_plan::streets_desired_phase_plan> &dpp_list,
        const std::shared_ptr<OpenAPI::OAIIntersection_info> intersection_info_ptr,
        const std::shared_ptr<signal_phase_and_timing::spat> spat_ptr,
        const std::shared_ptr<std::unordered_map<int, streets_tsc_configuration::signal_group_configuration>> sg_yellow_duration_red_clearnace_map_ptr,
        const std::shared_ptr<streets_vehicles::vehicle_list> veh_list_ptr,
        uint64_t initial_green_buffer,
        uint64_t final_green_buffer)
    {
        streets_signal_optimization::streets_desired_phase_plan_arbitrator arbitrator;
        _selected_final_dpp = arbitrator.select_optimal_dpp(dpp_list, intersection_info_ptr, spat_ptr, sg_yellow_duration_red_clearnace_map_ptr, veh_list_ptr, initial_green_buffer, final_green_buffer);
    }
}