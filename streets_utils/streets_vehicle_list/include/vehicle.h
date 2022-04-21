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
		protected:
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
			 * @brief Constructor.
			 */
			vehicle();
			/**
			 * @brief Copy Constructor.
			 * @param veh 
			 */
			vehicle(const vehicle &veh);
			/**
			 * @brief Copy assignment operator
			 * @return vehicle& 
			 */
			vehicle& operator=(const vehicle &);
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
			 * @brief Get the actual dt object
			 * 
			 * @return double 
			 */
			double get_actual_dt() const;
			/**
			 * @brief Get the cur time object
			 * 
			 * @return double 
			 */
			double get_cur_time() const;
			/**
			 * @brief Get the cur distance object
			 * 
			 * @return double 
			 */
			double get_cur_distance() const;
			/**
			 * @brief Get the cur speed object
			 * 
			 * @return double 
			 */
			double get_cur_speed() const;
			/**
			 * @brief Get the cur accel object
			 * 
			 * @return double 
			 */
			double get_cur_accel() const;
			/**
			 * @brief Get the cur lane id object
			 * 
			 * @return int 
			 */
			int get_cur_lane_id() const;
			/**
			 * @brief Get the cur state object
			 * 
			 * @return vehicle_state 
			 */
			vehicle_state get_cur_state() const;
			/**
			 * @brief Get the future info object
			 * 
			 * @return std::vector<future_information> 
			 */
			std::vector<future_information> get_future_info() const;

			/**
			 * @brief Set the id object
			 * 
			 * @param id 
			 */
			void set_id(const std::string &id);
			/**
			 * @brief Set the length object
			 * 
			 * @param length 
			 */
			void set_length(double length);
			/**
			 * @brief Set the min gap object
			 * 
			 * @param min_gap 
			 */
			void set_min_gap(double min_gap);
			/**
			 * @brief Set the reaction time object
			 * 
			 * @param reaction_t 
			 */
			void set_reaction_time(double reaction_t);
			/**
			 * @brief Set the accel max object
			 * 
			 * @param accel_max 
			 */
			void set_accel_max(double accel_max);
			void set_decel_max(double decel_max);
			void set_entry_lane_id(int entry_lane_id);
			void set_exit_lane_id(int exit_lane_id);
			void set_link_id(int link_id);
			void set_link_priority(int link_priority);
			void set_access(bool access);
			void set_departure_position(int departure_position);
			void set_actual_st(double actual_st);
			void set_actual_et(double actual_et);
			void set_actual_dt(double actual_dt);
			void set_cur_time(double cur_time);
			void set_cur_distance(double cur_distance);
			void set_cur_speed(double cur_speed);
			void set_cur_accel(double cur_accel);
			void set_cur_lane_id(int cur_lane_id);
			void set_cur_state(const vehicle_state &state);
			void set_future_info(const std::vector<future_information> &future_information);


	};
}
