#pragma once

#include "snmp_client.h"
#include "snmp_client_exception.h"
#include "ntcip_oids.h"
#include "spat.h"
#include <gtest/gtest_prod.h>
#include "monitor_states_exception.h"

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
        std::vector<int> concurrent_phases;
    };

    class tsc_state
    {
        private:
            /* A local pointer to an snmp_client object to be used through the tsc_state*/
            std::shared_ptr<snmp_client> snmp_client_worker_;

            /* Mapping between signal group ids(key) and their states(value) defined as a signal group state struct for all active vehicle phases*/
            std::unordered_map<int, signal_group_state> signal_group_state_map_;
            
            /* Map between signal group ids(key) and phase numbers(value) for all active vehicle phases in the Traffic Signal Controller*/
            std::unordered_map<int, int> signal_group_phase_map_;

            /* Map between phase numbers(key) and signal group ids(value) for all active vehicle phases in the Traffic Signal Controller*/
            std::unordered_map<int,int> vehicle_phase_num_map_;

            /* Map between phase numbers(key) and signal group ids(value) for all active pedestrian phases in the Traffic Signal Controller*/
            std::unordered_map<int,int> ped_phase_num_map_;

            /* The sequence of vehicle phases in ring 1 of TSC*/
            std::vector<int> phase_seq_ring1_;
            /* The sequence of vehicle phases in ring 2 of TSC*/
            std::vector<int> phase_seq_ring2_;

            // Number of required following movements on receiving a spat_ptr
            int required_following_movements_ = 3;

            // Conversion constants
            int HOUR_TO_SECONDS_ = 3600;
            int SECOND_TO_MILLISECONDS_ = 1000;

            /** @brief Creates vectors of channels associated with a vehicle phase and a pedestrian. Ignores overlap, ped Overlap, queueJump and other (types defined in NTCIP1202 v03)
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

            /** @brief Helper function to convert epoch time to hour-tenths time
            ** @param epoch_time_ms epoch time in milliseconds
            ** @return tenths of a second in current or next hour
            * **/
            uint16_t convert_msepoch_to_hour_tenth_secs(uint64_t epoch_time_ms) const;

            /** @brief Helper function to convert hour-tenths time to epoch time
            ** @param hour_tenth_secs tenths of a second in current or next hour
            ** @return epoch time in milliseconds
            * **/
            uint64_t convert_hour_tenth_secs2epoch_ts(uint16_t hour_tenth_secs) const;

            /** @brief Get predicted next movement event given a current event
            ** @param current_event movement_event from the next movement needs to be predicted
            ** @param current_event_end_time End time of the current event in epoch time (milliseconds)
            ** @param phase_state signal_group_state for the phase of which the movement events are a part
            ** @return predicted next movement event
            * **/
            signal_phase_and_timing::movement_event get_following_event(const signal_phase_and_timing::movement_event& current_event,
                                                                 uint64_t current_event_end_time, const signal_group_state& phase_state) const;

            //Add Friend Test to share private members
            FRIEND_TEST(traffic_signal_controller_service, test_get_following_movement_events);                                                              

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
             */
            std::unordered_map<int, signal_group_state>& get_signal_group_state_map();
            /** 
             * @brief Returns a map of pedestrian phases to signal group ids
             * @return a map of pedestrian phases to signal group ids 
            **/
            const std::unordered_map<int,int> & get_ped_phase_map();

            /** 
             * @brief Returns a map of pedestrian phases to signal group ids
             * @return a map of pedestrian phases to signal group ids
            **/
            const std::unordered_map<int,int>& get_vehicle_phase_map();

            /**
             * @brief Initialize tsc_state by making SNMP calls to TSC for phase sequence and timing information.
             * 
             * @return true if initialization is successful.
             */
            bool initialize();

            /** @brief Updates spat with future movement events for vehicle phases
            ** @param spat_ptr pointer to spat message to update
            ** @return true if update was successful, false if it failed
            * **/
            void add_future_movement_events(std::shared_ptr<signal_phase_and_timing::spat> spat_ptr);

    };
}