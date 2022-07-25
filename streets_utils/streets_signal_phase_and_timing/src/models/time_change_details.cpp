#include "time_change_details.h"

namespace signal_phase_and_timing {
    
    rapidjson::Value time_change_details::toJson(rapidjson::Document::AllocatorType &allocator) const {
        // Create time change detail JSON value
        rapidjson::Value detail(rapidjson::kObjectType);
        // Populate
        // OPTIONAL see J2735 TimeChangeDetails definition but required for CARMA Streets future phase information
        if ( start_time == 36001 ) {
           throw signal_phase_and_timing_exception("TimeChangeDetails is missing required start_time property!"); 
        }
        detail.AddMember("start_time", start_time,allocator);
        // value represents unknow see J2735 spec
        if (min_end_time == 36001 ) {
            throw signal_phase_and_timing_exception("TimeChangeDetails is missing required min_end_time property!");
        }
        detail.AddMember("min_end_time", min_end_time,allocator);
        // value represents unknow see J2735 spec
        if (max_end_time != 36001) {
            detail.AddMember("max_end_time",max_end_time,allocator);
        }
        // value represents unknow see J2735 spec
        if (likely_time != 36001) {
            detail.AddMember("likely_time", likely_time, allocator);
        }
        detail.AddMember("confidence", confidence, allocator);
        // value represents unknow see J2735 spec  
        if (next_time != 36001) {
            detail.AddMember("next_time", next_time, allocator);
        }
        return detail;
    }

    void time_change_details::fromJson( const rapidjson::Value &val) {

        if ( val.IsObject() ) {
            if ( val.HasMember("start_time") && val["start_time"].IsUint() ) {
                // OPTIONAL see J2735 TimeChangeDetails definition but required for CARMA Streets future phase information
                start_time =  static_cast<uint16_t>(val["start_time"].GetUint());
            }
            else {
               throw signal_phase_and_timing_exception("TimeChangeDetails is missing required start_time property!");
            }

            if (val.HasMember("min_end_time") && val["min_end_time"].IsUint() ) {
                // REQUIRED see J2735 TimeChangeDetails definition 
                min_end_time = static_cast<uint16_t>(val["min_end_time"].GetUint());
            }
            else {
               throw signal_phase_and_timing_exception("TimeChangeDetails is missing required min_end_time property!");
            }

            if (val.HasMember("max_end_time") && val["max_end_time"].IsUint() ) {
                // OPTIONAL see J2735 TimeChangeDetails definition 
                max_end_time = static_cast<uint16_t>(val["max_end_time"].GetUint());
            }

            if (val.HasMember("likely_time") &&  val["likely_time"].IsUint() ) {
                // OPTIONAL see J2735 TimeChangeDetails definition 
                likely_time = static_cast<uint16_t>(val["likely_time"].GetInt());
            }
            if ( val.HasMember("confidence") && val["confidence"].IsUint() ) {
                // OPTIONAL see J2735 TimeChangeDetails definition 
                confidence = static_cast<uint8_t>(val["confidence"].GetUint());
            }

            if (  val.HasMember("next_time") && val["next_time"].IsUint() ) {
                // OPTIONAL see J2735 TimeChangeDetails definition 
                next_time = static_cast<uint16_t>(val["next_time"].GetUint());
            }
        }
    }

    bool time_change_details::operator==(const time_change_details &other) const{
        return start_time == other.start_time && min_end_time == other.min_end_time && max_end_time == other.max_end_time
            && likely_time == other.likely_time && confidence == other.confidence;
    }

    bool time_change_details::operator!=(const time_change_details &other) const{
        return !operator==(other);
    }

    uint64_t time_change_details::convert_hour_tenth_secs2epoch_ts(uint16_t hour_tenth_secs) const{
        auto tp = std::chrono::system_clock::now();
        auto duration = tp.time_since_epoch();
        auto hours_since_epoch = std::chrono::duration_cast<std::chrono::hours>(duration).count();
        auto epoch_start_time = hours_since_epoch * HOUR_TO_SECONDS * SECOND_TO_MILLISECONDS + hour_tenth_secs * 100;
        return epoch_start_time;
    }

    uint64_t time_change_details::get_epoch_start_time() const{
        return convert_hour_tenth_secs2epoch_ts(start_time);
    }
    
    uint64_t time_change_details::get_epoch_min_end_time() const{
        return convert_hour_tenth_secs2epoch_ts(min_end_time);
    }
    
    uint64_t time_change_details::get_epoch_max_end_time() const{
        return convert_hour_tenth_secs2epoch_ts(max_end_time);
    }
    
    uint64_t time_change_details::get_epoch_next_time() const{
       return convert_hour_tenth_secs2epoch_ts(next_time);
    }
}