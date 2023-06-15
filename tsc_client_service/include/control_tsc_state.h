#pragma once

#include "streets_desired_phase_plan.h"
#include "streets_phase_control_schedule.h"
#include "streets_snmp_cmd_converter.h"
#include "snmp_client.h"
#include "ntcip_oids.h"
#include "monitor_tsc_state.h"
#include "control_tsc_state_exception.h"
#include "streets_clock_singleton.h"

namespace traffic_signal_controller_service
{
    class control_tsc_state
    {
        // This class aims to control the phases on the traffic signal controller using Omit and Hold SNMP commands
        private:

            /* A local pointer to an snmp_client object to be used through the tsc_state*/
            std::shared_ptr<snmp_client> snmp_client_worker_;

            std::shared_ptr<tsc_state> _tsc_state;
        
        public:

            /** 
             * @brief Constructor for the control_tsc_state class. Needs to be initialized with pointer to snmp client to make snmp calls and 
             * a shared pointer to the tsc_state which is used to store traffic signal controller configuration information
             * @param snmp_client A pointer to an snmp_client worker with a connection established to a traffic signal controller
             * @param _tsc_state A shared pointer to the tsc_state which stores information about traffic signal controller configuration
             **/
            explicit control_tsc_state(std::shared_ptr<snmp_client> snmp_client,std::shared_ptr<tsc_state> _tsc_state);
            
            /** 
             * @brief Method to update the queue of tsc_control
             * @param desired_phase_plan Pointer to the desired phase plan.
             * @param tsc_command_queue Queue of snmp commands to set HOLD and OMIT on the traffic signal controller
             **/
            void update_tsc_control_queue(std::shared_ptr<streets_desired_phase_plan::streets_desired_phase_plan> desired_phase_plan, 
                                        std::queue<streets_snmp_cmd::snmp_cmd_struct>& tsc_command_queue) const;

            /** 
             * @brief Method to update the queue of tsc_control
             * @param phase_control_schedule Pointer to the phase control schedule.
             * @param tsc_command_queue Queue of snmp commands to set HOLD and OMIT on the traffic signal controller.This queue is a managed by the tsc_service and passed by reference for update by incoming phase control schedule changes.
             **/
            void update_tsc_control_queue(std::shared_ptr<streets_phase_control_schedule::streets_phase_control_schedule> phase_control_schedule,
                                        std::queue<streets_snmp_cmd::snmp_cmd_struct>& tsc_command_queue) const;
            /** 
             * @brief Method to create OMIT snmp command for provided signal groups, which will result in the traffic signal controller skipping the specified phases. 
             * This method should be sent before any yellow phase for omission to take effect.
             * @param signal_groups A list of signal groups NOT to be omitted. Omit command will aim to omit everything besides signal groups specified here.
             * @param start_time Time at which the snmp command needs to be sent
             * @param is_reset if true, omit command is reset on the traffic signal controller to 0. 
             * If false will calculate the omit value required to reach given signal groups
             **/
            streets_snmp_cmd::snmp_cmd_struct create_omit_command(const std::vector<int>& signal_groups, int64_t start_time, bool is_reset = false) const;

            /** 
             * @brief Method to create command for Hold for provided signal groups, 
             * which will result in the traffic signal controller "holding" the specified phases till a change in the Hold command.
             * @param signal_groups A list of signal groups NOT to be omitted. Hold command will aim to hold the signal groups specified here.
             * @param start_time Time at which the snmp command needs to be sent
             * @param is_reset if true, hold command is reset on the traffic signal controller to 0. 
             * If false will calculate the value required to hold given signal groups
             **/
            streets_snmp_cmd::snmp_cmd_struct create_hold_command(const std::vector<int>& signal_groups, int64_t start_time, bool is_reset = false) const;

            /**
            * @brief Method to call the snmp command. Object type determines what SET command is sent. 
            * Types are Omit, Forceoff, Call and Hold.
            * @return True if SET commands are successful. False if command fails.
            * */
            bool run_snmp_cmd_set_request(streets_snmp_cmd::snmp_cmd_struct& snmp_cmd);
    };
}