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
            SPDLOG_ERROR("Failed to initialize streets service base!");
            return false;
        }
        SPDLOG_DEBUG("Intializing Sensor Data Sharing Service");
       
        // Read sensor configuration file and get WSG84 location/origin reference frame.
        const std::string sensor_config_file = streets_service::get_system_config("SENSOR_JSON_FILE_PATH", "/home/carma-streets/sensor_configurations/sensors.json");
        const std::string sensor_id = ss::streets_configuration::get_string_config("sensor_id");
        auto sensor_ref = parse_sensor_ref(sensor_config_file, sensor_id);
        if ( sensor_ref.reference_type == LocationDataType::CARTESIAN ) {
            SPDLOG_DEBUG("Reading CARTESIAN sensor location offset from lanelet2 osm map.");
            if (!streets_service::is_simulation_mode())
            {
                SPDLOG_WARN("CARTESIAN sensor location should only be used for simulation. Please use WGS84 location data!");
            }
            const std::string lanlet2_map =  streets_service::get_system_config("LANELET2_MAP", "/home/carma-streets/MAP/Intersection.osm");
            if (!read_lanelet_map(lanlet2_map)){
                SPDLOG_ERROR("Failed to read lanlet2 map {0} !", lanlet2_map);
                return false;
            }
            this->sdsm_reference_point =  this->map_projector->reverse(sensor_ref.cartesian_location);

        } else {
            this->sdsm_reference_point = sensor_ref.wgs84_location;
        }

        // Initialize SDSM Kafka producer
        const std::string sdsm_topic = ss::streets_configuration::get_string_config("sdsm_producer_topic");
        const std::string detection_topic = ss::streets_configuration::get_string_config("detection_consumer_topic");
        // Get Infrastructure ID for SDSM messages
        this->_infrastructure_id =  streets_service::get_system_config("INFRASTRUCTURE_ID", "");
        return initialize_kafka_producer(sdsm_topic, sdsm_producer) && initialize_kafka_consumer(detection_topic, detection_consumer);
    }

    bool sds_service::read_lanelet_map(const std::string &filepath) {

        try
        {
            int projector_type = 1;
            std::string target_frame;
            lanelet::ErrorMessages errors;
            // Parse geo reference info from the lanelet map (.osm)
            lanelet::io_handlers::AutowareOsmParser::parseMapParams(filepath, &projector_type, &target_frame);
            this->map_projector = std::make_unique<lanelet::projection::LocalFrameProjector>(target_frame.c_str());
            this->map_ptr = lanelet::load(filepath, *map_projector.get(), &errors);
            // 
            
            if (!this->map_ptr->empty())
            {
                return true;
            }
        }
        catch (const lanelet::ParseError &ex)
        {
            SPDLOG_ERROR("Cannot read osm file {0}. Error message: {1} .", filepath, ex.what());
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
                    // Get delay of detected object
                    auto delay = static_cast<int64_t>(ss::streets_clock_singleton::time_in_ms()) - static_cast<int64_t>(detected_object._timestamp);
                    SPDLOG_DEBUG("Detection Delay : {0}ms!", delay);
                    // if delay is greater than 500 ms skip detection to get more recent data
                    if ( delay >= 500 ) {
                        SPDLOG_WARN("Skipping incoming detection at {0}ms is not current or has invalid timestamp of {1}ms!" , ss::streets_clock_singleton::time_in_ms(), detected_object._timestamp );
                        continue;
                    }
                    // if delay is negative, and service is in simulation mode
                    // the detection message was processed before time sync message. Wait on time sync message
                    else if (is_simulation_mode() && delay < 0 ) {
                        SPDLOG_WARN("Current sim time {0} waiting for sim time {1}ms from detection ...",ss::streets_clock_singleton::time_in_ms(), detected_object._timestamp );
                        ss::streets_clock_singleton::sleep_for(abs(delay));
                    }
                    // If delay is negative and service is not in simulation mode
                    // indicates sensor and service are not time sychronized.
                    else if( delay < 0 ) {
                        SPDLOG_WARN(
                            R"(Skipping incoming detection. Current time is {0}ms and detection time stamp is {1}ms. 
                                Sensor Data Sharing Service and sensor producing detections to not appear to be time synchronized.)",
                            ss::streets_clock_singleton::time_in_ms(), 
                            detected_object._timestamp );
                        continue;

                    }

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
                    // Message Count max is 127, reset after max value
                    if ( this->_message_count <= 127) {
                        this->_message_count++;
                    }else {
                        this->_message_count = 0;
                    }
                    // Write Lock 
                    std::unique_lock lock(detected_objects_lock);
                    // Clear detected object
                    detected_objects.clear();
                }
            }
            catch( const streets_utils::json_utils::json_parse_exception &e) {
                SPDLOG_ERROR("Exception occurred producing SDSM : {0}", e.what());
            }         
            ss::streets_clock_singleton::sleep_for(100); // Sleep for 100 ms between publish  
        }
        SPDLOG_CRITICAL("SDSM Producers no longer running.");
       
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
        // Populate equipement type
        msg._equipment_type = sdsm::equipment_type::RSU;
        // Polulate ref position
        msg._ref_positon = to_position_3d(this->sdsm_reference_point);
        std::shared_lock lock(detected_objects_lock);
        for (const auto &[object_id, object] : detected_objects){
            auto ned_object = detected_object_enu_to_ned(object);
            auto detected_object_data = to_detected_object_data(ned_object,timestamp);
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

    streets_utils::messages::sdsm::position_3d to_position_3d(const lanelet::GPSPoint &ref_position) {
        streets_utils::messages::sdsm::position_3d position;
        // Convert to 1/10 of microdegrees
        position._longitude = static_cast<int>(ref_position.lon * 1e7);
        position._latitude = static_cast<int>(ref_position.lat *1e7);
        // Convert 0.1 meters
        position._elevation = static_cast<int>(ref_position.ele * 10);
        return position;

    }
}