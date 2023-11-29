#include "sensor_data_sharing_service.hpp"

namespace sensor_data_sharing_service {
    
    using namespace streets_service;
    using namespace streets_utils::messages;
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
        if (!streets_service::initialize()) {
            return false;
        }
        SPDLOG_DEBUG("Intializing Sensor Data Sharing Service");

        // Initialize SDSM Kafka producer
        std::string sdsm_topic = streets_configuration::get_string_config("sdsm_producer_topic");
        std::string detection_topic = streets_configuration::get_string_config("detection_consumer_topic");

        return initialize_kafka_producer(sdsm_topic, sdsm_producer) && initialize_kafka_consumer(detection_topic, detection_consumer);
    }

    void sds_service::consume_detections(){
        if ( !detection_consumer ) {
            throw std::runtime_error("Detection consumer is null!");
        }
        SPDLOG_DEBUG("Attempting to consume detections ...");
        // TODO: Move into intialize_kafka_consumer method
        detection_consumer->subscribe();
        while ( detection_consumer->is_running() ) {
            try{
                const std::string payload = detection_consumer->consume(1000);
                if (payload.length() != 0)
                {
                    detected_objects = std::make_unique<streets_utils::messages::detected_objects_msg::detected_objects_msg>(streets_utils::messages::detected_objects_msg::from_json(payload));
                    SPDLOG_DEBUG("Consumed: {0}", payload);
                }
            }
            catch (const streets_utils::json_utils::json_parse_exception &e) {
                SPDLOG_ERROR("Exception occured consuming detection message : {0}", e.what());
            }
        }
        SPDLOG_ERROR("Something went wrong, no longer consuming detections." );

    }

    void sds_service::produce_sdsms() const{
       
        SPDLOG_INFO("Starting SDSM Producer!");
        while ( sdsm_producer && sdsm_producer->is_running() ) {
            try{
                streets_utils::messages::sdsm::sensor_data_sharing_msg msg;
                const std::string json_msg = streets_utils::messages::sdsm::to_json(msg);
                SPDLOG_DEBUG("Sending SDSM : {0}", json_msg);
                sdsm_producer->send(json_msg);  
            }
            catch( const streets_utils::json_utils::json_parse_exception &e) {
                SPDLOG_ERROR("Exception occurred producing SDSM : {0}", e.what());
            }         
            streets_clock_singleton::sleep_for(1000); // Sleep for 10 second between publish  
        }
       
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