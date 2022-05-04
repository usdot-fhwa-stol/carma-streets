#pragma once

#include <rapidjson/document.h>
#include <spdlog/spdlog.h>



namespace streets_vehicles {
	struct future_information {

		/* timestamp for future point in seconds */
		double timestamp;
		/* lanelet id for future point */
		int lane_id;
		/* distance to end of lanelet */
		double distance;

	};

	enum class vehicle_state{
		EV=0,RDV=1,DV=2,LV=3,ND=-1
	};

	class vehicle{
		private:
			/* vehicle id */
			std::string id;
			/* vehicle length */
			double length;
			/* the minimum distance gap a vehicle needs to maintain from its preceding vehicle if it is stopped */
			double min_gap;
			/* vehicle's reaction time to the preceding vehicle's speed change */
			double reaction_time;
			/* maximum comfortable acceleration rate */
			double accel_max;
			/* maximum comfortable deceleration rate (it must have a negative value) */
			double decel_max;
			/* vehicle's entry lane id */
			int entry_lane_id;
			/* vehicle's departure lane id */
			int exit_lane_id;
			/* vehicle's connection link id */
			int link_id;
			/* vehicle turn direction in intersection */
			std::string direction;
			/* link lane priority */
			int link_priority;
			/* access to the intersection box */
			bool access;
			/* the departure position index of the vehicle */
			int departure_position=-1;
			/* vehicle's actual stopping time at the stop bar in seconds */
			double st_actual;
			/* vehicle's actual entering time to the intersection box in seconds */
			double et_actual;
			/*** vehicle's actual departure time from the intersection box in seconds */
			double dt_actual;
			/* the timestamp of the last update in seconds.*/
			double timestamp = 0;
			/* the distance between the vehicle and the end of its lane in the last update */
			double distance;
			/* vehicle's speed in the last update */
			double speed;
			/* vehicle's acceleration in the last update */
			double acceleration;
			/* vehicle's lane id in the last update */
			int lane_id;
			/* vehicle's state based on the last update */
			vehicle_state state = vehicle_state::ND;
			/* the estimated future path information of the vehicle */
			std::vector<future_information> future_info;
			/* mutex lock for vehicle*/
			std::mutex config_lock;

		public:
			/**
			 * @brief Default Constructor.
			 */
			vehicle() {};
			/**
			 * @brief Copy Constructor.
			 * @param veh Vehicle to copy.
			 */
			vehicle(const vehicle &veh) : 
				id(veh.id), length(veh.length), min_gap(veh.min_gap), reaction_time(veh.reaction_time), accel_max(veh.accel_max),
				decel_max(veh.decel_max), entry_lane_id(veh.entry_lane_id), link_id(veh.link_id), exit_lane_id(veh.exit_lane_id),
				lane_id(veh.lane_id), access(veh.access), departure_position(veh.departure_position), st_actual(veh.st_actual), 
				et_actual(veh.et_actual), dt_actual(veh.dt_actual), timestamp(veh.timestamp), distance(veh.distance), speed(veh.speed),
				acceleration(veh.acceleration), state(veh.state), future_info(veh.future_info) {};
			
			~vehicle() = default;
			/**
			 * @brief Copy assignment operator. Required default implementation due to non-copyable member std::mutex 
			 * config lock
			 * @param veh& Vehicle to copy.
			 * @return vehicle& Reference to this vehicle.
			 */
			vehicle& operator=(const vehicle &veh);
			/**
			 * @brief Get vehicle id.
			 * @return std::string 
			 */
			std::string get_id() const;
			/**
			 * @brief Get vehicle length.
			 * @return double 
			 */
			double get_length() const;
			/**
			 * @brief Get the min gap object
			 * 
			 * @return double 
			 */
			double get_min_gap() const;
			/**
			 * @brief Get the reaction t object
			 * 
			 * @return double 
			 */
			double get_reaction_t() const;
			/**
			 * @brief Get the accel max object
			 * 
			 * @return double 
			 */
			double get_accel_max() const;
			/**
			 * @brief Get the decel max object
			 * 
			 * @return double 
			 */
			double get_decel_max() const;
			/**
			 * @brief Get the entry lane id object
			 * 
			 * @return int 
			 */
			int get_entry_lane_id() const;
			/**
			 * @brief Get the exit lane id object
			 * 
			 * @return int 
			 */
			int get_exit_lane_id() const;
			/**
			 * @brief Get the link id object
			 * 
			 * @return int 
			 */
			int get_link_id() const;

			/**
			 * @brief Get the intended turn direction of vehicle in intersection. 
			 * 
			 * @return std::string turn direction.
			 */
			std::string get_direction() const;

			/**
			 * @brief Get the link priority object
			 * 
			 * @return int 
			 */
			int get_link_priority() const;
			/**
			 * @brief Get the access object
			 * 
			 * @return true 
			 * @return false 
			 */
			bool get_access() const;
			/**
			 * @brief Get the departure position object
			 * 
			 * @return int 
			 */
			int get_departure_position() const;
			/**
			 * @brief Get the actual st object
			 * 
			 * @return double 
			 */
			double get_actual_st() const;
			/**
			 * @brief Get the actual et object
			 * 
			 * @return double 
			 */
			double get_actual_et() const;
			/**
			 * @brief Get the actual dt in seconds.
			 * 
			 * @return double 
			 */
			double get_actual_dt() const;
			/**
			 * @brief Get the epoch time of last update in seconds.
			 * 
			 * @return double 
			 */
			double get_cur_time() const;
			/**
			 * @brief Get the curent distance from end of lanelet in meters. 
			 * 
			 * @return double
			 */
			double get_cur_distance() const;
			/**
			 * @brief Get the cur speed
			 * 
			 * @return double 
			 */
			double get_cur_speed() const;
			/**
			 * @brief Get the cur accel
			 * 
			 * @return double 
			 */
			double get_cur_accel() const;
			/**
			 * @brief Get the cur lane id 
			 * 
			 * @return int 
			 */
			int get_cur_lane_id() const;
			/**
			 * @brief Get the current state of the vehicle.
			 * 
			 * @return vehicle_state 
			 */
			vehicle_state get_cur_state() const;
			/**
			 * @brief Get the future info vector. Contains elements that represent vehicle estimated position for several future times.
			 * 
			 * @return std::vector<future_information> 
			 */
			std::vector<future_information> get_future_info() const;

			/**
			 * @brief Set vehicle id
			 * 
			 * @param id 
			 */
			void set_id(const std::string &id);
			/**
			 * @brief Set vehicle length in meters
			 * 
			 * @param length 
			 */
			void set_length(const double length);
			/**
			 * @brief Set the minimum gap in meters
			 * 
			 * @param min_gap 
			 */
			void set_min_gap(const double gap);
			/**
			 * @brief Set the reaction time in seconds.
			 * 
			 * @param reaction_t 
			 */
			void set_reaction_time(const double reaction_t);
			/**
			 * @brief Set max acceleration in m/s
			 * 
			 * @param accel_max 
			 */
			void set_accel_max(const double accel_max);
			/**
			 * @brief Set the maximum deceleration in m/s.
			 * 
			 * @param decel_max 
			 */
			void set_decel_max(const double decel_max);
			/**
			 * @brief Set the entry lane id. The entry lane id is the lanelet id in the lanelet2 map from which the vehicle
			 * is approaching the intersection.
			 * 
			 * @param entry_lane_id 
			 */
			void set_entry_lane_id(const int entry_lane_id);
			/**
			 * @brief Set the exit lane id. The exit id is the lanelet id in the lanelet2 map for the departure lanelet the
			 * vehicle intends to take leaving the intersection.
			 * 
			 * @param exit_lane_id 
			 */
			void set_exit_lane_id(const int exit_lane_id);
			/**
			 * @brief Set the link id. The link id is the lanelet id in the lanlet2 map, which the vehicle intends to take for 
			 * the linking the desired intersection approach lanelet with desired departure lanelet.
			 * 
			 * @param link_id 
			 */
			void set_link_id(const int link_id);
			/**
			 * @brief Set the direction. This string indicates the turn direction the vehicle intends to take in the intersection.
			 * 
			 * @param direction 
			 */
			void set_direction(const std::string &direction);
			/**
			 * @brief Set the link priority.
			 * 
			 * @param link_priority 
			 */
			void set_link_priority(const int link_priority);
			/**
			 * @brief Set the access for a vehicle. This flag indicates whether the vehicle is allowed by CARMA-Streets to enter the 
			 * intersection.
			 * 
			 * @param access 
			 */
			void set_access(const bool access);
			/**
			 * @brief Set the departure position. The index starting from 1 in which vehicle will be granted access to the intersection.
			 * 
			 * @param departure_position 
			 */
			void set_departure_position(const int departure_position);
			/**
			 * @brief Set the actual stopping epoch time in seconds. This is when the vehicle is first considered 
			 * stopped at the intersection stop line.
			 * 
			 * @param actual_st 
			 */
			void set_actual_st(const double actual_st);
			/**
			 * @brief Set the actual entering epoch time in seconds. This is when the vehicle first enters the intersection box.
			 * 
			 * @param actual_et 
			 */
			void set_actual_et(const double actual_et);
			/**
			 * @brief Set the actual departure epoch time in seconds. This is when the vehicle leaves the intersection box.
			 * 
			 * @param actual_dt 
			 */
			void set_actual_dt(const double actual_dt);
			/**
			 * @brief Set the epoch time of the last update in seconds.
			 * 
			 * @param cur_time 
			 */
			void set_cur_time(const double cur_time);
			/**
			 * @brief Set the current distance in meters.
			 * 
			 * @param cur_distance 
			 */
			void set_cur_distance(const double cur_distance);
			/**
			 * @brief Set the current speed in m/s.
			 * 
			 * @param cur_speed 
			 */
			void set_cur_speed(const double cur_speed);
			/**
			 * @brief Set the current acceleration in m/s^s
			 * 
			 * @param cur_accel 
			 */
			void set_cur_accel(const double cur_accel);
			/**
			 * @brief Set the current lane id. This is the id of the lanelet the vehicle is currently in within the lanelet2 map.
			 * 
			 * @param cur_lane_id 
			 */
			void set_cur_lane_id(const int cur_lane_id);
			/**
			 * @brief Set the vehicle current state.
			 * 
			 * @param state of the vehicle.
			 */
			void set_cur_state(const vehicle_state &state);
			/**
			 * @brief Set the future info vector.
			 * 
			 * @param future_information vector of future information points.
			 */
			void set_future_info(const std::vector<future_information> &future_information);


	};
}
