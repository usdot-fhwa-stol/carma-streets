#pragma once

#include "messages/sensor_data_sharing_msg/obstacle/obstacle_size.hpp"
#include "messages/sensor_data_sharing_msg/obstacle/obstacle_size_confidence.hpp"
namespace streets_utils::messages{
    struct detected_obstacle_data{
        /**
         * @brief Size of obstacle.
         */
        obstable_size _size;
        /**
         * @brief Confidence of reported size.
         */
        obstacle_size_confidence _size_confidence;
    };
}