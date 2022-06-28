#include "advisory_speed.h"

namespace signal_phase_and_timing
{
    rapidjson::Value advisory_speed::toJson(rapidjson::Document::AllocatorType &allocator ) const{
        // Create advisory speed JSON value
        rapidjson::Value adv_speed(rapidjson::kObjectType);
        // Populate
        adv_speed.AddMember("type", static_cast<int>(type), allocator);
        adv_speed.AddMember("speed", speed, allocator);
        if ( confidence != speed_confidence::unavailable ) {
            adv_speed.AddMember("confidence", static_cast<int>(confidence), allocator);
        }
        adv_speed.AddMember("distance", distance, allocator);
        adv_speed.AddMember("class", veh_class, allocator);
        return adv_speed;
    }

    void advisory_speed::fromJson( const rapidjson::Value &val ) {
        if ( val.IsObject() ) {
            if ( val.HasMember("type") && val["type"].IsInt() ) {
                // REQUIRED see J2735 AdvisorySpeed definition
                type =  static_cast<advisory_speed_type>(val["type"].GetInt());
            }
            else {
               throw signal_phase_and_timing_exception("AdvisorySpeed is missing required type property!");
            }
            if ( val.HasMember("speed") && val["speed"].IsUint() ){
                // OPTIONAL see J2735 AdvisorySpeed definition
                speed =  static_cast<uint16_t>(val["speed"].GetUint());

            }
            if ( val.HasMember("confidence") && val["confidence"].IsInt() ) {
                // OPTIONAL see J2735 AdvisorySpeed definition
                confidence = static_cast<speed_confidence>(val["confidence"].GetInt());
            }
            if ( val.HasMember("distance") && val["distance"].IsUint() ){
                // OPTIONAL see J2735 AdvisorySpeed definition
                distance =  static_cast<uint16_t>(val["distance"].GetUint());

            }
            if ( val.HasMember("class") && val["class"].IsUint() ){
                // OPTIONAL see J2735 AdvisorySpeed definition
                veh_class = static_cast<uint8_t>(val["class"].GetUint());

            }
        }
    }

    bool advisory_speed::operator==(const advisory_speed &compare) const {
        return type == compare.type && speed == compare.speed && confidence == compare.confidence 
            && distance == compare.distance && veh_class == compare.veh_class;
    }

    bool advisory_speed::operator!=(const advisory_speed &compare) const{
        return !operator==(compare);
    }

    
} 

