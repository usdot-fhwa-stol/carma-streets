#pragma once

#include "streets_desired_phase_plan.h"
#include "snmp_client.h"
#include "ntcip_oids.h"
#include "monitor_tsc_state.h"
#include "control_tsc_state_exception.h"

namespace traffic_signal_controller_service
{

    /** @brief Object to store snmp control commands. Contructed with an initialized snmp_client_worker_ this object stores SNMP HOLD and OMIT commands 
     to be executed at specified time */
    struct snmp_cmd_struct
    {
        /** @brief The type of control being set on the TSC */
        enum class control_type{
            Hold,
            Omit
        };
        /* Pointer to a snmp client object which can execute snmp commands*/
        std::shared_ptr<snmp_client> snmp_client_worker_;
        /*Value to be set for Hold/Omit*/
        snmp_response_obj set_val_;
        /*Time at which the snmp set command should be executed*/
        uint64_t start_time_;
        /*Type of the snmp set command this object creates- Hold or Omit*/
        control_type control_type_;

        snmp_cmd_struct(std::shared_ptr<snmp_client> snmp_client_worker, int64_t start_time, control_type type, int64_t val) 
                                : snmp_client_worker_(snmp_client_worker), start_time_(start_time), control_type_(type)
        {
            set_val_.type = snmp_response_obj::response_type::INTEGER;
            set_val_.val_int = val;
        }

        /**
         * @brief Method to call the snmp command. Object type determines what SET command is sent. 
         * Types are Omit and Hold.
         * @return True if SET commands are successful. False if command fails.
         * */
        bool run()
        {
            /*Type of request to be sent to the TSC, within this context it is always SET*/
            request_type type = request_type::SET;

            if(control_type_ == control_type::Omit)
            {
                if(!snmp_client_worker_->process_snmp_request(ntcip_oids::PHASE_OMIT_CONTROL, type, set_val_)){
                    return false;
                }
            }
            else
            {
                if(!snmp_client_worker_->process_snmp_request(ntcip_oids::PHASE_HOLD_CONTROL, type, set_val_)){
                    return false;
                }
            }

            return true;
            
        }
    };

    class control_tsc_state
    {
        // This class aims to control the phases on the traffic signal controller using Omit and Hold SNMP commands
        private:

            /* A local pointer to an snmp_client object to be used through the tsc_state*/
            std::shared_ptr<snmp_client> snmp_client_worker_;

            // Mapping from signal group ids to pedestrian phases
            std::unordered_map<int, int> signal_group_2ped_phase_map_;
        
        public:

            /** 
             * @brief Constructor for the control_tsc_state class. Needs to be initialized with pointer to snmp client to make snmp calls, 
             * A mapping between ped phases and signal group ids and desired phase plan
             * @param snmp_client A pointer to an snmp_client worker with a connection established to a traffic signal controller
             * @param phase_to_signal_group_map A map from pedestrian phases in the traffic signal controller to the signal group ids
             * @param desired_phase_plan Pointer to the desired phase plan.
             **/
            explicit control_tsc_state(std::shared_ptr<snmp_client> snmp_client,const std::unordered_map<int, int>& signal_group_to_phase_map);
            
            /** 
             * @brief Method to update the queue of tsc_control
             * @param desired_phase_plan Pointer to the desired phase plan.
             * @param tsc_command_queue Queue of snmp commands to set HOLD and OMIT on the traffic signal controller
             **/
            void update_tsc_control_queue(std::shared_ptr<streets_desired_phase_plan::streets_desired_phase_plan> desired_phase_plan, std::queue<snmp_cmd_struct>& tsc_command_queue);

            /** 
             * @brief Method to create OMIT snmp command for provided signal groups, which will result in the traffic signal controller skipping the specified phases. 
             * This method should be sent before any yellow phase for omission to take effect.
             * @param signal_groups A list of signal groups NOT to be omitted. Omit command will aim to omit everything besides signal groups specified here.
             * @param start_time Time at which the snmp command needs to be sent
             * @param is_reset if true, omit command is reset on the traffic signal controller to 0. 
             * If false will calculate the omit value required to reach given signal groups
             **/
            snmp_cmd_struct create_omit_command(const std::vector<int>& signal_groups, int64_t start_time, bool is_reset = false);

            /** 
             * @brief Method to create command for Hold for provided signal groups, 
             * which will result in the traffic signal controller "holding" the specified phases till a change in the Hold command.
             * @param signal_groups A list of signal groups NOT to be omitted. Hold command will aim to hold the signal groups specified here.
             * @param start_time Time at which the snmp command needs to be sent
             * @param is_reset if true, hold command is reset on the traffic signal controller to 0. 
             * If false will calculate the value required to hold given signal groups
             **/
            snmp_cmd_struct create_hold_command(const std::vector<int>& signal_groups, int64_t start_time, bool is_reset = false);
    };
}