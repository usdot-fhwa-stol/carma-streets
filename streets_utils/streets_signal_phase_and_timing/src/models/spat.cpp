#include "spat.h"

namespace signal_phase_and_timing{

    rapidjson::Value spat::toJson(rapidjson::Document::AllocatorType &allocator) const{
        
        // Create SPaT JSON value
        rapidjson::Value spat(rapidjson::kObjectType);
        
        // Populate SPat JSON
        if ( timestamp == 0 ) {
            throw signal_phase_and_timing_exception("SPaT message is missing required timestamp property!");
        }
        spat.AddMember("timestamp", timestamp, allocator);
        spat.AddMember( "name", name, allocator);
        if ( !intersection_state_list.empty() ) {
            rapidjson::Value list(rapidjson::kArrayType);
            for (const auto &state : intersection_state_list ) {
                list.PushBack(state.toJson(allocator), allocator);
            }
            spat.AddMember("intersection_state_list", list, allocator);
        }else {
            throw signal_phase_and_timing_exception("SPaT message is missing required intersection_state_list property!");
        }
        return spat;
    }

    void spat::fromJson(const rapidjson::Value &val ) {
        if ( val.IsObject() ) {
            if (val.FindMember("timestamp")->value.IsUint64()) {
                timestamp =  val["timestamp"].GetUint64(); // OPTIONAL in J2735 SPaT definition but required for CARMA-Streets
            } 
            else {
               throw signal_phase_and_timing_exception("SPaT message is missing required timestamp property!");
            }
            if (val.FindMember("name")->value.IsString() ) {
                name = val["name"].GetString();  // OPTIONAL see J2735 SPaT definition
            }
            if ( val.FindMember("intersection_state_list")->value.IsArray() ) {
                // REQUIRED see J2735 SPaT definition
                // Clear intersection state list in case it is populated.
                intersection_state_list.clear();
                for ( const auto &state : val["intersection_state_list"].GetArray() ){
                    intersection_state cur_state;
                    cur_state.fromJson(state);
                    intersection_state_list.push_back(cur_state);
                }
            }
            else {
               throw signal_phase_and_timing_exception("SPaT message is missing required intersection_state_list property!");
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