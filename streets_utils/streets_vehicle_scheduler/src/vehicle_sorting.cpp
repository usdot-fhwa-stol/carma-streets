#include "vehicle_sorting.h"

namespace streets_vehicle_scheduler {
    bool departure_position_comparator(const streets_vehicles::vehicle &prev_veh, const streets_vehicles::vehicle &next_veh){
        // -1 indicates unassigned departure position which should be last when sorted in ascending order
        if ((prev_veh._departure_position < 1  && next_veh._departure_position < 1) ||  prev_veh._departure_position < 1) {
            return prev_veh._departure_position > next_veh._departure_position;
        }
        else {
            return prev_veh._departure_position < next_veh._departure_position; 
        }
    }

    bool st_comparator(const streets_vehicles::vehicle &prev_veh, const streets_vehicles::vehicle &next_veh) {
        // -1 indicates unassigned departure position which should be last when sorted in ascending order
        if (prev_veh._actual_st == 0  || next_veh._actual_st == 0) {
            if ( prev_veh._actual_st < 1)
                throw scheduling_exception("Vehicle " + prev_veh._id + " has invalid actual st of " + std::to_string(prev_veh._actual_st) + "!");
            else if ( next_veh._actual_st < 1 )
                throw scheduling_exception("Vehicle " + next_veh._id + " has invalid actual st of " + std::to_string(next_veh._actual_st) + "!");
            return true;
        }
        else {
            return prev_veh._actual_st < next_veh._actual_st; 
        }

    }

    bool distance_comparator(const streets_vehicles::vehicle &prev_veh, const streets_vehicles::vehicle &next_veh ) {
        return prev_veh._cur_distance < next_veh._cur_distance;
    }

    bool delay_comparator( const std::shared_ptr<all_stop_intersection_schedule> &prev_sched, const std::shared_ptr<all_stop_intersection_schedule> &next_sched) {
        return prev_sched->get_delay() < next_sched->get_delay();
    }

    bool et_comparator( const signalized_vehicle_schedule &prev_sched,  const signalized_vehicle_schedule &next_sched ) {
        return prev_sched.et < next_sched.et;
    }

 
}