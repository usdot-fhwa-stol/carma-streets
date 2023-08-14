#pragma once

#include "snmp_client.h"
#include "snmp_client_exception.h"
#include "ntcip_oids.h"
#include "spat.h"
#include <gtest/gtest_prod.h>
#include "monitor_states_exception.h"
#include "tsc_configuration_state.h"

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
        /*Minimum green durations for each active vehicle phase in milliseconds*/
        int min_green;
        /*Maximum green durations for each active vehicle phase in milliseconds*/
        int max_green;
        /*Default Green duration for phase in milliseconds*/
        int green_duration;
        /*Yellow signal duration for phase in milliseconds*/
        int yellow_duration;
        /* Red clearace time for phase in milliseconds*/
        int red_clearance;
        /*Red signal duration for phase. This is the total time a phase is predicted to be red before its next green. In milliseconds*/
        int red_duration;
        /*Phase sequence in ring. Stores the sequence starting from the current phase*/
        std::vector<int> phase_seq;
        /*Phases in the same barrier or concurrent group excluding phases from same ring*/
        std::vector<int> concurrent_signal_groups;
    };

    class tsc_state
    {
        private:
            /* A local pointer to an snmp_client object to be used through the tsc_state*/
            std::shared_ptr<snmp_client> snmp_client_worker_;

            /* Mapping between signal group ids(key) and their states(value) defined as a signal group state struct for all active vehicle phases*/
            std::unordered_map<int, signal_group_state> signal_group_2tsc_state_map_;
            
            /* Map between signal group ids(key) and phase numbers(value) for all active vehicle phases in the Traffic Signal Controller*/
            std::unordered_map<int, int> signal_group_2vehiclephase_map_;

            /* Map between phase numbers(key) and signal group ids(value) for all active vehicle phases in the Traffic Signal Controller*/
            std::unordered_map<int,int> vehicle_phase_2signalgroup_map_;

            /* Map between phase numbers(key) and signal group ids(value) for all active pedestrian phases in the Traffic Signal Controller*/
            std::unordered_map<int,int> ped_phase_2signalgroup_map_;

            /* The sequence of vehicle phases in ring 1 of TSC*/
            std::vector<int> phase_seq_ring1_;
            /* The sequence of vehicle phases in ring 2 of TSC*/
            std::vector<int> phase_seq_ring2_;

            std::vector<int> vehicle_calls;

            std::vector<int> pedestrian_calls;

            // Number of required following movements on receiving a spat_ptr
            int required_following_movements_ = 3;

            // Conversion constants
            int HOUR_TO_SECONDS_ = 3600;
            int SECOND_TO_MILLISECONDS_ = 1000;

            /**
             * @brief Pointer to tsc_configuration_state object which is traffic signal controller
             * configuration information obtained from the tsc_state worker
             * and broadcast on the carma-streets kafka broker as a JSON message.
             */
            std::shared_ptr<streets_tsc_configuration::tsc_configuration_state> tsc_config_state_ptr_;

            /**
             * @brief Creates tsc_configuration_state object pointer which is used to forward configuration information required
             * to understand ring and barrier information on the traffic signal controller.
             * This includes signal_group_id, yellow_change duration, red_clearance and concurrent signal groups
             * @return shared ptr to tsc_config state
             */
            std::shared_ptr<streets_tsc_configuration::tsc_configuration_state> define_tsc_config_state();

            /** @brief Creates vectors of channels (or signal group id) associated with a vehicle phase and a pedestrian. Ignores overlap, ped Overlap, queueJump and other (types defined in NTCIP1202 v03)
             *  Expects empty input arguments which are filled in by reference.
             *  @param max_channels The maximum number of channels in the traffic signal controller.
             *  @param vehicle_channels a vector of active vehicle phases associated with a channel
             *  @param ped_channels a vector of active pedestrian phases associated with a channel
            **/
            void get_channels(int max_channels, std::vector<int>& vehicle_channels, std::vector<int>& ped_channels) const;

            /** @brief Creates a map of phases associated with a vehicle phase and a pedestrian phase.
             *  @param signal_group_ids a map of known signal_group_ids/channels for vehicle or pedestrian to the corresponding phases
             *  @return a mapping from phases to signal_group ids. Will return empty map if 
             *  no phases associated with a vehicle or pedestrian are found.
            **/
            std::unordered_map<int,int> get_phases_associated_with_channel(const std::vector<int>& signal_group_ids) const;

            /** 
             * @brief Method for getting populated rings in sequence (default sequence = 1).
             * Sequence table contains multiple sequence plans, consisting of the phase sequence in each ring available in the traffic signal controller.
             * We assume the active sequence plan is sequence number 1 by default.
             * @param max_rings Maximum number of rings in the traffic signal controller
             * @param vehicle_phase_2signalgroup_map a map for all vehicle phases and signal groups associated with them
             * @param sequence The sequence number to check for active rings, defaults to 1
             * @return List of active rings in the traffic signal controller
            **/
            std::vector<std::vector<int>> get_active_ring_sequences(int max_rings, std::unordered_map<int,int>& vehicle_phase_2signalgroup_map, int sequence = 1) const;


            /** 
             * @brief Method for mapping vehicle phases and signal groups. Modifies non-const arguments by reference.
             * Signal group map is expected to be passed empty. 
             * Vehicle phase map is expected to contain known vehicle phases. These are modified to only contain phases in the active ring sequence.
             * @param active_ring_sequences A list of phases from the traffic signal controller in rows corresponding to the ring number
             * @param vehicle_phase_2signalgroup_map a map for all vehicle phases and signal groups associated with them
             * @param signal_group_2vehiclephase_map Mapping from signal groups id to vehicle phases
             * 
            **/
            void map_phase_and_signalgroup(const std::vector<std::vector<int>>& active_ring_sequences, std::unordered_map<int,int>& vehicle_phase_2signalgroup_map, 
                                                                                            std::unordered_map<int, int>& signal_group_2vehiclephase_map) const;

            std::vector<int> process_bitwise_response( const streets_snmp_cmd::snmp_response_obj &resp, int offset );
            /** 
             * @brief Method for getting maximum channels for the traffic signal controller
             * @return number of maximum channels in the traffic signal controller
            **/
            int get_max_channels() const;

            /** 
             * @brief Method for getting maximum rings for the traffic signal controller
             * @return number of maximum rings in the traffic signal controller
            **/
            int get_max_rings() const;

            /** @brief Get minimum green time for a phase
             * @param phase_num The phase for which the min green needs to be requested
             * @return minimum green time in milliseconds
            **/
            int get_min_green(int phase_num) const;

            /** @brief Get maximum green time for a phase
             * @param phase_num The phase for which the maximum green needs to be requested
             * @return maximum green time in milliseconds
            **/
            int get_max_green(int phase_num) const;

            /** @brief Get yellow duration time for a phase
             * @param phase_num The phase for which the yellow duration needs to be requested
             * @return yellow duration time in milliseconds
            **/
            int get_yellow_duration(int phase_num) const;

            /** @brief Get red clearance time for a phase
             * @param phase_num The phase for which the red clearance needs to be requested
             * @return red clearance time in milliseconds
            **/
            int get_red_clearance(int phase_num) const;

            /** @brief Get red duration for a phase
             * @param phase_num The phase for which the red duration needs to be requested
             * @return red duration time in milliseconds
            **/
            int get_red_duration(int phase_num);

            /** @brief Get a sequence of phases following the given phase
             * @param phase_num The phase for which the sequence needs to be obtained
             * @param active_ring_sequences The sequence of phases in active rings in the traffic signal controller
             * @return a vector as a sequence of phases starting from the given phase
            **/
            std::vector<int> get_following_phases(int phase_num, const std::vector<std::vector<int>>& active_ring_sequences) const;

            /** @brief The concurrent phases that the given phase can be green along with
             * @param ring_num The phase for which the concurrent phases needs to be obtained
             * @return a vector of phases that may be concurrent with the given phase
            **/
            std::vector<int> get_concurrent_signal_groups(int phase_num);

            /** @brief Get predicted next movement event given a current event
             * @param current_event movement_event from the next movement needs to be predicted
             * @param current_event_end_time End time of the current event in epoch time (milliseconds)
             * @param phase_state signal_group_state for the phase of which the movement events are a part
             * @return predicted next movement event
            **/
            signal_phase_and_timing::movement_event get_following_event(const signal_phase_and_timing::movement_event& current_event,
                                                                 uint64_t current_event_end_time, const signal_group_state& phase_state) const;

            std::vector<int> convert_veh_phases_to_signal_groups(const std::vector<int> &veh_phases ) const;

            std::vector<int> convert_ped_phases_to_signal_groups(const std::vector<int> &ped_phases ) const;
            
            //Add Friend Test to share private members
            FRIEND_TEST(test_monitor_state, test_get_following_movement_events);
                                                              

        public:
            /** 
             * @brief Constructor for the tsc_state class 
             * @param snmp_client A pointer to an snmp_client worker with a connection established to a traffic signal controller
            **/
            explicit tsc_state(std::shared_ptr<snmp_client> snmp_client);
            /**
             * @brief Return reference to signal group state map 
             * 
             * @return std::unordered_map<int, signal_group_state>& 
            **/
            std::unordered_map<int, signal_group_state>& get_signal_group_state_map();
            /**
             * @brief Returns a map of signal group ids to pedestrian phases
             * 
             * @return a map of signal group ids to pedestrian phases
            **/
            std::unordered_map<int, int> get_signal_group_to_ped_phase_map();
            /** 
             * @brief Returns a map of pedestrian phases to signal group ids
             * @return a map of pedestrian phases to signal group ids 
            **/
            const std::unordered_map<int,int> & get_ped_phase_map();

            /** 
             * @brief Returns a map of vehicle phases to signal group ids
             * @return a map of vehicle phases to signal group ids
            **/
            const std::unordered_map<int,int>& get_vehicle_phase_map();

            /**
             * @brief Return a map of signal group ids to phases map.
             * @return a map of signal group ids to vehicle phases map.
            */
            const std::unordered_map<int, int> & get_signal_group_map();

            std::vector<int> get_vehicle_calls() const;

            std::vector<int> get_pedestrian_calls() const;
            /**
             * @brief Poll vehicle/pedestrian calls on phases 1-16
             */
            void poll_vehicle_pedestrian_calls();

            /**
             * @brief Get the phase number using signal group id.
             * 
             * @param signal_group_id 
             * @return int
             * @throws monitor_states_exception if signal group id is less than 1.
             */
            int get_phase_number(const int signal_group_id);

            /**
             * @brief Get the signal group id using phase number.Only for vehicle phases
             * 
             * @param phase_number 
             * @return int
             * @throws monitor_states_exception if phase number is less than 1. 
             */
            int get_vehicle_signal_group_id(const int phase_number);

            int get_pedestrian_signal_group_id(const int phase_number) const

            /**
             * @brief Initialize tsc_state by making SNMP calls to TSC for phase sequence and timing information.
             * 
             * @return true if initialization is successful.
             */
            bool initialize();

            /** @brief Updates spat with future movement events for vehicle phases
             * @param spat_ptr pointer to spat message to update
             * @return true if update was successful, false if it failed
            **/
            void add_future_movement_events(std::shared_ptr<signal_phase_and_timing::spat> spat_ptr);

            /** 
             * @brief Returns a pointer to the tsc_config_state 
             * @return pointer to the tsc_configuration_state used to forward configuration information required
             * to understand ring and barrier information on the traffic signal controller.
             * This includes signal_group_id, yellow_change duration, red_clearance and concurrent signal groups
            **/
            const std::shared_ptr<streets_tsc_configuration::tsc_configuration_state>& get_tsc_config_state() const;

    };
}