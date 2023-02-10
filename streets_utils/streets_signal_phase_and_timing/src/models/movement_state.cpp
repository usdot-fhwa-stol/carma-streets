#include "movement_state.h"

namespace signal_phase_and_timing {
    rapidjson::Value movement_state::toJson(rapidjson::Document::AllocatorType &allocator) const {
        // Create movement state JSON value
        rapidjson::Value move_state(rapidjson::kObjectType);
        // Populate
        move_state.AddMember( "movement_name", movement_name, allocator);
        if ( signal_group == 0 ) {
            throw signal_phase_and_timing_exception("MovementState is missing required signal_group property!");
        }
        move_state.AddMember( "signal_group", signal_group, allocator );
        if ( !state_time_speed.empty() ) {
            rapidjson::Value event_list(rapidjson::kArrayType);
            for (const auto &event: state_time_speed ) {
                event_list.PushBack( event.toJson(allocator), allocator);
            } 
            move_state.AddMember("state_time_speed", event_list, allocator);

        }
        if ( !maneuver_assist_list.empty() ) {
            rapidjson::Value maneuver_list(rapidjson::kArrayType);
            for (const auto &maneuver: maneuver_assist_list ) {
                maneuver_list.PushBack( maneuver.toJson(allocator), allocator);
            } 
            move_state.AddMember("maneuver_assist_list", maneuver_list, allocator);
        }
        return move_state;
    }

    void movement_state::fromJson( const rapidjson::Value &val ) {
        if ( val.IsObject() ) {
            if ( val.FindMember("movement_name")->value.IsString() ) {
                // OPTIONAL see J2735 MovementState definition
                movement_name =  val["movement_name"].GetString();
            }
            if ( val.HasMember("signal_group") && val["signal_group"].IsUint() ) {
                // REQUIRED see J2735 MovementState definition
                signal_group = static_cast<uint8_t>(val["signal_group"].GetUint());
            }
            else {
               throw signal_phase_and_timing_exception("MovementState is missing required signal_group property!");
            }
            if ( val.FindMember("state_time_speed")->value.IsArray() ) {
                // REQUIRED in J2735 MovementState definition 
                state_time_speed.clear();
                for (const auto &move_event: val["state_time_speed"].GetArray() ) {
                    movement_event event;
                    event.fromJson( move_event );
                    state_time_speed.push_back(event);
                }
            } 
            else {
               throw signal_phase_and_timing_exception("MovementState is missing required state_time_speed property!");
            }
            if ( val.FindMember("maneuver_assist_list")->value.IsArray() ) {
                // OPTIONAL see J2735 MovementState definition
                maneuver_assist_list.clear();
                for (const auto &maneuver: val["maneuver_assist_list"].GetArray() ) {
                    connection_maneuver_assist man;
                    man.fromJson( maneuver );
                    maneuver_assist_list.push_back(man);
                }
            }
        }
    }

    bool movement_state::operator==(const movement_state &other) const{
        return movement_name == other.movement_name && signal_group == other.signal_group 
                && state_time_speed == other.state_time_speed && maneuver_assist_list == other.maneuver_assist_list;
    }

    bool movement_state::operator!=(const movement_state &other) const{
        return !operator==(other);
    }
}