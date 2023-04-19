#pragma once 

#include "streets_configuration.h"
#include "kafka_client.h"
#include "streets_clock_singleton.h"
#include "time_sync_message.h"
#include <gtest/gtest.h>


namespace streets_service { 
    /** 
     * @brief Streets Service is a base class for CARMA-Streets services. It initializes the streets_clock_singleton 
     * is a maintains a time wrapper object that allows services to configurably use system time or time sync messages
     * as a source for time. This allows CARMA-Streets services that extend this class use CDASim as a source for time.
     * 
     * @author Paul Bourelly
    */
    class streets_service {
        public:
            streets_service() = default;

            ~streets_service();
            // Remove copy constructor 
            streets_service(const streets_service &) = delete;
            // Remove move constructor
            streets_service(streets_service&& ) = delete;
            // Remove copy assignment operator
            streets_service& operator=(const streets_service &) = delete;
            // Remove move assignment operator
            streets_service& operator=(const streets_service &&) = delete;

            virtual bool initialize();

            virtual void start();

        protected:

            bool initialize_kafka_producer( const std::string &producer_topic, std::shared_ptr<kafka_clients::kafka_producer_worker> &producer ) const;

            bool initialize_kafka_consumer( const std::string &consumer_topic, std::shared_ptr<kafka_clients::kafka_consumer_worker> &consumer ) const;

            std::string get_system_config(const char *config_name ) const;

            void consume_time_sync_message() const;

            std::string get_service_name() const;

            bool is_simulation_mode() const;

        private:
            std::string _service_name;

            bool _simulation_mode;

            std::shared_ptr<kafka_clients::kafka_consumer_worker> _time_consumer;

            FRIEND_TEST(test_streets_service, test_consume_time_sync_message);
            FRIEND_TEST(test_streets_service, test_initialize_consumer);
            FRIEND_TEST(test_streets_service, test_initialize_producer);
            FRIEND_TEST(test_streets_service, test_initialize_sim);
            FRIEND_TEST(test_streets_service, test_get_system_config);

            

            


    };  
}