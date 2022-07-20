#pragma once

#include "snmp_client.h"

namespace traffic_signal_controller_service
{
    struct signal_group_state
    {
        int signal_group_id;
        /*Phase associated with signal group*/
        int phase_num;
        /*Minimum green durations for each active vehicle phase*/
        int min_green;
        /*Maximum green durations for each active vehicle phase*/
        int max_green;
        /*Green duration for */
        int green_duration;
        /*Yellow signal duration*/
        int yellow_duration;
        /* Red clearace time */
        int red_clearance;
        /*Red signal duration*/
        int red_duration;
        /*Phase sequence in ring. Stores the sequence starting from the current phase*/
        std::vector<int> phase_seq;
        /*Phases in the same barrier or concurrent group excluding phases from same ring*/
        std::vector<int> concurrent_phases;
    };

    class tsc_state
    {
        private:
        /* A local pointer to an snmp_client object to be used through the tsc_state*/
        std::shared_ptr<snmp_client> snmp_client_worker_;

        /* Map between phase numbers(key) and signal group ids(value) for all active vehicle phases in the Traffic Signal Controller*/
        std::unordered_map<int,int> phase_num_map_;

        /* Map between signal group ids(key) and phase numbers(value) for all active vehicle phases in the Traffic Signal Controller*/
        std::unordered_map<int, int> signal_group_phase_map_;

        /* Mapping between signal group ids(key) and their states(value) defined as a signal group state struct*/
        std::unordered_map<int, signal_group_state> signal_group_state_map_;

        /* The sequence of phases in ring 1 of TSC*/
        std::vector<int> phase_seq_ring1_;
        /* The sequence of phases in ring 2 of TSC*/
        std::vector<int> phase_seq_ring2_;


        public:
        /** 
         * @brief Constructor for the tsc_state class 
         * @param snmp_client A pointer to an snmp_client worker with a connection established to a traffic signal controller
        **/
        explicit tsc_state(std::shared_ptr<snmp_client> snmp_client);

        /** 
         * @brief Method for getting maximum channels for the traffic signal controller
         * @return number of maximum channels in the traffic signal controller
        **/
        int get_max_channels() const;
        
        /** @brief Returns a vector of channels associated with a vehicle phase. Ignores pedestrian phase, overlap, ped Overlap, queueJump and other (types defined in NTCIP1202 v03)
        **  @param max_channels The maximum number of channels in the traffic signal controller.
        **  @return a vector of active vehicle phases associated with a channel
        **/
        std::vector<int> get_vehicle_phase_channels(int max_channels) const;

        /** @brief Constructs a map between phase number and signal group ids
        ** @param vehicle_phase_channels a vector of channel numbers in the traffic signal controller associated with a vehicle phase
        * According to the NTCIP 1202 v03 documentation signal group ids in the SPAT message are the channel numbers in the TSC
        * **/
        void map_phase_and_signalgroup(const std::vector<int>& vehicle_phase_channels);

        /** @brief Get minimum green time for a phase
        ** @param phase_num The phase for which the min green needs to be requested
        ** @return minimum green time in milliseconds
        * **/
        int get_min_green(int phase_num);

        /** @brief Get maximum green time for a phase
        ** @param phase_num The phase for which the maximum green needs to be requested
        ** @return maximum green time in milliseconds
        * **/
        int get_max_green(int phase_num);

        /** @brief Get yellow duration time for a phase
        ** @param phase_num The phase for which the yellow duration needs to be requested
        ** @return yellow duration time in milliseconds
        * **/
        int get_yellow_duration(int phase_num);

        /** @brief Get red clearance time for a phase
        ** @param phase_num The phase for which the red clearance needs to be requested
        ** @return red clearance time in milliseconds
        * **/
        int get_red_clearance(int phase_num);

        /** @brief Get red duration for a phase
        ** @param phase_num The phase for which the red duration needs to be requested
        ** @return red duration time in milliseconds
        * **/
        int get_red_duration(int phase_num);

        /** @brief Get a sequence of phases following the given phase
        ** @param phase_num The phase for which the sequence needs to be obtained
        ** @return a vector as a sequence of phases starting from the given phase
        * **/
        std::vector<int> get_following_phases(int phase_num);

        /** @brief Get a sequence of phases in the given ring
        ** @param ring_num The phase for which the sequence needs to be obtained
        ** @return a vector as a sequence of phases in the ring
        * **/
        std::vector<int> phase_seq(int ring_num);

        /** @brief The concurrent phases that the given phase can be green along with
        ** @param ring_num The phase for which the concurrent phases needs to be obtained
        ** @return a vector of phases that may be concurrent with the given phase
        * **/
        std::vector<int> get_concurrent_phases(int phase_num);
    };
}