#include "time_sync_message.h"

namespace streets_service::simulation
{
    void time_sync_message::fromJson( const std::string &json) {
        rapidjson::Document doc;
        doc.Parse(json.c_str());
        if (doc.HasParseError()) {
            throw std::runtime_error("Time Sync Message JSON is misformatted. JSON parsing failed!");  
        }
        // Write lock
        if ( doc.HasMember("timestep") && doc.FindMember("timestep")->value.IsUint64()) {
            timestep = doc["timestep"].GetUint64(); // OPTIONAL in J2735 SPaT definition
        } 
        else {
            throw std::runtime_error("Missing or missformatted required propert timestep !");
        }
        if ( doc.HasMember("seq") && doc.FindMember("seq")->value.IsUint64()) {
            timestep = doc["seq"].GetUint64(); // OPTIONAL in J2735 SPaT definition
        } 
        else {
            throw std::runtime_error("Missing or missformatted required propert seq!");
        } 
    }

    std::string time_sync_message::toJson() const {
        rapidjson::Document doc;
        auto allocator = doc.GetAllocator();
        rapidjson::Value msg(rapidjson::kObjectType);
        msg.AddMember("timestep", timestep, allocator);
        msg.AddMember("seq", seq, allocator);
        rapidjson::StringBuffer buffer;
        rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
        msg.Accept(writer);
        
        return buffer.GetString();
    }
    
} // namespace streets_service::simulation
