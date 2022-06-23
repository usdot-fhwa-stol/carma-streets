#include "spat.h"

namespace signal_phase_and_timing{

    rapidjson::Value spat::toJson(rapidjson::Document::AllocatorType &allocator) const{
        
        // Create SPaT JSON value
        rapidjson::Value spat(rapidjson::kObjectType);
        
        // Populate SPat JSON
        spat.AddMember("timestamp", timestamp, allocator);
        spat.AddMember( "name", name, allocator);
        if ( !intersection_state_list.empty() ) {
            rapidjson::Value list(rapidjson::kArrayType);
            for (const auto &state : intersection_state_list ) {
                list.PushBack(state.toJson(allocator), allocator);
            }
            spat.AddMember("intersection_state_list", list, allocator);
        }
        SPDLOG_INFO("Spat JSON Created");
        return spat;
    }

    void spat::fromJson(const rapidjson::Value &val ) {
        if ( val.IsObject() ) {
            if (val.FindMember("timestamp")->value.IsUint64()) {
                timestamp =  val["timestamp"].GetUint64();
            }
            if (val.FindMember("name")->value.IsString() ) {
                name = val["name"].GetString();
            }
            if ( val.FindMember("intersection_state_list")->value.IsArray() ) {
                // Clear intersection state list in case it is populated.
                intersection_state_list.clear();
                for ( const auto &state : val["intersection_state_list"].GetArray() ){
                    intersection_state cur_state;
                    cur_state.fromJson(state);
                    intersection_state_list.push_back(cur_state);
                }
            }
            
        }
    }

    bool spat::operator==(const spat &other) const{
        return timestamp == other.timestamp && name == other.name && intersection_state_list == other.intersection_state_list;
    }

    bool spat::operator!=(const spat &other) const{
        return !operator==(other);
    }
}