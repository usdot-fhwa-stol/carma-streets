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
        
        sds_service serv;
        // If consumer null expect runtime error
        EXPECT_THROW(serv.consume_detections(), std::runtime_error);

        serv.detection_consumer =  std::make_shared<kafka_clients::mock_kafka_consumer_worker>();
        EXPECT_CALL(dynamic_cast<kafka_clients::mock_kafka_consumer_worker&>(*serv.detection_consumer),subscribe()).Times(1);
        EXPECT_CALL(dynamic_cast<kafka_clients::mock_kafka_consumer_worker&>(*serv.detection_consumer),is_running()).Times(4).WillOnce(Return(true))
                                                        .WillOnce(Return(true))
                                                        .WillOnce(Return(true))
                                                        .WillRepeatedly(Return(false));
        EXPECT_CALL(dynamic_cast<kafka_clients::mock_kafka_consumer_worker&>(*serv.detection_consumer), consume(_)).Times(3).WillOnce(Return(""))
                                                            .WillOnce(Return("NOT JSON"))
                                                            .WillOnce(Return(
                                                                "{\"type\":\"CAR\",\"confidence\":0.7,\"sensorId\":\"sensor1\",\"projString\":\"projectionString2\",\"objectId\":\"Object7\",\"position\":{\"x\":-1.1,\"y\":-2.0,\"z\":-3.2},\"positionCovariance\":[[1.0,0.0,0.0],[1.0,0.0,0.0],[1.0,0.0,0.0]],\"velocity\":{\"x\":1.0,\"y\":1.0,\"z\":1.0},\"velocityCovariance\":[[1.0,0.0,0.0],[1.0,0.0,0.0],[1.0,0.0,0.0]],\"angularVelocity\":{\"x\":0.1,\"y\":0.2,\"z\":0.3},\"angularVelocityCovariance\":[[1.0,0.0,0.0],[1.0,0.0,0.0],[1.0,0.0,0.0]],\"size\":{\"length\":2.0,\"height\":1.0,\"width\":0.5}}"
                                                            ));
        serv.consume_detections();
        EXPECT_EQ(serv.detected_objects.size(), 1);
    }

     TEST(sensorDataSharingServiceTest, produceSdsms) {
        
        sds_service serv;
        serv._infrastructure_id = "rsu_1234";
        // Initialize streets_clock in non simulation mode
        streets_service::streets_clock_singleton::create(false);
        // If producer null expect runtime error

        EXPECT_THROW(serv.produce_sdsms(), std::runtime_error);
        const std::string detected_object_json ="{\"type\":\"CAR\",\"confidence\":0.7,\"sensorId\":\"sensor1\",\"projString\":\"projectionString2\",\"objectId\":\"1\",\"position\":{\"x\":-1.1,\"y\":-2.0,\"z\":-3.2},\"positionCovariance\":[[1.0,0.0,0.0],[1.0,0.0,0.0],[1.0,0.0,0.0]],\"velocity\":{\"x\":1.0,\"y\":1.0,\"z\":1.0},\"velocityCovariance\":[[1.0,0.0,0.0],[1.0,0.0,0.0],[1.0,0.0,0.0]],\"angularVelocity\":{\"x\":0.1,\"y\":0.2,\"z\":0.3},\"angularVelocityCovariance\":[[1.0,0.0,0.0],[1.0,0.0,0.0],[1.0,0.0,0.0]],\"size\":{\"length\":2.0,\"height\":1.0,\"width\":0.5}}";
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
        EXPECT_EQ(serv.detected_objects.size(), 0);
    }

    TEST(sensorDataSharingServiceTest,readLanelet2Map) {
        setenv("LANELET2_MAP", "/home/carma-streets/sample_map/town01_vector_map_test.osm", 1);
        setenv("SENSOR_JSON_FILE_PATH", "/home/carma-streets/sensor_data_sharing_service/test/test_files/sensors.json", 1);
        sds_service serv;
        streets_service::streets_configuration::create("/home/carma-streets/sensor_data_sharing_service/manifest.json");
        EXPECT_TRUE(serv.read_lanelet_map());
        EXPECT_NE(nullptr, serv.map_ptr);

    }
}