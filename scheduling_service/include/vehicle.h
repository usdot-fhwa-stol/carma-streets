#ifndef vehicle_H
#define vehicle_H

#include <string>
#include <vector>

#include "rapidjson/document.h"
#include "spdlog/spdlog.h"
#include "spdlog/cfg/env.h"

using namespace std;
using namespace rapidjson;

struct future_information {

	double timestamp;

	double lat;

	double lng;

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

		/* vehicle's actual stopping time at the stop bar */
		double st_actual;

		/* vehicle's actual entering time to the intersection box */
		double et_actual;

		/* vehicle's actual departure time from the intersection box */
		double dt_actual;

		/* the timestamp of the last update */
		double timestamp = 0;

		/* the vehicle's latitude in the last update */
		double lat;

		/* the vehicle's longitude in the last update */
		double lng;

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

		void update(const Document& message);

		double get_length();
		double get_minGap();
		double get_reactionT();
		double get_accelMax();
		double get_decelMax();
		
		string get_direction();
		string get_entryLaneID();
		string get_linkID();
		int get_linkPriority();
		
		int get_departurePosition();
		int get_flexET();
		double get_flexST();
		bool get_access();

		double get_actualST();
		double get_actualET();
		double get_actualDT();

		double get_curTime();
		double get_curLat();
		double get_curLng();
		double get_curDistance();
		double get_curSpeed();
		double get_curAccel();
		string get_curLaneID();
		string get_curState();

		vector<future_information> get_futureInfo();

		void set_departurePosition(int pos_index);
		void set_flexEt(int new_flex_et);
		void set_flexSt(double new_flex_st);

};

#endif