#include "movement_state.h"

namespace signal_phase_and_timing {
    rapidjson::Value movement_state::toJson(rapidjson::Document::AllocatorType &allocator) const {
        // Create movement state JSON value
        rapidjson::Value move_state(rapidjson::kObjectType);
        // Populate
        move_state.AddMember( "movement_name", movement_name, allocator);
        move_state.AddMember( "signal_group_id", signal_group_id, allocator );
        if ( !movement_event_list.empty() ) {
            rapidjson::Value event_list(rapidjson::kArrayType);
            for (const auto &event: movement_event_list ) {
                event_list.PushBack( event.toJson(allocator), allocator);
            } 
            move_state.AddMember("movement_event_list", event_list, allocator);

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
            if ( val.FindMember("signal_group_id")->value.IsInt() ) {
                // REQUIRED see J2735 MovementState definition
                signal_group_id = val["signal_group_id"].GetInt();
            }
            else {
               throw signal_phase_and_timing_exception("MovementState is missing required signal_group_id property!");
            }
            if ( val.FindMember("movement_event_list")->value.IsArray() ) {
                // REQUIRED in J2735 MovementState definition 
                movement_event_list.clear();
                for (const auto &move_event: val["movement_event_list"].GetArray() ) {
                    movement_event event;
                    event.fromJson( move_event );
                    movement_event_list.push_back(event);
                }
            } 
            else {
               throw signal_phase_and_timing_exception("MovementState is missing required movement_event_list property!");
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
        return movement_name == other.movement_name && signal_group_id == other.signal_group_id 
                && movement_event_list == other.movement_event_list && maneuver_assist_list == other.maneuver_assist_list;
    }

    bool movement_state::operator!=(const movement_state &other) const{
        return !operator==(other);
    }
}