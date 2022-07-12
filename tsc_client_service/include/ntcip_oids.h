#pragma once

#include <spdlog/spdlog.h>

/**
 * @brief Namespace for OIDs used for NTCIP calls.
 */
namespace ntcip_oids {
    /**
     * @brief OID for ENABLE_SPAT
     */
    static const std::string ENABLE_SPAT_OID = "1.3.6.1.4.1.1206.3.5.2.9.44.1.0";
    
    /**
     * @brief OID for MAX_CHANNELS: The maximum number of channels this TSC supports. 
    */
    static const std::string MAX_CHANNELS = "1.3.6.1.4.1.1206.4.2.1.8.1.0";

    /**
     * @brief OID for Channel control type: Used with (.channel number)
        This object defines the channel control type (Vehicle Phase,Pedestrian Phase or Overlap)
        SYNTAX INTEGER { other (1), phaseVehicle (2), phasePedestrian (3), overlap (4), pedOverlap (5), queueJump (6) } 
    */
    static const std::string CHANNEL_CONTROL_TYPE_PARAMETER = "1.3.6.1.4.1.1206.4.2.1.8.2.1.3";

    /**
     * @brief OID for Channel control source: Used with (.channel number)
        Value 00 = No Control (Not In Use)
        Value 01 = Phase 01 or Overlap A
        Value 02 = Phase 02 or Overlap B
        ||
        Value 15 = Phase 15 or Overlap O
        Value 16 = Phase 16 or Overlap P
        etc.
    */
    static const std::string CHANNEL_CONTROL_SOURCE_PARAMETER = "1.3.6.1.4.1.1206.4.2.1.8.2.1.2";

    /** @brief  Phase Minimum Green Parameter: The minimum green parameter in seconds.Used with (.phase number)*/
    static const std::string MINIMUM_GREEN = "1.3.6.1.4.1.1206.4.2.1.1.2.1.4";

    /** @brief This time shall determine the maximum length of time this phase may be held in Green in the absence of a serviceable conflicting call. In the absense of a serviceable conflicting call the 
     * maximum green timer shall be held reset unless Max Vehicle recall is enabled for this phase. This is the default maximum value to use. 
     * Used with (.phase number)**/
    static const std::string MAXIMUM_GREEN = "1.3.6.1.4.1.1206.4.2.1.1.2.1.6";

    /** @brief Phase Yellow Change Parameter: Following the green interval of each phase, 
     * the CU shall provide a yellow change interval which is timed according to the yellow change parameter for that phase. 
     * Used with (.phase number)**/
    static const std::string YELLOW_CHANGE_PARAMETER = "1.3.6.1.4.1.1206.4.2.1.1.2.1.8";

    /**
     * @brief Each octet is a phase number within the associated ring number. 
     * The phase number value shall not exceed the max phases object value. The order of phase numbers determines the phase sequence for that ring.
     * Used with (.sequence number)(.ring number)
     */
    static const std::string SEQUENCE_DATA = "1.3.6.1.4.1.1206.4.2.1.7.3.1.3";
}