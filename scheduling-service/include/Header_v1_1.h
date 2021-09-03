#pragma once
#include <stdio.h>
#include <iostream>
#include <cmath>
#include <vector>
#include <numeric>
#include <cstdlib>
#include <ctime>
#include <algorithm>
#include <iterator>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <map>
#include <unordered_map>
#include <string>
#include <chrono>
#include <thread>
#include <mutex>
#include <condition_variable>
//#include <json/value.h>
#include <json/json.h>
#include<json/writer.h>

//#include <sys/time.h>

using namespace std;
using std::chrono::duration_cast;
using std::chrono::milliseconds;
using std::chrono::seconds;
using std::chrono::system_clock;


class Setting {
	public:

		double consumer_frequency = 0.1; // sec (the frequency of consuming information from kafka)
		double scheduling_frequency = 1.0; // sec (the frequency of scheduling vehicles with CARMA Streets)
		int shutdown_switch = 0;
		int scheduling_switch = 1;

		//time_t last_schedule = time(NULL);
		//time_t scheduling_start_time = time(NULL);
		double last_schedule = double(duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count()) / 1000;
		double scheduling_start_time = double(duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count()) / 1000;


};

class Map {
	public:

		double stop_loc_buffer = 6;
		double stop_speed_buffer = 0.1;

		int id_count;
		vector<string> id_all;
		unordered_map<string, int> index_all;
		vector<double> end_lat;
		vector<double> end_lng;
		vector<double> speed_max;
		vector<double> length;
		vector<string> direction;
		vector<vector<int>> index_p;
		vector<vector<int>> index_n;

		vector<int> priority;

		//vector<double> stopBar_lat;
		//vector<double> stopBar_lng;
		vector<int> index_entryLane;
		vector<int> index_link;
		vector<int> index_exitLane;

		

		vector<vector<int>> link_conflict;
};

class Vehicles {
	public:

		vector<string> id;
		unordered_map<string, int> index;

		vector<double> length;
		vector<double> min_gap;
		vector<double> reaction_time;
		vector<double> accel_max;
		vector<double> decel_max;
		vector<string> direction;
		vector<int> entryLane_index;
		vector<int> link_index;
		vector<int> link_priority;

		vector<int> departurePosition_index;
		vector<int> flexibility_ET;
		vector<double> flexibility_ST;
		//vector<double> clearTime;
		vector<string> access;

		//vector<time_t> st_actual;
		//vector<time_t> et_actual;
		//vector<time_t> dt_actual;
		vector<double> st_actual;
		vector<double> et_actual;
		vector<double> dt_actual;

		//vector<vector<time_t>> time_past;
		vector<vector<double>> time_past;
		vector<vector<double>> lat_past;
		vector<vector<double>> lng_past;
		vector<vector<double>> speed;
		vector<vector<double>> acceleration;
		vector<vector<int>> lane_index;
		vector<vector<string>> state;

		//vector<vector<time_t>> time_future;
		vector<vector<double>> time_future;
		vector<vector<double>> lat_future;
		vector<vector<double>> lng_future;

};

class Scheduling {
	public:

		vector<string> id;
		unordered_map<string, int> index;

		//vector<time_t> time;
		vector<double> time;
		vector<double> lat;
		vector<double> lng;
		vector<double> speed;
		vector<double> acceleration;
		vector<int> lane_index;
		vector<string> state;

		vector<int> departurePosition_index;
		vector<double> position_lane;
		vector<double> clearance_time;
		//vector<time_t> est;
		//vector<time_t> st;
		//vector<time_t> et;
		//vector<time_t> dt;
		vector<double> est;
		vector<double> st;
		vector<double> et;
		vector<double> dt;

		vector<vector<int>> index_EVs;
		vector<int> index_RDVs;
		vector<int> index_DVs;
		vector<int> index_LVs;
};


/*	*/
//int sim_veh_count = 0;
class Simulated_Vehicle {
	public:
		string vehicle_id;
		int vehicle_index;
		Simulated_Vehicle(string id, int index) {
			vehicle_id = id;
			vehicle_index = index;
		}

		Simulated_Vehicle() {
			vehicle_id = "";
			vehicle_index = 0;
		}

		//int vehicle_index = sim_veh_count;
		double timestamp;
		double lat;
		double lng;
		double speed_cur;
		double speed_next;
		double acceleration;
		double reaction_time;
		double accel_max;
		double decel_max;
		double length;
		double min_gap;
		
		double est;
		double st;
		double et;
		double dt;

		string direction;
		string access;
		string state;

		int entryLane_index;
		int lane_index;
		int link_index;
		double position_lane;

		vector<double> time_future;
		vector<double> lat_future;
		vector<double> lng_future;

		void location_update(double delta_t) {
			/* update lat, lng, speed_cur, and acceleration */
			// lat =
			// lng = 
			acceleration = (speed_next - speed_cur) / delta_t;
			speed_cur = speed_next;
		}
		

};







