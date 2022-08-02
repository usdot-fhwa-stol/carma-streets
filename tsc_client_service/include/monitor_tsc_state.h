#pragma once

#include "snmp_client.h"

namespace traffic_signal_controller_service
{   
    /*The signal group state stores the traffic signal controller configuration parameters for a J2735 signal group
    * The parameters within it define the phase mapped to the signal group and other data necessary to modify a given phase*/
    struct signal_group_state
    {
        /* signal group id identifier for J2735 which is mapped to phase number in NTCIP */
        int signal_group_id;
        /*Phase associated with signal group*/
        int phase_num;
        /*Minimum green durations for each active vehicle phase in miiliseconds*/
        int min_green;
        /*Maximum green durations for each active vehicle phase in miiliseconds*/
        int max_green;
        /*Default Green duration for phase in miiliseconds*/
        int green_duration;
        /*Yellow signal duration for phase in miiliseconds*/
        int yellow_duration;
        /* Red clearace time for phase in miiliseconds*/
        int red_clearance;
        /*Red signal duration for phase. This is the total time a phase is predicted to be red before its next green. In milliseconds*/
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

        /* Map between signal group ids(key) and phase numbers(value) for all active vehicle phases in the Traffic Signal Controller*/
        std::unordered_map<int, int> signal_group_phase_map_;

        /* Mapping between signal group ids(key) and their states(value) defined as a signal group state struct*/
        std::unordered_map<int, signal_group_state> signal_group_state_map_;

        /* Map between phase numbers(key) and signal group ids(value) for all active vehicle phases in the Traffic Signal Controller*/
        std::unordered_map<int,int> vehicle_phase_num_map_;

        /* Map between phase numbers(key) and signal group ids(value) for all active pedestrian phases in the Traffic Signal Controller*/
        std::unordered_map<int,int> ped_phase_num_map_;

        /* The sequence of vehicle phases in ring 1 of TSC*/
        std::vector<int> phase_seq_ring1_;
        /* The sequence of vehicle phases in ring 2 of TSC*/
        std::vector<int> phase_seq_ring2_;

        /** @brief Returns a vector of channels associated with a vehicle phase. Ignores pedestrian phase, overlap, ped Overlap, queueJump and other (types defined in NTCIP1202 v03)
        **  @param max_channels The maximum number of channels in the traffic signal controller.
        **  @param vehicle_phase_channels a vector of active vehicle phases associated with a channel
        **  @param ped_phase_channels a vector of active pedestrian phases associated with a channel
        **/
        void get_phase_channels(int max_channels, std::vector<int>& vehicle_phase_channels, std::vector<int>& ped_phase_channels) const;

        /** @brief Constructs a map between phase number and signal group ids
        ** @param phase_channels a vector of channel numbers in the traffic signal controller associated with a phase
        ** @param is_source_vehicle_channel a boolean that indicates whether the phase_channels arg is for channels associated with vehicle phases. 
        ** In case they are not its assumed that they are pedestrian phases
        * According to the NTCIP 1202 v03 documentation signal group ids in the SPAT message are the channel numbers in the TSC
        * **/
        void map_phase_and_signalgroup(const std::vector<int>& phase_channels, bool is_source_vehicle_channel);

        /** 
         * @brief Method for getting maximum channels for the traffic signal controller
         * @return number of maximum channels in the traffic signal controller
        **/
        int get_max_channels() const;

        /** @brief Get minimum green time for a phase
        ** @param phase_num The phase for which the min green needs to be requested
        ** @return minimum green time in milliseconds
        * **/
        int get_min_green(int phase_num) const;

        /** @brief Get maximum green time for a phase
        ** @param phase_num The phase for which the maximum green needs to be requested
        ** @return maximum green time in milliseconds
        * **/
        int get_max_green(int phase_num) const;

        /** @brief Get yellow duration time for a phase
        ** @param phase_num The phase for which the yellow duration needs to be requested
        ** @return yellow duration time in milliseconds
        * **/
        int get_yellow_duration(int phase_num) const;

        /** @brief Get red clearance time for a phase
        ** @param phase_num The phase for which the red clearance needs to be requested
        ** @return red clearance time in milliseconds
        * **/
        int get_red_clearance(int phase_num) const;

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
        std::vector<int> get_concurrent_phases(int phase_num) const;

        public:

        /** 
         * @brief Constructor for the tsc_state class 
         * @param snmp_client A pointer to an snmp_client worker with a connection established to a traffic signal controller
        **/
        explicit tsc_state(std::shared_ptr<snmp_client> snmp_client);
        
        std::unordered_map<int, signal_group_state>& get_signal_group_state_map()
        {
            return signal_group_state_map_;
        }        
    };
}