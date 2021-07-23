#include "kafka_client.h"
#include <iostream>  
#include <thread>    
#include <chrono>    

void call_consumer_thread()
{
   kafka_clients::kafka_client *client = new kafka_clients::kafka_client();
    std::string file_name= "../manifest.json";
    rapidjson::Document doc_json = client->read_json_file(file_name); 
    std::string bootstrap_server = client->get_value_by_doc(doc_json, "BOOTSTRAP_SERVER");
    std::string group_id = client->get_value_by_doc(doc_json, "GROUP_ID");
    std::string topic = client->get_value_by_doc(doc_json, "CONSUMER_TOPIC");
    kafka_clients::kafka_consumer_worker *consumer_worker = client->create_consumer(bootstrap_server,topic,group_id);
    delete client;
    
    if(!consumer_worker->init())
    {
        spdlog::critical("kafka consumer initialize error");
    }
    else
    {
        consumer_worker->subscribe();
        if(!consumer_worker->is_running())
        {
            spdlog::critical("consumer_worker is not running");
        }
        
        while (consumer_worker->is_running()) 
        {
            const char* paylod= consumer_worker->consume(1000);
            if(strlen(paylod) > 0)
            {
                spdlog::info("Consumed message payload: {0}", paylod );
            }
        }
        
        consumer_worker->stop();
    }     
    free(consumer_worker);
    return;    
}

void call_producer_thread()
{
    kafka_clients::kafka_client *client = new kafka_clients::kafka_client(); 
                  
    std::string file_name="../manifest.json";
    rapidjson::Document doc_json = client->read_json_file(file_name);           
    std::string bootstrap_server =  client->get_value_by_doc(doc_json, "BOOTSTRAP_SERVER");
    std::string topic = client->get_value_by_doc(doc_json, "PRODUCER_TOPIC");
    kafka_clients::kafka_producer_worker *producer_worker  = client->create_producer(bootstrap_server, topic);
    delete client;

    char str_msg[]="";           
    if(!producer_worker->init())
    {
        spdlog::critical("kafka producer initialize error");
    }
    else
    {        
        while (true) 
        {   std::string msg_to_send = "\"This is a test\"";
            std::this_thread::sleep_for(std::chrono::seconds(5));
            producer_worker->send(msg_to_send);
        }
        producer_worker->stop();
    }
    free(producer_worker);
    
    return;
}

int main(int argc,char** argv)
{
        boost::thread t_producer{call_producer_thread};
        boost::thread t_consumer{call_consumer_thread};
        t_producer.join();
        return 0;
}