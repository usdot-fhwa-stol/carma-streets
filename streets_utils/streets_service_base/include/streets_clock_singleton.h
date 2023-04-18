#pragma once
#include "streets_singleton.h"
#include "carma-clock/carma_clock.h"

namespace streets_service {
    /**
     * @brief Streets Clock Singleton is a singleton that manages a CarmaClock object. The CarmaClock
     * object is created by the CARMA Time Library (https://github.com/usdot-fhwa-stol/carma-time-lib)
     * and is a wrapper object that allows for time based operation using chronos system time calls or
     * an external source for time. Its constructor takes a boolean parameter, what when set to true 
     * relies on update calls to save and store the current time. When passed false, the CarmaClock object
     * functions exactly as chrono system time calls including now() and sleep() calls, using the system time.
     * 
     * **NOTE** Before attempting to statically access any functionality in this class, call 
     * streets_clock_singleton::create(bool simulation_mode) to initialize the clock singleton in simulation mode
     * or real time mode.
     * 
     * 
     * @author Paul Bourelly
     */
    class streets_clock_singleton : public streets_singleton<fwha_stol::lib::time::CarmaClock, bool> {
        friend class streets_singleton<fwha_stol::lib::time::CarmaClock, bool>;

        public:
            /**
             * @brief Static access to the CarmaClock update method (see https://github.com/usdot-fhwa-stol/carma-time-lib).
             * Updates the current time held by the CarmaClock to the value of the time parameter (in milliseconds).
             * 
             * @throw std::invalid_arguement if called with CarmaClock not in simulation mode
             * @param time time to update to in milliseconds.
             */
            static void update( uint64_t time );
            /**
             * @brief Static access to the CarmaClock timeInMilliseconds method (see https://github.com/usdot-fhwa-stol/carma-time-lib).
             * Returns the epoch time in milliseconds. 
             * 
             * @return In simulation mode this returns the value from the last update call. 
             * If update has not been called yet this method will block until the first call to update and then return that value. 
             * When not in simulation mode return system epoch time in milliseconds.
             */
            static uint64_t time_in_ms();
            /**
             * @brief Sleep/block calling thread for provided duration in milliseconds. TODO: Add this functionality to the underlying
             * CarmaClock object
             *
             * @param ms time in milliseconds to sleep for.
             */
            static void sleep_for(uint64_t ms);
            /**
             * @brief Sleep/block calling thread until provided time in milliseconds. 
             * @param ms time in milliseconds to sleep until.
             */
            static void sleep_until(uint64_t ms);


        protected:
            // Hide get_singleton method. Use static methods instead.
            using streets_singleton::get_singleton;


    };
}