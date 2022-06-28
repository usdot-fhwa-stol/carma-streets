#include "time_change_details.h"

namespace signal_phase_and_timing {
    
    rapidjson::Value time_change_details::toJson(rapidjson::Document::AllocatorType &allocator) const {
        // Create time change detail JSON value
        rapidjson::Value detail(rapidjson::kObjectType);
        // Populate
        // OPTIONAL see J2735 TimeChangeDetails definition but required for CARMA Streets future phase information
        if ( start_time == 0 ) {
           throw signal_phase_and_timing_exception("TimeChangeDetails is missing required start_time property!"); 
        }
        detail.AddMember("start_time", start_time,allocator);
        if (min_end_time == 0 ) {
            throw signal_phase_and_timing_exception("TimeChangeDetails is missing required min_end_time property!");
        }
        detail.AddMember("min_end_time", min_end_time,allocator);
        detail.AddMember("max_end_time",max_end_time,allocator);
        detail.AddMember("likely_time", likely_time, allocator);
        detail.AddMember("confidence", confidence, allocator);
        return detail;
    }

    void time_change_details::fromJson( const rapidjson::Value &val) {
        if ( val.IsObject() ) {
            if ( val.FindMember("start_time")->value.IsUint64() ) {
                // OPTIONAL see J2735 TimeChangeDetails definition but required for CARMA Streets future phase information
                start_time =  val["start_time"].GetUint64();
            }
            else {
               throw signal_phase_and_timing_exception("TimeChangeDetails is missing required start_time property!");
            }
            if (val.FindMember("min_end_time")->value.IsUint64() ) {
                // REQUIRED see J2735 TimeChangeDetails definition 
                min_end_time = val["min_end_time"].GetUint64();
            }
            else {
               throw signal_phase_and_timing_exception("TimeChangeDetails is missing required min_end_time property!");
            }
            if (val.FindMember("max_end_time")->value.IsUint64() ) {
                // OPTIONAL see J2735 TimeChangeDetails definition 
                max_end_time = val["max_end_time"].GetUint64();
            }
            if (val.FindMember("likely_time")->value.IsUint64() ) {
                // OPTIONAL see J2735 TimeChangeDetails definition 
                likely_time = val["likely_time"].GetUint64();
            }
            if (val.FindMember("confidence")->value.IsUint() ) {
                // OPTIONAL see J2735 TimeChangeDetails definition 
                confidence = val["confidence"].GetUint();
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
}