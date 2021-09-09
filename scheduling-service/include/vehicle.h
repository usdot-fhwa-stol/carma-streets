#ifndef vehicle_H
#define vehicle_H

#include <string>
#include <iostream>
#include <vector>
#include "json.hpp"

using namespace std;
using json = nlohmann::json;

class vehicle{
	private:

		string id;
		int index;

		double length;
		double min_gap;
		double reaction_time;
		double accel_max;
		double decel_max;
		string direction;

		string entryLane_id;
		string link_id;
		int entryLane_index;
		int link_index;
		int link_priority;

		int departurePosition_index;
		int flexibility_ET;
		double flexibility_ST;
		bool access;

		double st_actual;
		double et_actual;
		double dt_actual;

		double timestamp;
		double lat;
		double lng;
		double distance;
		double speed;
		double acceleration;
		string lane_id;
		int lane_index;
		string state;

		vector<double> time_future;
		vector<string> lane_id_future;
		vector<double> distance_future;
		vector<double> lat_future;
		vector<double> lng_future;

		vector<double> speed_future;
		vector<double> accel_future;

	public:

		void update(json message);
		
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

		vector<double> get_futureTime();
		vector<string> get_futureLaneID();
		vector<double> get_futureDistance();
		vector<double> get_futureLat();
		vector<double> get_futureLng();

		vector<double> get_futureSpeed();
		vector<double> get_futureAccel();

		void set_departurePosition(int pos_index);
		void set_flexEt(int new_flex_et);
		void set_flexSt(double new_flex_st);

};

#endif