#include "kafka_producer_worker.h"
#include "kafka_consumer_worker.h"
#include <boost/thread.hpp>
#include <boost/chrono.hpp>
#include "spdlog/spdlog.h"
#include "spdlog/cfg/env.h"
#include <rapidjson/document.h>
#include <rapidjson/istreamwrapper.h>
#include <fstream>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/writer.h>

namespace kafka_clients
{
   
    class kafka_client
    {
        public:
            kafka_client();
            ~kafka_client();
            kafka_clients::kafka_consumer_worker* create_consumer(const std::string &broker_str, const std::string &topic_str, 
                                                std::string &group_id_str);
            kafka_clients::kafka_producer_worker* create_producer(const std::string &broker_str, const std::string &topic_str);
            rapidjson::Document read_json_file(std::string &json_file);
            std::string get_value_by_doc(rapidjson::Document &doc,const char *key);
    };
    
   
    
}