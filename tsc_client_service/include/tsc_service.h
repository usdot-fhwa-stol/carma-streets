#pragma once 

#include "kafka_client.h"
#include "spat_worker.h"
#include "streets_configuration.h"
#include "streets_configuration_exception.h"

namespace traffic_signal_controller_service {

    class tsc_service {
        private:
            std::string _bootstrap_server;
            std::string _spat_group_id;
            std::string _spat_topic_name;
            std::shared_ptr<kafka_clients::kafka_producer_worker> spat_producer;
            std::shared_ptr<spat_worker> spat_worker_ptr;
        public:
            tsc_service() = default;

            ~tsc_service();

            bool initialize();

            void start() const;

            void produce_spat_json();

    };
}