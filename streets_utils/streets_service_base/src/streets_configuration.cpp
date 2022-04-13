#include "streets_configuration.h"


namespace streets_service {
    // Constructor
    streets_configuration::streets_configuration( const std::string &filepath ): filepath(filepath){
        SPDLOG_INFO("Printing Configuration Parameters ---------------------------------------");
        rapidjson::Document doc = parse_configuration_file();
        configure_logger(doc);
        update_configuration(doc);
        for(const auto& conf : configuration_map)
        {
            SPDLOG_INFO("{0} : {1} ", conf.first.c_str(), conf.second.value.c_str());
        }
    };

    rapidjson::Document streets_configuration::parse_configuration_file() {
        // Open file
        std::ifstream file(filepath);
        if (!file.is_open()) {
            throw streets_service::streets_configuration_exception("Unable to open Streets configuration file " + filepath + " !"); 
        }
        // Add file contents to stream and parse stream into Document
        rapidjson::IStreamWrapper isw(file);
        rapidjson::Document doc;
        doc.ParseStream(isw);
        if (doc.HasParseError()){
            SPDLOG_ERROR("Error  : {0} Offset: {1} ", doc.GetParseError(), doc.GetErrorOffset());
            throw streets_service::streets_configuration_exception("Document parse error!");
        }
        file.close();
        return doc;
    };

    void streets_configuration::configure_logger( const rapidjson::Document &doc ) const {
        if ( doc.HasMember("service_name") && doc.FindMember("service_name")->value.IsString() ) {
            create_default_logger( doc.FindMember("service_name")->value.GetString());
        }
        else {
            SPDLOG_WARN("Parameter \"service_name\" missing/incorrectly formatted in manifest.json! Setting \"service_name\" to streets_service!");
            create_default_logger("streets_service");
        }
        if ( doc.HasMember("loglevel") && doc.FindMember("loglevel")->value.IsString() ) {
            set_loglevel( doc.FindMember("loglevel")->value.GetString() );
        }
        else {
            SPDLOG_WARN("Parameter \"loglevel\" missing/incorrectly formatted in manifest.json! Setting \"loglevel\" to INFO!");
            set_loglevel("info");
        }
    }

    void streets_configuration::update_configuration( const rapidjson::Document &doc) {
        SPDLOG_DEBUG("Updating Configuration Map");
        if ( doc.FindMember("configurations")->value.IsArray() ) {
            parse_configurations_array(doc.FindMember("configurations")->value.GetArray());
        }
        else {
            SPDLOG_WARN("No configurations found in manifest.json!");
        }   
    };

    void streets_configuration::update_log_level( const rapidjson::Document &doc ){
        if ( doc.FindMember("loglevel")->value.IsString() ) {
            set_loglevel( doc.FindMember("loglevel")->value.GetString() );
        }
        else {
            SPDLOG_WARN("Parameter \"loglevel\" missing/incorrectly formatted in manifest.json! Setting \"loglevel\" to INFO!");
            set_loglevel("info");
        }
    };

    void streets_configuration::parse_configurations_array( const rapidjson::GenericArray<true, rapidjson::Value> &arr) {
        std::unique_lock<std::mutex> lck(config_lock);
        for ( auto& cnf: arr ) {
            std::string property_name = cnf.FindMember("name")->value.GetString();
            auto val = configuration_map.find( property_name );
            if ( val != configuration_map.end() && cnf.IsObject()) {
                configuration config = val->second;
                configuration file_config;
                file_config.fromJson( cnf.GetObject());
                if ( config != file_config ) {
                    SPDLOG_INFO("Updating configuration {0}!", property_name);
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
                throw streets_service::streets_configuration_exception("Configuration parameter " + property_name + " not properly formatted!");
            }  
        }
    };

    std::string streets_configuration::get_string_config(const std::string &config_param_name) {
        // Get Singleton
        auto &instance = get_singleton();
        // update  Singleton
        instance.check_update();
        // Get Configuration
        auto string_config = instance.configuration_map.find(config_param_name);
        if ( string_config->second.type == data_type::config_string) {
            return string_config->second.value;
        }
        else {
            throw streets_service::streets_configuration_exception("Configuration parameter " + config_param_name + " is not data_type STRING!");
        }
    }

    int streets_configuration::get_int_config( const std::string &config_param_name) {
        // Get Singleton
        auto &instance = get_singleton();
        // update  Singleton
        instance.check_update();

        // Get Configuration
        auto int_config = instance.configuration_map.find(config_param_name);
        if ( int_config->second.type == data_type::config_int) {
            return std::stoi(int_config->second.value);
        }
        else {
            throw streets_service::streets_configuration_exception("Configuration parameter " + config_param_name + " is not data_type INTEGER!");
        }
    }

    double streets_configuration::get_double_config( const std::string &config_param_name) {
        // Get Singleton
        auto &instance = get_singleton();
        // update  Singleton
        instance.check_update();
        // Get Configuration
        auto double_config = instance.configuration_map.find(config_param_name);
        if ( double_config->second.type == data_type::config_double) {
            return std::stod(double_config->second.value);
        }
        else {
            throw streets_service::streets_configuration_exception("Configuration parameter " + config_param_name + " is not data_type DOUBLE!");
        }
    }

    bool streets_configuration::get_boolean_config( const std::string &config_param_name) {
        // Get Singleton
        auto &instance = get_singleton();
        // update  Singleton
        instance.check_update();
        // Get Configuration
        auto bool_config = instance.configuration_map.find(config_param_name);
        if ( bool_config->second.type == data_type::config_bool) {
            return bool_config->second.value.compare("true") == 0 ? true : false;
        }
        else {
            throw streets_service::streets_configuration_exception("Configuration parameter " + config_param_name + " is not data_type BOOL!");
        }
    }

    
    void streets_configuration::set_loglevel(const std::string &loglevel ) const{
        // Get main logger and set loglevel
        auto main_logger = spdlog::get("main");
        if ( main_logger != nullptr ) {
            main_logger->set_level(spdlog::level::from_str(loglevel));
            spdlog::set_level(spdlog::level::from_str(loglevel));
            SPDLOG_INFO( "Log Level set to {0}!", spdlog::level::to_string_view(spdlog::get_level()));
        }
        else {
            throw streets_configuration_exception("Log Level set failed! No main logger configured!");
        }
    }

    void streets_configuration::create_default_logger(const std::string &service_name ) const{
        try {
            // Create logger thread pool
            spdlog::init_thread_pool(8192, 1);
            // Create logger file and stdout sinks
            auto file_sink = std::make_shared<spdlog::sinks::daily_file_sink_mt>("../logs/" + service_name, 23, 3);
            auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
            // Link thread pool and sinks to async logger
            auto logger = std::make_shared<spdlog::async_logger>("main",  spdlog::sinks_init_list({console_sink, file_sink}),spdlog::thread_pool());
            // Set pattern [2022-3-31 13:00:00.000] [loglevel] [file_name.cpp:123]
            logger->set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%^%l%$] [%s:%#] %v");
            // Register logger and set as default
            spdlog::set_default_logger(logger);
            SPDLOG_INFO("Default Logger initialized!");
        }   
        catch (const spdlog::spdlog_ex& ex)
        {
            SPDLOG_ERROR( "Log initialization failed: {0}!",ex.what());
        }
    };

    void streets_configuration::initialize_logger(){
        auto &singleton = streets_configuration::get_singleton();
        auto doc = singleton.parse_configuration_file();
        singleton.configure_logger( doc );
    };

    void streets_configuration::check_update() {
        try {
            std::time_t time = boost::filesystem::last_write_time(filepath);
            SPDLOG_DEBUG("Last Modified Time {0} vs Stored Last Modified Time {1}", time, last_modified);
            if ( time > last_modified) {
                rapidjson::Document doc = parse_configuration_file();
                update_log_level(doc);
                update_configuration(doc);
                last_modified = time;
            }
        }
        catch (const std::exception &e) {
            throw streets_configuration_exception(e.what());
        }
    }

}






