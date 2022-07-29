#include "ntcip_1202_ext.h"

namespace ntcip {

    std::string ntcip_1202_ext::to_string() const{
        std::string message;
        message.append( "header:" + std::to_string(header) + "\n");
        message.append( "num_of_phases: " + std::to_string(num_of_phases) + "\n");
        for (const auto &phase_time : phase_times ) {
            message.append( phase_time.to_string());
            message.append("phase_status_red: " + std::to_string(get_phase_red_status(phase_time.phase_number)) + "\n");
            message.append("phase_status_yellow: " + std::to_string(get_phase_yellow_status(phase_time.phase_number)) + "\n" );
            message.append("phase_status_green: " + std::to_string(get_phase_green_status(phase_time.phase_number)) + "\n");
            message.append("phase_status_dont_walk: " + std::to_string(get_phase_dont_walk(phase_time.phase_number)) + "\n");
            message.append("phase_status_ped_clear: " + std::to_string(get_phase_ped_clear(phase_time.phase_number)) + "\n");
            message.append("phase_status_walk: " + std::to_string(get_phase_walk(phase_time.phase_number)) + "\n");
            message.append("overlap_status_red: " + std::to_string(get_overlap_red_status(phase_time.phase_number)) + "\n");
            message.append("overlap_status_yellow: " + std::to_string(get_overlap_yellow_status(phase_time.phase_number)) + "\n");
            message.append("overlap_status_green: " + std::to_string(get_overlap_green_status(phase_time.phase_number)) + "\n");
            message.append("flashing_phase_status: " + std::to_string(get_phase_flashing_status(phase_time.phase_number)) + "\n");
            message.append("flashing_overlap_status: " + std::to_string(get_overlap_flashing_status(phase_time.phase_number)) + "\n");
            message.append("spat_pedestrian_call: " + std::to_string(get_spat_pedestrian_call(phase_time.phase_number)) + "\n");
            message.append("spat_pedestrian_detect: " + std::to_string(get_spat_pedestrian_detect(phase_time.phase_number)) + "\n");
        }

        
        message.append("spat_intersection_status: " + std::to_string(spat_intersection_status) + "\n" );
        message.append("timebase_asc_action_status: " + std::to_string(timebase_asc_action_status) + "\n");
        message.append("spat_discontinuous_change_flag: " + std::to_string(spat_discontinuous_change_flag) + "\n");
        message.append("spat_message_seq_counter: " + std::to_string(spat_message_seq_counter) + "\n" );
        message.append("spat_timestamp_second_of_day: " + std::to_string(get_timestamp_seconds_of_day()) + "\n");
        message.append("spat_timestamp_msec: " +  std::to_string(ntohs(spat_timestamp_msec)) + "\n");
        

        return message;
    }

    bool ntcip_1202_ext::get_phase_red_status(const int phase_number) const {
        return (bool)((ntohs(phase_status_group_reds) >> (phase_number - 1)) & 0x01);
    }

    bool ntcip_1202_ext::get_phase_yellow_status(const int phase_number) const {
        return (bool)((ntohs(phase_status_group_yellows) >> (phase_number - 1))& 0x01);
    }

    bool ntcip_1202_ext::get_phase_green_status(const int phase_number) const {
        return (bool)((ntohs(phase_status_group_greens) >> (phase_number - 1)) & 0x01);
    }

    bool ntcip_1202_ext::get_phase_dont_walk(const int phase_number) const {
        return (bool)((ntohs(phase_status_group_dont_walks) >> (phase_number - 1)) & 0x01);
    }

    bool ntcip_1202_ext::get_phase_ped_clear(const int phase_number) const {
        return (bool)((ntohs(phase_status_group_ped_clears) >> (phase_number - 1)) & 0x01);
    }

    bool ntcip_1202_ext::get_phase_walk(const int phase_number) const {
        return (bool)((ntohs(phase_status_group_walks) >> (phase_number - 1)) & 0x01);
    }

    bool ntcip_1202_ext::get_overlap_red_status(const int phase_number) const {
        return (bool)((ntohs(overlap_status_group_reds) >> (phase_number - 1)) & 0x01);
    }

    bool ntcip_1202_ext::get_overlap_yellow_status(const int phase_number) const {
        return (bool)((ntohs(overlap_status_group_yellows) >> (phase_number - 1)) & 0x01);
    }

    bool ntcip_1202_ext::get_overlap_green_status(const int phase_number) const {
        return (bool)((ntohs(overlap_status_group_greens) >> (phase_number - 1)) & 0x01);
    }

    bool ntcip_1202_ext::get_phase_flashing_status(const int phase_number) const {
        return (bool)((ntohs(flashing_output_phase_status) >> (phase_number -1)) &0x01);
    }

    bool ntcip_1202_ext::get_overlap_flashing_status(const int phase_number) const {
        return (bool)((ntohs(flashing_output_overlap_status) >> (phase_number -1)) &0x01);
    }

    bool ntcip_1202_ext::get_spat_pedestrian_call(const int phase_number) const {
        return (bool)((ntohs(spat_pedestrian_call) >> (phase_number -1)) &0x01);
    }

    bool ntcip_1202_ext::get_spat_pedestrian_detect(const int phase_number) const {
        return (bool)((ntohs(spat_pedestrian_detect) >> (phase_number -1)) &0x01);
    }

    uint32_t ntcip_1202_ext::get_timestamp_seconds_of_day() const {
        return (uint32_t)(spat_timestamp_second_byte1 << 16) | (spat_timestamp_second_byte2 << 8 ) | spat_timestamp_second_byte3;
    }

    ntcip_1202_ext_phasetime& ntcip_1202_ext::get_phasetime(const int phase_number) {
        for ( auto i = 0 ; i < 16; i++) {
            if (phase_times[i].phase_number ==  phase_number) {
                return phase_times[i];
            }

        }
        throw signal_phase_and_timing::signal_phase_and_timing_exception("Could not find phasetime for phase number " +  std::to_string(phase_number) +  "!");
    }
}