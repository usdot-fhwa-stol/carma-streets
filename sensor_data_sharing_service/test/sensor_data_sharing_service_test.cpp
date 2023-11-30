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
#include<streets_clock_singleton.h>


using testing::_;
using testing::Return;
using testing::AnyNumber;
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
        // Initialize streets_clock in non simulation mode
        streets_service::streets_clock_singleton::create(false);
        // If producer null expect runtime error

        EXPECT_THROW(serv.produce_sdsms(), std::runtime_error);
        const std::string detected_object_json ="{\"type\":\"CAR\",\"confidence\":0.7,\"sensorId\":\"sensor1\",\"projString\":\"projectionString2\",\"objectId\":\"Object7\",\"position\":{\"x\":-1.1,\"y\":-2.0,\"z\":-3.2},\"positionCovariance\":[[1.0,0.0,0.0],[1.0,0.0,0.0],[1.0,0.0,0.0]],\"velocity\":{\"x\":1.0,\"y\":1.0,\"z\":1.0},\"velocityCovariance\":[[1.0,0.0,0.0],[1.0,0.0,0.0],[1.0,0.0,0.0]],\"angularVelocity\":{\"x\":0.1,\"y\":0.2,\"z\":0.3},\"angularVelocityCovariance\":[[1.0,0.0,0.0],[1.0,0.0,0.0],[1.0,0.0,0.0]],\"size\":{\"length\":2.0,\"height\":1.0,\"width\":0.5}}";
        auto detected_object = streets_utils::messages::detected_objects_msg::from_json(detected_object_json);
        serv.detected_objects[detected_object._object_id] =detected_object;
        serv.sdsm_producer =  std::make_shared<kafka_clients::mock_kafka_producer_worker>();
        EXPECT_CALL(dynamic_cast<kafka_clients::mock_kafka_producer_worker&>(*serv.sdsm_producer),is_running()).Times(3).WillOnce(Return(true))
                                                        .WillOnce(Return(true))
                                                        .WillRepeatedly(Return(false));
        const std::string sdsm_json ="{\"msg_cnt\":0,\"source_id\":\"\",\"equipment_type\":0,\"sdsm_time_stamp\":{\"second\":0,\"minute\":0,\"hour\":0,\"day\":0,\"month\":0,\"year\":0,\"offset\":0},\"ref_pos\":{\"long\":0,\"lat\":0},\"ref_pos_xy_conf\":{\"semi_major\":0,\"semi_minor\":0,\"orientation\":0},\"objects\":[]}";
        EXPECT_CALL(dynamic_cast<kafka_clients::mock_kafka_producer_worker&>(*serv.sdsm_producer), send(sdsm_json)).Times(1);
        serv.produce_sdsms();
        EXPECT_EQ(serv.detected_objects.size(), 0);
    }
}