#include "spat.h"

namespace signal_phase_and_timing{

    std::string spat::toJson() const{
        rapidjson::Document doc;
        auto allocator = doc.GetAllocator();
        // Create SPaT JSON value
        rapidjson::Value spat(rapidjson::kObjectType);
        // Populate SPat JSON
        spat.AddMember("time_stamp", timestamp, allocator);
        spat.AddMember( "name", name, allocator);
        if ( !intersections.empty() ) {
            rapidjson::Value list(rapidjson::kArrayType);
            for (const auto &intersection : intersections ) {
                list.PushBack(intersection.toJson(allocator), allocator);
            }
            spat.AddMember("intersections", list, allocator);
        }else {
            throw signal_phase_and_timing_exception("SPaT message is missing required intersections property!");
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

        if ( doc.HasMember("time_stamp") && doc.FindMember("time_stamp")->value.IsUint64()) {
            timestamp =  doc["time_stamp"].GetUint64(); // OPTIONAL in J2735 SPaT definition
        } 
        if (doc.FindMember("name")->value.IsString() ) {
            name = doc["name"].GetString();  // OPTIONAL see J2735 SPaT definition
        }
        if ( doc.FindMember("intersections")->value.IsArray() ) {
            // REQUIRED see J2735 SPaT definition
            // Clear intersection state list in case it is populated.
            intersections.clear();
            for ( const auto &intersection : doc["intersections"].GetArray() ){
                intersection_state cur_state;
                cur_state.fromJson(intersection);
                intersections.push_back(cur_state);
            }
        }
        else {
            throw signal_phase_and_timing_exception("SPaT message is missing required intersections property!");
        }
            
        
    }

    bool spat::operator==(const spat &other) const{
        return timestamp == other.timestamp && name == other.name && intersections == other.intersections;
    }

    bool spat::operator!=(const spat &other) const{
        return !operator==(other);
    }
}