#pragma once

#include <netinet/in.h>
#include <string>

namespace ntcip {

    struct ntcip_1202_ext_phasetime
    {
        /** @brief Phase number (NTCIP) of phase. */
        uint8_t phase_number = 0;
        /** @brief Time offset in deciseconds to min end time elapse for vehicle phase. 
         * DO NOT use member directly, stored in Network Byte Order. Use getter to intpret
         * correctly.
         */
        uint16_t spat_veh_min_time_to_change = 0;
        /** @brief Time offset in deciseconds to max end time elapse for vehicle phase. 
         * DO NOT use member directly, stored in Network Byte Order. Use getter to intpret
         * correctly.
         */
        uint16_t spat_veh_max_time_to_change = 0;
        /** @brief Time offset in deciseconds to min end time elapse for pedestrian phase. 
         * DO NOT use member directly, stored in Network Byte Order. Use getter to intpret
         * correctly.
         */
        uint16_t spat_ped_min_time_to_change = 0;
        /** @brief Time offset in deciseconds to max end time elapse for pedestrian phase. 
         * DO NOT use member directly, stored in Network Byte Order. Use getter to intpret
         * correctly.
         */
        uint16_t spat_ped_max_time_to_change = 0;
        /** @brief Time offset in deciseconds to min end time elapse for overlap. 
         * DO NOT use member directly, stored in Network Byte Order. Use getter to intpret
         * correctly.
         */
        uint16_t spat_ovlp_min_time_to_change = 0;
        /** @brief Time offset in deciseconds to max end time elapse for overlap. 
         * DO NOT use member directly, stored in Network Byte Order. Use getter to intpret
         * correctly.
         */
        uint16_t spat_ovp_max_time_to_change = 0;
        /**
         * @brief Get spat_veh_min_time_to_change and convert from 
         * network byte order to host byte order.
         * 
         * @return uint16_t 
         */
        uint16_t get_spat_veh_min_time_to_change() const;
        /**
         * @brief Get spat_veh_max_time_to_change and convert from 
         * network byte order to host byte order. 
         * 
         * @return uint16_t 
         */
        uint16_t get_spat_veh_max_time_to_change() const;
        /**
         * @brief Get spat_ped_min_time_to_change and convert from
         * network byte order to host byte order.
         * 
         * @return uint16_t 
         */
        uint16_t get_spat_ped_min_time_to_change() const;
        /**
         * @brief Get spat_ped_max_time_to_change and convert from
         * network byte order to host byte order.
         * 
         * @return uint16_t 
         */
        uint16_t get_spat_ped_max_time_to_change() const;
        /**
         * @brief Get spatOvlpMinTimeToChange and convert from 
         * network byte order to host byte order.
         * 
         * @return uint16_t 
         */
        uint16_t get_spat_ovlp_min_time_to_change() const;
        /**
         * @brief Get spat_ovp_max_time_to_change and convert from
         * netwrok byte order to host byte order.
         * 
         * @return uint16_t 
         */
        uint16_t get_spat_ovp_max_time_to_change() const;
        /**
         * @brief Convert information to string.
         * 
         * @return std::string 
         */
        std::string to_string() const;
    }__attribute__((packed));
}