#pragma once

#include "streets_desired_phase_plan.h"
#include "snmp_client.h"
#include "ntcip_oids.h"
#include "monitor_tsc_state.h"
#include "control_tsc_state_exception.h"

namespace traffic_signal_controller_service
{
    struct tsc_control_struct
    {
        std::shared_ptr<snmp_client> snmp_client_worker_;
        request_type type = request_type::SET;
        snmp_response_obj omit_;
        snmp_response_obj hold_; 

        tsc_control_struct(std::shared_ptr<snmp_client> snmp_client_worker, int64_t omit_val, int64_t hold_val) : snmp_client_worker_(snmp_client_worker)
        {
            
            omit_.type = snmp_response_obj::response_type::INTEGER;
            omit_.val_int = omit_val;
            
            hold_.type = snmp_response_obj::response_type::INTEGER;
            hold_.val_int = hold_val;       

        }

        bool run()
        {
            if(!snmp_client_worker_->process_snmp_request(ntcip_oids::PHASE_OMIT_CONTROL, type, omit_))
            {
                return false;
            }
            if(!snmp_client_worker_->process_snmp_request(ntcip_oids::PHASE_HOLD_CONTROL, type, hold_))
            {
                return false;
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
             * A mapping between ped phases and signal group ids and desired phase plan, because each new desired phase takes preference over the existing one.
             * @param snmp_client A pointer to an snmp_client worker with a connection established to a traffic signal controller
             * @param phase_to_signal_group_map A map from pedestrian phases in the traffic signal controller to the signal group ids
             * @param desired_phase_plan Pointer to the desired phase plan.
             **/
            explicit control_tsc_state(std::shared_ptr<snmp_client> snmp_client, std::unordered_map<int, int>& signal_group_to_phase_map);
            
            void update_tsc_control_queue(std::shared_ptr<streets_desired_phase_plan::streets_desired_phase_plan> desired_phase_plan, std::shared_ptr<std::queue<tsc_control_struct>> tsc_command_queue);

            void omit_and_hold_signal_groups(std::vector<int> signal_groups, std::shared_ptr<std::queue<tsc_control_struct>> tsc_command_queue);

    };
}