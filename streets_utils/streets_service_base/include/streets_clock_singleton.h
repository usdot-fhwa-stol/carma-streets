#pragma once
#include "streets_singleton.h"
#include "carma-clock/carma_clock.h"

namespace streets_service {
    class streets_clock_singleton : public streets_singleton<fwha_stol::lib::time::CarmaClock, bool> {
        friend class streets_singleton<fwha_stol::lib::time::CarmaClock, bool>;

        private:
            streets_clock_singleton( bool simulation_mode );

            ~streets_clock_singleton();
        public:

            static update( uint64_t time );

            static uint64_t time_in_ms() const;

        protected:
            // Hide get_singleton method. Use static methods instead.
            using streets_singleton::get_singleton;


    };
}