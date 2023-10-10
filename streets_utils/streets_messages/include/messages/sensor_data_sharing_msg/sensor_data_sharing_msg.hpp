#pragma once

#include "equipement_type.hpp"
#include "time_stamp.hpp"
#include "detected_object_data.hpp"
#include "position_3d.hpp"
#include "positional_accuracy.hpp"

#include <vector>
#include <string>
#include <optional>

namespace streets_utils::messages {


    /**
     * @brief 
     */
    struct sensor_data_sharing_msg {
        /**
         * @brief -- a count value which is incremented with each use [0,255]
         *  the next value after 255 shall be one value 
         *  0 (0x00) shall indicate that MsgCount is not available.
         */
        uint _msg_count;
        equipment_type _equipment_type;
        position_3d _ref_positon;
        std::optional<position_confidence> _ref_position_elavation_confidence;
        positional_accuracy _ref_position_confidence;
        time_stamp _time_stamp;
        std::string _source_id;
        std::vector<detected_object_data> _objects;
                
    };

}