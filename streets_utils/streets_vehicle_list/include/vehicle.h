#pragma once

#include <rapidjson/document.h>
#include <spdlog/spdlog.h>



namespace streets_vehicles {
	struct future_information {

		/* epoch timestamp for future point in milliseconds */
		uint64_t timestamp;
		/* lanelet id for future point */
		int lane_id;
		/* distance to end of lanelet */
		double distance;

	};

	enum class vehicle_state{
		EV=0,RDV=1,DV=2,LV=3,ND=-1
	};

	struct vehicle{
		
		/* vehicle id */
		std::string _id;
		/* vehicle length */
		double _length;
		/* the minimum distance gap a vehicle needs to maintain from its preceding vehicle if it is stopped */
		double _min_gap;
		/* vehicle's reaction time to the preceding vehicle's speed change */
		double _reaction_time;
		/* maximum comfortable acceleration rate */
		double _accel_max;
		/* maximum comfortable deceleration rate (it must have a negative value) */
		double _decel_max;
		/* vehicle's lane id in the last update */
		int _cur_lane_id;
		/* vehicle's entry lane id */
		int _entry_lane_id;
		/* vehicle's departure lane id */
		int _exit_lane_id;
		/* vehicle's connection link id */
		int _link_id;
		/* vehicle turn direction in intersection */
		std::string _direction;
		/* link lane priority */
		int _link_priority;
		/* access to the intersection box */
		bool _access;
		/* the departure position index of the vehicle */
		int _departure_position=-1;
		/* vehicle's actual epoch stopping time at the stop bar in milliseconds */
		u_int64_t _actual_st;
		/* vehicle's actual epoch entering time to the intersection box in milliseconds */
		u_int64_t _actual_et;
		/*** vehicle's actual epoch departure time from the intersection box in milliseconds */
		u_int64_t _actual_dt;
		/* the epoch timestamp of the last update in milliseconds.*/
		u_int64_t _cur_time = 0;
		/* the distance between the vehicle and the end of its lane in the last update */
		double _cur_distance;
		/* vehicle's speed in the last update */
		double _cur_speed;
		/* vehicle's acceleration in the last update */
		double _cur_accel;
		/* vehicle's state based on the last update */
		vehicle_state _cur_state = vehicle_state::ND;
		/* the estimated future path information of the vehicle */
		std::vector<future_information> _future_info;

	};
}
