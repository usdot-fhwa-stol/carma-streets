#include "sensor_data_sharing_service.hpp"

namespace sensor_data_sharing_service {
    using namespace streets_service;
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

    
    void sds_service::start() {
        SPDLOG_DEBUG("Starting Sensor Data Sharing Service");
        streets_service::streets_service::start();

    }
}