#pragma once

#include <stdlib.h>

namespace traffic_signal_controller_service{
    /**
     * @brief Enumeration to describe how we project future events on to the 
     * SPAT information received from the Traffic Signal Controller.
    */
    enum class spat_projection_mode {
        /**
         * @brief Do not append any future information to received SPAT.
        */
        no_projection = 0,
        /**
         * @brief Append future information reflected in received DPP
         */
        dpp_projection = 1,
        /**
         * @brief Append future information assuming fixed time, min vehicle recall on all vehicle phases.
         */
        fixed_projection =2;        
    };
}