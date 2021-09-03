#include "kafka_client.h"
#include <iostream>  
#include <thread>    
#include <chrono>  
#include <vector>
#include <unordered_map>
#include <condition_variable>
#include <mutex>
#include "configuration.h"
#include "osm.h"
#include "vehicle.h"
#include "sorting.h"
#include "scheduling.h"

using namespace std;
using json = nlohmann::json;
using namespace chrono;

mutex mtx_1;
mutex mtx_2;
condition_variable cv_1;
condition_variable cv_2;
//bool cv1_ready = true;
bool is_schedule_ready = false;

configuration config("configuration.json");
osm localmap("osm.json");
unordered_map<string, vehicle> list_veh;



void consumer_update(const char* paylod){

    // assuming these two lines will successfully convert the const char* payload into a json object.
    json message;
    stringstream(paylod) >> message;

    string veh_id = message["strategy_params"]["v_id"];

    if (!list_veh.count(veh_id)){
        string cur_lane_type = localmap.get_laneType(message["strategy_params"]["cur_lane_id"]);
        if (cur_lane_type == "entry" || cur_lane_type == "link"){
            list_veh[veh_id] = vehicle();
        }
    }
    
    if (list_veh.count(veh_id)){
        list_veh[veh_id].update(message, localmap);
        if (list_veh[veh_id].get_curState() == "LV"){
            list_veh.erase(veh_id);
        }
    }

}

void scheduling_func(unordered_map<string, vehicle> list_veh){

    scheduling schedule(unordered_map<string, vehicle> list_veh, osm localmap);

    /* estimate the departure times (DTs) of DVs */
    for (auto & vehicle_index : schedule.get_indexDVs()){
        
    }
    


}

void producer_func(){

}

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
                
                
                /* 
                * update function for updating the stored vehicle status and intents:
                *   note: 
                *   
                */
                unique_lock<mutex> lck_1(mtx_1);
                cv_1.wait(lck_1);
                consumer_update(paylod);
                lck_1.unlock();
                cv_1.notify_all();
            }
        }
        
        consumer_worker->stop();
    }     
    free(consumer_worker);
    return;    
}

void call_scheduling_thread(){
    
    while(true){
        if (duration<double>(system_clock::now().time_since_epoch()).count() - config.get_lastSchedulingT() >= config.get_schedulingDelta()){
            
            config.set_curSchedulingT(duration<double>(system_clock::now().time_since_epoch()).count());
            auto t = system_clock::now() + milliseconds(int(config.get_schedulingDelta()*1000));
            
            // lock the consumer mutex, copy the list_veh, unlock the consumer mutex, and notify the consumer.
            unique_lock<mutex> lck_1(mtx_1);
            cv_1.wait(lck_1);
            unordered_map<string, vehicle> list_veh_copy = list_veh;
            lck_1.unlock();
            cv_1.notify_all();

            // lock the producer mutex, run the scheduling service, unlock the producer mutex, and notify the producer.
            unique_lock<mutex> lck_2(mtx_2);
            cv_2.wait(lck_2, []{return !is_schedule_ready;});
            scheduling_func(list_veh_copy);
            is_schedule_ready = true;
            lck_2.unlock();
            cv_2.notify_all();

            // lock the consumer mutex, update vehicle info in list_veh, unlock the consumer mutex, and notify the consumer.
            unique_lock<mutex> lck_1(mtx_1);
            cv_1.wait(lck_1);
            // update vehicle info in list_veh
            lck_1.unlock();
            cv_1.notify_all();

            config.set_lastSchedulingT(config.get_curSchedulingT());
            if (system_clock::now() < t){
                this_thread::sleep_until(t);
            }
        }
    }
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
        {   
            
            unique_lock<mutex> lck_2(mtx_2);
            cv_2.wait(lck_2, []{return is_schedule_ready;});

            std::string msg_to_send = "\"This is a test\"";
            std::this_thread::sleep_for(std::chrono::seconds(5));
            producer_worker->send(msg_to_send);

            is_schedule_ready = false;
            lck_2.unlock();
            cv_2.notify_all();

        }
        producer_worker->stop();
    }
    free(producer_worker);
    
    return;
}


int main(int argc,char** argv)
{

    thread consumer(call_consumer_thread);
    thread scheduling(call_scheduling_thread);
    thread producer(call_producer_thread);

}