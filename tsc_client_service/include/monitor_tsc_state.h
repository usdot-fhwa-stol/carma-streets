#pragma once

#include "snmp_client.h"

namespace traffic_signal_controller_service
{
    class tsc_state
    {
        private:
        /* A local pointer to an snmp_client object to be used through the tsc_state*/
        std::shared_ptr<snmp_client> snmp_client_worker_;

        /* Map between phase numbers(key) and signal group ids(value) for all active vehicle phases in the Traffic Signal Controller*/
        std::unordered_map<int,int> phase_num_map_;

        /* Map between signal group ids(key) and phase numbers(value) for all active vehicle phases in the Traffic Signal Controller*/
        std::unordered_map<int,int> signal_group_map_;

        public:
        /** 
         * @brief Constructor for the tsc_state class 
         * @param snmp_client A pointer to an snmp_client worker with a connection established to a traffic signal controller
        **/
        tsc_state(std::shared_ptr<snmp_client> snmp_client);

        /** 
         * @brief Method for getting maximum channels for the traffic signal controller
         * @return number of maximum channels in the traffic signal controller
        **/
        int get_max_channels();
        
        /** @brief Returns a vector of channels associated with a vehicle phase. Ignores pedestrian phase, overlap, ped Overlap, queueJump and other (types defined in NTCIP1202 v03)
        **  @param max_channels The maximum number of channels in the traffic signal controller.
        **  @return a vector of active vehicle phases associated with a channel
        **/
        std::vector<int> get_vehicle_phase_channels(int max_channels);

        /** @brief Constructs a map between phase number and signal group ids
        ** @param vehicle_phase_channels a vector of channel numbers in the traffic signal controller associated with a vehicle phase
        * According to the NTCIP 1202 v03 documentation signal group ids in the SPAT message are the channel numbers in the TSC
        * **/
        void map_phase_and_signalgroup(const std::vector<int>& vehicle_phase_channels);

    };
}