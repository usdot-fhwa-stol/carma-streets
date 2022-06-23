#include "intersection_state.h"

namespace signal_phase_and_timing {

    rapidjson::Value intersection_state::toJson(rapidjson::Document::AllocatorType &allocator) const {
        // Create intersection state JSON value
        rapidjson::Value state(rapidjson::kObjectType);
        // Populate
        state.AddMember("name", name, allocator);
        state.AddMember("id", id, allocator);
        state.AddMember("message_count", message_count, allocator);
        state.AddMember("status", status, allocator);
        state.AddMember("minute_of_the_year", minute_of_the_year, allocator);
        state.AddMember("second", second, allocator);
        if ( !enabled_lane_list.empty() ) {
            rapidjson::Value lane_list(rapidjson::kArrayType);
            for (const auto &lane_id : enabled_lane_list) {
                lane_list.PushBack(lane_id, allocator);
            }
            state.AddMember("enabled_lane_list", lane_list, allocator);
        }
        if ( !movement_states.empty() ) {
            rapidjson::Value states_list(rapidjson::kArrayType);
            for (const auto &state : movement_states) {
                states_list.PushBack(state.toJson(allocator), allocator);
            }
            state.AddMember("states", states_list, allocator);
        }
        if ( !maneuver_assist_list.empty() ) {
            rapidjson::Value maneuver_list(rapidjson::kArrayType);
            for (const auto &maneuver : maneuver_assist_list) {
                maneuver_list.PushBack(maneuver.toJson(allocator), allocator);
            }
            state.AddMember("maneuver_assist_list", maneuver_list, allocator);

        }
        SPDLOG_INFO("Intersection state JSON Created");
        return state;
    }

    void intersection_state::fromJson(const rapidjson::Value &val) {
        if ( val.IsObject() ) {
            if ( val.FindMember("name")->value.IsString() ) {
                name =  val["name"].GetString();
            }
            if ( val.FindMember("id")->value.IsInt()) {
                id =  val["id"].GetInt();
            }
            if ( val.FindMember("message_count")->value.IsInt()) {
                message_count =  val["message_count"].GetInt();
            }
            if ( val.FindMember("status")->value.IsString()) {
                status =  val["status"].GetString();
            }
            if ( val.FindMember("minute_of_the_year")->value.IsUint64()) {
                minute_of_the_year =  val["minute_of_the_year"].GetUint64();
            }
            if ( val.FindMember("second")->value.IsUint()) {
                second =  val["second"].GetUint();
            }
            if ( val.FindMember("enabled_lane_list")->value.IsArray() ) {
                enabled_lane_list.clear();
                for (const auto &lane: val["enabled_lane_list"].GetArray()) {
                    enabled_lane_list.push_back(lane.GetInt());
                }
            }
            if ( val.FindMember("states")->value.IsArray() ) {
                movement_states.clear();
                for (const auto &state: val["states"].GetArray()) {
                    movement_state move_state;
                    move_state.fromJson( state );
                    movement_states.push_back( move_state);
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