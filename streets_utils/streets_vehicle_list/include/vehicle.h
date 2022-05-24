#pragma once

#include <rapidjson/document.h>
#include <spdlog/spdlog.h>



namespace streets_vehicles {
	/**
	 * @brief Future path information for vehicles.
	 * 
	 */
	struct future_information {
		/* epoch timestamp for future point in milliseconds */
		uint64_t timestamp;
		/* lanelet id for future point */
		int lane_id;
		/* distance to end of lanelet (m)*/
		double distance;

	};
	/**
	 * @brief Vehicle State Enum. EV (Entering Vehicle), RDV (Ready to Depart Vehicle),
	 * DV (Departing Vehicle), LV (Leaving Vehicle), ND (Not Defined).
	 * 
	 */
	enum class vehicle_state{
		EV=0,RDV=1,DV=2,LV=3,ND=-1
	};
	/**
	 * @brief Data struct for vehicle status and intent information.
	 * 
	 */
	struct vehicle{
		
		/* vehicle id */
		std::string _id;
		/* vehicle length (m) */
		double _length = 0.0;
		/* the minimum distance gap a vehicle needs to maintain from its preceding vehicle if it is stopped (m) */
		double _min_gap = 0.0;
		/* vehicle's reaction time to the preceding vehicle's speed change (s)*/
		double _reaction_time = 0.0;
		/* maximum comfortable acceleration rate (m/s^2)*/
		double _accel_max = 0.0;
		/* maximum comfortable deceleration rate (it must have a negative value) (m/s^2) */
		double _decel_max = 0.0;
		/* vehicle's lane id in the last update */
		int _cur_lane_id = 0;
		/* vehicle's entry lane id */
		int _entry_lane_id = 0;
		/* vehicle's departure lane id */
		int _exit_lane_id = 0;
		/* vehicle's connection link id */
		int _link_id = 0;
		/* vehicle turn direction in intersection */
		std::string _direction;
		/* link lane priority */
		int _link_priority;
		/* access to the intersection box */
		bool _access = false;
		/* the departure position index of the vehicle */
		int _departure_position=-1;
		/* vehicle's actual epoch stopping time at the stop bar in milliseconds */
		u_int64_t _actual_st = 0;
		/* vehicle's actual epoch entering time to the intersection box in milliseconds */
		u_int64_t _actual_et = 0;
		/*** vehicle's actual epoch departure time from the intersection box in milliseconds */
		u_int64_t _actual_dt = 0;
		/* the epoch timestamp of the last update in milliseconds.*/
		u_int64_t _cur_time = 0;
		/* the distance between the vehicle and the end of its lane in the last update (m)*/
		double _cur_distance = 0.0;
		/* vehicle's speed in the last update (m/s) */
		double _cur_speed = 0.0;
		/* vehicle's acceleration in the last update (m/s^2) */
		double _cur_accel = 0.0;
		/* vehicle's state based on the last update */
		vehicle_state _cur_state = vehicle_state::ND;
		/* the estimated future path information of the vehicle */
		std::vector<future_information> _future_info;

	};
}
