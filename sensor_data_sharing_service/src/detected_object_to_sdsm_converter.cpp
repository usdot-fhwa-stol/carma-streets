// Copyright 2019-2023 Leidos
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
#include "detected_object_to_sdsm_converter.hpp"

namespace sensor_data_sharing_service{

    streets_utils::messages::sdsm::time_stamp to_sdsm_timestamp(const uint64_t _epoch_time_ms) {
        streets_utils::messages::sdsm::time_stamp sdsm_timestamp; 

        // From millisecond time stamp
        boost::posix_time::ptime posix_time = boost::posix_time::from_time_t(_epoch_time_ms/SECONDS_TO_MILLISECONDS) +
                                        boost::posix_time::millisec( _epoch_time_ms % SECONDS_TO_MILLISECONDS);
        sdsm_timestamp.year = posix_time.date().year();
        sdsm_timestamp.month = posix_time.date().month();
        sdsm_timestamp.day = posix_time.date().day();

        sdsm_timestamp.hour = (unsigned int) posix_time.time_of_day().hours();
        sdsm_timestamp.minute = (unsigned int) posix_time.time_of_day().minutes();
        // Milliseconds of the current minute. The SDSM field is named seconds but is in the unit of milliseconds (see DDateTime from J2735).
        // Fractional_seconds returns microseconds from the current second since default time resolution is microseconds
        sdsm_timestamp.second = (unsigned int) (posix_time.time_of_day().seconds()*SECONDS_TO_MILLISECONDS + posix_time.time_of_day().fractional_seconds()/MILLISECONDS_TO_MICROSECONDS);
        return sdsm_timestamp;
    }

    streets_utils::messages::sdsm::detected_object_data to_detected_object_data(const streets_utils::messages::detected_objects_msg::detected_objects_msg &msg, uint64_t sdsm_message_timestamp ) {
        streets_utils::messages::sdsm::detected_object_data detected_object;
        detected_object._detected_object_common_data._object_type = to_object_type(msg._type);
        if (detected_object._detected_object_common_data._object_type == streets_utils::messages::sdsm::object_type::VEHICLE ) {
            streets_utils::messages::sdsm::detected_vehicle_data optional_data;
            // Size in cm
            streets_utils::messages::sdsm::vehicle_size veh_size;
            veh_size._length= static_cast<unsigned int>(msg._size._length*METERS_TO_CM);
            veh_size._width= static_cast<unsigned int>(msg._size._width*METERS_TO_CM);
            optional_data._size = veh_size;
            // Height in 5 cm
            optional_data._vehicle_height = static_cast<unsigned int>(msg._size._height * METERS_TO_5_CM);


            detected_object._detected_object_optional_data = optional_data;
        }
        else if ( detected_object._detected_object_common_data._object_type == streets_utils::messages::sdsm::object_type::VRU ) {
            streets_utils::messages::sdsm::detected_vru_data optional_data;
            // Populate Optional VRU data
            detected_object._detected_object_optional_data = optional_data;
        }
        else if (detected_object._detected_object_common_data._object_type == streets_utils::messages::sdsm::object_type::UNKNOWN ){
            streets_utils::messages::sdsm::detected_obstacle_data optional_data;
            // size dimensions in units of 0.1 m
            streets_utils::messages::sdsm::obstacle_size obs_size;
            obs_size._length = static_cast<unsigned int>(msg._size._length*METERS_TO_10_CM);
            obs_size._width = static_cast<unsigned int>(msg._size._width*METERS_TO_10_CM);
            obs_size._height = static_cast<unsigned int>(msg._size._height*METERS_TO_10_CM);
            optional_data._size = obs_size;

            detected_object._detected_object_optional_data = optional_data;

        }
        // Used to convey an offset in time relative to the sDSMTimeStamp associated with the reference position. Negative values indicate 
        // the provided detected object characteristics refer to a point in time after the sDSMTimeStamp
        detected_object._detected_object_common_data._time_measurement_offset = static_cast<int>(sdsm_message_timestamp - msg._timestamp);
        detected_object._detected_object_common_data._classification_confidence = static_cast<unsigned int>(msg._confidence*100);
        // TODO: Change Detected Object ID to int
        detected_object._detected_object_common_data._object_id = msg._object_id;
        // Units are 0.1 m 
        detected_object._detected_object_common_data._position_offset._offset_x = static_cast<unsigned int>(msg._position._x*METERS_TO_10_CM);
        detected_object._detected_object_common_data._position_offset._offset_y = static_cast<unsigned int>(msg._position._y*METERS_TO_10_CM);
        detected_object._detected_object_common_data._position_offset._offset_z = static_cast<unsigned int>(msg._position._z*METERS_TO_10_CM);
        // Position Confidence
        detected_object._detected_object_common_data._pos_confidence = to_position_confidence_set(msg._position_covariance);
        // Units are 0.02 m/s
        detected_object._detected_object_common_data._speed = static_cast<unsigned int>(std::hypot( msg._velocity._x* METERS_PER_SECOND_TO_2_CM_PER_SECOND,  msg._velocity._y* METERS_PER_SECOND_TO_2_CM_PER_SECOND));
        // Speed confidence
        detected_object._detected_object_common_data._speed_confidence = to_xy_speed_confidence(msg._velocity_covariance);
        // Speed Z
        detected_object._detected_object_common_data._speed_z = static_cast<unsigned int>(fabs(msg._velocity._z)* METERS_PER_SECOND_TO_2_CM_PER_SECOND);
        // Speed Z confidence
        detected_object._detected_object_common_data._speed_z_confidence = to_z_speed_confidence(msg._velocity_covariance);
        // Heading
        detected_object._detected_object_common_data._heading = to_heading(msg._velocity);
        // TODO: how to calculate heading confidence without orientation covariance
        // Possible approximation is velocity covariance since we are using that currently
        // Currently hard coding value
        detected_object._detected_object_common_data._heading_confidence = streets_utils::messages::sdsm::heading_confidence::PREC_0_1_deg;
        if(msg._angular_velocity){
            // Yaw rate OPTIONAL
            detected_object._detected_object_common_data._acceleration_4_way->_yaw_rate = to_yaw_rate(msg._angular_velocity.value()._z);
        }

        if(msg._angular_velocity_covariance){
            // Yaw rate confidence OPTIONAL
            detected_object._detected_object_common_data._yaw_rate_confidence = to_yaw_rate_confidence(msg._angular_velocity_covariance.value());
        }

        return detected_object;
    }

    streets_utils::messages::sdsm::object_type to_object_type(const std::string &detection_type){
        if ( sdsm_object_types.find(detection_type) != sdsm_object_types.end()) {
            return sdsm_object_types.at(detection_type);
        }
        return streets_utils::messages::sdsm::object_type::UNKNOWN;
    }

    streets_utils::messages::sdsm::position_confidence_set to_position_confidence_set(const std::vector<std::vector<double>> &_position_covariance) {
        auto x_variance = _position_covariance[0][0];
        auto y_variance = _position_covariance[1][1];
        auto z_variance = _position_covariance[2][2];
        // Assuming normal distribution and 95 % confidence interval (From J3224 Specification documentation of all accuracy/confidence measures)
        // Adding and subtracting two standard deviations from mean to achieve 95% confidence interval.
        // TODO: If position variance x is different from y how do we handle this? For now assuming X variance is the same as Y and justing using 
        // X
        //According to 68-95-99.7 rule, two standard deviations to get 95% confidence interval for normal distribution
        auto xy_accuracy = sqrt(x_variance) * 2 ; 
        auto z_accuracy = sqrt(z_variance) * 2;
        streets_utils::messages::sdsm::position_confidence_set position_confidence_set;
        position_confidence_set._position_confidence =  to_position_confidence(xy_accuracy);
        position_confidence_set._elevation_confidence = to_position_confidence(z_accuracy);
        return position_confidence_set;
    }

    streets_utils::messages::sdsm::position_confidence to_position_confidence(const double accuracy) {
        // Check to see if accuracy is greater than or equal to half way between to confidence enumeration
        if ( accuracy >= 350) {
            return streets_utils::messages::sdsm::position_confidence::A_500M;
        }
        else if (accuracy >= 150) {
            return streets_utils::messages::sdsm::position_confidence::A_200M;
        }
        else if (accuracy >= 75) {
            return streets_utils::messages::sdsm::position_confidence::A_100M;
        }
        else if (accuracy >= 35) {
            return streets_utils::messages::sdsm::position_confidence::A_50M;
        }
        else if (accuracy >= 15) {
            return streets_utils::messages::sdsm::position_confidence::A_20M;
        }
        else if (accuracy >= 7.5) {
            return streets_utils::messages::sdsm::position_confidence::A_10M;
        }
        else if (accuracy >= 3.5) {
            return streets_utils::messages::sdsm::position_confidence::A_5M;
        }
        else if (accuracy >= 1.5) {
            return streets_utils::messages::sdsm::position_confidence::A_2M;
        }
        else if (accuracy >= 0.75) {
            return streets_utils::messages::sdsm::position_confidence::A_1M;
        }
        else if (accuracy >= 0.35) {
            return streets_utils::messages::sdsm::position_confidence::A_50CM;
        }
        else if (accuracy >= 0.15) {
            return streets_utils::messages::sdsm::position_confidence::A_20CM;
        }
        else if (accuracy >= 0.075) {
            return streets_utils::messages::sdsm::position_confidence::A_10CM;
        }
        else if (accuracy >= 0.035) {
            return streets_utils::messages::sdsm::position_confidence::A_5CM;
        }
        else if (accuracy >= 0.015) {
            return streets_utils::messages::sdsm::position_confidence::A_2CM;
        }
        else {
            // This is the lowest position confidence the SDSM can reflect
            return streets_utils::messages::sdsm::position_confidence::A_1CM;
        }
        
    }

    unsigned int to_heading(const streets_utils::messages::detected_objects_msg::vector_3d &velocity){
        if ( std::abs(velocity._x) < 0.01 && std::abs(velocity._y) < 0.01) {
            return 0;
        }
        auto heading_radians = std::atan2(velocity._y,velocity._x);
        auto heading_degrees = heading_radians*(180/M_PI);
        // If angle is negative 360 + (-negative angle)
        if ( heading_degrees < 0 ) {
            heading_degrees = 360 + heading_degrees;
        }     
        // in units (x)  = 0.0125 degrees
        // 28800 x = 360 degrees
        return static_cast<unsigned int>(heading_degrees * (28800/360));
    }

    streets_utils::messages::sdsm::speed_confidence to_xy_speed_confidence(const std::vector<std::vector<double>> &velocity_covariance) {
        auto x_variance = velocity_covariance[0][0];
        auto y_variance = velocity_covariance[1][1];
        // Assuming normal distribution and 95 % confidence interval (From J3224 Specification documentation of all accuracy/confidence measures)
        // +/- 2*variance to achieve 95% confidence interval.
        // TODO: If position variance x is different from y how do we handle this? For now assuming X variance is the same as Y and justing using 
        // X
        // Multiply variance by 2 get 95% confidence interval for normal distribution
        auto xy_accuracy = sqrt(x_variance) * 2 ; 
        return to_speed_confidence(xy_accuracy);
    }

    streets_utils::messages::sdsm::speed_confidence to_z_speed_confidence(const std::vector<std::vector<double>> &velocity_covariance) {
        auto z_variance = velocity_covariance[2][2];
        // Assuming normal distribution and 95 % confidence interval (From J3224 Specification documentation of all accuracy/confidence measures)
        // +/- 2*variance to achieve 95% confidence interval.

        // Multiply variance by 2 get 95% confidence interval for normal distribution
        auto z_accuracy = sqrt(z_variance) * 2 ; 
        return to_speed_confidence(z_accuracy);
    }

    streets_utils::messages::sdsm::speed_confidence to_speed_confidence(const double accuracy) {
         // Check to see if accuracy is greater than or equal to half way between to confidence enumeration
        if ( accuracy >= 55) {
            return streets_utils::messages::sdsm::speed_confidence::PREC_100ms;
        }
        else if (accuracy >= 7.5) {
            return streets_utils::messages::sdsm::speed_confidence::PREC_10ms;
        }
        else if (accuracy >= 3) {
            return streets_utils::messages::sdsm::speed_confidence::PREC_5ms;
        }
        else if (accuracy >= .55) {
            return streets_utils::messages::sdsm::speed_confidence::PREC_1ms;
        }
        else if (accuracy >= .075) {
            return streets_utils::messages::sdsm::speed_confidence::PREC_0_1ms;
        }
        else if (accuracy >= .03) {
            return streets_utils::messages::sdsm::speed_confidence::PREC_0_05ms;
        }
        else {
            // This is the lowest speed confidence the SDSM can reflect
            return streets_utils::messages::sdsm::speed_confidence::PREC_0_01ms;
        }
    }


    streets_utils::messages::sdsm::angular_velocity_confidence to_yaw_rate_confidence( const std::vector<std::vector<double>> &angular_velocity_covariance ){
        auto z_variance = angular_velocity_covariance[2][2];
        auto z_accuracy = sqrt(z_variance) * 2 ; 
        return to_angular_velocity_confidence(z_accuracy);
    }
    

    streets_utils::messages::sdsm::angular_velocity_confidence to_angular_velocity_confidence(const double accuracy) {
         if ( accuracy >= 55) {
            return streets_utils::messages::sdsm::angular_velocity_confidence::DEGSEC_100;
        }
        else if (accuracy >= 7.5) {
            return streets_utils::messages::sdsm::angular_velocity_confidence::DEGSEC_10;
        }
        else if (accuracy >= 3) {
            return streets_utils::messages::sdsm::angular_velocity_confidence::DEGSEC_05;
        }
        else if (accuracy >= .55) {
            return streets_utils::messages::sdsm::angular_velocity_confidence::DEGSEC_01;
        }
        else if (accuracy >= .075) {
            return streets_utils::messages::sdsm::angular_velocity_confidence::DEGSEC_0_1;
        }
        else if (accuracy >= .03) {
            return streets_utils::messages::sdsm::angular_velocity_confidence::DEGSEC_0_05;
        }
        else {
            // This is the lowest speed confidence the SDSM can reflect
            return streets_utils::messages::sdsm::angular_velocity_confidence::DEGSEC_0_01;
        }
    }

    int to_yaw_rate( const double yaw_rate_radians_per_second ) {
        // Return in units of 0.01 degrees per second;
        return static_cast<int>( yaw_rate_radians_per_second * (180/(M_PI)) * 100);
    }

    


}