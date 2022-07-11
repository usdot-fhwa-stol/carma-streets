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

}