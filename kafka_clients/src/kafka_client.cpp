#include "kafka_client.h"

namespace kafka_clients
{
    kafka_client::kafka_client(){}

    kafka_client::~kafka_client(){} 
    
    kafka_clients::kafka_consumer_worker* kafka_client::create_consumer(const std::string &bootstrap_server, const std::string &topic_str, 
                                                std::string &group_id_str)
    {
        try
        {      
            int partition = 0;
            int partTest = 1;
            // int64_t cur_offset = RdKafka::Topic::OFFSET_BEGINNING;
            int64_t cur_offset = RdKafka::Topic::OFFSET_END;
            kafka_clients::kafka_consumer_worker *consumer = new kafka_clients::kafka_consumer_worker(bootstrap_server,topic_str,group_id_str,partition,cur_offset);
            return consumer;              
        }
        catch(...)
        {   
            std::exception_ptr p = std::current_exception();
            spdlog::critical("Create consumer failure: {0}",(p ? p.__cxa_exception_type()->name() : "null"));
            exit(1);
        }
    }

    kafka_clients::kafka_producer_worker* kafka_client::create_producer(const std::string &bootstrap_server, const std::string &topic_str)
    {
        try
        {   
            int partition = 0;
            kafka_clients::kafka_producer_worker *producer = new kafka_clients::kafka_producer_worker(bootstrap_server,topic_str,partition);
            return producer;           
        }
        catch(...)
        {   
            std::exception_ptr p = std::current_exception();
            spdlog::critical("Create consumer failure: {0}",(p ? p.__cxa_exception_type()->name() : "null"));
            exit(1);
        }
    }

    std::string kafka_client::get_value_by_doc(rapidjson::Document &doc, const char *key)
    {
        try
        { 
            std::string result;
            if(doc.HasMember(key))
                return doc[key].GetString();
            return std::string("");
        }
        catch(...)
        {   
            std::exception_ptr p = std::current_exception();
            spdlog::critical("get_value_by_doc failed: {0}",(p ? p.__cxa_exception_type()->name() : "null"));
            exit(1);
        }
    }

    rapidjson::Document kafka_client::read_json_file(std::string &json_file)
    {
        std::ifstream ifs {json_file};
        if(!ifs.is_open())
        {
            spdlog::critical("Could not open file: {0}",json_file.c_str());
            exit(1);
        }
        rapidjson::IStreamWrapper isw{ifs};
        rapidjson::Document doc{};
        try
        {            
            doc.ParseStream(isw);
            rapidjson::StringBuffer buffer{};
            rapidjson::Writer<rapidjson::StringBuffer>  writer {buffer};
            doc.Accept(writer);
            if(doc.HasParseError()){
                spdlog::critical("Error  : {0} Offset: {1} ", doc.GetParseError() ,doc.GetErrorOffset());
                exit(1);
            }
        }
        catch(...)
        {
            std::exception_ptr p = std::current_exception();
            spdlog::critical("Create consumer failure: {0}",(p ? p.__cxa_exception_type()->name() : "null"));
            exit(1);
        }
        
        return doc;
    }

     
 
}