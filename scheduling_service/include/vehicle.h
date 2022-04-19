#pragma once

#include <string>
#include <vector>
#include <math.h>
#include <rapidjson/document.h>
#include <spdlog/spdlog.h>

#include "intersection_client.h"
#include "streets_configuration.h"

using namespace std;
using namespace rapidjson;

struct future_information {

	/***
	* @brief the unit of timestamp here is second with decimal places.
	*/
	double timestamp;

	string lane_id;

	double distance;

	double speed;

	double acceleration;
};

class vehicle{
	private:

		/* vehicle id */
		string id;

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

		/* vehicle's direction at the intersection box */
		string direction;

		/* vehicle's entry lane id */
		string entryLane_id;

		/* vehicle's departure lane id */
		string exitLane_id;

		/* vehicle's connection link id */
		string link_id;

		/* the priority of the vehicle's connection link */
		int link_priority;

		/* the departure position index of the vehicle */
		int departurePosition_index;

		/* the flexibility of the vehicle on being pushed back in the departure sequence
		*  note: it is defined as an integer variable. the integer value specifies the 
		*        maximum number of backward shifts in the departure sequence the subject
		*        vehicle is flexible to have!
		*/
		int flexibility_ET;

		/* the flexibility of the vehicle on having delay in stopping time */
		double flexibility_ST;

		/* access to the intersection box */
		bool access;

		/*** vehicle's actual stopping time at the stop bar 
		* @brief the unit of timestamp here is second with decimal places.
		*/
		double st_actual;

		/*** vehicle's actual entering time to the intersection box 
		* @brief the unit of timestamp here is second with decimal places.
		*/
		double et_actual;

		/*** vehicle's actual departure time from the intersection box 
		* @brief the unit of timestamp here is second with decimal places.
		*/
		double dt_actual;

		/*** the timestamp of the last update 
		* @brief the unit of timestamp here is second with decimal places.
		*/
		double timestamp = 0;

		/* the distance between the vehicle and the end of its lane in the last update */
		double distance;

		/* vehicle's speed in the last update */
		double speed;

		/* vehicle's acceleration in the last update */
		double acceleration;

		/* vehicle's lane id in the last update */
		string lane_id;

		/* vehicle's state based on the last update
		*  example: ET = entering vehicle, RDV = ready-to-depart vehicle, DV = departing vehicle, LV = leaving vehicle
		*/
		string state;

		/* the estimated future path information of the vehicle */
		vector<future_information> future_info;


	public:

		void update(const Document& message, intersection_client& localmap);


		string get_id() const;
		bool message_hasError(const Document& message, intersection_client& localmap);



		double get_length() const;
		double get_minGap() const;
		double get_reactionT() const;
		double get_accelMax() const;
		double get_decelMax() const;
		
		string get_direction() const;
		string get_entryLaneID() const;
		string get_exitLaneID() const;
		string get_linkID() const;
		int get_linkPriority() const;

		
		int get_departurePosition() const;
		int get_flexET() const;
		double get_flexST() const;
		bool get_access() const;

		double get_actualST() const;
		double get_actualET() const;
		double get_actualDT() const;

		double get_curTime() const;
		double get_curDistance() const;
		double get_curSpeed() const;
		double get_curAccel() const;
		string get_curLaneID() const;
		string get_curState() const;

		vector<future_information> get_futureInfo() const;

		void set_departurePosition(int pos_index);
		void set_flexEt(int new_flex_et);
		void set_flexSt(double new_flex_st);

};

