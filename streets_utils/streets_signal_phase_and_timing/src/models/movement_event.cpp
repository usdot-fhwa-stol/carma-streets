#include "movement_event.h"

namespace signal_phase_and_timing {

    rapidjson::Value movement_event::toJson(rapidjson::Document::AllocatorType &allocator) const {
        // Create movement event JSON value
        rapidjson::Value event(rapidjson::kObjectType);

        // Populate movement event
        event.AddMember("event_state", static_cast<int>(event_state), allocator);
        event.AddMember("timing", timing.toJson(allocator), allocator);
        if ( !speeds.empty()) {
            rapidjson::Value advisory_speed_array(rapidjson::kArrayType);
            for ( const auto &speed : speeds) {
                advisory_speed_array.PushBack(speed.toJson(allocator), allocator);
            }
            event.AddMember("speeds", advisory_speed_array, allocator);
        }
        return event;
    }

    void movement_event::fromJson( const rapidjson::Value &val ) {
        if ( val.IsObject() ) {
            if ( val.HasMember("event_state") &&  val["event_state"].IsInt() ) {
                event_state = static_cast<movement_phase_state>( val["event_state"].GetInt());
            }
            time_change_details detail;
            detail.fromJson( val["timing"] );
            timing = detail;
            if ( val.FindMember("speeds")->value.IsArray() ) {
                speeds.clear();
                for ( const auto &sp : val["speeds"].GetArray() ){
                    advisory_speed speed;
                    speed.fromJson( sp );
                    speeds.push_back(speed);
                }
            } 
        }
    }

    bool movement_event::operator==(const movement_event &other) const{
        return event_state == other.event_state && timing == other.timing && speeds == other.speeds;
    }

    bool movement_event::operator!=(const movement_event &other) const{
        return !operator==(other);
    }
}