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
				if (payload.FindMember("est_paths")->value.IsArray()) {
					rapidjson::GenericArray<true, rapidjson::Value> est_path = payload.FindMember("est_paths")->value.GetArray();
					read_est_path(est_path, vehicle);
				}
				else{
					SPDLOG_WARN("The \"est_paths\" " + vehicle.get_id() + " is missing in received update!");
				}
				SPDLOG_DEBUG("Vehicle Class Vehicle Info Update - timestamp = {0}, vehicle = {1}, lane_id = {2}, state = {3}, speed = {4} m/s, distance = {5} m, access = {6}", 
					vehicle.get_cur_time(), vehicle.get_id(), vehicle.get_cur_lane_id(), vehicle.get_cur_speed(), vehicle.get_cur_distance(), vehicle.get_access());
			}
		}  
	}

	void all_stop_status_intent_processor::read_metadata(const rapidjson::GenericObject<true, rapidjson::Value> &metadata, vehicle &vehicle ) const{
		// MemberEnd check required for empty object possibility
		if ( metadata.FindMember("timestamp") != metadata.MemberEnd() && metadata.FindMember("timestamp")->value.IsInt64()) {
			// timestamp in seconds 
			u_int64_t timeout_time = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count() - timeout;
			u_int64_t cur_time = metadata.FindMember("timestamp")->value.GetInt64();
			if ( timeout_time > cur_time ) {
				throw status_intent_processing_exception("The \"timestamp\" " + std::to_string(cur_time) + " for update older than " + std::to_string(timeout_time) + "!");
			}

			u_int64_t update = cur_time/1000.0;
			if ( update > vehicle.get_cur_time()) {
				vehicle.set_cur_time(update);
			}
		}
		else{
			throw status_intent_processing_exception("The \"timestamp\" is missing/incorrect in update!");
		}
	}

	void all_stop_status_intent_processor::read_payload(const rapidjson::GenericObject<true, rapidjson::Value> &payload, vehicle &vehicle) const{
		
		/* this if condition checks whether the vehicle has been seen before or not */
		if (vehicle.get_id() == ""){
			
			if ( payload.FindMember("v_id")->value.IsString() ) {
				vehicle.set_id(payload["v_id"].GetString());
			} else {
				throw status_intent_processing_exception("Update is missing \"v_id\"! Cannot be processed!");
			}
			/* the unit of the received vehicle length from the message is centimeter without decimal places
			*  but the unit of the vehicle length defined in the vehicle class is meter with decimal places. 
			*  Therefore, a conversion has been added here.
			*/
			if (payload.FindMember("v_length")->value.IsInt()){
				vehicle.set_length((double)payload["v_length"].GetInt() / 100);
			} else{
				throw status_intent_processing_exception("The \"v_length\" " + vehicle.get_id() + " is missing/incorrect in the received update!");
			}
			
			if (payload.FindMember("min_gap")->value.IsDouble()){
				vehicle.set_min_gap(payload["min_gap"].GetDouble());
			} else{
				throw status_intent_processing_exception("The \"min_gap\" " + vehicle.get_id() + " is missing/incorrect in received update!");
			}
			
			if (payload.FindMember("react_t")->value.IsDouble()){
				vehicle.set_reaction_time(payload["react_t"].GetDouble());
			} else{
				throw status_intent_processing_exception("The \"react_t\" " + vehicle.get_id() + " is missing/incorrect in the received update!");
			}
			
			if (payload.FindMember("max_accel")->value.IsDouble()){
				vehicle.set_accel_max(payload["max_accel"].GetDouble());
			} else{
				throw status_intent_processing_exception("The \"max_accel\" " + vehicle.get_id() + " is missing/incorrect in the received update!");
			}

			if (payload.FindMember("max_decel")->value.IsDouble()){
				vehicle.set_decel_max(payload["max_decel"].GetDouble());
			} else{
				throw status_intent_processing_exception("The \"max_decel\" " + vehicle.get_id() + " is missing/incorrect in the received update!");
			}
		}

			
		/* the unit of the received speed from the message is 0.02 of meter per second
		*  the unit of the speed defined in the vehicle class is meter per second. 
		*/
		if (payload.FindMember("cur_speed")->value.IsDouble()){
			vehicle.set_cur_speed(payload["cur_speed"].GetDouble() * 0.02);
		} else{
			throw status_intent_processing_exception("The \"cur_speed\" " + vehicle.get_id() + " is missing/incorrect in received update!");
		}

		if (payload.FindMember("cur_accel")->value.IsDouble()){
			vehicle.set_cur_accel(payload["cur_accel"].GetDouble());
		} else{
			throw status_intent_processing_exception("The \"cur_accel\" " + vehicle.get_id() + " is missing/incorrect in received update!");
		}

		if (payload.FindMember("cur_lane_id")->value.IsInt()){
			vehicle.set_cur_lane_id(payload["cur_lane_id"].GetInt());
		} else{
			throw status_intent_processing_exception("The \"cur_lane_id\" " + vehicle.get_id() + " is missing/incorrect in received update!");
		}
		if (payload.FindMember("cur_ds")->value.IsDouble()){
			vehicle.set_cur_distance(payload["cur_ds"].GetDouble());
		} else{
			throw status_intent_processing_exception("The \"cur_ds\" " + vehicle.get_id() + " is missing/incorrect in received update!");
		}
			
		if (payload.FindMember("is_allowed")->value.IsBool()){
			vehicle.set_access(payload["is_allowed"].GetBool());
		} else{
			throw status_intent_processing_exception("The \"is_allowed\" " + vehicle.get_id() + " is missing/incorrect in received update!");
		}

		if (payload.FindMember("depart_pos")->value.IsInt64()){
			vehicle.set_departure_position(payload["depart_pos"].GetInt());
		} else{
			throw status_intent_processing_exception("The \"depart_pos\" " + vehicle.get_id() + " is missing/incorrect in received update!");

		}

		if (payload.FindMember("entry_lane_id")->value.IsInt() && payload["entry_lane_id"].GetInt() != 0){
			vehicle.set_entry_lane_id(payload["entry_lane_id"].GetInt());
		} else{
			throw status_intent_processing_exception("The \"entry_lane_id\" " + vehicle.get_id() + " is missing/incorrect in received update!");
		}

		if (payload.FindMember("dest_lane_id")->value.IsInt() && payload["dest_lane_id"].GetInt() != 0){
			vehicle.set_exit_lane_id(payload["dest_lane_id"].GetInt());
		} else{
			throw status_intent_processing_exception("The \"dest_lane_id\" " + vehicle.get_id() + " is missing/incorrect in received update!");
		}
		
		if (payload.FindMember("link_lane_id")->value.IsInt() && payload["link_lane_id"].GetInt() != 0){
			vehicle.set_link_id(payload["link_lane_id"].GetInt());
		} else{
			throw status_intent_processing_exception("The \"link_lane_id\" " + vehicle.get_id() + " is missing/incorrect in received update!");
		}

		if (payload.FindMember("direction")->value.IsString() ){
			vehicle.set_direction(payload["direction"].GetString());
		} else{
			throw status_intent_processing_exception("The \"direction\" " + vehicle.get_id() + " is missing/incorrect in received update!");
		}


		update_vehicle_state(vehicle);
	}

	void all_stop_status_intent_processor::update_vehicle_state( vehicle &vehicle ) const {

		if (vehicle.get_cur_state() == vehicle_state::ND ){
			if ( vehicle.get_cur_lane_id() == vehicle.get_entry_lane_id() ) {
				vehicle.set_cur_state( vehicle_state::EV );
				SPDLOG_DEBUG("Vehicle {0} state is updated from ND to EV.", vehicle.get_id());	 

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
			SPDLOG_DEBUG("Vehicle {0} state is updated from EV to RDV.", vehicle.get_id());	 
		}
		else if (vehicle.get_cur_state() == vehicle_state::RDV && vehicle.get_access() ){
			if ( vehicle.get_cur_lane_id() != vehicle.get_link_id() ){
				SPDLOG_CRITICAL("Departing vehicle with RDV (Ready to Depart Vehicle) VEHICLE STATE is NOT in entry lane {0} != {1}" ,
					vehicle.get_link_id(), vehicle.get_cur_lane_id() );
			}
			vehicle.set_cur_state( vehicle_state::DV );
			vehicle.set_actual_et(vehicle.get_cur_time());
			SPDLOG_DEBUG("Vehicle {0} state is updated from RDV to DV.", vehicle.get_id());	 

		} else if (vehicle.get_cur_lane_id() == vehicle.get_exit_lane_id() && vehicle.get_cur_state() == vehicle_state::DV) {
			vehicle.set_cur_state( vehicle_state::LV );
			vehicle.set_actual_dt(vehicle.get_cur_time());
			SPDLOG_DEBUG("Vehicle {0} state is updated from DV to LV.", vehicle.get_id());	 

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
			rapidjson::GenericObject<true, rapidjson::Value> path_point = est_path[i].GetObject();
			/* adding checks to make sure the necessary data exist in the future point */
			// Added member end check for Int64 type
			if (path_point.FindMember("ts") != path_point.MemberEnd() &&
				path_point.FindMember("ts")->value.IsInt64() && 
				path_point.FindMember("id")->value.IsInt() && 
				path_point.FindMember("ds")->value.IsDouble()){

				/* adding checks to make sure only valid future points will be saved */
				if (path_point["id"].GetInt() == 0){
					SPDLOG_WARN("Ignoring incorrect future point with index {0} for vehicle {1}: \"id\" cannot be 0!", i, vehicle.get_id() );
					continue;
				}
				else if (path_point["ds"].GetDouble() < 0){
					SPDLOG_WARN("Ignoring incorrect future point with index {0} for vehicle {1}: \"ds\" cannot be negative!", i, vehicle.get_id() );
					continue;
				}
			
				/* the future path received from CARMA Platform does not consider the stopping requirement at the stop bar.
				*  therefore, CARMA Streets will force the stopping requirement to the vehicle's future path.
				*  basically, if the vehicle is an EV, or and RDV without access, if the vehicle lane id in the future path is not the same as the vehicle entry lane id, the scheduling service will ignore the future points.
				* */
				if (future_info[future_info.size() - 1].distance >= path_point["ds"].GetDouble()){
					fi = future_information();
					// the unit of timestamp in here is sec with decimal places.
					fi.timestamp = ((double)path_point["ts"].GetInt64()) / 1000;
					fi.distance = path_point["ds"].GetDouble();
					fi.lane_id = path_point["id"].GetInt();
					SPDLOG_DEBUG("future path {0}: {1}, {2}, {3}, {4}", i, fi.lane_id, fi.distance);
					future_info.push_back(fi);
				} 
				else{
					SPDLOG_INFO("Vehicle {0}: the lane id of the vehicle in the est_path has changed!", vehicle.get_id());
					break;
				}

				
			}
			else{
				throw status_intent_processing_exception("Ignoring incorrect/incomplete future point with index " 
					+ std::to_string(i) +" for vehicle " + vehicle.get_id() + "!" );
			}
		}
		
		vehicle.set_future_info(future_info);
	}

	bool all_stop_status_intent_processor::is_vehicle_stopped(const vehicle &vehicle) const {
		SPDLOG_DEBUG("Stopping condition spd: {0}, dst: {1}; Vehicle condition spd: {2}, dst: {3} .", 
			stopping_speed, stopping_distance, vehicle.get_cur_speed(), vehicle.get_cur_distance());
		return vehicle.get_cur_speed() <= stopping_speed && vehicle.get_cur_distance() <= stopping_distance;
	}

	void all_stop_status_intent_processor::set_timeout(uint64_t timeout) {
		this->timeout = timeout;
	}

	uint64_t all_stop_status_intent_processor::get_timeout() const {
		return this->timeout;
	}

}