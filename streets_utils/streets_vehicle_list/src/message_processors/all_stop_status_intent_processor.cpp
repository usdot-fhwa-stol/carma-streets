#include "all_stop_status_intent_processor.h"

namespace streets_vehicles {

	void all_stop_status_intent_processor::from_json(const rapidjson::GenericObject<true,rapidjson::Value> &json, vehicle &vehicle) const{
		
		/* the main function will check whether veh_id is included in the message or not
		*  if it is not included, this function cannot be executed!
		*/
		// Get Meta Data
		if (json.FindMember("metadata")->value.IsObject()){
			rapidjson::GenericObject<true, rapidjson::Value> metadata = json.FindMember("metadata")->value.GetObject();
			if (json.FindMember("payload")->value.IsObject()){
				rapidjson::GenericObject<true, rapidjson::Value> payload = json.FindMember("payload")->value.GetObject();
				read_metadata( metadata, vehicle );
				read_payload( payload, vehicle );  
				if (payload.FindMember("est_path")->value.IsArray()) {
					rapidjson::GenericArray<true, rapidjson::Value> est_path = payload.FindMember("est_path")->value.GetArray();
					read_est_path(est_path, vehicle);
				}
				else{

					throw status_intent_processing_exception("the future paths of Vehicle " + vehicle.get_id() + " is missing in received update!");
				}
				SPDLOG_DEBUG("Vehicle Class Vehicle Info Update - timestamp = {0}, vehicle = {1}, lane_id = {2}, state = {3}, speed = {4} m/s, distance = {5} m, access = {6}", 
					vehicle.get_cur_time(), vehicle.get_id(), vehicle.get_cur_lane_id(), vehicle.get_cur_speed(), vehicle.get_cur_distance(), vehicle.get_access());
			}
		}  
	}

	void all_stop_status_intent_processor::read_metadata(const rapidjson::GenericObject<true, rapidjson::Value> &metadata, vehicle &vehicle ) const{
		if ( metadata.FindMember("timestamp")->value.IsInt64()) {
			// timestamp in seconds 
			u_int64_t update = metadata.FindMember("timestamp")->value.GetInt64()/1000.0 ;
			if ( update > vehicle.get_cur_time()) {
				vehicle.set_cur_time(update);
			}
		}
	}

	void all_stop_status_intent_processor::read_payload(const rapidjson::GenericObject<true, rapidjson::Value> &payload, vehicle &vehicle) const{
		
		/* this if condition checks whether the vehicle has been seen before or not */
		if (vehicle.get_id() == ""){
			
			if ( payload.FindMember("v_id")->value.IsString() ) {
				vehicle.set_id(payload["v_id"].GetString());
			} else {
				throw status_intent_processing_exception("Update is missing v_id! Cannot be processed!");
			}
			/* the unit of the received vehicle length from the message is centimeter without decimal places
			*  but the unit of the vehicle length defined in the vehicle class is meter with decimal places. 
			*  Therefore, a conversion has been added here.
			*/
			if (payload.FindMember("v_length")->value.IsInt()){
				vehicle.set_length((double)payload["v_length"].GetInt() / 100);
			} else{
				throw status_intent_processing_exception("The length of Vehicle " + vehicle.get_id() + " is missing in the received update!");
			}
			
			if (payload.FindMember("min_gap")->value.IsDouble()){
				vehicle.set_min_gap(payload["min_gap"].GetDouble());
			} else{
				throw status_intent_processing_exception("The min gap of Vehicle " + vehicle.get_id() + " is missing in received update!");
			}
			
			if (payload.FindMember("react_t")->value.IsDouble()){
				vehicle.set_reaction_time(payload["react_t"].GetDouble());
			} else{
				throw status_intent_processing_exception("The reaction time of Vehicle " + vehicle.get_id() + " is missing in the received update!");
			}
			
			if (payload.FindMember("max_accel")->value.IsDouble()){
				vehicle.set_accel_max(payload["max_accel"].GetDouble());
			} else{
				throw status_intent_processing_exception("The maximum accelerate of Vehicle " + vehicle.get_id() + " is missing in the received update!");
			}

			if (payload.FindMember("max_decel")->value.IsDouble()){
				vehicle.set_decel_max(payload["max_decel"].GetDouble());
			} else{
				throw status_intent_processing_exception("The maximum deceleration of the Vehicle " + vehicle.get_id() + " is missing in the received update!");
			}
		}

			
		/* the unit of the received speed from the message is 0.02 of meter per second
		*  the unit of the speed defined in the vehicle class is meter per second. 
		*/
		if (payload.FindMember("cur_speed")->value.IsDouble()){
			vehicle.set_cur_speed(payload["cur_speed"].GetDouble() * 0.02);
		} else{
			throw status_intent_processing_exception("The current speed of Vehicle " + vehicle.get_id() + " is missing in received update!");
		}

		if (payload.FindMember("cur_accel")->value.IsDouble()){
			vehicle.set_cur_accel(payload["cur_accel"].GetDouble());
		} else{
			throw status_intent_processing_exception("The current acceleration of Vehicle " + vehicle.get_id() + " is missing in received update!");
		}

		if (payload.FindMember("cur_lane_id")->value.IsInt()){
			vehicle.set_cur_lane_id(payload["cur_lane_id"].GetInt());
		} else{
			throw status_intent_processing_exception("The current lane id of Vehicle " + vehicle.get_id() + " is missing in received update!");
		}
		if (payload.FindMember("cur_ds")->value.IsDouble()){
			vehicle.set_cur_distance(payload["cur_ds"].GetDouble());
		} else{
			throw status_intent_processing_exception("The current destination of Vehicle " + vehicle.get_id() + " is missing in received update!");
		}
			
		if (payload.FindMember("is_allowed")->value.IsBool()){
			vehicle.set_access(payload["is_allowed"].GetBool());
		} else{
			throw status_intent_processing_exception("The access status of Vehicle " + vehicle.get_id() + " is missing in received update!");
		}

		if (payload.FindMember("depart_pos")->value.IsInt64()){
			vehicle.set_departure_position(payload["depart_pos"].GetInt());
		} else{
			throw status_intent_processing_exception("The departure position of Vehicle " + vehicle.get_id() + " is missing in received update!");
		}

		if (payload.FindMember("entry_lane_id")->value.IsInt() && payload["entry_lane_id"].GetInt() != 0){
			vehicle.set_entry_lane_id(payload["entry_lane_id"].GetInt());
		} else{
			throw status_intent_processing_exception("The entry lane id of Vehicle " + vehicle.get_id() + " is missing in received update!");
		}

		if (payload.FindMember("dest_lane_id")->value.IsInt() && payload["dest_lane_id"].GetInt() != 0){
			vehicle.set_exit_lane_id(payload["dest_lane_id"].GetInt());
		} else{
			throw status_intent_processing_exception("The exit lane id of Vehicle " + vehicle.get_id() + " is missing in received update!");
		}
		
		if (payload.FindMember("link_lane_id")->value.IsInt() && payload["link_lane_id"].GetInt() != 0){
			vehicle.set_link_id(payload["link_lane_id"].GetInt());
		} else{
			throw status_intent_processing_exception("The link lane id of Vehicle " + vehicle.get_id() + " is missing in received update!");
		}

		update_vehicle_state(vehicle);
	}

	void all_stop_status_intent_processor::update_vehicle_state( vehicle &vehicle ) const {

		if (vehicle.get_cur_state() == vehicle_state::ND ){
			if ( vehicle.get_cur_lane_id() == vehicle.get_entry_lane_id() ) {
				vehicle.set_cur_state( vehicle_state::EV );
			}
			else {
				SPDLOG_CRITICAL( "Vehicle with ND (Not Defined) STATE is NOT in entry lane {0} != {1}",
					vehicle.get_entry_lane_id(), vehicle.get_cur_lane_id());
			}

		}
		// Asssume vehicle is in entry lane if previous state is EV
		else if (vehicle.get_cur_state() == vehicle_state::EV && is_vehicle_stopped( vehicle ) ){
			if ( vehicle.get_cur_lane_id() != vehicle.get_entry_lane_id() ){
				SPDLOG_CRITICAL("Stopped vehicle with EV (Entering Vehicle) STATE is NOT in entry lane {0} != {1}" ,
					vehicle.get_entry_lane_id(), vehicle.get_cur_lane_id() );
			}
			vehicle.set_cur_state( vehicle_state::RDV );
			vehicle.set_actual_st(vehicle.get_cur_time());	 
		}
		else if (vehicle.get_cur_state() == vehicle_state::RDV && vehicle.get_access() ){
			if ( vehicle.get_cur_lane_id() != vehicle.get_link_id() ){
				SPDLOG_CRITICAL("Departing vehicle with RDV (Ready to Depart Vehicle) VEHICLE STATE is NOT in entry lane {0} != {1}" ,
					vehicle.get_link_id(), vehicle.get_cur_lane_id() );
			}
			vehicle.set_cur_state( vehicle_state::DV );
			vehicle.set_actual_et(vehicle.get_cur_time());
		} else if (vehicle.get_cur_lane_id() == vehicle.get_exit_lane_id() && vehicle.get_cur_state() == vehicle_state::DV) {
			vehicle.set_cur_state( vehicle_state::LV );
			vehicle.set_actual_dt(vehicle.get_cur_time());
		}
	}

	void all_stop_status_intent_processor::read_est_path(const rapidjson::GenericArray<true,rapidjson::Value> &est_path, vehicle &vehicle) const {

				
		/* the first point in the future path is the current point */
		std::vector<future_information> future_info;
		future_information fi;
		fi.timestamp = vehicle.get_cur_time();
		fi.lane_id = vehicle.get_cur_lane_id();
		fi.distance = vehicle.get_cur_distance();
		future_info.push_back(fi);
		for (rapidjson::SizeType i = 0; i < est_path.Size(); ++i){
			
			/* adding checks to make sure the necessary data exist in the future point */
			if (est_path[i].FindMember("ts")->value.IsInt64() && 
				est_path[i].FindMember("id")->value.IsInt() && 
				est_path[i].FindMember("ds")->value.IsDouble()){
				
				/* adding checks to make sure only valid future points will be saved */
				if (est_path[i]["id"].GetInt() == 0){
					throw status_intent_processing_exception("The lane id in the future path of Vehicle " + vehicle.get_id() + " is invalid in received update!");
				}
				else if (est_path[i]["ds"].GetDouble() < 0){
					throw status_intent_processing_exception("The distance to the end of lane in the future path of Vehicle " + vehicle.get_id() + " is invalid in received update!");
				}
			
				/* the future path received from CARMA Platform does not consider the stopping requirement at the stop bar.
				*  therefore, CARMA Streets will force the stopping requirement to the vehicle's future path.
				*  basically, if the vehicle is an EV, or and RDV without access, if the vehicle lane id in the future path is not the same as the vehicle entry lane id, the scheduling service will ignore the future points.
				* */
				if (future_info[future_info.size() - 1].distance >= est_path[i]["ds"].GetDouble()){
					fi = future_information();
					// the unit of timestamp in here is sec with decimal places.
					fi.timestamp = (double)est_path[i]["ts"].GetInt64() / 1000;
					fi.distance = est_path[i]["ds"].GetDouble();
					fi.lane_id = est_path[i]["id"].GetInt();
					SPDLOG_DEBUG("future path {0}: {1}, {2}, {3}, {4}", i, fi.lane_id, fi.distance);
					future_info.push_back(fi);
				} 
				else{
					SPDLOG_INFO("Vehicle {0}: the lane id of the vehicle in the est_path has changed!", vehicle.get_id());
					break;
				}

				
			}
			else{
				throw status_intent_processing_exception("A point in the future path of Vehicle " + vehicle.get_id() + " is not complete in received update!");
			}
		}
		
		vehicle.set_future_info(future_info);
	}

	bool all_stop_status_intent_processor::is_vehicle_stopped(const vehicle &vehicle) const {
		return vehicle.get_cur_speed() <= stopping_speed && vehicle.get_cur_distance() <= stopping_distance;
	}

}