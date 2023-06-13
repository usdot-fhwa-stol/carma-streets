#pragma once
#include <string>
#include <vector>
#include <memory>

namespace streets_snmp_cmd
{
    enum class REQUEST_TYPE
    {
        GET,
        SET,
        OTHER // Processing this request type is not a defined behavior, included for testing only
    };

    /** @brief The type of control being set on the TSC */
    enum class PHASE_CONTROL_TYPE
    {
        CALL_VEH_PHASES = 1, // Call a vehicle phase
        CALL_PED_PHASES = 2, // Call a pedestrian phase
        FORCEOFF_PHASES = 3, // Forceoff a vehicle phase
        HOLD_VEH_PHASES = 4, // Hold a vehicle phase
        OMIT_VEH_PHASES = 5, // Omit a vehicle phase
        OMIT_PED_PHASES = 6, // Omit a pedestrain phase
    };

    /** @brief The type of value being requested or set, on the TSC */
    enum class RESPONSE_TYPE
    {
        INTEGER,
        STRING
    };

    /** @brief A struct to hold the value being sent to the TSC, can be integer or string. Type needs to be defined*/
    struct snmp_response_obj
    {
        // snmp response values can be any asn.1 supported types.
        // Integer and string values can be processed here
        int64_t val_int = 0;
        std::vector<char> val_string;
        RESPONSE_TYPE type;

        inline bool operator==(const snmp_response_obj &obj2) const
        {
            return val_int == obj2.val_int && val_string == obj2.val_string && type == obj2.type;
        }
    };

    /** @brief Object to store snmp control commands. Contructed with an initialized snmp_client_worker_ this object stores SNMP HOLD and OMIT commands
     to be executed at specified time */
    struct snmp_cmd_struct
    {

        /*Value to be set for Hold/Omit*/
        snmp_response_obj set_val_;
        /*Time at which the snmp set command should be executed*/
        uint64_t start_time_;
        /*Type of the snmp set command this object creates- Hold or Omit*/
        PHASE_CONTROL_TYPE control_type_;

        snmp_cmd_struct(uint64_t start_time, PHASE_CONTROL_TYPE type, int64_t val)
            : start_time_(start_time), control_type_(type)
        {
            set_val_.type = RESPONSE_TYPE::INTEGER;
            set_val_.val_int = val;
        }

        /**
         * @brief Method to return information about the snmp set command
         * @return Returns string formatted as "control_cmd_type:<HOLD/OMIT>;execution_start_time:<calculated_start_time>;signal_groups_set:<signal groups being set separated by commas>".
         * */
        std::string get_cmd_info() const;
    };

} // namespace streets_snmp_cmd
