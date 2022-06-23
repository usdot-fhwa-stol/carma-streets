#include "advisory_speed.h"

namespace signal_phase_and_timing
{
    rapidjson::Value advisory_speed::toJson(rapidjson::Document::AllocatorType &allocator ) const{
        // Create advisory speed JSON value
        rapidjson::Value speed(rapidjson::kObjectType);
        // Populate
        speed.AddMember("type", static_cast<int>(type), allocator);
        speed.AddMember("speed_limit", speed_limit, allocator);
        speed.AddMember("speed_confidence", static_cast<int>(confidence), allocator);
        speed.AddMember("distance", distance, allocator);
        speed.AddMember("vehicle_class", veh_class, allocator);
        SPDLOG_INFO("Speed Advisory JSON Created");
        return speed;
    }

    void advisory_speed::fromJson( const rapidjson::Value &val ) {
        if ( val.IsObject() ) {
            if ( val.FindMember("type")->value.IsInt() ) {
                type =  static_cast<advisory_speed_type>(val["type"].GetInt());
            }
            if ( val.FindMember("speed_limit")->value.IsInt() ){
                speed_limit =  val["speed_limit"].GetInt();
            }
            if ( val.FindMember("speed_confidence")->value.IsInt() ) {
                confidence = static_cast<speed_confidence>(val["speed_confidence"].GetInt());
            }
            if ( val.FindMember("distance")->value.IsInt() ){
                distance =  val["distance"].GetInt();
            }
            if ( val.FindMember("vehicle_class")->value.IsInt() ){
                veh_class = val["vehicle_class"].GetInt();
            }
        }
    }

    bool advisory_speed::operator==(const advisory_speed &compare) const {
        return type == compare.type && speed_limit == compare.speed_limit && confidence == compare.confidence 
            && distance == compare.distance && veh_class == compare.veh_class;
    }

    bool advisory_speed::operator!=(const advisory_speed &compare) const{
        return !operator==(compare);
    }

    
} 

