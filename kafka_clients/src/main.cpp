#include "kafka_client.h"

void call_consumer_thread()
{
    auto client = std::make_shared<kafka_clients::kafka_client>();
    std::string file_name = "../manifest.json";
    rapidjson::Document doc_json = client->read_json_file(file_name);
    std::string bootstrap_server = client->get_value_by_doc(doc_json, "BOOTSTRAP_SERVER");
    std::string group_id = client->get_value_by_doc(doc_json, "GROUP_ID");
    std::string topic = client->get_value_by_doc(doc_json, "CONSUMER_TOPIC");
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
            const char *paylod = consumer_worker->consume(1000);
            if (strlen(paylod) > 0)
            {
                spdlog::info("message payload: {0}", paylod);
            }
        }

        consumer_worker->stop();
    }
    return;
}

void call_producer_thread()
{
    auto client = std::make_shared<kafka_clients::kafka_client>();
    std::string file_name = "../manifest.json";
    rapidjson::Document doc_json = client->read_json_file(file_name);
    std::string bootstrap_server = client->get_value_by_doc(doc_json, "BOOTSTRAP_SERVER");
    std::string topic = client->get_value_by_doc(doc_json, "PRODUCER_TOPIC");
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
    boost::thread t_producer{call_producer_thread};
    boost::thread t_consumer{call_consumer_thread};
    t_producer.join();
    return 0;
}