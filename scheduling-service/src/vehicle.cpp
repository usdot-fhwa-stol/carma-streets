
#include <string>
#include <iostream>
#include <vector>
#include "json.hpp"

#include "configuration.h"
#include "osm.h"
#include "vehicle.h"

using namespace std;
using json = nlohmann::json;

extern configuration config;
extern osm localmap;


/* */
void vehicle::update(json message){

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
		reaction_time = message["strategy_params"]["react_t"];	// !!!
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

/* */
double vehicle::get_length(){return length;}

/* */
double vehicle::get_minGap(){return min_gap;}

/* */
double vehicle::get_reactionT(){return reaction_time;}

/* */
double vehicle::get_accelMax(){return accel_max;}

/* */
double vehicle::get_decelMax(){return decel_max;}

/* */
string vehicle::get_direction(){return direction;}

/* */
string vehicle::get_entryLaneID(){return entryLane_id;}

/* */
string vehicle::get_linkID(){return link_id;}

/* */
int vehicle::get_linkPriority(){return link_priority;}

/* */
int vehicle::get_departurePosition(){return departurePosition_index;}

/* */
int vehicle::get_flexET(){return flexibility_ET;}

/* */
double vehicle::get_flexST(){return flexibility_ST;}

/* */
bool vehicle::get_access(){return access;}

/* */
double vehicle::get_actualST(){return st_actual;}

/* */
double vehicle::get_actualET(){return et_actual;}

/* */
double vehicle::get_actualDT(){return dt_actual;}

/* */
double vehicle::get_curTime(){return timestamp;}

/* */
double vehicle::get_curLat(){return lat;}

/* */
double vehicle::get_curLng(){return lng;}

/* */
double vehicle::get_curDistance(){return distance;}

/* */
double vehicle::get_curSpeed(){return speed;}

/* */
double vehicle::get_curAccel(){return acceleration;}

/* */
string vehicle::get_curLaneID(){return lane_id;}

/* */
string vehicle::get_curState(){return state;}

/* */
vector<double> vehicle::get_futureTime(){return time_future;}

/* */
vector<string> vehicle::get_futureLaneID(){return lane_id_future;}

/* */
vector<double> vehicle::get_futureDistance(){return distance_future;}

/* */
vector<double> vehicle::get_futureLat(){return lat_future;}

/* */
vector<double> vehicle::get_futureLng(){return lng_future;}

/* */
vector<double> vehicle::get_futureSpeed(){return speed_future;}

/* */
vector<double> vehicle::get_futureAccel(){return accel_future;}

/* */
void vehicle::set_flexEt(int new_flex_et){
	flexibility_ET = new_flex_et;
}

/* */
void vehicle::set_flexSt(double new_flex_st){
	flexibility_ST = new_flex_st;	
}