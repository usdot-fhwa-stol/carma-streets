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
}