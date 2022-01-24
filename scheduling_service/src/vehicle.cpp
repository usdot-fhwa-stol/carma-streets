
#include "vehicle.h"

using namespace std;
using namespace rapidjson;


/* */
void vehicle::update(const rapidjson::Document& message, intersection_client& localmap, const configuration& config){
	
	/* the main function will check whether veh_id is included in the message or not
	*  if it is not included, this function cannot be executed!
	*/
	string veh_id = message["payload"]["v_id"].GetString();

	if (message["metadata"].HasMember("timestamp") && (double)message["metadata"]["timestamp"].GetInt64() / 1000.0 >= timestamp){
		
		/* the unit of the received speed from the message is 0.02 of meter per second
		*  the unit of the speed defined in the vehicle class is meter per second. 
		*/
		if (message["payload"].HasMember("cur_speed")){
			if (message["payload"]["cur_speed"].GetDouble() * 0.02 > config.get_maxValidSpeed() && id != "" && message["payload"].HasMember("cur_ds") && message["payload"].HasMember("cur_lane_id")){
				if (to_string(message["payload"]["cur_lane_id"].GetInt()) == lane_id){
					speed = (distance - message["payload"]["cur_ds"].GetDouble()) / (((double)message["metadata"]["timestamp"].GetInt64() / 1000.0) - timestamp);
				}
				else{
					speed = (distance + (localmap.get_laneLength(to_string(message["payload"]["cur_lane_id"].GetInt())) - message["payload"]["cur_ds"].GetDouble())) / (((double)message["metadata"]["timestamp"].GetInt64() / 1000.0) - timestamp);
				}
				double invalid_speed = message["payload"]["cur_speed"].GetDouble() * 0.02;
				spdlog::critical("Invalid speed in the message received from {0}: {1}", veh_id, invalid_speed);
			}
			else{
				speed = message["payload"]["cur_speed"].GetDouble() * 0.02;
			}
		} else{
			spdlog::critical("the current speed of Vehicle {0} is missing in the received update!", veh_id);
		}

		if (message["payload"].HasMember("cur_accel")){
			acceleration = message["payload"]["cur_accel"].GetDouble();
		} else{
			spdlog::critical("the current acceleration of Vehicle {0} is missing in the received update!", veh_id);
		}

		/* the unit of the received timestamp from the message is milisecond without decimal places
		*  but the unit of the speed defined in the vehicle class is second with decimal places. 
		*  Therefore, a conversion has been added here.
		*/
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

		if (message["payload"].HasMember("entry_lane_id") && message["payload"]["entry_lane_id"].GetInt() != 0){
			entryLane_id = to_string(message["payload"]["entry_lane_id"].GetInt());
		} else{
			spdlog::critical("the entry lane id of Vehicle {0} is missing in the received update!", veh_id);
		}

		if (message["payload"].HasMember("dest_lane_id") && message["payload"]["dest_lane_id"].GetInt() != 0){
			exitLane_id = to_string(message["payload"]["dest_lane_id"].GetInt());
		} else{
			spdlog::critical("the exit lane id of Vehicle {0} is missing in the received update!", veh_id);
		}
		
		if (message["payload"].HasMember("link_lane_id") && message["payload"]["link_lane_id"].GetInt() != 0){
			link_id = to_string(message["payload"]["link_lane_id"].GetInt());
			link_priority = localmap.get_lanePriority(link_id);
		} else{
			spdlog::critical("the link lane id of Vehicle {0} is missing in the received update!", veh_id);
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
				decel_max = message["payload"]["max_decel"].GetDouble();
			} else{
				spdlog::critical("the maximum decelration of Vehicle {0} is missing in the received update!", veh_id);
			}

			if (message["payload"].HasMember("direction")){
				direction = message["payload"]["direction"].GetString();
			} else{
				spdlog::critical("the direction of Vehicle {0} at the intersection box is missing in the received update!", veh_id);
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
				if (distance <= config.get_stopDistance() + length && speed <= config.get_stopSpeed()){
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
					spdlog::critical("Vehicle Class Issue - The lane ID of vehicle {0} is not correct! Vehicle {0} is in EV state, but its lane ID in the status and intent message is a link ID; lane_id: {1}, entry_lane_id: {2}, link_id: {3}", veh_id, lane_id, entryLane_id, link_id);
				}
				else if (distance <= config.get_stopDistance() + length && speed <= config.get_stopSpeed()){
					state = "RDV";
					st_actual = timestamp;
				}
				access = false;
			} else if (state == "RDV"){
				if (access == false){
					lane_id = entryLane_id;
					// distance = 0.1;
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
			
			/* the first point in the future path is the current point */
			future_info.clear();
			future_information fi;
			fi.timestamp = timestamp;
			fi.lane_id = lane_id;
			fi.distance = distance;
			fi.speed = speed;
			fi.acceleration = acceleration;
			future_info.push_back(fi);
			if (message["payload"]["est_paths"].Size() > 1){
				for (SizeType i = 0; i < message["payload"]["est_paths"].Size(); ++i){
					
					/* adding checks to make sure the necessary data exist in the future point */
					if (message["payload"]["est_paths"][i].HasMember("ts") && message["payload"]["est_paths"][i].HasMember("id") && message["payload"]["est_paths"][i].HasMember("ds")){
						
						/* adding checks to make sure only valid future points will be saved */
						if (message["payload"]["est_paths"][i]["id"].GetInt() == 0){
							spdlog::critical("the lane id in the future path of Vehicle {0} is invalid in the received update!", veh_id);
						}
						else if (message["payload"]["est_paths"][i]["ds"].GetDouble() < 0){
							spdlog::critical("the distance to the end of lane in the future path of Vehicle {0} is invalid in the received update!", veh_id);
						}
						else{


							/* the future path received from CARMA Platform does not consider the stopping requirement at the stop bar.
							*  therefore, CARMA Streets will force the stopping requirement to the vehicle's future path.
							*  basically, if the vehicle is an EV, or and RDV without access, if the vehicle lane id in the future path is not the same as the vehicle entry lane id, the scheduling service will ignore the future points.
							* */
							if ((state == "EV" || (state == "RDV" && access == false)) && (to_string(message["payload"]["est_paths"][i]["id"].GetInt()) != entryLane_id || future_info[future_info.size() - 1].distance < message["payload"]["est_paths"][i]["ds"].GetDouble())){
								spdlog::critical("Vehicle Class log - vehicle {0}: est_path point is not located in an entry lane!", veh_id);
								break;
							}
							else{
								
								fi = future_information();
								fi.timestamp = (double)message["payload"]["est_paths"][i]["ts"].GetInt64() / 1000;
								if (future_info[future_info.size() - 1].distance >= message["payload"]["est_paths"][i]["ds"].GetDouble()) {
									fi.distance = future_info[future_info.size() - 1].distance - message["payload"]["est_paths"][i]["ds"].GetDouble();
									fi.lane_id = to_string(message["payload"]["est_paths"][i]["id"].GetInt());
								} else{
									if (future_info[future_info.size() - 1].lane_id == entryLane_id){
										if (future_info[future_info.size() - 1].distance + localmap.get_laneLength(link_id) >= message["payload"]["est_paths"][i]["ds"].GetDouble()){
											fi.distance = future_info[future_info.size() - 1].distance + localmap.get_laneLength(link_id) - message["payload"]["est_paths"][i]["ds"].GetDouble();
											fi.lane_id = link_id;
										} 
										else if (future_info[future_info.size() - 1].distance + localmap.get_laneLength(link_id) + localmap.get_laneLength(exitLane_id) >= message["payload"]["est_paths"][i]["ds"].GetDouble()){
											fi.distance = future_info[future_info.size() - 1].distance + localmap.get_laneLength(link_id) + localmap.get_laneLength(exitLane_id) - message["payload"]["est_paths"][i]["ds"].GetDouble();
											fi.lane_id = exitLane_id;
										}
										else {
											break;
										}
									}
									else if (future_info[future_info.size() - 1].lane_id == link_id){
										if (future_info[future_info.size() - 1].distance + localmap.get_laneLength(exitLane_id) >= message["payload"]["est_paths"][i]["ds"].GetDouble()){
											fi.distance = future_info[future_info.size() - 1].distance + localmap.get_laneLength(exitLane_id) - message["payload"]["est_paths"][i]["ds"].GetDouble();
											fi.lane_id = exitLane_id;
										}
										else {
											break;
										}
									} 
									else{
										break;
									}

								}

								fi.speed = message["payload"]["est_paths"][i]["ds"].GetDouble() / (fi.timestamp - future_info[future_info.size() - 1].timestamp);
								fi.acceleration = (fi.speed - future_info[future_info.size() - 1].speed) / (fi.timestamp - future_info[future_info.size() - 1].timestamp);

								spdlog::debug("future path {0}: {1}, {2}, {3}, {4}", i, fi.lane_id, fi.distance, fi.speed, fi.acceleration);
								future_info.push_back(fi);
							}


						}
					}
					else{
						spdlog::critical("a point in the future path of Vehicle {0} is not complete in the received update!", veh_id);
					}
				}
			}
		}
		else{
			spdlog::critical("the future paths of Vehicle {0} is missing in the received update!", veh_id);
		}

		spdlog::info("Vehicle Class Vehicle Info Update - timestamp = {0}, vehicle = {1}, lane_id = {2}, state = {3}, speed = {4} m/s, distance = {5} m, access = {6}, departure position index = {7}", timestamp, id, lane_id, state, speed, distance, access, departurePosition_index);

	}

}

/* */
bool vehicle::message_hasError(const Document& message, intersection_client& localmap){


	if (!message.HasMember("payload")){
		spdlog::critical("payload is missing in the received status and intent update!");
		return true;
	} 
	else if (!message["payload"].HasMember("v_id")){
		spdlog::critical("vehicle id is missing in the received status and intent update!");
		return true;
	}
	else if (!message["payload"].HasMember("cur_lane_id") || message["payload"]["cur_lane_id"].GetInt() == 0){
		string veh_id = message["payload"]["v_id"].GetString();
		spdlog::critical("the current lane id of Vehicle {0} is missing in the received update!", veh_id);
		return true;
	}
	/* if it is the first time processing the vehicle update, the entry_lane_id and link_lane_id must be valid. */
	else if (id == ""){
		string veh_id = message["payload"]["v_id"].GetString();
		if (!message["payload"].HasMember("entry_lane_id") || message["payload"]["entry_lane_id"].GetInt() == 0){
			spdlog::critical("the entry lane id of Vehicle {0} is missing in the received update!", veh_id);
			return true;
		}
		else if (!message["payload"].HasMember("link_lane_id") || message["payload"]["link_lane_id"].GetInt() == 0){
			spdlog::critical("the link lane id of Vehicle {0} is missing in the received update!", veh_id);
			return true;
		}
		else if (message["payload"]["cur_lane_id"].GetInt() != message["payload"]["entry_lane_id"].GetInt() && message["payload"]["cur_lane_id"].GetInt() != message["payload"]["link_lane_id"].GetInt()){
			spdlog::critical("Vehicle {0} is not in the communication area yet!", veh_id);
			return true;
		}
		else{
			return false;
		}
	} 
	/* if a vehicle update has been succesfully processed before, the vehicle cur_lane_id must be the same as entry_lane_id, link_lane_id, or depart_lane_id . */
	else{
		string veh_id = message["payload"]["v_id"].GetString();
		if (to_string(message["payload"]["cur_lane_id"].GetInt()) != entryLane_id && to_string(message["payload"]["cur_lane_id"].GetInt()) != link_id && to_string(message["payload"]["cur_lane_id"].GetInt()) != exitLane_id){
			
			spdlog::critical("the current lane id of Vehicle {0} is not correct! entry_lane_id: {1}, link_lane_id: {2}, exit_lane_id: {3}, cur_lane_id: {4}", veh_id, entryLane_id, link_id, exitLane_id, to_string(message["payload"]["cur_lane_id"].GetInt()));
			return true;
		}
		else{
			return false;
		}
	}

}

/* */
string vehicle::get_id() const {return id;};

/* */
double vehicle::get_length() const {return length;}

/* */
double vehicle::get_minGap() const {return min_gap;}

/* */
double vehicle::get_reactionT() const {return reaction_time;}

/* */
double vehicle::get_accelMax() const {return accel_max;}

/* */
double vehicle::get_decelMax() const {return decel_max;}

/* */
string vehicle::get_direction() const {return direction;}

/* */
string vehicle::get_entryLaneID() const {return entryLane_id;}

/* */
string vehicle::get_linkID() const {return link_id;}

/* */
string vehicle::get_exitLaneID() const {return exitLane_id;}

/* */
int vehicle::get_linkPriority() const {return link_priority;}

/* */
int vehicle::get_departurePosition() const {return departurePosition_index;}

/* */
int vehicle::get_flexET() const {return flexibility_ET;}

/* */
double vehicle::get_flexST() const {return flexibility_ST;}

/* */
bool vehicle::get_access() const {return access;}

/* */
double vehicle::get_actualST() const {return st_actual;}

/* */
double vehicle::get_actualET() const {return et_actual;}

/* */
double vehicle::get_actualDT() const {return dt_actual;}

/* */
double vehicle::get_curTime() const {return timestamp;}

/* */
double vehicle::get_curDistance() const {return distance;}

/* */
double vehicle::get_curSpeed() const {return speed;}

/* */
double vehicle::get_curAccel() const {return acceleration;}

/* */
string vehicle::get_curLaneID() const {return lane_id;}

/* */
string vehicle::get_curState() const {return state;}

/* */
vector<future_information> vehicle::get_futureInfo() const {return future_info;}

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

