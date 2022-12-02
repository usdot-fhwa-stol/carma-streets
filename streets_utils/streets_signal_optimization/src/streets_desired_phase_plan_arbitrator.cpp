#include "streets_desired_phase_plan_arbitrator.h"

namespace streets_signal_optimization
{

    streets_desired_phase_plan::streets_desired_phase_plan streets_desired_phase_plan_arbitrator::select_optimal_dpp(
        const std::vector<streets_desired_phase_plan::streets_desired_phase_plan> &dpp_list,
        const std::shared_ptr<OpenAPI::OAIIntersection_info> intersection_info_ptr,
        const std::shared_ptr<signal_phase_and_timing::spat> spat_ptr,
        const std::shared_ptr<streets_tsc_configuration::tsc_configuration_state> tsc_state, // reuse tsc_configuration_state
        const std::shared_ptr<streets_vehicles::vehicle_list> veh_list_ptr,
        uint64_t initial_green_buffer,
        uint64_t final_green_buffer, 
        const double so_radius) const
    {
        streets_desired_phase_plan::streets_desired_phase_plan result;
        if (!dpp_list.empty())
        {
            if (spat_ptr == nullptr)
            {
                throw(streets_desired_phase_plan_arbitrator_exception("The spat pointer is nullptr."));
            }

            if(veh_list_ptr == nullptr)
            {
                throw(streets_desired_phase_plan_arbitrator_exception("The vehicle list pointer is nullptr."));
            }

            int dpp_index = 0;
            std::unordered_map<int, float> ddp_delay_measures;
            for (const auto &candidate_dpp : dpp_list)
            {
                // A copy of spat object into local variable, and update local spat with candidatedesired phase plan
                auto local_spat_ptr = std::make_shared<signal_phase_and_timing::spat>();
                auto intersection_state = spat_ptr->get_intersection();
                local_spat_ptr->set_intersection(intersection_state);
                update_spat_with_candidate_dpp(local_spat_ptr, candidate_dpp, tsc_state);

                // Given spat and vehicle list, estimate current vehicles' ET and EET from signalized intersection schedule
                auto all_schedule_ptr = std::make_shared<streets_vehicle_scheduler::signalized_intersection_schedule>();
                // set_timestamp for the all_schedule_ptr current Unix timestamp
                calculate_vehicle_schedules(all_schedule_ptr, local_spat_ptr, veh_list_ptr, intersection_info_ptr, initial_green_buffer, final_green_buffer);

                // Add vehicle schedules from all_schedule_ptr for vehicle that are in SO area.
                auto schedule_ptr = std::make_shared<streets_vehicle_scheduler::signalized_intersection_schedule>();
                schedule_ptr->timestamp = all_schedule_ptr->timestamp;
                for (const auto& veh_sched : all_schedule_ptr->vehicle_schedules) {
                    if (veh_sched.state == streets_vehicles::vehicle_state::EV && veh_list_ptr->get_vehicles().at(veh_sched.v_id)._cur_distance <= so_radius) {
                        schedule_ptr->vehicle_schedules.push_back(veh_sched);
                    }
                }

                // Given vehicles' ET and EET from signalized intersection schedule, Calculate vehicles' delay measure for the current candidatedesired phase plan
                float delay_measure = calculate_delay_measure(schedule_ptr, candidate_dpp);

                // Add delay measure and candidatedesired phase plan index mapping to a list
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
            SPDLOG_WARN("The list of possible desired phase plan is empty.");
        }
        return result;
    }

    void streets_desired_phase_plan_arbitrator::update_spat_with_candidate_dpp(
                        std::shared_ptr<signal_phase_and_timing::spat> local_spat_ptr,
                        const streets_desired_phase_plan::streets_desired_phase_plan &candidate_dpp,
                        const std::shared_ptr<streets_tsc_configuration::tsc_configuration_state> tsc_state) const
    {
        local_spat_ptr->update_spat_with_candidate_dpp(candidate_dpp, tsc_state);
    }

    void streets_desired_phase_plan_arbitrator::calculate_vehicle_schedules(
                            std::shared_ptr<streets_vehicle_scheduler::intersection_schedule> all_schedule_ptr,
                            std::shared_ptr<signal_phase_and_timing::spat> local_spat_ptr,
                            const std::shared_ptr<streets_vehicles::vehicle_list> veh_list_ptr,
                            const std::shared_ptr<OpenAPI::OAIIntersection_info> intersection_info_ptr,
                            uint64_t initial_green_buffer,
                            uint64_t final_green_buffer) const
    {
        auto scheduler_ptr = std::make_unique<streets_vehicle_scheduler::signalized_vehicle_scheduler>();
        scheduler_ptr->set_intersection_info(intersection_info_ptr);
        scheduler_ptr->set_spat(local_spat_ptr);
        scheduler_ptr->set_initial_green_buffer(initial_green_buffer);
        scheduler_ptr->set_final_green_buffer(final_green_buffer);
        auto vehicles = veh_list_ptr->get_vehicles();
        all_schedule_ptr->timestamp = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
        scheduler_ptr->schedule_vehicles(vehicles, all_schedule_ptr);
    }

    float streets_desired_phase_plan_arbitrator::calculate_delay_measure(
        const std::shared_ptr<streets_vehicle_scheduler::signalized_intersection_schedule> schedule_ptr,
        const streets_desired_phase_plan::streets_desired_phase_plan &candidate_dpp) const
    {
        if (schedule_ptr->vehicle_schedules.empty())
        {
            throw(streets_desired_phase_plan_arbitrator_exception("Vehicle schedule cannot be empty. Vehicles maybe outside of the radius."));
        }
        u_int64_t TBD_delay = 0;
        u_int64_t candidate_vehicles_delay = 0;
        for (const auto &veh_schedule : schedule_ptr->vehicle_schedules)
        {
            // If find the vehicles’ schedule ET > candidate desired phase plan end time, these vehicles are considered TBD.
            if (veh_schedule.et > candidate_dpp.desired_phase_plan.back().end_time)
            {
                TBD_delay += veh_schedule.get_delay();
                SPDLOG_DEBUG("TBD veh_schedule v_ID= {0}, get_delay = {1}", veh_schedule.v_id, veh_schedule.get_delay());
            }
            // If find the vehicles’ schedule ET <= candidate desired phase plan end time and the vehicles’ schedule ET => candidate desired phase plan start time.
            else if (veh_schedule.et >= candidate_dpp.desired_phase_plan.back().start_time)
            {
                candidate_vehicles_delay += veh_schedule.get_delay();
                SPDLOG_DEBUG("candidate veh_schedule v_ID= {0}, get_delay = {1}", veh_schedule.v_id, veh_schedule.get_delay());
            }
        }

        float delay_measure = 0.0;
        if (TBD_delay > 0)
        {
            delay_measure = (float)candidate_vehicles_delay / (float)TBD_delay;
        }
        else
        {
            delay_measure = (float)candidate_vehicles_delay;
        }
        SPDLOG_DEBUG("calculated delay_measure (= candidate/TBD) = {0}", delay_measure);

        if (enable_so_logging) {
            auto logger = spdlog::get("so_csv_logger");
            if ( logger != nullptr ) {
                logger->info( dpp_delay_toCSV(candidate_dpp, candidate_vehicles_delay, TBD_delay, delay_measure) );
            }
        }

        return delay_measure;
    }

    streets_desired_phase_plan::streets_desired_phase_plan streets_desired_phase_plan_arbitrator::identify_ddp_by_delay_measures(
        const std::vector<streets_desired_phase_plan::streets_desired_phase_plan> &dpp_list,
        const std::unordered_map<int, float> &ddp_index_delay_measure_mappings) const
    {
        streets_desired_phase_plan::streets_desired_phase_plan dpp_result;
        if (!ddp_index_delay_measure_mappings.empty())
        {
            int dpp_index = ddp_index_delay_measure_mappings.begin()->first;
            float max_delay_measure = ddp_index_delay_measure_mappings.begin()->second;

            // Find the highest delay_measure value, and return its desired phase plan index
            for (const auto &[cur_dpp_index, delay_measure] : ddp_index_delay_measure_mappings)
            {
                SPDLOG_DEBUG("Curent Desired phase plan = {0} , delay measure = {1}", dpp_list.at(dpp_index).toJson(), delay_measure);
                if (max_delay_measure < delay_measure)
                {
                    max_delay_measure = delay_measure;
                    dpp_index = cur_dpp_index;
                }
            }
            dpp_result = dpp_list.at(dpp_index);
            SPDLOG_DEBUG("Curent Desired phase plan = {0} , MAX delay measure = {1}", dpp_list.at(dpp_index).toJson(), max_delay_measure);
        }

        return dpp_result;
    }

    std::string streets_desired_phase_plan_arbitrator::dpp_delay_toCSV(
                const streets_desired_phase_plan::streets_desired_phase_plan &candidate_dpp, 
                const u_int64_t candidate_vehicle_delay, 
                const u_int64_t TBD_delay, 
                const float delay_measure) const 
    {
        
        std::string so_info = "";
        // timestamp
        so_info += std::to_string(candidate_dpp.timestamp) + ",";
        // candidate vehicle delay
        so_info += std::to_string(candidate_vehicle_delay) + ",";
        // TBD delay
        so_info += std::to_string(TBD_delay) + ",";
        // delay measure
        so_info += std::to_string(delay_measure) + ",";
        //desired phase plan
        so_info += candidate_dpp.toJson();

        return so_info;
    }

    void streets_desired_phase_plan_arbitrator::set_enable_so_logging(const bool _enable_so_logging) {
        enable_so_logging = _enable_so_logging;
    }
}