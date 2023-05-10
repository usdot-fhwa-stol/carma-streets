#include "streets_clock_singleton.h"

namespace streets_service {

    void streets_clock_singleton::update( uint64_t timestep) {
        auto &inst = get_singleton();
        inst.update(timestep);
        SPDLOG_TRACE("streets clock is updated to {0}", timestep);
    }

    uint64_t streets_clock_singleton::time_in_ms() {
        auto &inst = get_singleton();
        SPDLOG_TRACE("Calling thread is waiting on streets clock.");
        inst.wait_for_initialization();
        SPDLOG_TRACE("Thread is released after initializeation.");
        return inst.nowInMilliseconds();
    }

    void streets_clock_singleton::sleep_for(uint64_t ms ) {
        auto &inst = get_singleton();
        inst.wait_for_initialization();
        auto cur_time = inst.nowInMilliseconds();
        inst.sleep_until(cur_time + ms);
    }

    void streets_clock_singleton::sleep_until(uint64_t ms) {
        auto &inst = get_singleton();
        inst.wait_for_initialization();
        inst.sleep_until(ms);
    }
}