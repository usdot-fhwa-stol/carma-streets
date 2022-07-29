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
            if ( val.FindMember("status")->value.IsUint()) {
                // REQUIRED see J2735 IntersectionState definition
                status =  val["status"].GetUint();
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

    uint64_t intersection_state::convert_min_mills2epoch_ts(uint32_t moy_ll , uint16_t min_mills_timestamp) const{
        //Calculate timestamp for beginning of the year
        std::chrono::system_clock::time_point now = std::chrono::system_clock::now();
        time_t tt = std::chrono::system_clock::to_time_t(now);
        tm utctime;
        gmtime_r(&tt, &utctime);
        auto timeinfo = tm();
        timeinfo.tm_year = utctime.tm_year;
        timeinfo.tm_mon = 0; //January
        timeinfo.tm_mday = 1; //1st
        timeinfo.tm_hour = 0; //00
        timeinfo.tm_min = 0; //00
        timeinfo.tm_sec = 0; //00
        time_t year_begin_utc_t = timegm(&timeinfo);
        auto year_begin_utc_tp = std::chrono::system_clock::from_time_t(year_begin_utc_t);
        auto year_begin_utc_timestamp = std::chrono::duration_cast<std::chrono::milliseconds>(year_begin_utc_tp.time_since_epoch()).count();
        auto epoch_timestamp = year_begin_utc_timestamp + static_cast<long>(moy_ll) * MIN_TO_SECONDS * SECOND_TO_MILLISECONDS + min_mills_timestamp;
        return epoch_timestamp;
    }

    uint64_t intersection_state::get_epoch_timestamp() const{
       auto epoch_timestamp = convert_min_mills2epoch_ts(moy, time_stamp);
       return epoch_timestamp;
    }

    void intersection_state::set_timestamp_ntcip(const uint32_t second_of_day, const uint16_t millisecond_of_second ) {
        std::chrono::system_clock::time_point now = std::chrono::system_clock::now();
        time_t tt = std::chrono::system_clock::to_time_t(now);
        tm utc_tm = *gmtime(&tt);
        // Day of the year * 24 hours * 60 minutes + second of the day / 60 = minute of the year
        moy = utc_tm.tm_yday*24*60+ trunc(second_of_day/ 60); 
        // Remaineder of (second of the day/ 60 ) * 1000 + millisecond of the current second
        time_stamp = ((second_of_day%60)*1000) + millisecond_of_second;
    }
       

    void intersection_state::set_timestamp_local() {
        std::chrono::system_clock::time_point now = std::chrono::system_clock::now();
        time_t tt = std::chrono::system_clock::to_time_t(now);
        tm utc_tm = *gmtime(&tt);
        moy = utc_tm.tm_yday*60*24 + utc_tm.tm_hour*60 + utc_tm.tm_min;
        auto millisecond_of_second = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()).count() % 1000;
        // Millisecond of second + second of minute * 1000
        time_stamp = millisecond_of_second + utc_tm.tm_sec*1000;
       
    }

    void intersection_state::update_intersection_state( const uint8_t _status, const uint8_t message_count) {
        status = status;
        revision = message_count;
    }

    movement_state& intersection_state::get_movement(const int signal_group_id ) {
        // std::iterator<movement_state> it = states.begin();
        for ( std::list<movement_state>::iterator it = states.begin(); it != states.end(); it++) {
            if ( it->signal_group ==  signal_group_id ) {
                return *it;
            }
        }
        throw signal_phase_and_timing_exception("No movement_state found for movement with signal group id " + std::to_string(signal_group_id) + "!");   
    }

    void intersection_state::initialize_movement_states( const std::unordered_map<int,int> &phase_number_to_signal_group) {
        if (!states.empty()) {
            states.clear();
        }
        for (const auto &phase_2_sig_group : phase_number_to_signal_group) {
            movement_state state;
            state.signal_group =  phase_2_sig_group.second;
            states.push_back(state);
        }
    }

    void intersection_state::update_movement_state( ntcip::ntcip_1202_ext &spat_data, const int signal_group_id, const int phase_number ) {
        movement_state &movement = get_movement(signal_group_id);
        // Get current movement_event
        movement_event &cur_event = movement.state_time_speed.front();
        // Set event state 
        bool is_flashing = spat_data.get_phase_flashing_status( phase_number );
        if ( spat_data.get_phase_red_status(phase_number) ) {
            if (is_flashing) {
                cur_event.event_state = movement_phase_state::stop_then_proceed; 
            }else {
                cur_event.event_state =  movement_phase_state::stop_and_remain;
            }

        }
        else if ( spat_data.get_phase_yellow_status( phase_number ) ) {
            if ( is_flashing ) {
                cur_event.event_state =  movement_phase_state::caution_conflicting_traffic;
            }else {
                cur_event.event_state =  movement_phase_state::protected_clearance;
            }
        }
        else if ( spat_data.get_phase_green_status(phase_number)) {
            cur_event.event_state = movement_phase_state::protected_movement_allowed;
        }
        else {
            cur_event.event_state = movement_phase_state::dark;
        }
        // Set event timing
        cur_event.timing.max_end_time = convert_offset(spat_data.get_phasetime(phase_number).get_spat_veh_max_time_to_change());
        cur_event.timing.min_end_time = convert_offset(spat_data.get_phasetime(phase_number).get_spat_veh_min_time_to_change());

    }

    void intersection_state::update_movements(ntcip::ntcip_1202_ext &spat_data, const std::unordered_map<int,int> &phase_number_to_signal_group ) {
        for ( auto &move_state : states ) {
            // Clear any previous movement events
            move_state.state_time_speed.clear();
            // Add current movement_event
            movement_event cur_event;
            move_state.state_time_speed.push_front(cur_event);
        }
        
        for ( const auto &phase_2_signal_group : phase_number_to_signal_group ) {
            update_movement_state(spat_data, phase_2_signal_group.second, phase_2_signal_group.first);
        }
    }

    uint32_t intersection_state::convert_offset( const uint16_t offset_tenths_of_seconds) {
        // Convert tenths of seconds to milliseconds
        int offset_ms = offset_tenths_of_seconds * 100;

        std::chrono::system_clock::time_point nowTimePoint = std::chrono::system_clock::now();
        std::chrono::system_clock::time_point nowPlusOffsetTimePoint = nowTimePoint +  std::chrono::milliseconds(offset_ms);

        std::chrono::system_clock::duration tp = nowPlusOffsetTimePoint.time_since_epoch();

        std::chrono::hours h =  std::chrono::duration_cast<std::chrono::hours>(tp);
        tp -= h;

        std::chrono::minutes m =  std::chrono::duration_cast<std::chrono::minutes>(tp);
        tp -= m;

        std::chrono::seconds s =  std::chrono::duration_cast<std::chrono::seconds>(tp);
        tp -= s;

        std::chrono::milliseconds ms = std::chrono::duration_cast<std::chrono::milliseconds>(tp);

        double fractionSeconds = s.count() + (ms.count()/1000.0);
        double retTimeD = ((m.count() * 60) + fractionSeconds) * 10;
        SPDLOG_INFO("Converted offset {0} to {1}", offset_tenths_of_seconds, retTimeD);
        return (uint32_t) retTimeD;
    }
}