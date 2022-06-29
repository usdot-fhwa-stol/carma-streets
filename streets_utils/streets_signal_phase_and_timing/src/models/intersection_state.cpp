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
            throw signal_phase_and_timing_exception("IntersectionState is missing required id property!");  
        }
        state.AddMember("id", id, allocator);
        // REQUIRED see J2735 IntersectionState definition
        if ( revision ==  0) {
            throw signal_phase_and_timing_exception("IntersectionState is missing required revision property!");
        }
        state.AddMember("revision", revision, allocator);
        // REQUIRED see J2735 IntersectionState definition
        if ( status.empty() ) {
            throw signal_phase_and_timing_exception("IntersectionState is missing required status property!");  
        }
        state.AddMember("status", status, allocator);
        if ( moy == 0 ) {
            throw signal_phase_and_timing_exception("IntersectionState is missing required moy property!");
        }
        // OPTIONAL see J2735 IntersectionState definition but required for CARMA-Streets
        state.AddMember("moy", moy, allocator);
        // OPTIONAL see J2735 IntersectionState definition but required for CARMA-Streets
        if ( time_stamp == 0 ) {
            throw signal_phase_and_timing_exception("IntersectionState is missing required time_stamp property!");
        }
        state.AddMember("time_stamp", time_stamp, allocator);
        // OPTIONAL see J2735 IntersectionState definition
        if ( !enabled_lanes.empty() ) {
            rapidjson::Value lane_list(rapidjson::kArrayType);
            for (const auto &lane_id : enabled_lanes) {
                lane_list.PushBack(lane_id, allocator);
            }
            state.AddMember("enabled_lanes", lane_list, allocator);
        }
        // REQUIRED see J2735 IntersectionState definition
        if ( !states.empty() ) {
            rapidjson::Value states_list(rapidjson::kArrayType);
            for (const auto &move_state : states) {
                states_list.PushBack(move_state.toJson(allocator), allocator);
            }
            state.AddMember("states", states_list, allocator);
        }
        else {
               throw signal_phase_and_timing_exception("IntersectionState is missing required states property!");
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
            if ( val.HasMember("id") && val["id"].IsUint()) {
                // REQUIRED see J2735 IntersectionState definition
                id =  static_cast<uint16_t>(val["id"].GetUint());
            }
            else {
               throw signal_phase_and_timing_exception("IntersectionState is missing required id property!");
            }
            if ( val.HasMember("revision") && val["revision"].IsUint()) {
                // REQUIRED see J2735 IntersectionState definition
                revision =  static_cast<uint8_t>(val["revision"].GetUint());
            }
            else {
               throw signal_phase_and_timing_exception("IntersectionState is missing required revision property!");
            }
            if ( val.FindMember("status")->value.IsString()) {
                // REQUIRED see J2735 IntersectionState definition
                status =  val["status"].GetString();
            }
            else {
               throw signal_phase_and_timing_exception("IntersectionState is missing required status property!");
            }
            if ( val.HasMember("moy") &&  val["moy"].IsUint()) {
                // OPTIONAL see J2735 IntersectionState definition but required for CARMA-Streets
                moy = val["moy"].GetUint();
            }
            else {
               throw signal_phase_and_timing_exception("IntersectionState is missing required moy property!");
            }
            if ( val.HasMember("time_stamp") && val["time_stamp"].IsUint()) {
                // OPTIONAL see J2735 IntersectionState definition but required for CARMA-Streets
                time_stamp =  static_cast<uint16_t>(val["time_stamp"].GetUint());
            }
            else {
               throw signal_phase_and_timing_exception("IntersectionState is missing required time_stamp property!");
            }
            if ( val.FindMember("enabled_lanes")->value.IsArray() ) {
                // OPTIONAL see J2735 IntersectionState definition
                enabled_lanes.clear();
                for (const auto &lane: val["enabled_lanes"].GetArray()) {
                    enabled_lanes.push_back(lane.GetInt());
                }
            }
            if ( val.FindMember("states")->value.IsArray() ) {
                // REQUIRED see J2735 IntersectionState definition
                states.clear();
                for (const auto &movement_st: val["states"].GetArray()) {
                    movement_state move_state;
                    move_state.fromJson( movement_st );
                    states.push_back( move_state);
                }
            }
            else {
               throw signal_phase_and_timing_exception("IntersectionState is missing required states property!");
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
        return name == other.name && id == other.id && revision == other.revision && status == other.status &&
            moy && other.moy && time_stamp == other.time_stamp && enabled_lanes == other.enabled_lanes
            && states == other.states && maneuver_assist_list == other.maneuver_assist_list;
    }

    bool intersection_state::operator!=(const intersection_state &compare) const{
        return !operator==(compare);
    }
}