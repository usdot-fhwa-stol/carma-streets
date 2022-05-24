#include "intersection_schedule.h"

namespace streets_vehicle_scheduler {
    uint64_t intersection_schedule::get_delay() {
        uint64_t delay = 0; 
        for (auto veh_sched : vehicle_schedules ) {
            delay += veh_sched.et - veh_sched.st; 
        }
    }
}