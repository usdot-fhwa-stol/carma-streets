#include "tsc_configuration_state.h"

namespace streets_tsc_configuration
{
    
    rapidjson::Value signal_group_configuration::toJson(rapidjson::Document::AllocatorType &allocator) const {
        // Create signal group configuration JSON value
        rapidjson::Value state(rapidjson::kObjectType);
        //Populate
        // REQUIRED 
        if (signal_group_id == 0) {
            throw tsc_configuration_state_exception("tsc_configuration_state signal group id invalid");
        }
        state.AddMember("signal_group_id", signal_group_id, allocator);
        
        state.AddMember("yellow_change_duration", yellow_change_duration, allocator);
        
        state.AddMember("red_clearance", red_clearance, allocator);

        if( !concurrent_signal_groups.empty()) {
            rapidjson::Value concurrent_groups(rapidjson::kArrayType);
            for(const auto &signal_group : concurrent_signal_groups){
                concurrent_groups.PushBack(signal_group, allocator);
            }
            state.AddMember("concurrent_signal_groups", concurrent_groups, allocator);
        }

        return state;    
    }

    void signal_group_configuration::fromJson(const rapidjson::Value &val) {
        if (val.IsObject()) {
            if( val.HasMember("signal_group_id") && val["signal_group_id"].IsUint() ) {
                //REQUIRED
                signal_group_id = static_cast<uint8_t>(val["signal_group_id"].GetUint());
            }
            else{
                throw tsc_configuration_state_exception("tsc_configuration_state is missing required sign_group_id property");
            }

            if(val.HasMember("yellow_change_duration") && val["yellow_change_duration"].IsUint()){
                // REQUIRED
                yellow_change_duration = static_cast<uint16_t>(val["yellow_change_duration"].GetUint());
            }
            else{
                throw tsc_configuration_state_exception("tsc_configuration_state is missing required yellow_change_duration property");
            }

            if(val.HasMember("red_clearance") && val["red_clearance"].IsUint()){
                //REQUIRED
                red_clearance = static_cast<uint16_t>(val["red_clearance"].GetUint());
            }
            else{
                throw tsc_configuration_state_exception("tsc_configuration_state is missing required red_clearance property");
            }

            if(val.FindMember("concurrent_signal_groups")->value.IsArray()){
                // OPTIONAL
                concurrent_signal_groups.clear();
                for (const auto& concurrent_group : val["concurrent_signal_groups"].GetArray()){
                    concurrent_signal_groups.push_back(static_cast<uint8_t>(concurrent_group.GetUint()));
                }
            }

        }

    }

    std::string tsc_configuration_state::toJson() const {
        rapidjson::Document doc;
        auto allocator = doc.GetAllocator();
        // Create tsc configuration state JSON value
        rapidjson::Value config(rapidjson::kObjectType);
        // Populate
        if(!tsc_config_list.empty()){
            rapidjson::Value config_list(rapidjson::kArrayType);
            for(const auto &signal_group : tsc_config_list) {
                config_list.PushBack(signal_group.toJson(allocator), allocator);
            }
            config.AddMember("tsc_config_list", config_list, allocator);
        }
        else {
            throw tsc_configuration_state_exception("tsc_configuration_state is missing required tsc configuration information!");
        }

        rapidjson::StringBuffer buffer;
        try {
            rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
            config.Accept(writer);
        }
        catch( const std::exception &e ) {
            throw tsc_configuration_state_exception(e.what());
        }
        return buffer.GetString();
    }

    void tsc_configuration_state::fromJson(const std::string &json){
        rapidjson::Document doc;
        doc.Parse(json);
        if (doc.HasParseError()) {
            throw tsc_configuration_state_exception("TSC Configuration State JSON is misformatted. JSON parsing failed!");  
        }
        
        if(doc.FindMember("tsc_config_list")->value.IsArray()){
            tsc_config_list.clear();
            for(const auto &signal_group: doc["tsc_config_list"].GetArray()){
                signal_group_configuration config;
                config.fromJson(signal_group);
                tsc_config_list.push_back(config);
            }
        }
        else {
            throw tsc_configuration_state_exception("TSC Configuration State JSON is missing required tsc configuration information!");
        }
        
    }    
}