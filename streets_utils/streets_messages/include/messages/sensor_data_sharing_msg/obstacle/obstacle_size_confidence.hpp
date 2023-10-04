#pragma once
#include "../size_value_confidence.hpp"
namespace streets_utils::messages{
    struct obstacle_size_confidence{
        /**
         * @brief Confidence in reported width
         */
        size_value_confidence _width_confidence;
        /**
         * @brief Confidence in reported length
         */
        size_value_confidence _length_confidence;
        /**
         * @brief Confidence in reported height
         */
        size_value_confidence _height_confidence;
    };
}