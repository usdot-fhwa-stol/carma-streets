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
                // REQUIRED see J2735 AdvisorySpeed definition
                type =  static_cast<advisory_speed_type>(val["type"].GetInt());
            }
            else {
               throw signal_phase_and_timing_exception("AdvisorySpeed is missing required type property!");
            }
            if ( val.FindMember("speed_limit")->value.IsInt() ){
                // OPTIONAL see J2735 AdvisorySpeed definition
                speed_limit =  val["speed_limit"].GetInt();
            }
            if ( val.FindMember("speed_confidence")->value.IsInt() ) {
                // OPTIONAL see J2735 AdvisorySpeed definition
                confidence = static_cast<speed_confidence>(val["speed_confidence"].GetInt());
            }
            if ( val.FindMember("distance")->value.IsInt() ){
                // OPTIONAL see J2735 AdvisorySpeed definition
                distance =  val["distance"].GetInt();
            }
            if ( val.FindMember("vehicle_class")->value.IsInt() ){
                // OPTIONAL see J2735 AdvisorySpeed definition
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

