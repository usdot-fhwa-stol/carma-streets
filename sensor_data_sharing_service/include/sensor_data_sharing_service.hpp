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

#include <kafka_client.h>
#include <streets_configuration.h>
#include <streets_configuration_exception.h>
#include <streets_service.h>
#include <streets_clock_singleton.h>
#include <string>
#include <streets_utils/streets_messages_lib/sensor_data_sharing_msg/sensor_data_sharing_msg.hpp>
#include <streets_utils/streets_messages_lib/serializers/sensor_data_sharing_msg_json_serializer.hpp>
#include <streets_utils/streets_messages_lib/detected_object_msg/detected_object_msg.hpp>
#include <streets_utils/streets_messages_lib/deserializers/detected_obj_msg_deserializer.hpp>
// Lanelet2 libraries
#include <lanelet2_core/LaneletMap.h>
#include <lanelet2_io/Io.h>
#include <lanelet2_core/primitives/LineString.h>
#include <lanelet2_core/primitives/Point.h>
#include <lanelet2_core/primitives/Polygon.h>
#include <lanelet2_core/utility/Units.h>
#include <lanelet2_core/geometry/BoundingBox.h>
#include <lanelet2_core/primitives/BoundingBox.h>
#include <lanelet2_extension/projection/mgrs_projector.h>
#include <lanelet2_extension/projection/local_frame_projector.h>
#include <lanelet2_extension/io/autoware_osm_parser.h>
#include <lanelet2_io/Io.h>
#include <lanelet2_io/io_handlers/Factory.h>
#include <lanelet2_core/geometry/Point.h>
#include <lanelet2_projection/UTM.h>
#include <map>
#include <shared_mutex>


#include "sensor_configuration_parser.hpp"
#include "detected_object_to_sdsm_converter.hpp"


namespace sensor_data_sharing_service {

    class sds_service : public streets_service::streets_service {
        private:
            /**
             * @brief Kafka producer for SDSM JSON
             */
            std::shared_ptr<kafka_clients::kafka_producer_worker> sdsm_producer;
            /*
             * @brief Kafka consumer for consuming Detected Object JSON
             */
            std::shared_ptr<kafka_clients::kafka_consumer_worker> detection_consumer;
            /**
             * @brief Map of detected objects. New detections of existing objects will replace old detections.
             */
            std::map<std::string,streets_utils::messages::detected_objects_msg::detected_objects_msg, std::less<>> detected_objects;
            /**
             * @brief Mutex for thread safe operations on detected objects map.
             */
            std::shared_mutex detected_objects_lock;
            /**
             * @brief Lanelet2 Map pointer
             */
            lanelet::LaneletMapPtr map_ptr;

            /**
             * @brief WSG84 Map projection
             */
            std::unique_ptr<lanelet::projection::LocalFrameProjector> map_projector;

            /**
             * @brief Location of sensor.This is also the sensor's coordinate frame orignin meaning all offsets 
             * are interpreted relative to this location.
             */
            lanelet::GPSPoint sdsm_reference_point;
            
            /**
             * @brief Infrastructure ID used as source ID for SDSM broadcast
             */
            std::string _infrastructure_id;

            /**
             * @brief Message count for SDSM
             */
            uint8_t _message_count = 0;

            /**
             * @brief Initialize Kafka consumers and producers for sensor data sharing service.
             * @return true if successful and false if unsuccessful.
             */
            bool initialize_kafka_consumers_producers( );
            /**
             * @brief Loop to consume detections from kafka detection consumer. Will terminate if kafka consumer
             * is no longer running
             * 
             * @throws std::runtime exception if detection_consumer == nullptr
             */
            void consume_detections();
            /**
             * @brief Loop to produce sensor data sharing messages from detected_objects. Loop will populate sensor data sharing message with 
             * most recent detection information, publish message and clear detected object map. Will terminate if kafka producer is no longer
             * running.
             * 
             * @throws std::runtime exception if sdsm_producer == nullptr
             */
            void produce_sdsms();
            /**
             * @brief Method to read lanelet2 map.
             * @param filepath to lanelet2 osm file.
             * @return true if successful.
             */
            bool read_lanelet_map(const std::string &filepath);
            
            /**
             * @brief Method to create SDSM from detected objects.
             * @return sensor_data_sharing_msg created from detected objects.
             */
            streets_utils::messages::sdsm::sensor_data_sharing_msg create_sdsm();


        public:
            sds_service() = default;

            ~sds_service();
            
            /**
             * @brief Method to initialize the sds_service.
             * 
             * @return true if successful.
             * @return false if not successful.
             */
            bool initialize() override;

            /**
             * @brief Method to start all threads included in the tsc_service.
             */
            void start() override;

            FRIEND_TEST(sensorDataSharingServiceTest, consumeDetections);
            FRIEND_TEST(sensorDataSharingServiceTest, produceSdsms);
            FRIEND_TEST(sensorDataSharingServiceTest, readLanelet2Map);
    };
}