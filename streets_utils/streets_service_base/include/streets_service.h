#pragma once 

#include "streets_configuration.h"
#include "kafka_client.h"


namespace streets_service { 
    class streets_service {
        public:

            virtual bool initialize();

            virtual void start();

        protected:
            
            std::string _service_name;

            bool _simulation_mode;

            bool initialize_kafka_producer( const std::string &topic_name, std::shared_ptr<kafka_clients::kafka_producer_worker> producer );

            bool initialize_kafka_producer( const std::string &topic_name, std::shared_ptr<kafka_clients:kafka_consumer_worker> consumer );


    }   
}