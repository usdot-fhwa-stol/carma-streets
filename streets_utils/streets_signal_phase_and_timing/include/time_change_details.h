#pragma once
#include "streets_clock_singleton.h"
#include "signal_phase_and_timing_exception.h"
#include <rapidjson/rapidjson.h>
#include <rapidjson/document.h>
#include <spdlog/spdlog.h>


namespace signal_phase_and_timing{
    struct time_change_details{
        int HOUR_TO_SECONDS = 3600;
        int SECOND_TO_MILLISECONDS = 1000;
        /**
         * @brief When this phase first started. (tenths of a second in current or next hour)
         */
        uint16_t start_time = 36001;
        /**
         * @brief Expected shortest end time. (tenths of a second in current or next hour)
         */
        uint16_t min_end_time = 36001;
        /**
         * @brief Expected longest end time.  (tenths of a second in current or next hour)
         */
        uint16_t max_end_time = 36001;
        /**
         * @brief Best predicted value based on other data (tenths of a second in current or next hour)
         */
        uint16_t likely_time = 36001;
        /**
         * @brief Applies to the above time interval only
         * -- Value   Probability
         * --   0         21%
         * --   1         36%
         * --   2         47%
         * --   3         56%
         * --   4         62%
         * --   5         68%
         * --   6         73%
         * --   7         77%
         * --   8         81%
         * --   9         85%
         * --   10        88%
         * --   11        91%
         * --   12        94%
         * --   13        96%
         * --   14        98%
         * --   15        100%
         */
        uint8_t confidence = 0;

        /**
         * @brief A rough estimate of time when this phase may next occur. (tenths of a second in current or next hour)
         */
        uint16_t next_time = 36001;
        /**
         * @brief Serialize Time Change Details object to rapidjson::Value for writing as JSON string
         * 
         * @return rapidjson::Value serialize Time Change Details object
         */
        rapidjson::Value toJson(rapidjson::Document::AllocatorType &allocator) const;
        /**
         * @brief Deserialize Time Change Details JSON into Time Change Details object.
         * 
         * @param val Time Change Details JSON.
         */
        void fromJson(const rapidjson::Value &val);
        /**
         * @brief Equals operator to asses whether two objects contain equivalent data.
         * 
         * @param compare second object to compare current object with.
         * @return true if both object contain equivalent data.
         * @return false if not.
         */
        bool operator==(const time_change_details &other) const;
        /**
         * @brief Returns the inverse of equals operator.
         * 
         * @param other second object to compare current object with.
         * @return true if both objects do not contain equivalent data.
         * @return false if both objects do contain equivalent data.
         */
        bool operator!=(const time_change_details &other) const;
        /**
         * @brief Convert current hours of the year and milliseconds of the current hours into epoch timestamp in milliseconds
         * 
         * @param hour_tenth_secs  The tenth of seconds of the current hour
         * @return uint64_t epoch timestamp in milliseconds
         */
        uint64_t convert_hour_tenth_secs2epoch_ts(uint16_t hour_tenth_secs) const;

        /**
         * @brief Get the epoch timestamp of the start timestamp 
         * 
         * @return ** uint64_t epoch timestamp in unit of milliseconds
         */
        uint64_t get_epoch_start_time() const;
        /**
         * @brief Get the epoch timestamp of the min end timestamp 
         * 
         * @return ** uint64_t epoch timestamp in unit of milliseconds
         */
        uint64_t get_epoch_min_end_time() const;
        /**
         * @brief Get the epoch timestamp of the max end timestamp 
         * 
         * @return ** uint64_t epoch timestamp in unit of milliseconds
         */
        uint64_t get_epoch_max_end_time() const;
        /**
         * @brief Get the epoch timestamp of the next end timestamp  
         * 
         * @return ** uint64_t epoch timestamp in unit of milliseconds
         */
        uint64_t get_epoch_next_time() const;
        /**
         * @brief Convert epoch timestamp in milliseconds into tenths of seconds of the current UTC hour. Any value that
         * results in tenths of seconds greater than 36000 will be set to 36000 (See TimeMark J2735 definition).
         *
         * @param hour_tenth_secs  The tenth of seconds of the current hour
         * @return uint64_t epoch timestamp in milliseconds
         */
        uint16_t convert_msepoch_to_hour_tenth_secs(uint64_t epoch_time_ms) const;
        /**
         * @brief Set start_time for movement_event. Method expects epoch time in milliseconds and converts
         * to tenths of seconds from current hour. Any value that results in tenths of seconds greater than 36000
         *  will be set to 36000 (See TimeMark J2735 definition).
         * 
         * @param epoch_time_ms start_time for movement_event in millisecond epoch time.
         */
        void set_start_time(uint64_t epoch_time_ms);
        
        /**
         * @brief Set min_end_time for movement event. Method expects epoch time in milliseconds and converts
         * to tenths of seconds from current hour. Any value that results in tenths of seconds greater than 36000
         * will be set to 36000 (See TimeMark J2735 definition).
         * 
         * @param epoch_time_ms min_end time for movement_event in millisecond epoch time.
         */
        void set_min_end_time(uint64_t epoch_time_ms);
        
        /**
         * @brief Set min_end_time for movement event. Method expects epoch time in milliseconds and converts
         * to tenths of seconds from current hour. Any value that results in tenths of seconds greater than 36000
         * will be set to 36000 (See TimeMark J2735 definition).
         * 
         * @param epoch_time_ms max_end time for movement_event in millisecond epoch time.
         */
        void  set_max_end_time(uint64_t epoch_time_ms);
        /**
         * @brief Set next_time for movement event. Method expects epoch time in milliseconds and converts
         * to tenths of seconds from current hour. Any value that results in tenths of seconds greater than 36000
         *  will be set to 36000 (See TimeMark J2735 definition).
         * 
         * @param epoch_time_ms next_time for movement_event in millisecond epoch time.
         */
        void set_next_time(uint64_t epoch_time_ms);
    };

}