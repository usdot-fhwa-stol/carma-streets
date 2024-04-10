#pragma once

#include <stdlib.h>

namespace traffic_signal_controller_service{
    /**
     * @brief Enumeration to describe how we project future events on to the 
     * SPAT information received from the Traffic Signal Controller.
    */
    enum class SPAT_PROJECTION_MODE {
        /**
         * @brief Do not append any future information to received SPAT.
        */
        NO_PROJECTION = 0,
        /**
         * @brief Append future information reflected in received DPP
         */
        DPP_PROJECTION = 1,
        /**
         * @brief Append future information assuming fixed time, min vehicle recall on all vehicle phases.
         */
        FIXED_TIMING_PROJECTION = 2,
        // TODO: Remove if no longer necessary for TM/TSP MMITSS Integration
        // /**
        //  * @brief Append future information assuming reflected in received MRP (MMITSS Road Side Processor) Phase Schedule.
        //  */
        // MMITSS_PHASE_SCHEDULE = 3 
        // -------------------------------------------------------------------------------------       
    };

    SPAT_PROJECTION_MODE spat_projection_mode_from_int( const int i );
}