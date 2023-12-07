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
#include "sensor_data_sharing_service.hpp"

namespace sensor_data_sharing_service {
    
    namespace ss = streets_service;
    namespace sdsm = streets_utils::messages::sdsm;
    namespace detected_objects_message = streets_utils::messages::detected_objects_msg;
    sds_service::~sds_service() {

        if (sdsm_producer)
        {
            SPDLOG_WARN("Stopping SDSM producer!");
            sdsm_producer->stop();
        }

        if(detection_consumer)
        {
            SPDLOG_WARN("Stopping Detection consumer!");
            detection_consumer->stop();
        }
    }

    bool sds_service::initialize() {
        if (!streets_service::initialize() ) {
            return false;
        }
        if (!read_lanelet_map()){
            return false;
        }

        SPDLOG_DEBUG("Intializing Sensor Data Sharing Service");

        // Initialize SDSM Kafka producer
        const std::string sdsm_topic = ss::streets_configuration::get_string_config("sdsm_producer_topic");
        const std::string detection_topic = ss::streets_configuration::get_string_config("detection_consumer_topic");
        const std::string sdsm_geo_reference = ss::streets_configuration::get_string_config("sdsm_geo_reference");
        // Get Infrastructure ID for SDSM messages
        this->_infrastructure_id =  streets_service::get_system_config("INFRASTRUCTURE_ID", "");
        return initialize_kafka_producer(sdsm_topic, sdsm_producer) && initialize_kafka_consumer(detection_topic, detection_consumer);
    }

    bool sds_service::read_lanelet_map() {
        const auto filename = streets_service::get_system_config("LANELET2_MAP", "/home/carma-streets/MAP/Intersection.osm");

        try
        {
            int projector_type = 1;
            std::string target_frame;
            lanelet::ErrorMessages errors;

            // Parse geo reference info from the lanelet map (.osm)
            lanelet::io_handlers::AutowareOsmParser::parseMapParams(filename, &projector_type, &target_frame);
            this->map_projector = std::make_unique<lanelet::projection::LocalFrameProjector>(target_frame.c_str());
            this->map_ptr = lanelet::load(filename, *map_projector.get(), &errors);
            // 
            auto pose = parse_sensor_location( streets_service::get_system_config("SENSOR_JSON_FILE_PATH", "/home/carma-streets/sensor_configurations/sensors.json"), ss::streets_configuration::get_string_config("sensor_id"));
            this->sdsm_reference_point =  this->map_projector->reverse(pose);
            if (!this->map_ptr->empty())
            {
                return true;
            }
        }
        catch (const std::exception &ex)
        {
            SPDLOG_ERROR("Cannot read osm file {0}. Error message: {1} ", filename, ex.what());
        }
        return false;
    }

    void sds_service::consume_detections(){
        if ( !detection_consumer ) {
            throw std::runtime_error("Detection consumer is null!");
        }
        SPDLOG_DEBUG("Attempting to consume detections ...");
        detection_consumer->subscribe();
        while ( detection_consumer->is_running() ) {
            try{
                const std::string payload = detection_consumer->consume(1000);
                if (payload.length() != 0)
                {
                    auto detected_object = streets_utils::messages::detected_objects_msg::from_json(payload);
                    // Write Lock
                    std::unique_lock lock(detected_objects_lock);
                    detected_objects[detected_object._object_id] = detected_object;
                    SPDLOG_DEBUG("Detected Object List Size {0} after consumed: {1}", detected_objects.size(), payload);
                    
                }
            }
            catch (const streets_utils::json_utils::json_parse_exception &e) {
                SPDLOG_ERROR("Exception occured consuming detection message : {0}", e.what());
            }
        }
        SPDLOG_ERROR("Something went wrong, no longer consuming detections." );

    }

    void sds_service::produce_sdsms() {
        if ( !sdsm_producer )  {
            throw std::runtime_error("SDSM consumer is null!");
        }
        SPDLOG_INFO("Starting SDSM Producer!");
        while ( sdsm_producer->is_running() ) {
            try{
                if ( !detected_objects.empty() ) {
                    streets_utils::messages::sdsm::sensor_data_sharing_msg msg = create_sdsm();
                    const std::string json_msg = streets_utils::messages::sdsm::to_json(msg);
                    SPDLOG_DEBUG("Sending SDSM : {0}", json_msg);
                    sdsm_producer->send(json_msg);
                    this->_message_count++;
                    // Clear detected object
                    detected_objects.clear();
                }
            }
            catch( const streets_utils::json_utils::json_parse_exception &e) {
                SPDLOG_ERROR("Exception occurred producing SDSM : {0}", e.what());
            }         
            ss::streets_clock_singleton::sleep_for(1000); // Sleep for 10 second between publish  
        }
       
    }

    streets_utils::messages::sdsm::sensor_data_sharing_msg sds_service::create_sdsm() {
        streets_utils::messages::sdsm::sensor_data_sharing_msg msg;
        // Read lock
        uint64_t timestamp = ss::streets_clock_singleton::time_in_ms();
        msg._time_stamp = to_sdsm_timestamp(timestamp);
        // Populate with rolling counter
        msg._msg_count = this->_message_count;
        // Populate with infrastructure id
        msg._source_id = this->_infrastructure_id;
        msg._equipment_type = sdsm::equipment_type::RSU;
        std::shared_lock lock(detected_objects_lock);
        for (const auto &[object_id, object] : detected_objects){
            auto detected_object_data = to_detected_object_data(object);
            // TODO: Update time offset. Currently CARMA-Streets detected object message does not support timestamp
            // This is a bug and needs to be addressed.
            msg._objects.push_back(detected_object_data);
        }
        return msg;
    }

    
    void sds_service::start() {
        SPDLOG_DEBUG("Starting Sensor Data Sharing Service");
        streets_service::start();
        std::thread detection_thread(&sds_service::consume_detections, this);
        std::thread sdsm_thread(&sds_service::produce_sdsms, this);
        detection_thread.join();
        sdsm_thread.join();
    }


}