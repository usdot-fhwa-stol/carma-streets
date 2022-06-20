#define RAPIDJSON_HAS_STDSTRING 1
#include <thread>    
#include <chrono>  
#include <unordered_map>
#include <QCoreApplication>
#include <spdlog/spdlog.h>
#include <rapidjson/writer.h>
#include <rapidjson/stringbuffer.h>
#include "kafka_client.h"
#include "streets_configuration.h"
#include "scheduling_worker.h"


using namespace std;
using namespace rapidjson;
using namespace chrono;
using namespace OpenAPI;


/**
 * Method to configure spdlog::logger for logging scheduling metrics into daily rotating csv file.
 */  
void configure_csv_logger() {
    try{
        auto csv_logger = spdlog::daily_logger_mt<spdlog::async_factory>(
            "csv_logger",  // logger name
            streets_service::streets_configuration::get_string_config("schedule_log_path")+
                streets_service::streets_configuration::get_string_config("schedule_log_filename") +".csv",  // log file name and path
            23, // hours to rotate
            59 // minutes to rotate
            );
        // Only log log statement content
        csv_logger->set_pattern("%v");
        csv_logger->set_level(spdlog::level::info);
    }
    catch (const spdlog::spdlog_ex& ex)
    {
        spdlog::error( "Log initialization failed: {0}!",ex.what());
    }
}


void call_consumer_thread(std::shared_ptr<streets_vehicles::vehicle_list> veh_list)
{
  
    auto client = std::make_shared<kafka_clients::kafka_client>();
    std::string bootstrap_server = streets_service::streets_configuration::get_string_config("bootstrap_server");
    std::string group_id = streets_service::streets_configuration::get_string_config("group_id");
    std::string topic = streets_service::streets_configuration::get_string_config("consumer_topic");
    auto consumer_worker = client->create_consumer(bootstrap_server,topic,group_id);
   

    if(!consumer_worker->init())
    {
        SPDLOG_CRITICAL("kafka consumer initialize error");
    }
    else
    {
        consumer_worker->subscribe();
        if(!consumer_worker->is_running())
        {
            SPDLOG_CRITICAL("consumer_worker is not running");
        }
        
        while (consumer_worker->is_running()) 
        {
            
            const std::string payload = consumer_worker->consume(1000);

            if(payload.length() > 0)
            {                

                veh_list->process_update(payload);
    
            }
        }
        
        consumer_worker->stop();
    }     
    return;    
}



void call_scheduling_thread(std::shared_ptr<streets_vehicles::vehicle_list> veh_list, std::shared_ptr<streets_vehicle_scheduler::vehicle_scheduler> scheduler){

    auto client = std::make_shared<kafka_clients::kafka_client>();         
    std::string bootstrap_server =  streets_service::streets_configuration::get_string_config("bootstrap_server");
    std::string topic = streets_service::streets_configuration::get_string_config("producer_topic");
    auto producer_worker  = client->create_producer(bootstrap_server, topic);

    u_int64_t last_schedule_timestamp;
    

    // Create logger
    if ( streets_service::streets_configuration::get_boolean_config("enable_schedule_logging") ) {
        configure_csv_logger();
    }
    char str_msg[]="";           
    if(!producer_worker->init())
    {
        SPDLOG_CRITICAL("kafka producer initialize error");
    }
    else
    {        
        
        int sch_count = 0;
        while (true) 
        {   
            auto scheduling_delta=streets_service::streets_configuration::get_double_config("scheduling_delta");

            if (duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count() - milliseconds(last_schedule_timestamp).count() >= milliseconds(int(scheduling_delta*1000)).count()){
                
                SPDLOG_INFO("schedule number #{0}", sch_count);      
                auto t = system_clock::now() + milliseconds(int(scheduling_delta*1000));

                streets_vehicle_scheduler::intersection_schedule schedule;
                schedule.timestamp = duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
                unordered_map<std::string, streets_vehicles::vehicle> veh_map = veh_list->get_vehicles();
                scheduler->schedule_vehicles(veh_map, schedule);

                string msg_to_send = schedule.toCSV();
                last_schedule_timestamp = schedule.timestamp;
                
                /* produce the scheduling plan to kafka */
                producer_worker->send(msg_to_send);

                if ( streets_service::streets_configuration::get_boolean_config("enable_schedule_logging") ) {
                    auto logger = spdlog::get("csv_logger");
                    if ( logger != nullptr ){
                        logger->info( schedule.toCSV());
                    }
                }

                // update the previous scheduling time and sleep until next schedule
                if (system_clock::now() < t){
                    this_thread::sleep_until(t);
                }

                sch_count += 1;

            }

        }
        producer_worker->stop();

    }

    return;

}



int main(int argc,char** argv)
{
    QCoreApplication a(argc, argv);

    streets_service::streets_configuration::initialize_logger();

    auto scheduling_worker = std::make_shared<streets_scheduling_worker::scheduling_worker>();

    auto intersection_info = scheduling_worker->get_intersection_info(); 
    auto veh_list = scheduling_worker->create_veh_list(); 
    auto scheduler = scheduling_worker->create_scheduler(); 

    boost::thread consumer(call_consumer_thread, veh_list);
    boost::thread scheduling{call_scheduling_thread, veh_list, scheduler};
    consumer.join();
    scheduling.join();
    return 0;

}


