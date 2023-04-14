#pragma once
#include "streets_singleton.h"
#include "carma-clock/carma_clock.h"

namespace streets_service {
    class streets_clock_singleton : public streets_singleton<fwha_stol::lib::time::CarmaClock, bool> {
        friend class streets_singleton<fwha_stol::lib::time::CarmaClock, bool>;

        public:

            static void update( uint64_t time );

            static uint64_t time_in_ms();

            static void sleep_for(uint64_t ms);


        protected:
            // Hide get_singleton method. Use static methods instead.
            using streets_singleton::get_singleton;


    };
}