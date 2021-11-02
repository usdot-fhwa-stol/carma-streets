
/**
# LocationECEF.msg

# This is used to describe a global location in
# "earth-centered, earth-fixed" (ECEF) frame in centimeters.
# Location unavailable can be represented by (0, 0, 0)

# X coordinate in ECEF frame
# the range for this value is [-638363700, 638363700]
int32   ecef_x

# Y coordinate in ECEF frame
# the range for this value is [-638363700, 638363700]
int32   ecef_y

# Z coordinate in ECEF frame
# the range for this value is [-636225200, 636225200]
int32   ecef_z

# GPS timestamp in milliseconds
uint64  timestamp
***/
#ifndef LOCATION_ECEF_H
#define LOCATION_ECEF_H

#include <iostream>

namespace message_services
{
    namespace models
    {
        typedef struct locationECEF
        {
            std::int32_t ecef_x = 0;
            std::int32_t ecef_y = 0;
            std::int32_t ecef_z = 0;
            std::uint64_t timestamp = 0;
        } locationECEF_t;
    }
}

#endif