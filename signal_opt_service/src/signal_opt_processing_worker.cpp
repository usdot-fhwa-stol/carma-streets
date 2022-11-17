#include "signal_opt_processing_worker.h"
namespace signal_opt_service
{
    
    void signal_opt_processing_worker::configure_signal_opt_processing_worker(
                    const streets_signal_optimization::streets_desired_phase_plan_generator_configuration &dpp_config) 
    {    
        dpp_arbitrator_ptr = std::make_shared<streets_signal_optimization::streets_desired_phase_plan_arbitrator>();
        if (enable_so_logging) {
            dpp_arbitrator_ptr->set_enable_so_logging(enable_so_logging);
        }

        dpp_generator_ptr = std::make_shared<streets_signal_optimization::streets_desired_phase_plan_generator>();
        dpp_generator_ptr->set_configuration(dpp_config.initial_green_buffer,
                                            dpp_config.final_green_buffer,
                                            dpp_config.et_inaccuracy_buffer,
                                            dpp_config.queue_max_time_headway,
                                            dpp_config.so_radius,
                                            dpp_config.min_green,
                                            dpp_config.max_green,
                                            dpp_config.desired_future_move_group_count);
        is_configured = true;
    }
    
    streets_desired_phase_plan::streets_desired_phase_plan signal_opt_processing_worker::convert_spat_to_dpp(
                        const std::shared_ptr<signal_phase_and_timing::spat> &spat_ptr,
                        const std::shared_ptr<streets_signal_optimization::movement_groups> &move_groups) const
    {
        streets_desired_phase_plan::streets_desired_phase_plan spat_dpp;
    
        signal_phase_and_timing::intersection_state int_state = spat_ptr->get_intersection();
        if (dpp_generator_ptr) {
            try 
            {
                spat_dpp = dpp_generator_ptr->convert_spat_to_dpp(int_state, move_groups);
            }
            catch (const streets_signal_optimization::streets_desired_phase_plan_generator_exception &ex) 
            {
                SPDLOG_ERROR("streets_desired_phase_plan_generator SPaT to dpp conversion failure: {0} ", ex.what());
            }
            return spat_dpp;
        }
        else {
            throw streets_signal_optimization::streets_desired_phase_plan_generator_exception("dpp_generator_ptr is not initialized!");
        }
    }
    
    streets_desired_phase_plan::streets_desired_phase_plan signal_opt_processing_worker::select_optimal_dpp(
        const std::shared_ptr<OpenAPI::OAIIntersection_info> &intersection_info_ptr,
        const std::shared_ptr<signal_phase_and_timing::spat> &spat_ptr, 
        const std::shared_ptr<streets_tsc_configuration::tsc_configuration_state> &tsc_config_state, 
        const std::shared_ptr<streets_vehicles::vehicle_list> &veh_list_ptr,
        const std::shared_ptr<streets_signal_optimization::movement_groups> &move_groups,
        const streets_signal_optimization::streets_desired_phase_plan_generator_configuration &dpp_config)
    {
        if (!is_configured) {
            configure_signal_opt_processing_worker(dpp_config);
        }

        auto intersection_state = spat_ptr->get_intersection();
        auto vehicle_map = veh_list_ptr->get_vehicles();

        std::vector<streets_desired_phase_plan::streets_desired_phase_plan> dpp_list;
        try 
        {
            dpp_generator_ptr->create_signal_group_entry_lane_mapping(intersection_info_ptr);
            dpp_list = dpp_generator_ptr->generate_desire_phase_plan_list(intersection_info_ptr, 
                                                                            vehicle_map, 
                                                                            intersection_state, 
                                                                            move_groups, 
                                                                            tsc_config_state);
        }
        catch (const streets_signal_optimization::streets_desired_phase_plan_generator_exception &ex) 
        {
            SPDLOG_ERROR("streets_desired_phase_plan_generator desired phase plan list generation failure: {0} ", ex.what());
        }

        streets_desired_phase_plan::streets_desired_phase_plan chosen_dpp;
        try
        {
            chosen_dpp = dpp_arbitrator_ptr->select_optimal_dpp(dpp_list, 
                                                                intersection_info_ptr, 
                                                                spat_ptr, 
                                                                tsc_config_state, 
                                                                veh_list_ptr, 
                                                                dpp_config.initial_green_buffer, 
                                                                dpp_config.final_green_buffer, 
                                                                dpp_config.so_radius);
        }
        catch(const streets_signal_optimization::streets_desired_phase_plan_arbitrator_exception &ex)
        {
            SPDLOG_ERROR("streets_desired_phase_plan_arbitrator optimal desired phase plan selection failure: {0} ", ex.what());
        }

        return chosen_dpp;
    }

    void signal_opt_processing_worker::set_enable_so_logging(const bool _enable_so_logging) {
        enable_so_logging = _enable_so_logging;
    }

    bool signal_opt_processing_worker::get_is_configured() const {
        return is_configured;
    }
}