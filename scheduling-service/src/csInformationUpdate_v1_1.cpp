
/**/

#include "Header_v1_1.h"

struct SortAscending_i
{
	const vector<int> & value_vector;

	SortAscending_i(const vector<int> & val_vec) :
		value_vector(val_vec) {}

	bool operator()(int i1, int i2)
	{
		return value_vector[i1] < value_vector[i2];
	}
};

struct SortDecending_i
{
	const vector<int> & value_vector;

	SortDecending_i(const vector<int> & val_vec) :
		value_vector(val_vec) {}

	bool operator()(int i1, int i2)
	{
		return value_vector[i1] > value_vector[i2];
	}
};

struct SortAscending_d
{
	const vector<double> & value_vector;

	SortAscending_d(const vector<double> & val_vec) :
		value_vector(val_vec) {}

	bool operator()(int i1, int i2)
	{
		return value_vector[i1] < value_vector[i2];
	}
};

struct SortDecending_d
{
	const vector<double> & value_vector;

	SortDecending_d(const vector<double> & val_vec) :
		value_vector(val_vec) {}

	bool operator()(int i1, int i2)
	{
		return value_vector[i1] > value_vector[i2];
	}
};

int findLaneIndex(double lat, double lng) {
	/**/
	int lane_index = 1;
	return lane_index;
}

void csInformationUpdate_v1_1(Setting& setting, Vehicles& vehicles, Scheduling& scheduling, Map map, vector<string> list_json_files) {


	/* read the json files and update the vehicle class */
	double vehicle_index;
	double timestamp;
	double lat;
	double lng;
	double position_lane;
	int lane_index;
	int link_index;
	string vehicle_id;
	string file_name;
	double speed;
	double acceleration;
	double time;
	string state;

	/*
	* sequentially, read the json files from the json file lists and update the vehicle information class.
	* Note: currently, we assume that the json file list contains the name of the json files stored in the directory!
	*/
	for (int f = 0; f < list_json_files.size(); ++f) {

		file_name = list_json_files[f];
		ifstream file_input(file_name);
		Json::Reader reader;
		Json::Value root;
		reader.parse(file_input, root);
		file_input >> root;

		timestamp = root["metadata"]["timestamp"].asDouble();
		vehicle_id = root["payload"]["vehicle_id"].asString();
		lat = root["payload"]["cur_loc"]["lat"].asDouble();
		lng = root["payload"]["cur_loc"]["lng"].asDouble();
		lane_index = findLaneIndex(lat, lng);

		/* check whether this is the first time receiving information from the vehicle or not! */
		/* if this is the first time receiving information from the vehicle */
		if (vehicles.index.count(vehicle_id) == 0) {

			/* if the vehicle is located inside an entry lane or a connection link */
			if (find(map.index_exitLane.begin(), map.index_exitLane.end(), lane_index) == map.index_exitLane.end()) {

				vehicles.id.push_back(vehicle_id);
				vehicle_index = vehicles.id.size() - 1;
				vehicles.index.insert({ vehicle_id, vehicle_index });
				vehicles.length.push_back(root["payload"]["vehicle_length"].asDouble());
				vehicles.min_gap.push_back(root["payload"]["min_gap"].asDouble());
				vehicles.reaction_time.push_back(root["payload"]["reaction_t"].asDouble());
				vehicles.accel_max.push_back(root["payload"]["max_accel"].asDouble());
				vehicles.decel_max.push_back(root["payload"]["max_decel"].asDouble());
				vehicles.direction.push_back(root["payload"]["direction"].asString());

				vehicles.time_past.push_back({ timestamp });
				vehicles.lat_past.push_back({ lat });
				vehicles.lng_past.push_back({ lng });
				vehicles.speed.push_back({ root["payload"]["cur_speed"].asDouble() });
				vehicles.acceleration.push_back({ root["payload"]["cur_accel"].asDouble() });
				vehicles.lane_index.push_back({ lane_index });
				//vehicles.link_index.push_back(root["payload"]["link_lane_idx"].asInt());

				/* if the vehicle is located inside an entry lane */
				if (find(map.index_entryLane.begin(), map.index_entryLane.end(), lane_index) != map.index_entryLane.end()) {

					vehicles.entryLane_index.push_back(lane_index);
					link_index = -1;
					for (int i = 0; i < map.index_p[lane_index].size(); ++i) {
						if (vehicles.direction[vehicle_index] == map.direction[map.index_p[lane_index][i]]) {
							link_index = map.index_p[lane_index][i];
							break;
						}
					}
					vehicles.link_index.push_back(link_index);
					vehicles.link_priority.push_back(map.priority[link_index]);
					vehicles.access.push_back("N");
					vehicles.st_actual.push_back(-1);
					vehicles.et_actual.push_back(-1);
					vehicles.dt_actual.push_back(-1);

					vehicles.state.push_back({ "EV" });

				}
				else {

					vehicles.link_index.push_back(lane_index);
					vehicles.link_priority.push_back(map.priority[lane_index]);
					vehicles.entryLane_index.push_back(map.index_n[lane_index][0]);
					vehicles.access.push_back("Y");
					vehicles.st_actual.push_back(timestamp - 1);
					vehicles.et_actual.push_back(timestamp);
					vehicles.dt_actual.push_back(-1);

					vehicles.state.push_back({ "DV" });
				}

				vehicles.departurePosition_index.push_back(1000);
				//vehicles.departurePosition_index.push_back(-1);
				vehicles.flexibility_ET.push_back(5);
				vehicles.flexibility_ST.push_back(15);

				vehicles.time_future.push_back({});
				vehicles.lat_future.push_back({});
				vehicles.lng_future.push_back({});
				for (int i = 0; i < root["payload"]["est_paths"].size(); ++i) {
					vehicles.time_future[vehicle_index].push_back(root["payload"]["est_paths"][i]["timestamp"].asDouble());
					vehicles.lat_future[vehicle_index].push_back(root["payload"]["est_paths"][i]["lat"].asDouble());
					vehicles.lng_future[vehicle_index].push_back(root["payload"]["est_paths"][i]["lng"].asDouble());
				}


				scheduling.id.push_back(vehicle_id);
				scheduling.index.insert({ vehicle_id, vehicle_index });
				scheduling.lat.push_back(lat);
				scheduling.lng.push_back(lng);
				scheduling.speed.push_back(root["payload"]["cur_speed"].asDouble());
				scheduling.acceleration.push_back(root["payload"]["cur_accel"].asDouble());
				scheduling.lane_index.push_back(lane_index);
				scheduling.state.push_back(vehicles.state[vehicle_index].back());
				scheduling.departurePosition_index.push_back(vehicles.departurePosition_index[vehicle_index]);
				scheduling.position_lane.push_back(-1);						// ??
				scheduling.clearance_time.push_back(-1);					// ??
				scheduling.est.push_back(-1);								// ??
				scheduling.st.push_back(vehicles.st_actual[vehicle_index]);
				scheduling.et.push_back(vehicles.et_actual[vehicle_index]);
				scheduling.dt.push_back(vehicles.dt_actual[vehicle_index]);

			}
			else {
				cout << "Vehicle " + vehicle_id + " is an LV" << endl;
			}
		}
		else {

			vehicle_index = vehicles.index[vehicle_id];

			vehicles.time_past[vehicle_index].push_back(timestamp);
			vehicles.lat_past[vehicle_index].push_back(lat);
			vehicles.lng_past[vehicle_index].push_back(lng);
			vehicles.speed[vehicle_index].push_back(root["payload"]["cur_speed"].asDouble());
			vehicles.acceleration[vehicle_index].push_back(root["payload"]["cur_accel"].asDouble());

			if (vehicles.state[vehicle_index].back() == "EV") {
				vehicles.lane_index[vehicle_index].push_back(vehicles.lane_index[vehicle_index].back());
				if (lane_index == vehicles.entryLane_index[vehicle_index]) {
					position_lane = sqrt(pow(map.end_lat[vehicles.entryLane_index[vehicle_index]] - lat, 2) +
						pow(map.end_lng[vehicles.entryLane_index[vehicle_index]] - lng, 2));
				}
				else {
					position_lane = 0.01;
				}
				if (position_lane <= map.stop_loc_buffer && vehicles.speed[vehicle_index].back() <= map.stop_speed_buffer) {
					vehicles.state[vehicle_index].push_back("RDV");
					vehicles.st_actual[vehicle_index] = timestamp;
				}
				else {
					vehicles.state[vehicle_index].push_back("EV");
				}
				vehicles.access[vehicle_index] = "N";
			}
			else if (vehicles.state[vehicle_index].back() == "RDV") {
				if (vehicles.access[vehicle_index] == "N") {
					vehicles.lane_index[vehicle_index].push_back(vehicles.lane_index[vehicle_index].back());
					vehicles.state[vehicle_index].push_back("RDV");
				}
				else {
					//vehicles.lane_index[vehicle_index].push_back(lane_index);
					vehicles.lane_index[vehicle_index].push_back(vehicles.entryLane_index[vehicle_index]);
					vehicles.state[vehicle_index].push_back("DV");
					vehicles.et_actual[vehicle_index] = timestamp;
				}
			}
			else if (vehicles.state[vehicle_index].back() == "DV") {

				//vehicles.lane_index[vehicle_index].push_back(lane_index);
				if (find(map.index_entryLane.begin(), map.index_entryLane.end(), lane_index) == map.index_entryLane.end()) {
					vehicles.lane_index[vehicle_index].push_back(lane_index);
				}
				else {
					vehicles.lane_index[vehicle_index].push_back(vehicles.link_index[vehicle_index]);
				}

				if (find(map.index_exitLane.begin(), map.index_exitLane.end(), lane_index) == map.index_exitLane.end()) {
					vehicles.state[vehicle_index].push_back("DV");
				}
				else {
					vehicles.state[vehicle_index].push_back("LV");
					vehicles.dt_actual[vehicle_index] = timestamp;
				}
			}
			else if (vehicles.state[vehicle_index].back() == "LV") {
				vehicles.lane_index[vehicle_index].push_back(lane_index);
				vehicles.state[vehicle_index].push_back("LV");
			}


			vehicles.time_future[vehicle_index].clear();
			vehicles.lat_future[vehicle_index].clear();
			vehicles.lng_future[vehicle_index].clear();
			for (int i = 0; i < root["payload"]["est_paths"].size(); ++i) {
				vehicles.time_future[vehicle_index].push_back(root["payload"]["est_paths"][i]["timestamp"].asDouble());
				vehicles.lat_future[vehicle_index].push_back(root["payload"]["est_paths"][i]["lat"].asDouble());
				vehicles.lng_future[vehicle_index].push_back(root["payload"]["est_paths"][i]["lng"].asDouble());
			}

			scheduling.lat[vehicle_index] = lat;
			scheduling.lng[vehicle_index] = lng;
			scheduling.speed[vehicle_index] = root["payload"]["cur_speed"].asDouble();
			scheduling.acceleration[vehicle_index] = root["payload"]["cur_accel"].asDouble();
			scheduling.lane_index[vehicle_index] = vehicles.lane_index[vehicle_index].back();
			scheduling.state[vehicle_index] = vehicles.state[vehicle_index].back();
			scheduling.departurePosition_index[vehicle_index] = vehicles.departurePosition_index[vehicle_index];
			scheduling.position_lane[vehicle_index] = -1;					// ??
			scheduling.clearance_time[vehicle_index] = -1;					// ??
			scheduling.est[vehicle_index] = -1;								// ??
			scheduling.st[vehicle_index] = vehicles.st_actual[vehicle_index];
			scheduling.et[vehicle_index] = vehicles.et_actual[vehicle_index];
			scheduling.dt[vehicle_index] = vehicles.dt_actual[vehicle_index];
		}
	}


	/* delete LVs information from the vehicle class */
	int count_LV = 0;
	for (int vehicle_index = vehicles.id.size() - 1; vehicle_index >= 0; --vehicle_index) {
		if (vehicles.state[vehicle_index].back() == "LV") {

			vehicle_id = vehicles.id[vehicle_index];

			vehicles.index.erase(vehicle_id);
			vehicles.id.erase(vehicles.id.begin() + vehicle_index);
			vehicles.length.erase(vehicles.length.begin() + vehicle_index);
			vehicles.min_gap.erase(vehicles.min_gap.begin() + vehicle_index);
			vehicles.reaction_time.erase(vehicles.reaction_time.begin() + vehicle_index);
			vehicles.accel_max.erase(vehicles.accel_max.begin() + vehicle_index);
			vehicles.decel_max.erase(vehicles.decel_max.begin() + vehicle_index);
			vehicles.entryLane_index.erase(vehicles.entryLane_index.begin() + vehicle_index);
			vehicles.link_index.erase(vehicles.link_index.begin() + vehicle_index);
			vehicles.link_priority.erase(vehicles.link_priority.begin() + vehicle_index);
			vehicles.departurePosition_index.erase(vehicles.departurePosition_index.begin() + vehicle_index);
			vehicles.flexibility_ET.erase(vehicles.flexibility_ET.begin() + vehicle_index);
			vehicles.flexibility_ST.erase(vehicles.flexibility_ST.begin() + vehicle_index);
			vehicles.direction.erase(vehicles.direction.begin() + vehicle_index);
			vehicles.access.erase(vehicles.access.begin() + vehicle_index);
			vehicles.st_actual.erase(vehicles.st_actual.begin() + vehicle_index);
			vehicles.et_actual.erase(vehicles.et_actual.begin() + vehicle_index);
			vehicles.dt_actual.erase(vehicles.dt_actual.begin() + vehicle_index);
			vehicles.time_past.erase(vehicles.time_past.begin() + vehicle_index);
			vehicles.lat_past.erase(vehicles.lat_past.begin() + vehicle_index);
			vehicles.lng_past.erase(vehicles.lng_past.begin() + vehicle_index);
			vehicles.speed.erase(vehicles.speed.begin() + vehicle_index);
			vehicles.acceleration.erase(vehicles.acceleration.begin() + vehicle_index);
			vehicles.lane_index.erase(vehicles.lane_index.begin() + vehicle_index);
			vehicles.state.erase(vehicles.state.begin() + vehicle_index);
			vehicles.time_future.erase(vehicles.time_future.begin() + vehicle_index);
			vehicles.lat_future.erase(vehicles.lat_future.begin() + vehicle_index);
			vehicles.lng_future.erase(vehicles.lng_future.begin() + vehicle_index);

			scheduling.index.erase(vehicle_id);
			scheduling.id.erase(scheduling.id.begin() + vehicle_index);
			scheduling.time.erase(scheduling.time.begin() + vehicle_index);
			scheduling.lat.erase(scheduling.lat.begin() + vehicle_index);
			scheduling.lng.erase(scheduling.lng.begin() + vehicle_index);
			scheduling.speed.erase(scheduling.speed.begin() + vehicle_index);
			scheduling.acceleration.erase(scheduling.acceleration.begin() + vehicle_index);
			scheduling.lane_index.erase(scheduling.lane_index.begin() + vehicle_index);
			scheduling.state.erase(scheduling.state.begin() + vehicle_index);
			scheduling.departurePosition_index.erase(scheduling.departurePosition_index.begin() + vehicle_index);
			scheduling.position_lane.erase(scheduling.position_lane.begin() + vehicle_index);
			scheduling.clearance_time.erase(scheduling.clearance_time.begin() + vehicle_index);
			scheduling.est.erase(scheduling.est.begin() + vehicle_index);
			scheduling.st.erase(scheduling.st.begin() + vehicle_index);
			scheduling.et.erase(scheduling.et.begin() + vehicle_index);
			scheduling.dt.erase(scheduling.dt.begin() + vehicle_index);

			count_LV += 1;
		}
	}
	if (count_LV > 0) {
		for (int vehicle_index = 0; vehicle_index < vehicles.id.size(); ++vehicle_index) {
			vehicles.index[vehicles.id[vehicle_index]] = vehicle_index;
			scheduling.index[vehicles.id[vehicle_index]] = vehicle_index;
		}
	}


	scheduling.index_EVs.clear();
	scheduling.index_EVs.resize(map.index_entryLane.size());
	scheduling.index_RDVs.clear();
	scheduling.index_DVs.clear();
	scheduling.index_LVs.clear();

	/* for each vehicle, predict the vehicle location, speed, etc. at the next scheduling time point */
	for (int vehicle_index = 0; vehicle_index < vehicles.id.size(); ++vehicle_index) {

		/* initially, set the vehicle location, speed, state, etc. to the latest received information from the vehicle */
		vehicle_id = vehicles.id[vehicle_index];
		scheduling.time[vehicle_index] = vehicles.time_past[vehicle_index].back();
		scheduling.lat[vehicle_index] = vehicles.lat_past[vehicle_index].back();
		scheduling.lng[vehicle_index] = vehicles.lng_past[vehicle_index].back();
		scheduling.speed[vehicle_index] = vehicles.speed[vehicle_index].back();
		scheduling.acceleration[vehicle_index] = vehicles.acceleration[vehicle_index].back();
		scheduling.lane_index[vehicle_index] = vehicles.lane_index[vehicle_index].back();
		scheduling.state[vehicle_index] = vehicles.state[vehicle_index].back();
		scheduling.departurePosition_index[vehicle_index] = vehicles.departurePosition_index[vehicle_index];


		/* estimate the vehicle location, speed, state, etc. at a given future timestamp! */
		int indicator = 0;
		while (scheduling.time[vehicle_index] < setting.scheduling_start_time + setting.scheduling_frequency) {

			if (indicator < vehicles.time_future.size()) {

				time = vehicles.time_future[vehicle_index][indicator];
				lat = vehicles.lat_future[vehicle_index][indicator];
				lng = vehicles.lng_future[vehicle_index][indicator];
				speed = sqrt(pow(scheduling.lat[vehicle_index] - lat, 2) + pow(scheduling.lng[vehicle_index] - lng, 2)) / (time - scheduling.time[vehicle_index]);
				acceleration = speed - scheduling.speed[vehicle_index] / (time - scheduling.time[vehicle_index]);
				if (scheduling.state[vehicle_index] == "EV") {
					lane_index = scheduling.lane_index[vehicle_index];
					state = scheduling.state[vehicle_index];
				}
				else if (scheduling.state[vehicle_index] == "RDV") {
					lane_index = scheduling.lane_index[vehicle_index];
					state = scheduling.state[vehicle_index];
				}
				else if (scheduling.state[vehicle_index] == "DV") {
					lane_index = findLaneIndex(lat, lng);
					if (lane_index == vehicles.link_index[vehicle_index] || lane_index == vehicles.entryLane_index[vehicle_index]) {
						state = "DV";
					}
					else {
						state = "LV";
						scheduling.dt[vehicle_index] = time;
					}
				}
				else if (scheduling.state[vehicle_index] == "LV") {
					lane_index = scheduling.lane_index[vehicle_index];
					state = scheduling.state[vehicle_index];
				}

				scheduling.time[vehicle_index] = time;
				scheduling.lat[vehicle_index] = lat;
				scheduling.lng[vehicle_index] = lng;
				scheduling.speed[vehicle_index] = speed;
				scheduling.acceleration[vehicle_index] = acceleration;
				scheduling.lane_index[vehicle_index] = lane_index;
				scheduling.state[vehicle_index] = state;

			}
			else {
				cout << "error - vehicle " + vehicle_id + " does not have enough future lat/lng information!" << endl;
				//fprintf('error - \n');
				break;
			}

		}

		/* calculating the distance between the vehicle and the end of its lane */
		if (scheduling.state[vehicle_index] == "EV") {
			if (findLaneIndex(scheduling.lat[vehicle_index], scheduling.lng[vehicle_index]) == vehicles.entryLane_index[vehicle_index]) {
				scheduling.position_lane[vehicle_index] = sqrt(pow(map.end_lat[vehicles.entryLane_index[vehicle_index]] - scheduling.lat[vehicle_index], 2) +
					pow(map.end_lng[vehicles.entryLane_index[vehicle_index]] - scheduling.lng[vehicle_index], 2));
			}
			else {
				scheduling.position_lane[vehicle_index] = 0.01;
			}
		}
		else if (scheduling.state[vehicle_index] == "RDV") {
			scheduling.position_lane[vehicle_index] = sqrt(pow(map.end_lat[vehicles.link_index[vehicle_index]] - scheduling.lat[vehicle_index], 2) +
				pow(map.end_lng[vehicles.link_index[vehicle_index]] - scheduling.lng[vehicle_index], 2));
		}
		else if (scheduling.state[vehicle_index] == "DV") {
			if (findLaneIndex(scheduling.lat[vehicle_index], scheduling.lng[vehicle_index]) == vehicles.entryLane_index[vehicle_index] ||
				findLaneIndex(scheduling.lat[vehicle_index], scheduling.lng[vehicle_index]) == vehicles.link_index[vehicle_index]) {
				scheduling.position_lane[vehicle_index] = sqrt(pow(map.end_lat[vehicles.link_index[vehicle_index]] - scheduling.lat[vehicle_index], 2) +
					pow(map.end_lng[vehicles.link_index[vehicle_index]] - scheduling.lng[vehicle_index], 2));
			}
		}
		else {
			scheduling.position_lane[vehicle_index] = 300;
		}

		/* if the vehicle is in the EV state, estimate its earliest stopping time */
		if (scheduling.state[vehicle_index] == "EV") {

			double Dx1 = max(scheduling.position_lane[vehicle_index], 0.01);
			double Dx2 = -pow(scheduling.speed[vehicle_index], 2) / (2 * vehicles.decel_max[vehicle_index]);
			double Dx3 = ((pow(map.speed_max[scheduling.lane_index[vehicle_index]], 2) - pow(scheduling.speed[vehicle_index], 2)) / (2 * vehicles.accel_max[vehicle_index])) -
				(pow(map.speed_max[scheduling.lane_index[vehicle_index]], 2) / (2 * vehicles.decel_max[vehicle_index]));
			double t_a;
			double t_d;
			double t_cr;
			if (Dx1 < Dx2) {
				double d_emergency = -pow(scheduling.speed[vehicle_index], 2) / (2 * Dx1);
				t_a = 0.0;
				t_d = -scheduling.speed[vehicle_index] / d_emergency;
				t_cr = 0.0;
			}
			else if (Dx1 < Dx3) {
				double v_mid = sqrt(vehicles.decel_max[vehicle_index] * ((2 * Dx1 * vehicles.accel_max[vehicle_index]) + pow(scheduling.speed[vehicle_index], 2)) /
					(vehicles.decel_max[vehicle_index] - vehicles.accel_max[vehicle_index]));
				t_a = (v_mid - scheduling.speed[vehicle_index]) / vehicles.accel_max[vehicle_index];
				t_d = -v_mid / vehicles.decel_max[vehicle_index];
				t_cr = 0.0;
			}
			else {
				double v_mid = map.speed_max[scheduling.lane_index[vehicle_index]];
				t_a = (v_mid - scheduling.speed[vehicle_index]) / vehicles.accel_max[vehicle_index];
				t_d = -v_mid / vehicles.decel_max[vehicle_index];
				t_cr = (Dx1 - Dx3) / v_mid;
			}

			scheduling.est[vehicle_index] = scheduling.time[vehicle_index] + max(t_a + t_d + t_cr, 0.01);
		}
		else {
			scheduling.est[vehicle_index] = -1;
		}

		/* calculate the clearance time required by the vehicle to pass through its associated connection link*/
		if (scheduling.state[vehicle_index] == "EV" || scheduling.state[vehicle_index] == "RDV") {
			if (pow(map.speed_max[vehicles.link_index[vehicle_index]], 2) / (2 * vehicles.accel_max[vehicle_index]) < map.length[vehicles.link_index[vehicle_index]]) {
				scheduling.clearance_time[vehicle_index] = (map.length[vehicles.link_index[vehicle_index]] / map.speed_max[vehicles.link_index[vehicle_index]]) +
					(map.speed_max[vehicles.link_index[vehicle_index]] / (2 * vehicles.accel_max[vehicle_index]));
			}
			else {
				scheduling.clearance_time[vehicle_index] = sqrt((2 * map.length[vehicles.link_index[vehicle_index]]) / vehicles.accel_max[vehicle_index]);
			}
			scheduling.clearance_time[vehicle_index] = ceil(scheduling.clearance_time[vehicle_index]);
		}
		else if (scheduling.state[vehicle_index] == "DV") {
			double dx = (pow(map.speed_max[vehicles.link_index[vehicle_index]], 2) - pow(scheduling.speed[vehicle_index], 2)) / (2 * vehicles.accel_max[vehicle_index]);
			if (scheduling.position_lane[vehicle_index] < dx) {
				scheduling.clearance_time[vehicle_index] = (sqrt(pow(scheduling.speed[vehicle_index], 2) + (2 * vehicles.accel_max[vehicle_index] * scheduling.position_lane[vehicle_index])) -
					scheduling.speed[vehicle_index]) / vehicles.accel_max[vehicle_index];
			}
			else {
				scheduling.clearance_time[vehicle_index] = ((map.speed_max[vehicles.link_index[vehicle_index]] - scheduling.speed[vehicle_index]) / vehicles.accel_max[vehicle_index]) +
					((scheduling.position_lane[vehicle_index] - dx) / map.speed_max[vehicles.link_index[vehicle_index]]);
			}
			scheduling.clearance_time[vehicle_index] = ceil(scheduling.clearance_time[vehicle_index]);
		}
		else {
			scheduling.clearance_time[vehicle_index] = -1;
		}

		/* create the EV, RDV, DV, and LV lists */
		if (scheduling.state[vehicle_index] == "EV") {
			for (int i = 0; i < map.index_entryLane.size(); ++i) {
				if (scheduling.lane_index[vehicle_index] == map.index_entryLane[i]) {
					scheduling.index_EVs[i].push_back(vehicle_index);
					break;
				}
			}
			scheduling.st[vehicle_index] = -1;
			scheduling.et[vehicle_index] = -1;
			scheduling.dt[vehicle_index] = -1;
		}
		else if (scheduling.state[vehicle_index] == "RDV") {
			scheduling.index_RDVs.push_back(vehicle_index);
			scheduling.st[vehicle_index] = vehicles.st_actual[vehicle_index];
			scheduling.et[vehicle_index] = -1;
			scheduling.dt[vehicle_index] = -1;
		}
		else if (scheduling.state[vehicle_index] == "DV") {
			scheduling.index_DVs.push_back(vehicle_index);
			scheduling.st[vehicle_index] = vehicles.st_actual[vehicle_index];
			scheduling.et[vehicle_index] = vehicles.et_actual[vehicle_index];
			scheduling.dt[vehicle_index] = -1;
		}
		else if (scheduling.state[vehicle_index] == "LV") {
			scheduling.index_LVs.push_back(vehicle_index);
			scheduling.st[vehicle_index] = vehicles.st_actual[vehicle_index];
			scheduling.et[vehicle_index] = vehicles.et_actual[vehicle_index];
			//scheduling.dt[vehicle_index] = -1;
		}
	}

	sort(scheduling.index_DVs.begin(), scheduling.index_DVs.end(), SortAscending_i(scheduling.departurePosition_index));
	for (int n = 0; n < scheduling.index_DVs.size(); ++n) {
		vehicle_index = scheduling.index_DVs[n];
		scheduling.departurePosition_index[vehicle_index] = n + 1;
	}
	sort(scheduling.index_RDVs.begin(), scheduling.index_RDVs.end(), SortAscending_i(vehicles.link_priority));
	sort(scheduling.index_RDVs.begin(), scheduling.index_RDVs.end(), SortAscending_d(scheduling.st));
	sort(scheduling.index_RDVs.begin(), scheduling.index_RDVs.end(), SortAscending_i(scheduling.departurePosition_index));
	for (int n = 0; n < scheduling.index_RDVs.size(); ++n) {
		vehicle_index = scheduling.index_RDVs[n];
		scheduling.departurePosition_index[vehicle_index] = scheduling.index_DVs.size() + n + 1;
	}
	for (int i = 0; i < map.index_entryLane.size(); ++i) {
		sort(scheduling.index_EVs[i].begin(), scheduling.index_EVs[i].end(), SortAscending_d(scheduling.position_lane));
	}


}



