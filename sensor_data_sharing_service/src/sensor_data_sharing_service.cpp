#include "sensor_data_sharing_service.hpp"

namespace sensor_data_sharing_service {
    using namespace streets_service;
    using namespace streets_utils::messages::sdsm;
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
        SPDLOG_DEBUG("Intializing Sensor Data Sharing Service");
        if (!streets_service::initialize()) {
            return false;
        }
        // Initialize SDSM Kafka producer
        std::string sdsm_topic = streets_configuration::get_string_config("sdsm_producer_topic");
        if (!initialize_kafka_producer(sdsm_topic, sdsm_producer)) {
            return false;
        }
        // Initialize
        std::string detection_topic = streets_configuration::get_string_config("detection_consumer_topic");
        if (!initialize_kafka_consumer(detection_topic, detection_consumer)) {
          return false; 
        }
        return true;
    }

    void sds_service::consume_detections(){
        try{
            while ( detection_consumer && detection_consumer->is_running() ) {
                const std::string payload = detection_consumer->consume(1000);
                if (payload.length() != 0)
                {
                    SPDLOG_DEBUG("Consumed: {0}", payload);
                }
            }
        }
        catch (const std::runtime_error &e) {
            SPDLOG_ERROR("Exception occured consuming detection message : {0}", e.what());
        }

    }

    void sds_service::produce_sdsms() {
        try{
            SPDLOG_INFO("Starting SDSM Producer!");
            while ( sdsm_producer && sdsm_producer->is_running() ) {
                sensor_data_sharing_msg msg;
                msg._equipment_type = equipment_type::RSU;
                msg._msg_count = 255;
                msg._source_id = "00000001";
                msg._ref_positon._latitude=900000001;
                msg._ref_positon._longitude=1800000001;
                msg._time_stamp.second = 65535;
                msg._time_stamp.minute = 60;
                msg._time_stamp.hour= 31;
                msg._time_stamp.day = 31;
                msg._time_stamp.month = 12;
                msg._time_stamp.year = 4095; // Max
                msg._ref_position_confidence._semi_major_axis_accuracy = 255;
                msg._ref_position_confidence._semi_minor_axis_accuracy = 255;
                msg._ref_position_confidence._semi_major_axis_orientation = 65535;
                // Add Detected Object
                detected_object_data detected_object;
                detected_object._detected_object_common_data._object_id = 65525;
                detected_object._detected_object_common_data._object_type = object_type::ANIMAL;
                detected_object._detected_object_common_data._classification_confidence = 101;
                detected_object._detected_object_common_data._heading = 28800;
                detected_object._detected_object_common_data._heading_confidence = heading_confidence::PREC_01_deg;
                detected_object._detected_object_common_data._position_offset._offset_x = 32767;
                detected_object._detected_object_common_data._position_offset._offset_y = 32767;
                detected_object._detected_object_common_data._position_offset._offset_z = 32767;
                detected_object._detected_object_common_data._pos_confidence._position_confidence = position_confidence::A_10CM;
                detected_object._detected_object_common_data._pos_confidence._elevation_confidence =  position_confidence::A_1M;
                detected_object._detected_object_common_data._speed = 8191;
                detected_object._detected_object_common_data._time_measurement_offset = 1500;
                detected_object._detected_object_common_data._time_confidence = time_confidence::TIME_000_000_002;
                msg._objects.push_back(detected_object);
                const std::string json_msg = to_json(msg);
                SPDLOG_DEBUG("Sending SDSM : {0}", json_msg);
                sdsm_producer->send(json_msg);           
                streets_clock_singleton::sleep_for(1000); // Sleep for 10 second between publish  
            }
        }
        catch( const std::runtime_error &e) {
            SPDLOG_ERROR("Exception occurred producing SDSM : {0}", e.what());
        }
    }

    
    void sds_service::start() {
        SPDLOG_DEBUG("Starting Sensor Data Sharing Service");
        streets_service::streets_service::start();
        std::thread detection_thread(&sds_service::consume_detections, this);
        std::thread sdsm_thread(&sds_service::produce_sdsms, this);
        detection_thread.join();
        sdsm_thread.join();
    }
}