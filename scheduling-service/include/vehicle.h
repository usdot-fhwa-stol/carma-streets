#ifndef vehicle_H
#define vehicle_H

#include <string>
#include <iostream>
#include <vector>
#include "json.hpp"
#include "osm.h"
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

		//vehicle();
		void update(json message, osm localmap);
		
		//string get_id();
		double get_length(){return length;}
		double get_minGap(){return min_gap;}
		double get_reactionT(){return reaction_time;}
		double get_accelMax(){return accel_max;}
		double get_decelMax(){return decel_max;}
		
		string get_direction(){return direction;}
		string get_entryLaneID(){return entryLane_id;}
		string get_linkID(){return link_id;}
		int get_linkPriority(){return link_priority;}
		
		int get_departurePosition(){return departurePosition_index;}
		int get_flexET(){return flexibility_ET;}
		double get_flexST(){return flexibility_ST;}
		bool get_access(){return access;}

		double get_actualST(){return st_actual;}
		double get_actualET(){return et_actual;}
		double get_actualDT(){return dt_actual;}

		double get_curTime(){return timestamp;}
		double get_curLat(){return lat;}
		double get_curLng(){return lng;}
		double get_curDistance(){return distance;}
		double get_curSpeed(){return speed;}
		double get_curAccel(){return acceleration;}
		string get_curLaneID(){return lane_id;}
		string get_curState(){return state;}

		vector<double> get_futureTime(){return time_future;}
		vector<string> get_futureLaneID(){return lane_id_future;}
		vector<double> get_futureDistance(){return distance_future;}
		vector<double> get_futureLat(){return lat_future;}
		vector<double> get_futureLng(){return lng_future;}

		vector<double> get_futureSpeed(){return speed_future;}
		vector<double> get_futureAccel(){return accel_future;}

};

/*
vehicle::vehicle(){

} */

void vehicle::update(json message, osm localmap){

	speed = message["strategy_params"]["cur_speed"];
	acceleration = message["strategy_params"]["cur_accel"];
	timestamp = message["strategy_params"]["timestamp"];

	lat = message["strategy_params"]["lat"];
	lng = message["strategy_params"]["lng"];
	lane_id = message["strategy_params"]["cur_lane_id"];
	distance = message["strategy_params"]["cur_ds"];
	access = message["strategy_params"]["is_allowed"];

	st_actual = -1;
	et_actual = -1;
	dt_actual = -1;

	if (id == ""){
		
		id = message["strategy_params"]["v_id"];
		length = message["strategy_params"]["v_length"];
		min_gap = message["strategy_params"]["min_gap"];
		reaction_time = message["strategy_params"]["react_t"];	//!!!
		accel_max = message["strategy_params"]["max_accel"];
		decel_max = message["strategy_params"]["max_decel"];

		direction = message["strategy_params"]["direction"];
		entryLane_id = message["strategy_params"]["entry_lane_id"];
		link_id = message["strategy_params"]["link_lane_id"];
		//entryLane_index;
		//link_index;
		link_priority = localmap.get_lanePriority(link_id);

		departurePosition_index = 1000;
		flexibility_ET = 5;
		flexibility_ST = 15;
		access = false;

		if (lane_id == entryLane_id){
			if (distance <= 2 && speed <= 0.1){
				state = "RDV";
				st_actual = timestamp;
			} else{
				state = "EV";
			}
		} else if (lane_id == link_id){
			state = "DV";
			st_actual = timestamp - 1;
			et_actual = timestamp;
		} else{
			state = "LV";
			st_actual = timestamp - 2;
			et_actual = timestamp - 1;
			dt_actual = timestamp;
		}

	} else{

		if (state == "EV"){
			if (lane_id != entryLane_id){
				lane_id = entryLane_id;
				distance = 0.1;
			}
			if (distance <= 2 && speed <= 0.1){
				state == "RDV";
				st_actual = timestamp;
			}
			access = false;
		} else if (state == "RDV"){
			if (access == false){
				lane_id = entryLane_id;
				distance = 0.1;
			} else{
				lane_id = link_id;
				distance = localmap.get_laneLength(lane_id);
				state = "DV";
				et_actual = timestamp;
			}
		} else if (state == "DV"){
			access = true;
			if (lane_id == entryLane_id){
				lane_id = link_id;
				distance = localmap.get_laneLength(lane_id);
			} else if (lane_id != link_id){
				state = "LV";
				dt_actual = timestamp;
				access = false;
			} 
		}

	}

	// assuming the times in the future paths are actual times, not time interval from the previous time
	time_future.clear();
	lat_future.clear();
	lng_future.clear();
	lane_id_future.clear();
	distance_future.clear();
	for (int i = 0; i < message["strategy_params"]["est_paths"].size(); ++i){
		time_future.push_back(message["strategy_params"]["est_paths"][i]["ts"]);
		lat_future.push_back(message["strategy_params"]["est_paths"][i]["lat"]);
		lng_future.push_back(message["strategy_params"]["est_paths"][i]["lng"]);
		lane_id_future.push_back(message["strategy_params"]["est_paths"][i]["id"]);
		distance_future.push_back(message["strategy_params"]["est_paths"][i]["ds"]);

		double speed_c;
		double accel_c;
		if (i == 0){
			speed_c = sqrt(pow(lng_future[i] - lat, 2) + pow(lng_future[i] - lng, 2)) / (time_future[i] - timestamp);
			accel_c = speed_c - speed / (time_future[i] - timestamp);
		} else{
			speed_c = sqrt(pow(lng_future[i] - lng_future[i - 1], 2) + pow(lng_future[i] - lng_future[i - 1], 2)) / (time_future[i] - time_future[i - 1]);
			accel_c = speed_c - speed_future[i - 1] / (time_future[i] - time_future[i - 1]);
		}
		speed_future.push_back(speed_c);
		accel_future.push_back(accel_c);
	}

}


#endif