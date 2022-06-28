#include "signal_opt_service.h"

namespace signal_opt_service
{
    bool signal_opt_service::initialize()
    {
        try
        {
            _so_msgs_worker_ptr = std::make_shared<signal_opt_messages_worker>();

            // Kafka config
            auto client = std::make_unique<kafka_clients::kafka_client>();
            _bootstrap_server = streets_service::streets_configuration::get_string_config("bootstrap_server");
            _spat_topic_name = streets_service::streets_configuration::get_string_config("spat_consumer_topic");
            _spat_group_id = streets_service::streets_configuration::get_string_config("spat_group_id");
            _vsi_topic_name = streets_service::streets_configuration::get_string_config("vsi_consumer_topic");
            _vsi_group_id = streets_service::streets_configuration::get_string_config("vsi_group_id");

            _spat_consumer = client->create_consumer(_bootstrap_server, _spat_topic_name, _spat_group_id);
            _vsi_consumer = client->create_consumer(_bootstrap_server, _vsi_topic_name, _vsi_group_id);

            if (!_spat_consumer->init() || !_vsi_consumer->init())
            {
                SPDLOG_CRITICAL("kafka consumers ( _spat_consumer_worker  or _vsi_consumer_worker) initialize error");
                exit(EXIT_FAILURE);
            }
            else
            {
                _spat_consumer->subscribe();
                _vsi_consumer->subscribe();
                if (!_spat_consumer->is_running() || !_vsi_consumer->is_running())
                {
                    SPDLOG_CRITICAL("kafka consumers ( _spat_consumer_worker or _vsi_consumer_worker) is not running");
                    exit(EXIT_FAILURE);
                }
            }

            // Serice config
            auto sleep_millisecs = std::stoul(streets_service::streets_configuration::get_string_config("sleep_millisecs"));
            auto int_client_request_attempts = std::stoul(streets_service::streets_configuration::get_string_config("int_client_request_attempts"));            

            // HTTP request to update intersection information
            if (!update_intersection_info(sleep_millisecs, int_client_request_attempts))
            {
                SPDLOG_CRITICAL("Failed to initialize signal_opt_service due to intersection client request failure!");
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
        std::thread spat_t(&signal_opt_service::consume_msg, this, std::ref(_spat_consumer), CONSUME_MSG_TYPE::SPAT);
        std::thread vsi_t(&signal_opt_service::consume_msg, this, std::ref(_vsi_consumer), CONSUME_MSG_TYPE::VEHICLE_STATUS_INTENT);
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
                SPDLOG_DEBUG("Consumed: {0}", payload);
                if (!_so_msgs_worker_ptr)
                {
                    SPDLOG_CRITICAL("Message worker is not initialized");
                    break;
                }

                switch (consume_msg_type)
                {
                case CONSUME_MSG_TYPE::SPAT:
                    _so_msgs_worker_ptr->update_spat(payload);
                    if (!_so_msgs_worker_ptr->update_spat(payload))
                    {
                        SPDLOG_CRITICAL("Error occurred when updating SPAT.");                       
                    }
                    break;
                case CONSUME_MSG_TYPE::VEHICLE_STATUS_INTENT:
                    if (!_so_msgs_worker_ptr->add_update_vehicle(payload))
                    {
                        SPDLOG_CRITICAL("Error occurred when updating vehicle list.");
                    }
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
        auto sleep_secs = static_cast<unsigned int>(sleep_millisecs / 1000);
        SPDLOG_INFO("Send client request to update intersection inforamtion every {0} seconds for maximum {1} times.", sleep_secs, int_client_request_attempts);
        int attempt_count = 0;
        while (attempt_count < int_client_request_attempts)
        {
            // Send HTTP request, and update intersection information. If updated successfully, it returns true and exit the while loop.
            if (_so_msgs_worker_ptr && _so_msgs_worker_ptr->request_intersection_info())
            {
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