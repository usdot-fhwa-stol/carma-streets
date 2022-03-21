#include "streets_configuration.h"
#include <spdlog/spdlog.h>



// Constructor
streets_configuration::streets_configuration( const std::string &filepath ): filepath(filepath){
    auto doc = parse_configuration_file();
    initialize( doc );
    update_configuration( doc );
    spdlog::info("Printing Configuration Parameters ---------------------------------------");
    for(const auto& conf : configuration_map)
    {
        spdlog::info("{0} : {1} ", conf.first.c_str(), conf.second.value.c_str());
    }
};

rapidjson::Document streets_configuration::parse_configuration_file() {
    config_lock.lock();
    // Open file
    std::ifstream file(filepath);
    if (!file.is_open()) {
       throw streets_configuration_exception("Unable to open Streets configuration file " + filepath + " !"); 
    }
    // Add file contents to stream and parse stream into Document
    rapidjson::IStreamWrapper isw(file);
    rapidjson::Document doc;
    doc.ParseStream(isw);
    if (doc.HasParseError()){
        spdlog::error("Error  : {0} Offset: {1} ", doc.GetParseError(), doc.GetErrorOffset());
        throw streets_configuration_exception("Document parse error!");
    }
    file.close();
    config_lock.unlock();
    return doc;
};

void streets_configuration::initialize( const rapidjson::Document &doc ) {
    if ( doc.HasMember("service_name") && doc.FindMember("service_name")->value.IsString() ) {
        configure_logger( doc.FindMember("service_name")->value.GetString());
    }
    else {
        spdlog::warn("Parameter \"service_name\" missing/incorrectly formatted in manifest.json! Setting \"service_name\" to streets_service!");
        configure_logger("streets_service");
    }
    if ( doc.HasMember("loglevel") && doc.FindMember("loglevel")->value.IsString() ) {
        set_loglevel( doc.FindMember("loglevel")->value.GetString() );
    }
    else {
        spdlog::warn("Parameter \"loglevel\" missing/incorrectly formatted in manifest.json! Setting \"loglevel\" to INFO!");
        set_loglevel("info");
    }
}

void streets_configuration::update_configuration( const rapidjson::Document &doc) {
    spdlog::debug("Updating Configuration Map");
    config_lock.lock();
    if ( doc.HasMember("configurations") && doc.FindMember("configurations")->value.IsArray() ) {
        for ( auto& cnf: doc.FindMember("configurations")->value.GetArray() ) {
            std::string property_name = cnf.FindMember("name")->value.GetString();
            auto val = configuration_map.find( property_name );
            if ( val != configuration_map.end() && cnf.IsObject()) {
                configuration config = val->second;
                configuration file_config;
                file_config.fromJson( cnf.GetObject());
                if ( config != file_config ) {
                    spdlog::info("Updating configuration {0}!", property_name);
                    configuration_map.erase(property_name);
                    configuration_map.insert(std::make_pair(property_name, file_config));
                }
            }
            else if ( val == configuration_map.end() && cnf.IsObject() ) {
                configuration file_config;
                file_config.fromJson(cnf.GetObject() );
                configuration_map.insert(std::make_pair(property_name, file_config));
            }
            else {
                throw streets_configuration_exception("Configuration parameter " + property_name + " not properly formatted!");
            }  
        } 
    }
    config_lock.unlock();
    
};

std::string streets_configuration::get_string_config(const std::string &config_param_name) {
    // Get Singleton
    auto &instance = get_singleton();
    // update  Singleton
    instance.update_configuration(instance.parse_configuration_file());

    // Get Configuration
    auto string_config = instance.configuration_map.find(config_param_name);
    if ( string_config->second.type == data_type::config_string) {
        return string_config->second.value;
    }
    else {
        throw streets_configuration_exception("Configuration parameter " + config_param_name + " is not data_type string!");
    }
}

int streets_configuration::get_int_config( const std::string &config_param_name) {
     // Get Singleton
    auto &instance = get_singleton();
    // update  Singleton
    instance.update_configuration(instance.parse_configuration_file());

    // Get Configuration
    auto int_config = instance.configuration_map.find(config_param_name);
    if ( int_config->second.type == data_type::config_int) {
        return std::stoi(int_config->second.value);
    }
    else {
        throw streets_configuration_exception("Configuration parameter " + config_param_name + " is not data_type string!");
    }
}

double streets_configuration::get_double_config( const std::string &config_param_name) {
     // Get Singleton
    auto &instance = get_singleton();
    // update  Singleton
    instance.update_configuration(instance.parse_configuration_file());
    // Get Configuration
    auto double_config = instance.configuration_map.find(config_param_name);
    if ( double_config->second.type == data_type::config_double) {
        return std::stod(double_config->second.value);
    }
    else {
        throw streets_configuration_exception("Configuration parameter " + config_param_name + " is not data_type string!");
    }
}

bool streets_configuration::get_boolean_config( const std::string &config_param_name) {
     // Get Singleton
    auto &instance = get_singleton();
    // update  Singleton
    instance.update_configuration(instance.parse_configuration_file());
    // Get Configuration
    auto bool_config = instance.configuration_map.find(config_param_name);
    if ( bool_config->second.type == data_type::config_bool) {
        return bool_config->second.value.compare("true") == 0 ? true : false;
    }
    else {
        throw streets_configuration_exception("Configuration parameter " + config_param_name + " is not data_type BOOL!");
    }
}

 
void streets_configuration::set_loglevel(const std::string &loglevel ) {
    // Get main logger and set loglevel
    auto main_logger = spdlog::get("main");
    if ( main_logger != nullptr ) {
        main_logger->set_level(spdlog::level::from_str(loglevel));
        spdlog::info( "Log Level set to {0}!", spdlog::level::to_string_view(spdlog::get_level()));
    }
    else {
        throw streets_configuration_exception("Log Level set failed! No main logger configured!");
    }
}

void streets_configuration::configure_logger(const std::string &service_name ){
    try {
        // Create logger thread pool
        spdlog::init_thread_pool(8192, 1);
        // Create logger file and stdout sinks
        auto file_sink = std::make_shared<spdlog::sinks::daily_file_sink_mt>("../logs/" + service_name, 23, 3);
        auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
        // Link thread pool and sinks to async logger
        auto logger = std::make_shared<spdlog::async_logger>("main",  spdlog::sinks_init_list({console_sink, file_sink}),spdlog::thread_pool());
        // Register logger and set as default
        spdlog::set_default_logger(logger);
        spdlog::info("Default Logger initialized!");
    }   
    catch (const spdlog::spdlog_ex& ex)
    {
        spdlog::error( "Log initialization failed: {0}!",ex.what());
    }
};





