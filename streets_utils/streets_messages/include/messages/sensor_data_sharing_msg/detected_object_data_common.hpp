#pragma once

#include "object_type.hpp"
#include "time_confidence.hpp"
#include "position_offset.hpp"
#include "position_confidence_set.hpp"
#include "speed_confidence.hpp"
#include "heading_confidence.hpp"
#include "acceleration_set_4_way.hpp"
#include "acceleration_confidence_set_4_way.hpp"
#include <stdlib.h>
#include <optional>

namespace streets_utils::messages {
    struct detected_object_data_common{
        /**
         * @brief Object type enumeration 
         */
        object_type _object_type;
        /**
         * @brief Confidence in object type classification [0,101]
         */
        unsigned int  _classification_confidence;
        /**
         * @brief Object ID [0, 65535] 
         */
        unsigned int  _object_id;
        /**
         * @brief Time relative to SDSM timestamp assoicated with detection [-1500, 1500]
         */
        int _time_measurement_offset;
        /**
         * @brief Time Confidence enumeration for time offset.
         */
        time_confidence _time_confidence;
        /**
         * @brief Cartesian offset from SDSM reporter reference location to represent detected object location
         */
        position_offset _position_offset;
        /**
         * @brief Confidence in reported position
         */
        position_confidence_set _pos_confidence;
        /**
         * @brief Object speed in unit (0.02 m/s) [0, 8191]
         */
        unsigned int  _speed;
        /**
         * @brief Confidence in reported speed 
         */
        speed_confidence _speed_confidence;
        /**
         * @brief Object speed along Z axis unit (0.02 m/s) [0, 8191]
         */
        std::optional<unsigned int > _speed_z;
        /**
         * @brief Confidence in reported speed z 
         */
        std::optional<speed_confidence> _speed_z_confidence;
        /**
         * @brief Heading in 0.0125 degrees [0, 28800]
         */
        unsigned int  _heading;
        /**
         * @brief Confidence in reported heading
         */
        heading_confidence _heading_confidence;
        /**
         * @brief Acceleration in longitudinal, lateral, vertical and angular velocity.
         */
        std::optional<acceleration_set_4_way> _acceleration_4_way;
        /**
         * @brief Confidence in reported 4 way acceleration.
         */
        std::optional<acceleration_confidence_set_4_way> _acceleration_confidence;


    };
}