#include "spat_projection_mode.h"

namespace traffic_signal_controller_service{
    spat_projection_mode spat_projection_mode_from_int( const int i ) {
        switch (i)
        {
        case 0:
            return spat_projection_mode::no_projection;
            break;
        case 1:
            return spat_projection_mode::dpp_projection;
            break;
        case 2:
            return spat_projection_mode::fixed_projection;
            break;
        default:
            return spat_projection_mode::no_projection;
        }
    }
}