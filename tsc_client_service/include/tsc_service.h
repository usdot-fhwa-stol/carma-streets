#pragma once 

#include "kafka_client.h"
#include "spat_worker.h"
#include "streets_configuration.h"
#include "streets_configuration_exception.h"
#include "monitor_tsc_state.h"
#include "snmp_client.h"
#include "spat.h"
#include "udp_socket_listener.h"
#include "intersection_client.h"
#include "ntcip_oids.h"

namespace traffic_signal_controller_service {

    class tsc_service {
        private:
            /**
             * @brief kafka broker host
             */
            std::string _bootstrap_server;
            /**
             * @brief Kafka spat topic group id
             */
            std::string _spat_group_id;
            /**
             * @brief kafka spat topic name
             */
            std::string _spat_topic_name;
            /**
             * @brief Kafka producer for spat JSON
             */
            std::shared_ptr<kafka_clients::kafka_producer_worker> spat_producer;
            std::shared_ptr<spat_worker> spat_worker_ptr;
            std::shared_ptr<tsc_state> tsc_state_ptr;
            std::shared_ptr<snmp_client> snmp_client_ptr;
            std::shared_ptr<signal_phase_and_timing::spat> spat_ptr;
            std::shared_ptr<intersection_client> intersection_client_ptr;
        public:
            tsc_service() = default;

            ~tsc_service();

            bool initialize();

            void start() ;

            void produce_spat_json() const;

    };
}