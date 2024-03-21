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
#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <spdlog/spdlog.h>
#include <sensor_data_sharing_service.hpp>
#include <streets_environment_variables.h>
#include <mock_kafka_consumer_worker.h>
#include <mock_kafka_producer_worker.h>
#include <mock_kafka_client.h>
#include <streets_utils/streets_messages_lib/deserializers/detected_obj_msg_deserializer.hpp>
#include <streets_clock_singleton.h>
#include <streets_configuration.h>
#include <streets_utils/streets_messages_lib/deserializers/sensor_data_sharing_msg_json_deserializer.hpp>



using testing::_;
using testing::Return;
using testing::AnyNumber;
using testing::SaveArg;
using testing::DoAll;
namespace sensor_data_sharing_service {

    TEST(sensorDataSharingServiceTest, consumeDetections) {
        // Set simulation mode to false
        setenv(streets_service::SIMULATION_MODE_ENV.c_str(), "FALSE", 1);
        sds_service serv;
            
        serv.initialize();
        // If consumer null expect runtime error
        EXPECT_THROW(serv.consume_detections(), std::runtime_error);
        serv.detection_consumer =  std::make_shared<kafka_clients::mock_kafka_consumer_worker>();
        EXPECT_CALL(dynamic_cast<kafka_clients::mock_kafka_consumer_worker&>(*serv.detection_consumer),subscribe()).Times(1);
        EXPECT_CALL(dynamic_cast<kafka_clients::mock_kafka_consumer_worker&>(*serv.detection_consumer),is_running()).Times(4)
                                                        .WillOnce(Return(true))
                                                        .WillOnce(Return(true))
                                                        .WillOnce(Return(true))
                                                        .WillRepeatedly(Return(false));
        EXPECT_CALL(dynamic_cast<kafka_clients::mock_kafka_consumer_worker&>(*serv.detection_consumer), consume(_)).Times(3).WillOnce(Return(""))
                                                            .WillOnce(Return("NOT JSON"))
                                                            .WillOnce(Return(
                                                                R"(
                                                                    {
                                                                        "type":"CAR",
                                                                        "confidence":0.7,
                                                                        "sensorId":"sensor1",
                                                                        "projString":"projectionString2",
                                                                        "objectId":123,
                                                                        "position":{
                                                                            "x":-1.1,
                                                                            "y":-2.0,
                                                                            "z":-3.2
                                                                        },
                                                                        "positionCovariance":[[1.0,0.0,0.0],[1.0,0.0,0.0],[1.0,0.0,0.0]],
                                                                        "velocity":{
                                                                            "x":1.0,
                                                                            "y":1.0,
                                                                            "z":1.0
                                                                        },
                                                                        "velocityCovariance":[[1.0,0.0,0.0],[1.0,0.0,0.0],[1.0,0.0,0.0]],
                                                                        "angularVelocity":{
                                                                            "x":0.1,
                                                                            "y":0.2,
                                                                            "z":0.3
                                                                        },
                                                                        "angularVelocityCovariance":[[1.0,0.0,0.0],[1.0,0.0,0.0],[1.0,0.0,0.0]],
                                                                        "size":{
                                                                            "length":2.0,
                                                                            "height":1.0,
                                                                            "width":0.5
                                                                        },
                                                                        "timestamp":1000
                                                                    }
                                                                    )"
                                                            ));
        
        serv.consume_detections();
        // Consumed detection timestamp is more than 500 ms older that current real wall time. 
        // TODO: Create test case that covers condition for consuming up to date detection data.
        EXPECT_EQ(serv.detected_objects.size(), 0);
    }

     TEST(sensorDataSharingServiceTest, produceSdsms) {
        
        sds_service serv;
        serv._infrastructure_id = "rsu_1234";
        // Initialize streets_clock in non simulation mode
        streets_service::streets_clock_singleton::create(false);
        // If producer null expect runtime error

        EXPECT_THROW(serv.produce_sdsms(), std::runtime_error);
        const std::string detected_object_json =
            R"(
                {
                    "type":"TRUCK",
                    "confidence":1.0,
                    "sensorId":"IntersectionLidar",
                    "projString":"+proj=tmerc +lat_0=0 +lon_0=0 +k=1 +x_0=0 +y_0=0 +datum=WGS84 +units=m +geoidgrids=egm96_15.gtx +vunits=m +no_defs",
                    "objectId":222,
                    "position":{
                        "x":-23.70157158620998,
                        "y":-4.561758806718842,
                        "z":-9.991932254782586
                    },
                    "positionCovariance":[[0.04000000000000001,0.0,0.0],[0.0,0.04000000000000001,0.0],[0.0,0.0,0.04000000000000001]],
                    "velocity":{
                        "x":1.0,
                        "y":0.0,
                        "z":0.0
                    },
                    "velocityCovariance":[[0.04000000000000001,0.0,0.0],[0.0,0.04000000000000001,0.0],[0.0,0.0,0.04000000000000001]],
                    "angularVelocity":{
                        "x":0.0,
                        "y":0.0,
                        "z":0.0
                    },
                    "angularVelocityCovariance":[[0.010000000000000002,0.0,0.0],[0.0,0.010000000000000002,0.0],[0.0,0.0,0.010000000000000002]],
                    "size":{
                        "length":2.601919174194336,
                        "height":1.3072861433029175,
                        "width":1.2337223291397095
                    },
                    "timestamp":41343
                }
            )";
        auto detected_object = streets_utils::messages::detected_objects_msg::from_json(detected_object_json);
        serv.detected_objects[detected_object._object_id] =detected_object;
        serv.sdsm_producer =  std::make_shared<kafka_clients::mock_kafka_producer_worker>();
        EXPECT_CALL(dynamic_cast<kafka_clients::mock_kafka_producer_worker&>(*serv.sdsm_producer),is_running()).Times(3).WillOnce(Return(true))
                                                        .WillOnce(Return(true))
                                                        .WillRepeatedly(Return(false));
        std::string sdsm_json;
        EXPECT_CALL(dynamic_cast<kafka_clients::mock_kafka_producer_worker&>(*serv.sdsm_producer), send(_)).WillOnce( DoAll(SaveArg<0>(&sdsm_json)) );
        serv.produce_sdsms();

        // Verify produced json
        SPDLOG_INFO("Produces SDMS {0}", sdsm_json);
        streets_utils::messages::sdsm::sensor_data_sharing_msg msg = streets_utils::messages::sdsm::from_json(sdsm_json);
        EXPECT_EQ( msg._equipment_type, streets_utils::messages::sdsm::equipment_type::RSU);
        EXPECT_EQ( msg._source_id, serv._infrastructure_id );
        // http://en.cppreference.com/w/cpp/chrono/c/time
        const std::time_t now = std::time(nullptr) ; // get the current time point

        // convert it to (local) calendar time
        // http://en.cppreference.com/w/cpp/chrono/c/localtime
        const std::tm calendar_time = *std::localtime( std::addressof(now) ) ;
        EXPECT_EQ(calendar_time.tm_year+1900, msg._time_stamp.year);
        EXPECT_EQ(calendar_time.tm_mon+1, msg._time_stamp.month);
        EXPECT_EQ(calendar_time.tm_mday, msg._time_stamp.day);
        EXPECT_EQ(calendar_time.tm_hour, msg._time_stamp.hour);
        // Account for runs near close of minute
        EXPECT_NEAR(calendar_time.tm_min, msg._time_stamp.minute, 1);
        EXPECT_EQ(1 , msg._objects.size());
        EXPECT_EQ(streets_utils::messages::sdsm::object_type::VEHICLE, msg._objects[0]._detected_object_common_data._object_type);
        EXPECT_TRUE(msg._objects[0]._detected_object_common_data._yaw_rate_confidence.has_value());
        EXPECT_EQ(streets_utils::messages::sdsm::angular_velocity_confidence::DEGSEC_0_1 , msg._objects[0]._detected_object_common_data._yaw_rate_confidence);
        EXPECT_EQ(serv.detected_objects.size(), 0);
        // SDSM assumes NED coordinate frame. Incoming detection is ENU. 1,0 in ENU is 0,1 in NED and is a 90 degree heading (heading is calculated from velocity)
        EXPECT_NEAR( msg._objects[0]._detected_object_common_data._heading, 7200, 2);
    }

    TEST(sensorDataSharingServiceTest,readLanelet2Map) {
        sds_service serv;
        EXPECT_TRUE(serv.read_lanelet_map("/home/carma-streets/sample_map/town01_vector_map_test.osm"));
        EXPECT_NE(nullptr, serv.map_ptr);
    }

    TEST(sensorDataSharingServiceTest, toPosition3d) {
        lanelet::GPSPoint point{38.9551605829,-77.14701567,1};
        streets_utils::messages::sdsm::position_3d position = to_position_3d(point);
        EXPECT_EQ(389551605, position._latitude );
        EXPECT_EQ(-771470156, position._longitude);
        EXPECT_EQ(10, position._elevation);
    }


}