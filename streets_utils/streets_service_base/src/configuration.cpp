#include "configuration.h"

namespace streets_service {
    bool configuration::operator==(const configuration& t) const {
        return name == t.name && value == t.value && description == t.description && type == t.type;
    };

    bool configuration::operator!=(const configuration& t) const {
        return !(name == t.name && value == t.value && description == t.description && type == t.type);
    }

    void configuration::fromJson(const rapidjson::Value &val ){
        if (!val.FindMember("name")->value.IsString()) {
            throw streets_service::streets_configuration_exception("Configuration is missing name!");
        }
        name = val.FindMember("name")->value.GetString();

        if ( !val.FindMember("description")->value.IsString() ){
            throw streets_service::streets_configuration_exception("Configuration is" + name + " missing a description");
        } 
        description = val.FindMember("description")->value.GetString();

        if ( val.FindMember("type")->value.IsString() ){
            std::string doc_type = val.FindMember("type")->value.GetString();
            if ( doc_type == "STRING" ) {
                type = data_type::config_string;
                if ( !val.FindMember("value")->value.IsString() ) {
                    throw streets_service::streets_configuration_exception("Configuration is missing/incorrect a value");
                }
                value = val.FindMember("value")->value.GetString();
            }
            else if (doc_type == "INTEGER"){
                type = data_type::config_int;
                if ( !val.FindMember("value")->value.IsInt() ) {
                    throw streets_service::streets_configuration_exception("Configuration is missing/incorrect a value");
                }
                value = std::to_string(val.FindMember("value")->value.GetInt());
            }
            else if (doc_type == "DOUBLE"){
                type = data_type::config_double;
                if ( !val.FindMember("value")->value.IsDouble() ) {
                    throw streets_service::streets_configuration_exception("Configuration is missing/incorrect a value");
                }
                value = std::to_string(val.FindMember("value")->value.GetDouble());
            }
            else if (doc_type == "BOOL") {
                type = data_type::config_bool;
                if ( !val.FindMember("value")->value.IsBool() ) {
                    throw streets_service::streets_configuration_exception("Configuration is missing/incorrect a value");
                }
                value = val.FindMember("value")->value.GetBool() ? "true" : "false";
                
            } 
            else {
                throw streets_service::streets_configuration_exception("Data type configuration is invalid!");
            }
        } 
        else {
            throw streets_service::streets_configuration_exception("Configuration " + name + " is missing a type");
        }
        
        // spdlog::debug("Create config name : {0}, value : {1}, type : {2}, description : {3} ",name, value, type, description );

    };
}