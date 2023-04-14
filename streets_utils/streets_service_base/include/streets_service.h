#pragma once 

#include "streets_configuration.h"
#include "kafka_client.h"
#include "streets_clock_singleton.h"
#include "time_sync_message.h"
#include <gtest/gtest.h>


namespace streets_service { 

    class streets_service {
        FRIEND_TEST(test_service_test,test_tsc_control);

        public:
            streets_service() = default;

            ~streets_service();

            virtual bool initialize();

            virtual void start();

        protected:
            
            std::string _service_name;

            bool _simulation_mode;

            std::shared_ptr<kafka_clients::kafka_consumer_worker> _time_consumer;

            bool initialize_kafka_producer( const std::string &producer_topic, std::shared_ptr<kafka_clients::kafka_producer_worker> &producer);

            bool initialize_kafka_consumer( const std::string &consumer_topic, 
                                            std::shared_ptr<kafka_clients::kafka_consumer_worker> &consumer );

            std::string get_system_config(const char *config_name ) const;

            void consume_time_sync_message();



    };  
}