#include "spat.h"

namespace signal_phase_and_timing{

    std::string spat::toJson() const{
        rapidjson::Document doc;
        auto allocator = doc.GetAllocator();
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
        }else {
            throw signal_phase_and_timing_exception("SPaT message is missing required intersection_state_list property!");
        }
        rapidjson::StringBuffer buffer;
        try {
            rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
            spat.Accept(writer);
        }
        catch( const std::exception &e ) {
            throw signal_phase_and_timing_exception(e.what());
        }
        return buffer.GetString();
    }

    void spat::fromJson(const std::string &json ) {
        rapidjson::Document doc;
        doc.Parse(json);
        if (doc.HasParseError()) {
            throw signal_phase_and_timing_exception("SPaT message JSON is misformatted. JSON parsing failed!");  
        }

        if (doc.FindMember("timestamp")->value.IsUint64()) {
            timestamp =  doc["timestamp"].GetUint64(); // OPTIONAL in J2735 SPaT definition
        } 
        if (doc.FindMember("name")->value.IsString() ) {
            name = doc["name"].GetString();  // OPTIONAL see J2735 SPaT definition
        }
        if ( doc.FindMember("intersection_state_list")->value.IsArray() ) {
            // REQUIRED see J2735 SPaT definition
            // Clear intersection state list in case it is populated.
            intersection_state_list.clear();
            for ( const auto &state : doc["intersection_state_list"].GetArray() ){
                intersection_state cur_state;
                cur_state.fromJson(state);
                intersection_state_list.push_back(cur_state);
            }
        }
        else {
            throw signal_phase_and_timing_exception("SPaT message is missing required intersection_state_list property!");
        }
            
        
    }

    bool spat::operator==(const spat &other) const{
        return timestamp == other.timestamp && name == other.name && intersection_state_list == other.intersection_state_list;
    }

    bool spat::operator!=(const spat &other) const{
        return !operator==(other);
    }
}