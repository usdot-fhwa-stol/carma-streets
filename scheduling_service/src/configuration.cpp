
#include "configuration.h"


/* */
configuration::configuration(){

    string json_file="../manifest.json";
    ifstream ifs {json_file};
    if(!ifs.is_open())
    {
        spdlog::critical("Could not open file: {0}", json_file.c_str());
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
            spdlog::critical("Error  : {0} Offset: {1} ", doc.GetParseError(), doc.GetErrorOffset());
            exit(1);
        }
    }
    catch(...)
    {
        std::exception_ptr p = std::current_exception();
        spdlog::critical("Reading {0} failure: {1}", json_file.c_str(), (p ? p.__cxa_exception_type()->name() : "null"));
        exit(1);
    }
    
    if(doc.HasMember("EXPIRATION_DELTA")){
        update_expiration_delta = doc["EXPIRATION_DELTA"].GetDouble();
        spdlog::info("update_expiration_delta :  {0}", update_expiration_delta);
    } else{
        spdlog::critical("Reading {0} failure: {1} is missing in {0}", json_file.c_str(), "EXPIRATION_DELTA");
        exit(1);
    }

}

/* */
double configuration::get_schedulingDelta(){return scheduling_delta;}

/* */
double configuration::get_lastSchedulingT(){return last_schedule_start_time;}

/* */
double configuration::get_curSchedulingT(){return cur_schedule_start_time;}

/* */
double configuration::get_expDelta(){return update_expiration_delta;}

/* */
void configuration::set_lastSchedulingT(double t){last_schedule_start_time = t;}

/* */
void configuration::set_curSchedulingT(double t){cur_schedule_start_time = t;}

/* */
void configuration::set_schedulingDelta(double delta){scheduling_delta = delta;}

/* */
void configuration::set_expDelta(double delta){update_expiration_delta = delta;}

