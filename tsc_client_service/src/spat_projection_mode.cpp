#include "spat_projection_mode.h"

namespace traffic_signal_controller_service{
    SPAT_PROJECTION_MODE spat_projection_mode_from_int( const int i ) {
        switch (i)
        {
        case 0:
            return SPAT_PROJECTION_MODE::NO_PROJECTION;
        case 1:
            return SPAT_PROJECTION_MODE::DPP_PROJECTION;
        case 2:
            return SPAT_PROJECTION_MODE::FIXED_TIMING_PROJECTION;
        default:
            return SPAT_PROJECTION_MODE::NO_PROJECTION;
        }
    }
}