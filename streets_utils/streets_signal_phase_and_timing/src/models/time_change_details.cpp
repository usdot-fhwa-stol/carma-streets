#include "time_change_details.h"

namespace signal_phase_and_timing {
    
    rapidjson::Value time_change_details::toJson(rapidjson::Document::AllocatorType &allocator) const {
        // Create time change detail JSON value
        rapidjson::Value detail(rapidjson::kObjectType);
        // Populate
        // OPTIONAL see J2735 TimeChangeDetails definition but required for CARMA Streets future phase information
        if ( start_time >= 36001 ) {
           throw signal_phase_and_timing_exception("TimeChangeDetails has invalid value of " + std::to_string(min_end_time) + " for start_time property! Valid range (0-36000).");
        }
        detail.AddMember("start_time", start_time,allocator);
        // value represents unknow see J2735 spec
        if (min_end_time >= 36001 ) {
            throw signal_phase_and_timing_exception("TimeChangeDetails has invalid value of " + std::to_string(min_end_time) + " for min_end_time property! Valid range (0-36000).");
        }
        detail.AddMember("min_end_time", min_end_time,allocator);
        // value represents unknow see J2735 spec
        if (max_end_time <= 36000) {
            detail.AddMember("max_end_time",max_end_time,allocator);
        } else {
            SPDLOG_TRACE("Skipping invalid value {0} for max_end_time! Valid range (0-36000).", max_end_time);
        }
        // value represents unknow see J2735 spec
        if (likely_time <= 36000) {
            detail.AddMember("likely_time", likely_time, allocator);
        }
        else {
            SPDLOG_TRACE("Skipping invalid value {0} for likely_time! Valid range (0-36000).", likely_time);
        }
        detail.AddMember("confidence", confidence, allocator);
        // value represents unknow see J2735 spec  
        if (next_time <= 36000) {
            detail.AddMember("next_time", next_time, allocator);
        }
        else {
            SPDLOG_TRACE("Skipping invalid value {0} for next_time! Valid range (0-36000).", next_time);
        }
        return detail;
    }

    void time_change_details::fromJson( const rapidjson::Value &val) {

        if ( val.IsObject() ) {
            if ( val.HasMember("start_time") && val["start_time"].IsUint() && val["start_time"].GetUint() <= 36000 ) {
                // OPTIONAL see J2735 TimeChangeDetails definition but required for CARMA Streets future phase information
                start_time =  static_cast<uint16_t>(val["start_time"].GetUint());
            }
            else {
               throw signal_phase_and_timing_exception("TimeChangeDetails contains missing/invalid required start_time property!");
            }

            if (val.HasMember("min_end_time") && val["min_end_time"].IsUint() && val["min_end_time"].GetUint() <= 36000) {
                // REQUIRED see J2735 TimeChangeDetails definition 
                min_end_time = static_cast<uint16_t>(val["min_end_time"].GetUint());
            }
            else {
               throw signal_phase_and_timing_exception("TimeChangeDetails contains missing/invalid required min_end_time property!");
            }

            if (val.HasMember("max_end_time") && val["max_end_time"].IsUint() && val["max_end_time"].GetUint() <= 36000) {
                // OPTIONAL see J2735 TimeChangeDetails definition 
                max_end_time = static_cast<uint16_t>(val["max_end_time"].GetUint());
            }
            else if ( val.HasMember("max_end_time") && val["max_end_time"].IsUint() && val["max_end_time"].GetUint() > 36000) {
                SPDLOG_TRACE("Skipping invalid value {0} for max_end_time! Valid range (0-36000).", val["max_end_time"].GetUint());
            }

            if (val.HasMember("likely_time") &&  val["likely_time"].IsUint() && val["likely_time"].GetUint() <= 36000 ) {
                // OPTIONAL see J2735 TimeChangeDetails definition 
                likely_time = static_cast<uint16_t>(val["likely_time"].GetInt() );
            }
            else if ( val.HasMember("likely_time") && val["likely_time"].IsUint() && val["likely_time"].GetUint() > 36000) {
                SPDLOG_TRACE("Skipping invalid value {0} for likely_time! Valid range (0-36000).", val["likely_time"].GetUint());
            }
            if ( val.HasMember("confidence") && val["confidence"].IsUint() ) {
                // OPTIONAL see J2735 TimeChangeDetails definition 
                confidence = static_cast<uint8_t>(val["confidence"].GetUint());
            }
            
            if (  val.HasMember("next_time") && val["next_time"].IsUint() && val["next_time"].GetUint() <= 36000) {
                // OPTIONAL see J2735 TimeChangeDetails definition 
                next_time = static_cast<uint16_t>(val["next_time"].GetUint());
            }
            else if (val.HasMember("next_time") && val["next_time"].IsUint() && val["next_time"].GetUint() > 36000) {
                SPDLOG_TRACE("Skipping invalid value {0} for next_time! Valid range (0-36000).", val["next_time"].GetUint());
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
        auto hours_since_epoch = std::floor(streets_service::streets_clock_singleton::time_in_ms()/ (HOUR_TO_SECONDS * SECOND_TO_MILLISECONDS) );
        auto epoch_start_time = hours_since_epoch * HOUR_TO_SECONDS * SECOND_TO_MILLISECONDS + hour_tenth_secs * 100;
        return epoch_start_time;
    }

    uint16_t time_change_details::convert_msepoch_to_hour_tenth_secs(uint64_t epoch_time_ms) const{
        auto hours_since_epoch = std::floor(streets_service::streets_clock_singleton::time_in_ms()/ (HOUR_TO_SECONDS * SECOND_TO_MILLISECONDS) );
        auto hours_since_epoch_ms = hours_since_epoch * HOUR_TO_SECONDS * SECOND_TO_MILLISECONDS;
        uint64_t tenth_seconds_from_current_hour;
        if ( hours_since_epoch_ms > epoch_time_ms ) {
            SPDLOG_WARN("Epoch time provided {0} is smaller than the current UTC hour time {1}! SPaT cannot represent a time from a previous hour. Approximating time as beginning of current hour."
                , epoch_time_ms, hours_since_epoch_ms);
            tenth_seconds_from_current_hour = 0;
        }
        else {
            tenth_seconds_from_current_hour = (epoch_time_ms - hours_since_epoch_ms )/100;
        }
        // TimeMark Max value is 36001 and 36001 is reserved for invalid unknown times (see J2737 TimeMark message documentation).
        // Relevant section
        //         TimeMark ::= INTEGER (0..36001)
        //          -- Tenths of a second in the current or next hour
        //          -- In units of 1/10th second from UTC time
        //          -- A range of 0~36000 covers one hour
        //          -- The values 35991..35999 are used when a leap second occurs
        //          -- The value 36000 is used to indicate time >3600 seconds
        //          -- 36001 is to be used when value undefined or unknown
        //          -- Note that this is NOT expressed in GPS time
        //          -- or in local time
        if ( tenth_seconds_from_current_hour >= 36000) {
            SPDLOG_WARN("Approximating {0} as 36000! TimeMark maximum value is 36000!", tenth_seconds_from_current_hour);
            tenth_seconds_from_current_hour = 36000;
        }
        return static_cast<uint16_t>(tenth_seconds_from_current_hour);
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

    void time_change_details::set_start_time(uint64_t epoch_time_ms) {
        start_time = convert_msepoch_to_hour_tenth_secs(epoch_time_ms);
    }
    
    void time_change_details::set_min_end_time(uint64_t epoch_time_ms) {
        min_end_time = convert_msepoch_to_hour_tenth_secs(epoch_time_ms);
    }
    
    void time_change_details::set_max_end_time(uint64_t epoch_time_ms) {
        max_end_time =  convert_msepoch_to_hour_tenth_secs(epoch_time_ms);
    }
    
    void time_change_details::set_next_time(uint64_t epoch_time_ms) {
        next_time =  convert_msepoch_to_hour_tenth_secs(epoch_time_ms);
    }
}