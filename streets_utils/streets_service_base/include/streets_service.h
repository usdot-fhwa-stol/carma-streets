#pragma once 

#include "streets_configuration.h"
#include "kafka_client.h"
#include "streets_clock_singleton.h"
#include "time_sync_message.h"
#include "streets_environment_variables.h"
#include <gtest/gtest.h>


namespace streets_service { 
    /** 
     * @brief Streets Service is a base class for CARMA-Streets services. It initializes the streets_clock_singleton 
     * is a maintains a time wrapper object that allows services to configurably use system time or time sync messages
     * as a source for time. This allows CARMA-Streets services that extend this class use CDASim as a source for time.
     * This class also initializes the streets_configuration singleton using a file path provided by the CONFIG_FILE_PATH
     * environment variable. 
     * 
     * @author Paul Bourelly
    */
    class streets_service {
        public:
            /**
             * @brief Constructor.
             */
            streets_service() = default;
            /**
             * @brief Destructor stop time_consumer kafka producer if not null.
            */
            ~streets_service();
            // Remove copy constructor 
            streets_service(const streets_service &) = delete;
            // Remove move constructor
            streets_service(streets_service&& ) = delete;
            // Remove copy assignment operator
            streets_service& operator=(const streets_service &) = delete;
            // Remove move assignment operator
            streets_service& operator=(const streets_service &&) = delete;

            /**
             * @brief Method to initialize services members, singletons, kafka consumers and producers and any other 
             * connections required for service. Classes that extend this streets_service need to override this method
             * and call the streets_service::initialize() in the override method.
             * 
             * @return bool depending on whether initialize method completes successfully.
             */
            virtual bool initialize();
            /**
             * @brief Method to start all threads needed for the Streets Service. This includes but is not limited to any 
             * threads to consume/produce message to kafka, or poll internal/external resources. Classes that extend this 
             * streets_service need to override this method and call the streets_service::start() in the override method.
             */
            virtual void start();

        protected:
            /**
             * @brief Helper method to initialize Kafka producer for a given topic.
             * 
             * @param producer_topic topic name.
             * @param producer shared_ptr to kafka producer.
             * @return true if initialization is successful and false if initialization fails.
             */
            bool initialize_kafka_producer( const std::string &producer_topic, std::shared_ptr<kafka_clients::kafka_producer_worker> &producer );
            /**
             * @brief Helper method to initialise Kafka consumer. NOTE: Will assign consumer group id as service_name.
             * 
             * @param consumer_topic topic name.
             * @param consumer shared_ptr to kafka consumer.
             * @return true if initialization is successful and false if initialization fails.
             */
            bool initialize_kafka_consumer( const std::string &consumer_topic, std::shared_ptr<kafka_clients::kafka_consumer_worker> &consumer );
            /**
             * @brief Returns string value of environment variable with given name. If no value is found, returns default.
             * @param config_name name of environment variable.
             * @param default_val string default value for environment variable if no value is found.
             * @throws runtime_error if config_name is nullptr or environment variable is not set.
             * @return value of environment variable.
             */
            std::string get_system_config(const char *config_name , const std::string &default_val) const noexcept;
            /**
             * @brief Method to consume continously consume time sync messages from Kafka topic and update 
             * streets_clock_singleton with received data.
             */
            void consume_time_sync_message() const;
            /**
             * @brief Returns service name set in configuration file.
             * @return Returns service name set in configuration file.
             */
            std::string get_service_name() const;
            /**
             * @brief Returns boolean if service is currently configured to be in simulation_mode. This corresponsds
             * to the value of the environment variable SIMULATION_MODE. Simulation mode indicates whether the streets_clock_singleton
             * uses simulation time for kafka or system time.
             * @return true if in simulation mode, false if in real time more
             */
            bool is_simulation_mode() const;
            /**
             * @brief Method to create SPDLOG Daily rotating file logger. The logger is accessible by calling spdlog::get(name). The log
             * files created by this logger will include data and time in name and will be stored in the LOGS_DIRECTORY set path.
             * @param name Name of the logger 
             * @param extension file extension for log files created
             * @param pattern pattern for output of log statments. Please review SPDLOG documentation 
             * (https://github.com/gabime/spdlog/wiki/3.-Custom-formatting#customizing-format-using-set_pattern).
             * @param level log level to set for logger. Can be dynamically changed by getting logger and changing log level.
             */
            std::shared_ptr<spdlog::logger> create_daily_logger(const std::string &name, const std::string &extension = ".log", const std::string &pattern = "[%Y-%m-%d %H:%M:%S.%e] %v", 
                                const spdlog::level::level_enum &level = spdlog::level::info ) const;
            
            /**
             * @brief Method used to set kafka_client member used to create kafka producers and consumers. For unit testing purposes can
             * allow injection of mocks by setting a kafka_client mock and telling it to return mock producers/consumers on calls to
             * create_consumer() create_producer.
             * @param kafka_client 
             */
            void set_kafka_client(const std::shared_ptr<kafka_clients::kafka_client> kafka_client);

        private:
            std::string _service_name;

            bool _simulation_mode;

            std::shared_ptr<kafka_clients::kafka_client> _kafka_client;

            std::shared_ptr<kafka_clients::kafka_consumer_worker> _time_consumer;

            std::string _logs_directory;

            FRIEND_TEST(test_streets_service, test_consume_time_sync_message);
            FRIEND_TEST(test_streets_service, test_initialize_consumer);
            FRIEND_TEST(test_streets_service, test_initialize_producer);
            FRIEND_TEST(test_streets_service, test_initialize_consumer_fail);
            FRIEND_TEST(test_streets_service, test_initialize_producer_fail);
            FRIEND_TEST(test_streets_service, test_initialize_sim);
            FRIEND_TEST(test_streets_service, test_initialize_sim_fail);
            FRIEND_TEST(test_streets_service, test_get_system_config);
            FRIEND_TEST(test_streets_service, test_create_daily_logger);
            FRIEND_TEST(test_streets_service, test_create_daily_logger_default);
            FRIEND_TEST(test_streets_service, test_start);



            

            


    };  
}