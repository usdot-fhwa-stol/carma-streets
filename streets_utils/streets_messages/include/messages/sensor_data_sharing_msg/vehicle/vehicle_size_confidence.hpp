#pragma once
#include "../size_value_confidence.hpp"
#include <optional>
namespace streets_utils::messages{
    struct vehicle_size_confidence{
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
        std::optional<size_value_confidence> _height_confidence;
    };
}