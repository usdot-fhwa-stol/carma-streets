#include "streets_clock_singleton.h"

namespace streets_service {

    void streets_clock_singleton::update( uint64_t timestep) {
        auto &inst = get_singleton();
        inst.update(timestep);
    }

    uint64_t streets_clock_singleton::time_in_ms() {
        auto &inst = get_singleton();
        inst.wait_for_initialization();
        return inst.nowInMilliseconds();
    }

    void streets_clock_singleton::sleep_for(uint64_t ms ) {
        auto &inst = get_singleton();
        auto cur_time = time_in_ms();
        inst.sleep_until(cur_time + ms);
    }

    void streets_clock_singleton::sleep_until(uint64_t ms) {
        auto &inst = get_singleton();
        inst.sleep_until(ms);
    }
}