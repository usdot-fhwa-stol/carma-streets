#include "detected_object_to_sdsm_converter.hpp"

namespace sensor_data_sharing_service{

    streets_utils::messages::sdsm::time_stamp to_sdsm_timestamp(const uint64_t _epoch_time_ms) {
        streets_utils::messages::sdsm::time_stamp rtn; 

        // From millisecond time stamp
        boost::posix_time::ptime posix_time = boost::posix_time::from_time_t(_epoch_time_ms/1000) +
                                        boost::posix_time::millisec(_epoch_time_ms % 1000);
        SPDLOG_INFO("Epoch time {0} corresponds to date time {1}", _epoch_time_ms, boost::posix_time::to_simple_string(posix_time));
        rtn.year = posix_time.date().year();
        rtn.month = posix_time.date().month();
        rtn.day = posix_time.date().day();

        rtn.hour = posix_time.time_of_day().hours();
        rtn.minute = posix_time.time_of_day().minutes();
        // Milliseconds of the current minute.
        rtn.second = posix_time.time_of_day().seconds()*1000 + posix_time.time_of_day().fractional_seconds();
        return rtn;
    }

    streets_utils::messages::sdsm::detected_object_data to_detected_object_data(const streets_utils::messages::detected_objects_msg::detected_objects_msg &msg) {
        streets_utils::messages::sdsm::detected_object_data rtn;
        if (msg._type == "CAR" || msg._type == "TRUCK" || msg._type == "VAN") {
            rtn._detected_object_common_data._object_type = streets_utils::messages::sdsm::object_type::VEHICLE;
            streets_utils::messages::sdsm::detected_vehicle_data optional_data;
            // Size in cm
            optional_data._size->_length= msg._size._length*(0.01);
            optional_data._size->_width= msg._size._width*(0.01);
            // Height in 5 cm
            optional_data._vehicle_height = msg._size._height *(0.05);


            rtn._detected_object_optional_data = optional_data;
        }
        else if ( msg._type == "PEDESTRIAN" || msg._type == "MOTORCYCLE") {
            rtn._detected_object_common_data._object_type = streets_utils::messages::sdsm::object_type::VRU;
            streets_utils::messages::sdsm::detected_vru_data optional_data;
            // Populate Optional VRU data
            rtn._detected_object_optional_data = optional_data;
        }
        else {
            rtn._detected_object_common_data._object_type = streets_utils::messages::sdsm::object_type::UNKNOWN;
            streets_utils::messages::sdsm::detected_obstacle_data optional_data;
            // size dimensions in units of 0.1 m
            optional_data._size._length = msg._size._length*(0.1);
            optional_data._size._width = msg._size._width*(0.1);
            optional_data._size._height = msg._size._height*(0.1);

            rtn._detected_object_optional_data = optional_data;

        }
        rtn._detected_object_common_data._classification_confidence = msg._confidence*100;
        // TODO: Change Detected Object ID to int
        rtn._detected_object_common_data._object_id = std::stoi(msg._object_id);
        // Units are 0.1 m
        rtn._detected_object_common_data._position_offset._offset_x = msg._position._x/10;
        rtn._detected_object_common_data._position_offset._offset_y = msg._position._y/10;
        rtn._detected_object_common_data._position_offset._offset_z = msg._position._z/10;
        // Units are 0.02 m/s
        rtn._detected_object_common_data._speed = std::hypot( msg._velocity._x* (0.02),  msg._velocity._y* (0.02));
        rtn._detected_object_common_data._speed_z =  msg._velocity._z* (0.02);

        return rtn;
    }
}