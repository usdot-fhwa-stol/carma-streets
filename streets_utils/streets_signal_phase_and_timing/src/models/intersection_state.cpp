#include "intersection_state.h"

namespace signal_phase_and_timing {

    rapidjson::Value intersection_state::toJson(rapidjson::Document::AllocatorType &allocator) const {
        // Create intersection state JSON value
        rapidjson::Value state(rapidjson::kObjectType);
        // Populate
        // OPTIONAL see J2735 IntersectionState definition
        state.AddMember("name", name, allocator);
        // REQUIRED see J2735 IntersectionState definition
        if (id == 0 ) {
            throw new signal_phase_and_timing_exception("IntersectionState is missing required id property!");  
        }
        state.AddMember("id", id, allocator);
        // REQUIRED see J2735 IntersectionState definition
        if ( message_count ==  0) {
            throw new signal_phase_and_timing_exception("IntersectionState is missing required message_count property!");
        }
        state.AddMember("message_count", message_count, allocator);
        // REQUIRED see J2735 IntersectionState definition
        state.AddMember("status", status, allocator);
        if ( status.empty() ) {
            throw new signal_phase_and_timing_exception("IntersectionState is missing required status property!");  
        }
        // OPTIONAL see J2735 IntersectionState definition
        state.AddMember("minute_of_the_year", minute_of_the_year, allocator);
        // OPTIONAL see J2735 IntersectionState definition
        state.AddMember("second", second, allocator);
        // OPTIONAL see J2735 IntersectionState definition
        if ( !enabled_lane_list.empty() ) {
            rapidjson::Value lane_list(rapidjson::kArrayType);
            for (const auto &lane_id : enabled_lane_list) {
                lane_list.PushBack(lane_id, allocator);
            }
            state.AddMember("enabled_lane_list", lane_list, allocator);
        }
        // REQUIRED see J2735 IntersectionState definition
        if ( !movement_states.empty() ) {
            rapidjson::Value states_list(rapidjson::kArrayType);
            for (const auto &state : movement_states) {
                states_list.PushBack(state.toJson(allocator), allocator);
            }
            state.AddMember("states", states_list, allocator);
        }
        else {
               throw new signal_phase_and_timing_exception("IntersectionState is missing required states property!");
        }
        // OPTIONAL see J2735 IntersectionState definition
        if ( !maneuver_assist_list.empty() ) {
            rapidjson::Value maneuver_list(rapidjson::kArrayType);
            for (const auto &maneuver : maneuver_assist_list) {
                maneuver_list.PushBack(maneuver.toJson(allocator), allocator);
            }
            state.AddMember("maneuver_assist_list", maneuver_list, allocator);

        }
        return state;
    }

    void intersection_state::fromJson(const rapidjson::Value &val) {
        if ( val.IsObject() ) {
            if ( val.FindMember("name")->value.IsString() ) {
                // OPTIONAL see J2735 IntersectionState definition
                name =  val["name"].GetString();
            } 
            if ( val.FindMember("id")->value.IsInt()) {
                // REQUIRED see J2735 IntersectionState definition
                id =  val["id"].GetInt();
            }
            else {
               throw new signal_phase_and_timing_exception("IntersectionState is missing required id property!");
            }
            if ( val.FindMember("message_count")->value.IsInt()) {
                // REQUIRED see J2735 IntersectionState definition
                message_count =  val["message_count"].GetInt();
            }
            else {
               throw new signal_phase_and_timing_exception("IntersectionState is missing required message_count property!");
            }
            if ( val.FindMember("status")->value.IsString()) {
                // REQUIRED see J2735 IntersectionState definition
                status =  val["status"].GetString();
            }
            else {
               throw new signal_phase_and_timing_exception("IntersectionState is missing required status property!");
            }
            if ( val.FindMember("minute_of_the_year")->value.IsUint64()) {
                // OPTIONAL see J2735 IntersectionState definition
                minute_of_the_year =  val["minute_of_the_year"].GetUint64();
            }
            if ( val.FindMember("second")->value.IsUint()) {
                // OPTIONAL see J2735 IntersectionState definition
                second =  val["second"].GetUint();
            }
            else {
               throw new signal_phase_and_timing_exception("IntersectionState is missing required second property!");
            }
            if ( val.FindMember("enabled_lane_list")->value.IsArray() ) {
                // OPTIONAL see J2735 IntersectionState definition
                enabled_lane_list.clear();
                for (const auto &lane: val["enabled_lane_list"].GetArray()) {
                    enabled_lane_list.push_back(lane.GetInt());
                }
            }
            if ( val.FindMember("states")->value.IsArray() ) {
                // REQUIRED see J2735 IntersectionState definition
                movement_states.clear();
                for (const auto &state: val["states"].GetArray()) {
                    movement_state move_state;
                    move_state.fromJson( state );
                    movement_states.push_back( move_state);
                }
            }
            else {
               throw new signal_phase_and_timing_exception("IntersectionState is missing required states property!");
            }
            if ( val.FindMember("maneuver_assist_list")->value.IsArray() ) {
                // OPTIONAL see J2735 IntersectionState definition
                maneuver_assist_list.clear();
                for (const auto &state: val["maneuver_assist_list"].GetArray()) {
                    connection_maneuver_assist maneuver;
                    maneuver.fromJson( state );
                    maneuver_assist_list.push_back( maneuver);
                }
            }
        }
    }

    bool intersection_state::operator==(const intersection_state &other) const{
        return name == other.name && id == other.id && message_count == other.message_count && status == other.status &&
            minute_of_the_year && other.minute_of_the_year && second == other.second && enabled_lane_list == other.enabled_lane_list
            && movement_states == other.movement_states && maneuver_assist_list == other.maneuver_assist_list;
    }

    bool intersection_state::operator!=(const intersection_state &compare) const{
        return !operator==(compare);
    }
}