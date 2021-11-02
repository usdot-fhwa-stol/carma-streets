/**
# LocationOffsetECEF.msg

# Location offset is used to describe a trajectory based on
# the combination of start location and a list of offsets.
# Each offset indicates the offset from last offset/location in
# centimeters. The time interval between each pair of neighbor
# points should be 0.1 second. So the length of trajectory
# will depend on the vehicle current speed.

# offset in X coordinate of ECEF frame
# the x, y, z range is in [-500, 501] and 501 means offset unavailable
int16  offset_x

# offset in Y coordinate of ECEF frame
int16  offset_y

# offset in Z coordinate of ECEF frame
int16  offset_z
**/

#ifndef LOCATION_OFFSET_ECEF_H
#define LOCATION_OFFSET_ECEF_H

#include <iostream>

namespace message_services
{
    namespace models
    {
        typedef struct locationOffsetECEF
        {
            std::int16_t offset_x = 0 ;
            std::int16_t offset_y = 0;
            std::int16_t offset_z = 0;
        } locationOffsetECEF_t;
    }
}

#endif