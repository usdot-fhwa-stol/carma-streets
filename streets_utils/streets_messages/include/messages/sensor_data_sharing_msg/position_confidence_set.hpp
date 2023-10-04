#pragma once

#include "position_confidence.hpp"

namespace streets_utils::messages {
    struct position_confidence_set{
        position_confidence _position_confidence;
        position_confidence _elavation_confidence;

    };
}