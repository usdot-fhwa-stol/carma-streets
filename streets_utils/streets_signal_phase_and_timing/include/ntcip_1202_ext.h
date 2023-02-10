#pragma once
#include <netinet/in.h>
#include <iostream>
#include <sstream>
#include <bitset>

#include "ntcip_1202_ext_phasetime.h"
#include "signal_phase_and_timing_exception.h"

namespace ntcip {

    struct ntcip_1202_ext
    {
        /** @brief header **/
        int8_t header = 0;
        /** @brief number of phases **/
        int8_t num_of_phases = 0;
        /** @brief phase_time object for each phase */
        ntcip_1202_ext_phasetime phase_times[16];
        /** @brief int where each bit indicates whether that phase is red (1-16) **/
        uint16_t phase_status_group_reds = 0;
        /** @brief int where each bit indicates whether that phase is yellow (1-16) **/
        uint16_t phase_status_group_yellows = 0;
        /** @brief int where each bit indicates whether that phase is green (1-16) **/
        uint16_t phase_status_group_greens = 0;
        /** @brief int where each bit indicates whether that phase is dont walk (Pedestrian Phases) (1-16) **/
        uint16_t phase_status_group_dont_walks = 0;
        /** @brief int where each bit indicates whether that phase is pedestrian clear (Pedestrian Phases) (1-16) **/
        uint16_t phase_status_group_ped_clears = 0;
        /** @brief int where each bit indicates whether that phase is walk (Pedestrian Phases) (1-16) **/
        uint16_t phase_status_group_walks = 0;
        /** @brief int where each bit indicates whether that overlap is red (1-16) **/
        uint16_t overlap_status_group_reds = 0;
        /** @brief int where each bit indicates whether that overlap is yellow (1-16) **/
        uint16_t overlap_status_group_yellows = 0;
        /** @brief int where each bit indicates whether that overlap is green (1-16) **/
        uint16_t overlap_status_group_greens = 0;
        /** @brief int where each bit indicates whether that phase is flashing (1-16) **/
        uint16_t flashing_output_phase_status = 0;
        /** @brief int where each bit indicates whether that overlap is flashing (1-16) **/
        uint16_t flashing_output_overlap_status = 0;
        /** @brief bit string for intersection status  **/
        uint8_t spat_intersection_status = 0;
        /** @brief current action plan **/
        uint8_t timebase_asc_action_status = 0;
        /** @brief upper 5 bits are msg version **/
        uint8_t spat_discontinuous_change_flag = 0;
        /** @brief lower byte of up-time in deciseconds **/
        uint8_t spat_message_seq_counter = 0;
        /** 3 bytes for second of the UTC day */
        u_char spat_timestamp_second_byte1 = 0;
        u_char spat_timestamp_second_byte2 = 0;
        u_char spat_timestamp_second_byte3 = 0;
        /** @brief milliseconds of current second */
        uint16_t spat_timestamp_msec = 0;
        /** @brief int where each bit indicates whether there is a pedestrian call on a phase (Pedestrian Phases) (1-16) **/
        uint16_t spat_pedestrian_call = 0;
        /** @brief int where each bit indicates whether there is a pedestrian detect on a phase (Pedestrian Phases) (1-16) **/
        uint16_t spat_pedestrian_detect = 0;

        /**
         * @brief Returns red state of phase.
         * 
         * @param phase_number of phase.
         * @return true if phase is red.
         * @return false if phase is not red.
         */
        bool get_phase_red_status(const int phase_number) const;
        /**
         * @brief Returns yellow state of phase.
         * 
         * @param phase_number of phase.
         * @return true if phase is yellow.
         * @return false if phase is not yellow.
         */
        bool get_phase_yellow_status(const int phase_number) const;
        /**
         * @brief Returns green state of phase.
         * 
         * @param phase_number of phase.
         * @return true if phase is green.
         * @return false if phase is not green.
         */
        bool get_phase_green_status(const int phase_number) const;
        /**
         * @brief Returns dont walk state of phase. Only applies to pedestrian phases.
         * 
         * @param phase_number of phase.
         * @return true if phase is dont walk.
         * @return false if phase is not dont walk.
         */
        bool get_phase_dont_walk(const int phase_number) const;
        /**
         * @brief Returns pedestrian clear state of phase. Only applies to pedestrian phases.
         * 
         * @param phase_number of phase.
         * @return true if phase is pedestrian clear.
         * @return false if phase is not pedestrian clear.
         */
        bool get_phase_ped_clear(const int phase_number) const;
        /**
         * @brief Returns walk state of phase. Only applies to pedestrian phases.
         * 
         * @param phase_number of phase/
         * @return true if phase is walk.
         * @return false if phase is not walk.
         */
        bool get_phase_walk(const int phase_number) const;
        /**
         * @brief Returns red overlap state of phase.
         * 
         * @param phase_number 
         * @return true 
         * @return false 
         */
        bool get_overlap_red_status(const int phase_number) const;
        /**
         * @brief Returns yellow overlap state of phase.
         * 
         * @param phase_number 
         * @return true 
         * @return false 
         */
        bool get_overlap_yellow_status(const int phase_number) const;
        /**
         * @brief Returns green overlap state of phase.
         * 
         * @param phase_number 
         * @return true 
         * @return false 
         */
        bool get_overlap_green_status(const int phase_number) const;
        /**
         * @brief Returns flashing state of phase.
         * 
         * @param phase_number of phase.
         * @return true if phase is flashing.
         * @return false if phase is not flashing.
         */
        bool get_phase_flashing_status(const int phase_number) const;
        /**
         * @brief Returns flashing state of overlap phase.
         * 
         * @param phase_number 
         * @return true 
         * @return false 
         */
        bool get_overlap_flashing_status(const int phase_number) const;
        /**
         * @brief Returns state of pedestrian calls on phase.
         * 
         * @param phase_number 
         * @return true 
         * @return false 
         */
        bool get_spat_pedestrian_call(const int phase_number) const;
        /**
         * @brief Returns state of pedestrian detect on phase.
         * 
         * @param phase_number 
         * @return true 
         * @return false 
         */
        bool get_spat_pedestrian_detect(const int phase_number) const;
        /**
         * @brief Returns 3 bytes translated into seconds of the UTC day
         * message timestamp
         * 
         * @return uint32_t message timestamp in seconds of UTC day
         */
        uint32_t get_timestamp_seconds_of_day() const;
        /**
         * @brief Returns reference to ntcip_1202_ext_phasetime of phase.
         * 
         * @param phase_number of phase
         * @return ntcip_1202_ext_phasetime& 
         */
        ntcip_1202_ext_phasetime& get_phasetime(const int phase_number);
        /**
         * @brief Returns std::string representation of ntcip_1202_ext message including phase_times array.
         * 
         * @return std::string 
         */
        std::string to_string() const; 
        
    }__attribute__((packed));
  
}