#include "vehicle_sorting.h"

namespace streets_vehicle_scheduler {
    bool departure_position_comparator(const streets_vehicles::vehicle &veh1, const streets_vehicles::vehicle &veh2){
		// -1 indicates unassigned departure position which should be last when sorted in ascending order
		if (veh1._departure_position == -1  && veh2._departure_position == -1 ) {
			return true;
		}
		else if ( veh1._departure_position > 0 && veh2._departure_position > 0) {
			return veh1._departure_position < veh2._departure_position; 
		}
		else {
			if ( veh1._departure_position < 1) {
				throw scheduling_exception("Vehicle " + veh1._id + " has invalid departure position of " + std::to_string(veh1._departure_position) + "!");
			}
			else if ( veh2._departure_position < 1 ) {
				throw scheduling_exception("Vehicle " + veh2._id + " has invalid departure position of " + std::to_string(veh2._departure_position) + "!");
			}
		}
	}

    bool st_comparator(const streets_vehicles::vehicle &veh1, const streets_vehicles::vehicle &veh2) {
		// -1 indicates unassigned departure position which should be last when sorted in ascending order
		if (veh1._actual_st == -1  || veh2._actual_st == -1 ) {
			if ( veh1._departure_position < 1)
				throw scheduling_exception("Vehicle " + veh1._id + " has invalid actual st of " + std::to_string(veh1._actual_st) + "!");
			else if ( veh2._departure_position < 1 )
				throw scheduling_exception("Vehicle " + veh2._id + " has invalid actual st of " + std::to_string(veh2._actual_st) + "!");
		}
		else {
			return veh1._departure_position < veh2._departure_position; 
		}

	}

	bool delay_comparator( const intersection_schedule &sched1, const intersection_schedule &sched2) {
		return sched1.get_delay() < sched2.get_delay();
	}
}