#pragma once
#include "vehicle.h"
#include "scheduling_exception.h"
#include "intersection_schedule.h"

#include <vector>

namespace streets_vehicle_scheduler{
	/**
	 * @brief Comparator for vehicle collections. Used for sorting vehicle objects in a collection in 
	 * ascending order with respect to their departure position value. Values lower than one are treated
	 * as unset and order last after the largest positive departure position
	 * 
	 * @param prev_veh first vehicle.
	 * @param next_veh next vehicle.
	 * @return true if first should come before next. 
	 * @return false if first should not come before next.
	 */
	bool departure_position_comparator(const streets_vehicles::vehicle &prev_veh, const streets_vehicles::vehicle &next_veh) ;

	/**
	 * @brief Comparator for vehicle collections. Used for sorting vehicle objects in a collection in 
	 * ascending order with respect to their stopping time value. 
	 * 
	 * @param prev_veh first vehicle.
	 * @param next_veh next vehicle.
	 * @return true if first should come before next. 
	 * @return false if first should not come before next.
	 * @throw scheduling_exception if stopping time is 0 which is an invalid stopping time and can not be sorted.
	 */
	bool st_comparator(const streets_vehicles::vehicle &prev_veh, const streets_vehicles::vehicle &next_veh) ;
	/**
	 * @brief Comparator for vehicle collections. Used for sorting vehicle objects in a collection in 
	 * ascending order with respect to their distance value. 
	 * 
	 * @param prev_veh first vehicle.
	 * @param next_veh next vehicle.
	 * @return true if first should come before next. 
	 * @return false if first should not come before next.
	 */
	bool distance_comparator( const streets_vehicles::vehicle &prev_veh,  const streets_vehicles::vehicle &next_veh );
	/**
	 * @brief Comparator for intersection_schedule collections. Used for sorting intersection_schedule objects in a collection in 
	 * ascending order with respect to their delay value. 
	 * 
	 * @param prev_sched first intersection_schedule.
	 * @param next_sched next intersection_schedule.
	 * @return true if first should come before next. 
	 * @return false if first should not come before next.
	 */
	bool delay_comparator( const intersection_schedule &prev_sched, const intersection_schedule &next_sched) ;

}