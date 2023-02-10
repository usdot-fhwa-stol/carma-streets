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
     * @brief OID for MAX_RINGS: The maximum number of rings this TSC supports. 
    */
    static const std::string MAX_RINGS = "1.3.6.1.4.1.1206.4.2.1.7.1.0";

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

    /**
     * @brief  Following the yellow change interval for each phase, the CU shall provide a red clearance interval which is 
     * timed according to the red clearance parameter for that phase.
     * Added Note:A clearance interval which may follow the yellow change interval during which both the terminating phase and 
     * the next phase display Red signal indicators
     * Used with (.phase number)
     */
    static const std::string RED_CLEAR_PARAMETER = "1.3.6.1.4.1.1206.4.2.1.1.2.1.9";

    /**
     * @brief Each octet contains a phase number that may run concurrently with the associated phase. Phases that are contained in the same ring may
     * not run concurrently.
     * Used with (.phase number)
     */
    static const std::string PHASE_CONCURRENCY =  "1.3.6.1.4.1.1206.4.2.1.1.2.1.23";

    /**
     * @brief Phase Omit Control object is used to allow omission of pedestrian phases from being serviced in the device.
     * It takes in a 8 bit argument, one for each phase. When a bit=1 the Traffic Signal Controller will omit 
     * that phase and keep omitting till the bit is set to 0.
     * Values range from 0-255.
     * .1 is needed at the end to set the phase control. NTCIP documentation is unclear about why its required or what it represents
     */
    static const std::string PHASE_OMIT_CONTROL = "1.3.6.1.4.1.1206.4.2.1.1.5.1.2.1";

    /**
     * @brief Phase Hold Control object is used to Hold pedestrian phases.
     * It takes in a 8 bit argument, one for each phase. When a bit=1 the Traffic Signal Controller will hold 
     * that phase when it turns green and keep holding till the bit is set to 0.
     * Values range from 0-255.
     * .1 is needed at the end to set the phase control. NTCIP documentation is unclear about why its required or what it represents
     */
    static const std::string PHASE_HOLD_CONTROL = "1.3.6.1.4.1.1206.4.2.1.1.5.1.4.1";
    /**
     * @brief Phase Status Group Phase Next object is used to determine whether any vehicle phases are committed by the
     * TSC to be served next. It returns a 8 bit value that represent a state for each of the 8 vehicle phases. When a bit = 1,
     * the corresponding phase is currently commited to be next, when a bit = 0, this phase is NOT commited to be next. 
     * The phase commited to be next is determined at the end of each green interval, except if that decision cannot be made.
     * In this case it will wait until all vehicle change and clearance intervales. NOTE: Uses first entry in the phaseStatusGroupTable 
     * which is assumed to be the vehicle phase group.
     * 
     */
    static const std::string PHASE_STATUS_GROUP_PHASE_NEXT = "1.3.6.1.4.1.1206.4.2.1.1.4.1.11.1";

}