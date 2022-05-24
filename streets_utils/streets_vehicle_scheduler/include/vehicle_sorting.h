#pragma once
#include "vehicle.h"
#include "scheduling_exception.h"
#include "intersection_schedule.h"

#include <vector>

namespace streets_vehicle_scheduler{

	bool departure_position_comparator(const streets_vehicles::vehicle &veh1, const streets_vehicles::vehicle &veh2) ;

	bool st_comparator(const streets_vehicles::vehicle &veh1, const streets_vehicles::vehicle &veh2) ;

	bool delay_comparator( const intersection_schedule &sched1, const intersection_schedule & sched2) ;
}