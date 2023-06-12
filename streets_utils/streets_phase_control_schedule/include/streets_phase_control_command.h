#pragma once
#include <stdint.h>
#include <string>
#include <boost/algorithm/string.hpp>
#include <iostream>
#include <iomanip>

namespace streets_phase_control_schedule
{
    // Define Command Actions (Used in phase controls)
    enum class COMMAND_TYPE
    {
        CALL_VEH_PHASES = 1, // Call a vehicle phase
        CALL_PED_PHASES = 2, // Call a pedestrian phase
        FORCEOFF_PHASES = 3, // Forceoff a vehicle phase
        HOLD_VEH_PHASES = 4, // Hold a vehicle phase
        OMIT_VEH_PHASES = 5, // Omit a vehicle phase
        OMIT_PED_PHASES = 6, // Omit a pedestrain phase
    };

    /**
     * @brief This struct provides command information received from MMITSS. (https://github.com/mmitss/mmitss-az/tree/master/src/mrp/traffic-controller-interface).
     * The command will be translate into carma-streets snmp_command structure and sent to the Traffic signal controller.
     */
    struct streets_phase_control_command
    {
        COMMAND_TYPE command_type; // Action
        int command_phase = 0;         // Affected phase (bitstring to integer)
        uint64_t command_start_time = 0;  // Start time in epoch unix timestamp in millisecond
        uint64_t command_end_time = 0;    // End time  in epoch unix timestamp in millisecond
        streets_phase_control_command() = default;
        /***
         * @brief Constructor with arguments to initialize the command object
         */
        streets_phase_control_command(const std::string &command_type_str, int command_phase, uint64_t command_start_time, uint64_t command_end_time);
        /**
         * @brief Set the command type variable
         * @param string command type in string format
         */
        void set_command_type(const std::string &command_type_str);
        ~streets_phase_control_command() = default;

        // Overload operator<< to print command
        friend std::ostream &operator<<(std::ostream &os, const streets_phase_control_command& command);
    };
}
