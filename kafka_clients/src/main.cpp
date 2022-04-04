#include "kafka_client.h"
#include "streets_configuration.h"

void call_consumer_thread()
{
    auto client = std::make_shared<kafka_clients::kafka_client>();
    std::string bootstrap_server = streets_service::streets_configuration::get_string_config( "BOOTSTRAP_SERVER");
    std::string group_id = streets_service::streets_configuration::get_string_config("GROUP_ID");
    std::string topic = streets_service::streets_configuration::get_string_config("CONSUMER_TOPIC");
    auto consumer_worker = client->create_consumer(bootstrap_server, topic, group_id);
    if (!consumer_worker->init())
    {
        spdlog::critical("kafka consumer initialize error");
    }
    else
    {
        consumer_worker->subscribe();
        if (!consumer_worker->is_running())
        {
            spdlog::critical("consumer_worker is not running");
        }

        while (consumer_worker->is_running())
        {
            const std::string payload = consumer_worker->consume(1000);
            if(payload.length() > 0)
            {
                spdlog::info("message payload: {0}", payload);
            }
        }

        consumer_worker->stop();
    }
    return;
}

void call_producer_thread()
{
    auto client = std::make_shared<kafka_clients::kafka_client>();
    std::string bootstrap_server = streets_service::streets_configuration::get_string_config("BOOTSTRAP_SERVER");
    std::string topic = streets_service::streets_configuration::get_string_config("PRODUCER_TOPIC");
    auto producer_worker = client->create_producer(bootstrap_server, topic);
    if (!producer_worker->init())
    {
        spdlog::critical("kafka producer initialize error");
    }
    else
    {
        spdlog::info("Type message and hit enter to producer message. Exit type \"exit\"");
        for (std::string msg_to_send; std::getline(std::cin, msg_to_send);)
        {
            if (strcmp(msg_to_send.c_str(), "exit") == 0)
            {
                break;
            }
            producer_worker->send(msg_to_send);
        }
        producer_worker->stop();
    }

    return;
}

int main(int argc, char **argv)
{
    streets_service::streets_configuration::initialize_logger();
    boost::thread t_producer{call_producer_thread};
    boost::thread t_consumer{call_consumer_thread};
    t_producer.join();
    return 0;
}