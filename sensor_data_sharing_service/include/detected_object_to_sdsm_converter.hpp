// Copyright 2023 Leidos
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
#pragma once 

#include <streets_utils/streets_messages_lib/detected_object_msg/detected_object_msg.hpp>
#include <streets_utils/streets_messages_lib/sensor_data_sharing_msg/sensor_data_sharing_msg.hpp>
#include <boost/date_time/posix_time/posix_time.hpp> //include all types plus i/o
#include <spdlog/spdlog.h>
#include <map>
#include <set>
#include <math.h>

namespace sensor_data_sharing_service {
    /**
     * @brief Map of detection types to SDSM object types.
     */
    inline const std::map<std::string, streets_utils::messages::sdsm::object_type, std::less<>> sdsm_object_types = {
        {"CAR",streets_utils::messages::sdsm::object_type::VEHICLE },
        {"VAN",streets_utils::messages::sdsm::object_type::VEHICLE },
        {"TRUCK",streets_utils::messages::sdsm::object_type::VEHICLE },
        {"PEDESTRIAN",streets_utils::messages::sdsm::object_type::VRU },
        {"CYCLIST",streets_utils::messages::sdsm::object_type::VRU },
        {"MOTORCYCLE",streets_utils::messages::sdsm::object_type::VRU },
    };

    inline const int MILLISECONDS_TO_MICROSECONDS = 1000;

    inline const int SECONDS_TO_MILLISECONDS = 1000;

    inline const int METERS_TO_CM = 100;

    inline const int METERS_TO_5_CM = 20;

    inline const int METERS_TO_10_CM = 10;

    inline const int METERS_PER_SECOND_TO_2_CM_PER_SECOND = 50;
    /**
     * @brief convert epoch millisecond timestamp to sdsm timestamp object
     * @return streets_utils::messages::sdsm::time_stamp.
     */
    streets_utils::messages::sdsm::time_stamp to_sdsm_timestamp(const uint64_t _epoch_time_ms);
    /**
     * @brief convert detected_object_msg to sdsm detection_object_data.
     * @note method assumes detected_object_msg is in NED coordinate frame (NED is required for SDSM based on J3223 specification) 
     * @return streets_utils::messages::sdsm::detected_object_data.
     */
    streets_utils::messages::sdsm::detected_object_data to_detected_object_data(const streets_utils::messages::detected_objects_msg::detected_objects_msg &msg, const uint64_t sdsm_message_timestamp);
    /**
     * @brief convert string detection type to sdsm object_type.
     * @param detection_type string detection classification for detection message. 
     * @return streets_utils::messages::sdsm::object_type
     */
    streets_utils::messages::sdsm::object_type to_object_type(const std::string &detection_type);
    /**
     * @brief Convert XY velocity into 2 dimensional heading measured in 0.0125 degree units
     * @param velocity vector of velocity
     * @return 2 dimensional heading in units of 0.0125 degrees
     */
    unsigned int to_heading(const streets_utils::messages::detected_objects_msg::vector_3d &velocity);
    /**
     * @brief Convert position covariance to position confidence set which includes positionXY confidence
     * and position z confidence.
     * @param _position_covariance 
     * @return position confidence set.
     */
    streets_utils::messages::sdsm::position_confidence_set to_position_confidence_set( const std::vector<std::vector<double>> &_position_covariance);
    /**
     * @brief Convert velocity covariance to xy speed confidence.
     * @param velocity_covariance 
     * @return xy speed confidence
     */
    streets_utils::messages::sdsm::speed_confidence to_xy_speed_confidence(const std::vector<std::vector<double>> &velocity_covariance);
     /**
     * @brief Convert velocity covariance to z speed confidence.
     * @param velocity_covariance 
     * @return z speed confidence
     */
    streets_utils::messages::sdsm::speed_confidence to_z_speed_confidence(const std::vector<std::vector<double>> &velocity_covariance);
    /**
     * @brief Convert accuracy value to speed confidence enumeration. To minimize error in conversion, method 
     * rounds up/down to the nearest enum value
     * @param accuracy 
     * @return speed confidence enumeration
     */
    streets_utils::messages::sdsm::speed_confidence to_speed_confidence(const double accuracy);
    /**
     * @brief Convert angular velocity covariance to angular velocity confidence.
     * @param angular_velocity_covariance 
     * @return angular velocity confidence
     */
    streets_utils::messages::sdsm::angular_velocity_confidence to_yaw_rate_confidence( const std::vector<std::vector<double>> &angular_velocity_covariance );
    /**
     * @brief Convert accuracy value to position confidence enumeration. To minimize error in conversion, method 
     * rounds up/down to the nearest enum value
     * @param accuracy 
     * @return position confidence enumeration
     */
    streets_utils::messages::sdsm::position_confidence to_position_confidence(const double accuracy);
    /**
     * @brief Convert accuracy value to angular velocity confidence enumeration. To minimize error in conversion, method 
     * rounds up/down to the nearest enum value
     * @param accuracy 
     * @return angular velocity confidence
     */
    streets_utils::messages::sdsm::angular_velocity_confidence to_angular_velocity_confidence(const double accuracy);

    /**
     * @brief Convert yaw rate in radians per second to 100ths of a degree per second 
     * @param yaw_rate_radians_per_second 
     * @return yaw rate in 0.01 degrees/second.
     */
    int to_yaw_rate( const double yaw_rate_radians_per_second );




}