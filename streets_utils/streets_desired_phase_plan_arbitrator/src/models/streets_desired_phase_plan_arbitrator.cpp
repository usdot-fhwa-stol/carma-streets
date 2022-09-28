#include "streets_desired_phase_plan_arbitrator.h"

namespace streets_desired_phase_plan_arbitrator
{

    streets_desired_phase_plan::streets_desired_phase_plan streets_desired_phase_plan_arbitrator::filter_dpp_list_by_optimization_algorithm(
        const std::vector<streets_desired_phase_plan::streets_desired_phase_plan> &dpp_list,
        const OpenAPI::OAIIntersection_info &intersection_info,
        std::shared_ptr<signal_phase_and_timing::spat> spat_ptr,
        const streets_vehicles::vehicle_list &veh_list)
    {
    }

    void streets_desired_phase_plan_arbitrator::update_spat_with_proposed_dpp(std::shared_ptr<signal_phase_and_timing::spat> spat_ptr, streets_desired_phase_plan::streets_desired_phase_plan dpp)
    {
    }

    streets_vehicle_scheduler::signalized_intersection_schedule streets_desired_phase_plan_arbitrator::calculate_vehicle_schedules(std::shared_ptr<signal_phase_and_timing::spat> spat_ptr, const streets_vehicles::vehicle_list &veh_list, const OpenAPI::OAIIntersection_info &intersection_info)
    {
    }

    ddp_delay_measures_mapping streets_desired_phase_plan_arbitrator::calculate_delay_measures(const streets_vehicle_scheduler::signalized_intersection_schedule &shcedule, streets_desired_phase_plan::streets_desired_phase_plan dpp)
    {
    }

    streets_desired_phase_plan::streets_desired_phase_plan streets_desired_phase_plan_arbitrator::identify_ddp_by_delay_measures(const std::vector<streets_desired_phase_plan::streets_desired_phase_plan> &dpp_list, const ddp_delay_measures_mapping &ddp_delay_measures)
    {
    }
}