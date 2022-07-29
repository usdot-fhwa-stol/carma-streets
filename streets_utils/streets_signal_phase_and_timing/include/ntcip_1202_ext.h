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
        int8_t header = 0;
        int8_t num_of_phases = 0;

        ntcip_1202_ext_phasetime phase_times[16];

        uint16_t phase_status_group_reds = 0;
        uint16_t phase_status_group_yellows = 0;
        uint16_t phase_status_group_greens = 0;
        uint16_t phase_status_group_dont_walks = 0;
        uint16_t phase_status_group_ped_clears = 0;
        uint16_t phase_status_group_walks = 0;
        uint16_t overlap_status_group_reds = 0;
        uint16_t overlap_status_group_yellows = 0;
        uint16_t overlap_status_group_greens = 0;
        uint16_t flashing_output_phase_status = 0;
        uint16_t flashing_output_overlap_status = 0;
        uint8_t spat_intersection_status = 0;
        uint8_t timebase_asc_action_status = 0;
        uint8_t spat_discontinuous_change_flag = 0;
        uint8_t spat_message_seq_counter = 0;
        u_char spat_timestamp_second_byte1 = 0;
        u_char spat_timestamp_second_byte2 = 0;
        u_char spat_timestamp_second_byte3 = 0;
        uint16_t spat_timestamp_msec = 0;
        uint16_t spat_pedestrian_call = 0;
        uint16_t spat_pedestrian_detect = 0;

        bool get_phase_red_status(const int phase_number) const;
        bool get_phase_yellow_status(const int phase_number) const;
        bool get_phase_green_status(const int phase_number) const;
        bool get_phase_dont_walk(const int phase_number) const;
        bool get_phase_ped_clear(const int phase_number) const;
        bool get_phase_walk(const int phase_number) const;
        bool get_overlap_red_status(const int phase_number) const;
        bool get_overlap_yellow_status(const int phase_number) const;
        bool get_overlap_green_status(const int phase_number) const;
        bool get_phase_flashing_status(const int phase_number) const;
        bool get_overlap_flashing_status(const int phase_number) const;
        bool get_spat_pedestrian_call(const int phase_number) const;
        bool get_spat_pedestrian_detect(const int phase_number) const;
        uint32_t get_timestamp_seconds_of_day() const;
        ntcip_1202_ext_phasetime& get_phasetime(const int phase_number);

        std::string to_string() const; 
        
    }__attribute__((packed));
  
}