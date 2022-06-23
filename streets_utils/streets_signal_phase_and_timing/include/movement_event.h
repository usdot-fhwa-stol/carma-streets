#pragma once
#define RAPIDJSON_HAS_STDSTRING 1

#include "time_change_details.h"
#include "advisory_speed.h"

#include <rapidjson/rapidjson.h>
#include <rapidjson/document.h>
#include <spdlog/spdlog.h>
#include <list>


namespace signal_phase_and_timing{
    enum class movement_phase_state{
        /*
         * Note that based on the regions and the operating mode not every 
         * phase will be used in all transportation modes and that not 
         * every phase will be used in all transportation modes 
         */

        /**
         * @brief This state is used for unknown or error.
         */
        unavailable=0, 
        /**
         * @brief The signal head is dark(unlit).
         */
        dark=1,
 
        // Reds

        /**
         * @brief Often called 'flashing red' in US. 
         * Driver Action:
         *  Stop vehicle at stope line.
         *  Do not proceed unless it is safe.
         * Note that the right to proceed either right or left when it is safe may be contained in lane
         * description to handle what is called a 'right on red'.
         */
        stop_Then_proceed=2, 
        /**
         * @brief e.g. called 'red light' in US.
         * Driver Action:
         *  Stop vehicle at stop line.
         *  Do not proceed.
         * Note that the right to proceed either right or left when it is safe may be contained in lane 
         * description to handle what is called a 'right on red'/
         */
        stop_and_remain=3, 

        // Greens
        /**
         * @brief Not used in the US, red+yellow partly in EU
         * Driver Action:
         *  Stop vehicle.
         *  Prepare to proceed (pending green)
         *  (Prepare for transition to green/go)
         */
        pre_movement=4,
        /**
         * @brief Often called 'permissive green' un US
         * Driver Action:
         *  Proceed with caution,
         *  must yield to all conflicting traffic
         * Conflicting traffic may be present in the intersection conflict area 
         * 
         */
        permissive_movement_allowed=5,
        /**
         * @brief Often called 'protected green' in US
         * Driver Action:
         *  Proceed, tossing caution to the wind,
         *  in indicated (allowed) direction.
         * 
         */
        protected_movement_allowed=6,

        // Yellows / Ambers
        // The vehicle is not allowed to cross the stop bar if it is possible to stop without danger.

        /**
         * @brief Often called 'permissive yellow' in US
         * Driver Action:
         *  Prepare to stop.
         *  Proceed if unable to stop,
         *  Clear Intersection
         * Conflicting traffic may be present in the intersection conflict area.
         */
        permissive_clearance=7,
        /**
         * @brief Often called 'protected yellow' in US
         * Driver Action:
         *  Prepare to stop.
         *  Proceed if unable,
         *  in indicated direction (to connected lane)
         *  Clear intersection. 
         */
        protected_clearance=8,
        /**
         * @brief Often called 'flashing yellow' in US
         * Often used for extend periods of time
         * Driver Action:
         *  Proceed with caution,
         * Conflicting traffic may be present in the intersection conflict area
         * 
         */
        caution_conflicting_traffic=9
    };

    struct movement_event{
        
        /**
         * @brief Consisting of Phase state (the basic 11 states) Directional, protected or permissive state.
         */
        movement_phase_state event_state;

        /**
         * @brief Timing data in UTC time stamps for events. This includes state and min/max end times of phase
         * confidence and estimated next occurrence.
         */
        time_change_details timing;

        /**
         * @brief Various speed advisories for use by general and specific types of vehicles supporting green-wave
         * and other flow needs (units m/s)
         */
        std::list<advisory_speed> speeds;

        /**
         * @brief Serialize Movement Event object to rapidjson::Value for writing as JSON string
         * 
         * @return rapidjson::Value serialize Movement Event object
         */
        rapidjson::Value toJson(rapidjson::Document::AllocatorType &allocator) const;
        /**
         * @brief Deserialize Movement Event JSON into Movement Event object.
         * 
         * @param val Movement Event JSON.
         */
        void fromJson(const rapidjson::Value &val);

        bool operator==(const movement_event &other) const;

        bool operator!=(const movement_event &other) const;
    
    };

    
    
}