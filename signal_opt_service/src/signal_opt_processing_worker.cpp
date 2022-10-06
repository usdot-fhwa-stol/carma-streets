#include "signal_opt_processing_worker.h"
namespace signal_opt_service
{
    void signal_opt_processing_worker::select_optimal_dpp(
        const std::vector<streets_desired_phase_plan::streets_desired_phase_plan> &dpp_list,
        const std::shared_ptr<OpenAPI::OAIIntersection_info> &intersection_info_ptr,
        std::shared_ptr<signal_phase_and_timing::spat> spat_ptr,
        std::shared_ptr<std::unordered_map<int, streets_tsc_configuration::signal_group_configuration>> sg_yellow_duration_red_clearnace_map_ptr,
        streets_vehicles::vehicle_list &veh_list,
        uint64_t initial_green_buffer,
        uint64_t final_green_buffer)
    {

        const auto arbitrator_ptr = std::make_shared<streets_desired_phase_plan_arbitrator::streets_desired_phase_plan_arbitrator>();
        _selected_final_dpp = arbitrator_ptr->select_optimal_dpp(dpp_list, intersection_info_ptr, spat_ptr, sg_yellow_duration_red_clearnace_map_ptr, veh_list, initial_green_buffer, final_green_buffer);
    }
}