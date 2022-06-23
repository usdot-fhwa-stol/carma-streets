#include "signal_opt_service.h"

namespace signal_opt_service
{
    bool signal_opt_service::initialize()
    {
        try
        {
            // Kafka config
            auto client = std::make_unique<kafka_clients::kafka_client>();
            this->bootstrap_server = streets_service::streets_configuration::get_string_config("bootstrap_server");
            this->spat_topic_name = streets_service::streets_configuration::get_string_config("spat_consumer_topic");
            this->spat_group_id = streets_service::streets_configuration::get_string_config("spat_group_id");
            this->vsi_topic_name = streets_service::streets_configuration::get_string_config("vsi_consumer_topic");
            this->vsi_group_id = streets_service::streets_configuration::get_string_config("vsi_group_id");

            _spat_consumer = client->create_consumer(this->bootstrap_server, this->spat_topic_name, this->spat_group_id);
            _vsi_consumer = client->create_consumer(this->bootstrap_server, this->vsi_topic_name, this->vsi_group_id);

            if (!_spat_consumer->init() || !_vsi_consumer->init())
            {
                SPDLOG_CRITICAL("kafka consumers (_spat_consumer_worker  or _vsi_consumer_worker) initialize error");
                exit(-1);
            }
            else
            {
                _spat_consumer->subscribe();
                _vsi_consumer->subscribe();
                if (!_spat_consumer->is_running() || !_vsi_consumer->is_running())
                {
                    SPDLOG_CRITICAL("kafka consumers (_spat_consumer_worker or _vsi_consumer_worker) is not running");
                    exit(-1);
                }
            }

            // Serice config
            auto sleep_millisecs = std::stoul(streets_service::streets_configuration::get_string_config("sleep_millisecs"));
            auto int_client_request_attempts = std::stoul(streets_service::streets_configuration::get_string_config("int_client_request_attempts"));
            so_msgs_worker_ptr = std::make_shared<signal_opt_messages_worker>();

            // HTTP request to update intersection information
            if (!update_intersection_info(sleep_millisecs, int_client_request_attempts))
            {
                return false;
            }
            SPDLOG_INFO("signal_opt_service initialized successfully!!!");
            return true;
        }
        catch (const streets_service::streets_configuration_exception &ex)
        {
            SPDLOG_ERROR("Signal Optimization Service Initialization failure: {0} ", ex.what());
            return false;
        }
    }

    void signal_opt_service::start() const
    {
        std::thread spat_t(&signal_opt_service::consume_msg, this, std::ref(this->_spat_consumer), CONSUME_MSG_TYPE::SPAT);
        std::thread vsi_t(&signal_opt_service::consume_msg, this, std::ref(this->_vsi_consumer), CONSUME_MSG_TYPE::VEHICLE_STATUS_INTENT);
        spat_t.join();
        vsi_t.join();
    }

    void signal_opt_service::consume_msg(std::shared_ptr<kafka_clients::kafka_consumer_worker> consumer, CONSUME_MSG_TYPE consume_msg_type) const
    {
        while (consumer->is_running())
        {
            const std::string payload = consumer->consume(1000);
            if (payload.length() != 0)
            {
                if (!this->so_msgs_worker_ptr)
                {
                    SPDLOG_CRITICAL("Message worker is not initialized");
                    break;
                }

                switch (consume_msg_type)
                {
                case CONSUME_MSG_TYPE::SPAT:
                    this->so_msgs_worker_ptr->update_spat(payload);
                    break;
                case CONSUME_MSG_TYPE::VEHICLE_STATUS_INTENT:
                    this->so_msgs_worker_ptr->add_update_vehicle(payload);
                    break;
                default:
                    SPDLOG_ERROR("Unknown consumer type!");
                    break;
                }
            }
        }
    }

    bool signal_opt_service::update_intersection_info(unsigned long sleep_millisecs, unsigned long int_client_request_attempts) const
    {
        unsigned int sleep_secs = static_cast<unsigned int>(sleep_millisecs / 1000);
        SPDLOG_INFO("Send client request to update intersection inforamtion every {0} for {1} times.", sleep_secs, int_client_request_attempts);
        int attempt_count = 0;
        while (attempt_count < int_client_request_attempts)
        {
            // Send HTTP request, and update intersection information. If updated successfully, it returns true and exit the while loop.
            if (this->so_msgs_worker_ptr && this->so_msgs_worker_ptr->request_intersection_info())
            {
                SPDLOG_INFO("Intersection information is updated with valid signal group ids! ");
                return true;
            }
            sleep(sleep_secs);
            attempt_count++;
        }
        // If failed to update the intersection information after certain numbers of attempts
        SPDLOG_ERROR("Updating Intersection information failed. ");
        return false;
    }

    signal_opt_service::~signal_opt_service()
    {
        if (_spat_consumer)
        {
            _spat_consumer->stop();
        }

        if (_vsi_consumer)
        {
            _vsi_consumer->stop();
        }
    }
}