
#include <string>
#include <vector>
#include <math.h>

#include "rapidjson/document.h"

#include "configuration.h"
#include "osm.h"
#include "vehicle.h"

using namespace std;
using namespace rapidjson;

extern configuration config;
extern osm localmap;


/* */
void vehicle::update(rapidjson::Document &message){
	
	speed = message["payload"]["cur_speed"].GetDouble();
	acceleration = message["payload"]["cur_accel"].GetDouble();
	timestamp = message["payload"]["timestamp"].GetDouble();

	lat = message["payload"]["lat"].GetDouble();
	lng = message["payload"]["lng"].GetDouble();
	lane_id = message["payload"]["cur_lane_id"].GetString();
	distance = message["payload"]["cur_ds"].GetDouble();
	access = message["payload"]["is_allowed"].GetBool();

	st_actual = -1;
	et_actual = -1;
	dt_actual = -1;

	if (id == ""){
		
		id = message["payload"]["v_id"].GetString();
		length = message["payload"]["v_length"].GetDouble();
		min_gap = message["payload"]["min_gap"].GetDouble();
		reaction_time = message["payload"]["react_t"].GetDouble();	// !!!
		accel_max = message["payload"]["max_accel"].GetDouble();
		decel_max = message["payload"]["max_decel"].GetDouble();

		direction = message["payload"]["direction"].GetString();
		entryLane_id = message["payload"]["entry_lane_id"].GetString();
		link_id = message["payload"]["link_lane_id"].GetString();
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
	future_info.clear();
	for (SizeType i = 0; i < message["payload"]["est_paths"].Size(); ++i){
		
		future_information fi;
		fi.timestamp = message["payload"]["est_paths"][i]["ts"].GetDouble();
		fi.lat = message["payload"]["est_paths"][i]["lat"].GetDouble();
		fi.lng = message["payload"]["est_paths"][i]["lng"].GetDouble();
		fi.lane_id = message["payload"]["est_paths"][i]["id"].GetString();
		fi.distance = message["payload"]["est_paths"][i]["ds"].GetDouble();
		
		double speed_c;
		double accel_c;
		if (i == 0){
			speed_c = sqrt(pow(fi.lat - lat, 2) + pow(fi.lng - lng, 2)) / (fi.timestamp - timestamp);
			accel_c = (speed_c - speed) / (fi.timestamp - timestamp);
		} else{
			speed_c = sqrt(pow(fi.lat - future_info[i - 1].lat, 2) + pow(fi.lng - future_info[i - 1].lng, 2)) / (fi.timestamp - future_info[i - 1].timestamp);
			accel_c = (speed_c - future_info[i - 1].speed) / (fi.timestamp - future_info[i - 1].timestamp);
		}
		fi.speed = speed_c;
		fi.acceleration = accel_c;

		future_info.push_back(fi);
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
vector<future_information> vehicle::get_futureInfo(){return future_info;}

/* */
void vehicle::set_departurePosition(int pos_index){
	departurePosition_index = pos_index;
}

/* */
void vehicle::set_flexEt(int new_flex_et){
	flexibility_ET = new_flex_et;
}

/* */
void vehicle::set_flexSt(double new_flex_st){
	flexibility_ST = new_flex_st;	
}

