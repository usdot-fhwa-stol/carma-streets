
#include "vehicle.h"

using namespace std;
using namespace rapidjson;

extern configuration config;
//extern osm localmap;

/* */
void vehicle::update(const rapidjson::Document& message, osm& localmap){	
	
	/* the main function will check whether veh_id is included in the message or not
	*  if it is not included, this function cannot be executed!
	*/
	string veh_id = message["payload"]["v_id"].GetString();

	if (message["metadata"].HasMember("timestamp") && (double)message["metadata"]["timestamp"].GetInt64() / 1000.0 >= timestamp){
		
		/* the unit of the received speed from the message is mile per hour
		*  but the unit of the speed defined in the vehicle class is meter per second. 
		*  Therefore, a conversion has been added here.
		*/
		if (message["payload"].HasMember("cur_speed")){
			speed = message["payload"]["cur_speed"].GetDouble() * 0.44704;
		} else{
			spdlog::critical("the current speed of Vehicle {0} is missing in the received update!", veh_id);
		}

		if (message["payload"].HasMember("cur_accel")){
			acceleration = message["payload"]["cur_accel"].GetDouble();
		} else{
			spdlog::critical("the current acceleration of Vehicle {0} is missing in the received update!", veh_id);
		}

		timestamp = (double)message["metadata"]["timestamp"].GetInt64() / 1000.0;
	
		if (message["payload"].HasMember("cur_lane_id")){
			lane_id = to_string(message["payload"]["cur_lane_id"].GetInt());
		} else{
			spdlog::critical("the current lane id of Vehicle {0} is missing in the received update!", veh_id);
		}

		if (message["payload"].HasMember("cur_ds")){
			distance = message["payload"]["cur_ds"].GetDouble();
		} else{
			spdlog::critical("the current destination of Vehicle {0} from the end of the lane is missing in the received update!", veh_id);
		}
			
		if (message["payload"].HasMember("is_allowed")){
			access = message["payload"]["is_allowed"].GetBool();
		} else{
			spdlog::critical("the access status of Vehicle {0} is missing in the received update!", veh_id);
		}

		/* this if condition checks whether the vehicle has been seen before or not */
		if (id == ""){
			
			id = message["payload"]["v_id"].GetString();
			
			/* the unit of the received vehicle length from the message is centimeter without decimal places
			*  but the unit of the vehicle length defined in the vehicle class is meter with decimal places. 
			*  Therefore, a conversion has been added here.
			*/
			if (message["payload"].HasMember("v_length")){
				length = (double)message["payload"]["v_length"].GetInt() / 100;
			} else{
				spdlog::critical("the length of Vehicle {0} is missing in the received update!", veh_id);
			}
			
			if (message["payload"].HasMember("min_gap")){
				min_gap = message["payload"]["min_gap"].GetDouble();
			} else{
				spdlog::critical("the min gap of Vehicle {0} is missing in the received update!", veh_id);
			}
			
			if (message["payload"].HasMember("react_t")){
				reaction_time = message["payload"]["react_t"].GetDouble();	// !!!
			} else{
				spdlog::critical("the reaction time of Vehicle {0} is missing in the received update!", veh_id);
			}
			
			if (message["payload"].HasMember("max_accel")){
				accel_max = message["payload"]["max_accel"].GetDouble();
			} else{
				spdlog::critical("the maximum acceleration of Vehicle {0} is missing in the received update!", veh_id);
			}

			if (message["payload"].HasMember("max_decel")){
				decel_max = -message["payload"]["max_decel"].GetDouble();
			} else{
				spdlog::critical("the maximum decelration of Vehicle {0} is missing in the received update!", veh_id);
			}

			if (message["payload"].HasMember("direction")){
				direction = message["payload"]["direction"].GetString();
			} else{
				spdlog::critical("the direction of Vehicle {0} at the intersection box is missing in the received update!", veh_id);
			}
			
			if (message["payload"].HasMember("entry_lane_id")){
				entryLane_id = to_string(message["payload"]["entry_lane_id"].GetInt());
			} else{
				spdlog::critical("the entry lane id of Vehicle {0} is missing in the received update!", veh_id);
			}
			
			if (message["payload"].HasMember("link_lane_id")){
				link_id = to_string(message["payload"]["link_lane_id"].GetInt());
				link_priority = localmap.get_lanePriority(link_id);
			} else{
				spdlog::critical("the link lane id of Vehicle {0} is missing in the received update!", veh_id);
			}

			departurePosition_index = 1000;
			flexibility_ET = 5;
			flexibility_ST = 15;
			access = false;

			/* vehicle state determination */
			st_actual = -1;
			et_actual = -1;
			dt_actual = -1;
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
			
			/* vehicle state update */
			if (state == "EV"){
				if (lane_id != entryLane_id){
					lane_id = entryLane_id;
					distance = 0.1;
				}
				if (distance <= 2 && speed <= 0.1){
					state = "RDV";
					st_actual = timestamp;
				}
				access = false;
			} else if (state == "RDV"){
				if (access == false){
					lane_id = entryLane_id;
					distance = 0.1;
				} else{
					if (lane_id != link_id){
						lane_id = link_id;
						distance = localmap.get_laneLength(lane_id);
					}
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
		if (message["payload"].HasMember("est_paths")){
			
			future_info.clear();
			for (SizeType i = 0; i < message["payload"]["est_paths"].Size(); ++i){
				
				future_information fi;
	
				if (message["payload"]["est_paths"][i].HasMember("ts")){
					fi.timestamp = (double)message["payload"]["est_paths"][i]["ts"].GetInt64() / 1000;
				} else{
					spdlog::critical("the timestamp in the future path of Vehicle {0} is missing in the received update!", veh_id);
				}

				if (message["payload"]["est_paths"][i].HasMember("id")){
					fi.lane_id = to_string(message["payload"]["est_paths"][i]["id"].GetInt());
				} else{
					spdlog::critical("the lane id in the future path of Vehicle {0} is missing in the received update!", veh_id);
				}

				if (message["payload"]["est_paths"][i].HasMember("ds")){
					fi.distance = message["payload"]["est_paths"][i]["ds"].GetDouble();
				} else{
					spdlog::critical("the distance from the end of the lane in the future path of Vehicle {0} is missing in the received update!", veh_id);
				}

				double speed_c;
				double accel_c;
				if (i == 0){
					speed_c = (distance - fi.distance) / (fi.timestamp - timestamp);
					accel_c = (speed_c - speed) / (fi.timestamp - timestamp);
				} else{
					speed_c = (fi.distance - future_info[i - 1].distance) / (fi.timestamp - future_info[i - 1].timestamp);
					accel_c = (speed_c - future_info[i - 1].speed) / (fi.timestamp - future_info[i - 1].timestamp);
				}
				fi.speed = speed_c;
				fi.acceleration = accel_c;

				future_info.push_back(fi);
			}
		}
		else{
			spdlog::critical("the future paths of Vehicle {0} is missing in the received update!", veh_id);
		}

		spdlog::info("At timestamp {0}, vehicle {1} from lane {2} is in state {3} with speed {4} m/s and is {5} m far from the end of the lane!", timestamp, id, lane_id, state, speed, distance);

	}

}


/* */
string vehicle::get_id(){return id;};

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

