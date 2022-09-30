#include "streets_desired_phase_plan_arbitrator.h"

namespace streets_desired_phase_plan_arbitrator
{

    streets_desired_phase_plan::streets_desired_phase_plan streets_desired_phase_plan_arbitrator::filter_dpp_list_by_optimization_algorithm(
        const std::vector<streets_desired_phase_plan::streets_desired_phase_plan> &dpp_list,
        const std::shared_ptr<OpenAPI::OAIIntersection_info> &intersection_info_ptr,
        std::shared_ptr<signal_phase_and_timing::spat> spat_ptr,
        std::shared_ptr<std::unordered_map<int, streets_tsc_configuration::signal_group_configuration>> sg_yellow_duration_red_clearnace_map_ptr,
        streets_vehicles::vehicle_list &veh_list,
        uint64_t initial_green_buffer,
        uint64_t final_green_buffer)
    {
        streets_desired_phase_plan::streets_desired_phase_plan result;
        if (dpp_list.size() != 0)
        {
            if (spat_ptr == nullptr)
            {
                SPDLOG_ERROR("The spat pointer is nullptr.");
            }

            int dpp_index = 0;
            std::unordered_map<int, float> ddp_delay_measures;
            for (const auto &proposed_dpp : dpp_list)
            {
                // A copy of spat object into local variable, and update local spat with proposed desired phase plan
                auto local_spat = *spat_ptr;
                update_spat_with_proposed_dpp(local_spat, proposed_dpp,sg_yellow_duration_red_clearnace_map_ptr);

                // Given spat and vehicle list, estimate current vehicles' ET and EET from signalized intersection schedule
                auto schedule_ptr = std::make_shared<streets_vehicle_scheduler::signalized_intersection_schedule>();
                calculate_vehicle_schedules(schedule_ptr, local_spat, veh_list, intersection_info_ptr, initial_green_buffer, final_green_buffer);

                // Given vehicles' ET and EET from signalized intersection schedule, Calculate vehicles' delay measure for the current proposed desired phase plan
                float delay_measure = calculate_delay_measure(schedule_ptr, proposed_dpp);

                // Add delay measure and proposed desired phase plan index mapping to a list
                ddp_delay_measures.insert({dpp_index, delay_measure});
                dpp_index++;
            }

            if (ddp_delay_measures.empty())
            {
                throw(streets_desired_phase_plan_arbitrator_exception("There is no desired phase plan and delay measures mapping."));
            }

            // Find the largest delay_measure value, and return final desired phase plan
            result = identify_ddp_by_delay_measures(dpp_list, ddp_delay_measures);
        }
        else
        {
            SPDLOG_ERROR("The list of possible desired phase plan is empty.");
        }
        return result;
    }

    void streets_desired_phase_plan_arbitrator::update_spat_with_proposed_dpp(signal_phase_and_timing::spat &local_spat, const streets_desired_phase_plan::streets_desired_phase_plan& proposed_dpp, std::shared_ptr<std::unordered_map<int, streets_tsc_configuration::signal_group_configuration>> sg_yellow_duration_red_clearnace_map_ptr)
    {
        local_spat.update_spat_with_proposed_dpp(proposed_dpp, sg_yellow_duration_red_clearnace_map_ptr);
    }

    void streets_desired_phase_plan_arbitrator::calculate_vehicle_schedules(std::shared_ptr<streets_vehicle_scheduler::intersection_schedule> schedule_ptr, signal_phase_and_timing::spat &local_spat, streets_vehicles::vehicle_list &veh_list, const std::shared_ptr<OpenAPI::OAIIntersection_info> &intersection_info_ptr, uint64_t initial_green_buffer, uint64_t final_green_buffer)
    {
        auto scheduler_ptr = std::unique_ptr<streets_vehicle_scheduler::signalized_vehicle_scheduler>(new streets_vehicle_scheduler::signalized_vehicle_scheduler());
        scheduler_ptr->set_intersection_info(intersection_info_ptr);
        auto local_spat_ptr = std::make_shared<signal_phase_and_timing::spat>(local_spat);
        scheduler_ptr->set_spat(local_spat_ptr);
        scheduler_ptr->set_initial_green_buffer(initial_green_buffer);
        scheduler_ptr->set_final_green_buffer(final_green_buffer);
        auto vehicles = veh_list.get_vehicles();
        scheduler_ptr->schedule_vehicles(vehicles, schedule_ptr);
        if (schedule_ptr == nullptr)
        {
            throw(streets_desired_phase_plan_arbitrator_exception("Schedule cannot be empty."));
        }
    }

    float streets_desired_phase_plan_arbitrator::calculate_delay_measure(std::shared_ptr<streets_vehicle_scheduler::signalized_intersection_schedule> schedule_ptr, streets_desired_phase_plan::streets_desired_phase_plan proposed_dpp)
    {
        if (schedule_ptr->vehicle_schedules.empty())
        {
            throw(streets_desired_phase_plan_arbitrator_exception("Vehicle schedule cannot be empty."));
        }
        float TBD_delay = 0.0;
        float proposed_vehicles_delay = 0.0;
        for (const auto &veh_schedule : schedule_ptr->vehicle_schedules)
        {
            if (veh_schedule.et - veh_schedule.eet < 0)
            {
                throw(streets_desired_phase_plan_arbitrator_exception("Vehicle schedule EET cannot be greater than ET."));
            }

            // If find the vehicles’ schedule ET >= proposed desired phase plan end time, these vehicles are considered TBD.
            if (veh_schedule.et >= proposed_dpp.desired_phase_plan.back().end_time)
            {
                TBD_delay += veh_schedule.et - veh_schedule.eet;
            }
            else
            {
                proposed_vehicles_delay += veh_schedule.et - veh_schedule.eet;
            }
        }

        float delay_measure = proposed_vehicles_delay / TBD_delay;
        if (TBD_delay > 0.0)
        {
            delay_measure = proposed_vehicles_delay / TBD_delay;
        }
        else
        {
            delay_measure = proposed_vehicles_delay;
        }
        return delay_measure;
    }

    streets_desired_phase_plan::streets_desired_phase_plan streets_desired_phase_plan_arbitrator::identify_ddp_by_delay_measures(const std::vector<streets_desired_phase_plan::streets_desired_phase_plan> &dpp_list, const std::unordered_map<int, float> &ddp_index_delay_measure_mappings)
    {
        streets_desired_phase_plan::streets_desired_phase_plan dpp_result;
        int dpp_index = 0;
        float max_delay_measure = ddp_index_delay_measure_mappings.at(dpp_index);

        // Find the largest delay_measure value, and return its desired phase plan index
        for (const auto &dpp_dm_mapping : ddp_index_delay_measure_mappings)
        {
            if (max_delay_measure < dpp_dm_mapping.second)
            {
                max_delay_measure = dpp_dm_mapping.second;
                dpp_index = dpp_dm_mapping.first;
            }
        }
        dpp_result = dpp_list.at(dpp_index);
        return dpp_result;
    }
}