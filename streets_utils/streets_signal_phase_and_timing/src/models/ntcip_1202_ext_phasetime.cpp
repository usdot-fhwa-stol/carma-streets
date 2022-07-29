#include "ntcip_1202_ext_phasetime.h"

namespace ntcip {
    std::string ntcip_1202_ext_phasetime::to_string() const{
        std::string message;
        message.append("phase_number: " + std::to_string(phase_number) +  "\n");
        message.append("spat_veh_min_time_to_change: " + std::to_string(get_spat_veh_min_time_to_change()) + "\n");
        message.append("spat_veh_max_time_to_change: " + std::to_string(get_spat_veh_max_time_to_change()) + "\n" );
        message.append("spat_ped_min_time_to_change: " + std::to_string(get_spat_ped_min_time_to_change()) + "\n");
        message.append("spat_ped_max_time_to_change: " + std::to_string(get_spat_ped_max_time_to_change()) + "\n");
        message.append("spat_ovlp_min_time_to_change: " + std::to_string(get_spat_ovlp_min_time_to_change()) + "\n");
        message.append("spat_ovp_max_time_to_change: " + std::to_string(get_spat_ovp_max_time_to_change()) + "\n");
        return message;
    }

    uint16_t ntcip_1202_ext_phasetime::get_spat_veh_min_time_to_change() const{
        return ntohs(spat_veh_min_time_to_change);
    }

    uint16_t ntcip_1202_ext_phasetime::get_spat_veh_max_time_to_change() const {
        return ntohs(spat_veh_max_time_to_change);
    }

    uint16_t ntcip_1202_ext_phasetime::get_spat_ped_min_time_to_change() const {
        return ntohs(spat_ped_min_time_to_change);
    }

    uint16_t ntcip_1202_ext_phasetime::get_spat_ped_max_time_to_change() const {
        return ntohs(spat_ped_max_time_to_change);
    }

    uint16_t ntcip_1202_ext_phasetime::get_spat_ovlp_min_time_to_change() const {
        return ntohs(spat_ovlp_min_time_to_change);
    }

    uint16_t ntcip_1202_ext_phasetime::get_spat_ovp_max_time_to_change() const {
        return ntohs(spat_ovp_max_time_to_change);
    }
}