#ifndef osm_H
#define osm_H

#include <string>
#include <fstream>
#include <iostream>
#include <vector>
#include <unordered_map>
#include "json.hpp"
using namespace std;
using json = nlohmann::json;

class osm{
	private:

		int lane_count;
		vector<string> lane_id_all;
		unordered_map<string, int> lane_index_all;

		vector<string> lane_id_entry;
		vector<string> lane_id_exit;
		vector<string> lane_id_link;

		unordered_map<string, string> lane_type;
		unordered_map<string, string> lane_direction;
		unordered_map<string, double> lane_length;
		unordered_map<string, double> lane_speed_limit;

		unordered_map<string, vector<double>> lane_left_boundary;
		unordered_map<string, vector<double>> lane_right_boundary;
		unordered_map<string, vector<double>> lane_up_boundary;
		unordered_map<string, vector<double>> lane_down_boundary;

		unordered_map<string, vector<string>> lane_from_id;
		unordered_map<string, vector<string>> lane_to_id;

		unordered_map<string, vector<string>> lane_conflicting_lanes;
		vector<vector<int>> lane_conflict_status;

        unordered_map<string, int> lane_priority;

	public:

		osm(string json_file_name);

		int get_laneCount();
		vector<string> get_laneIdAll();
		vector<string> get_laneIdEntry();
		vector<string> get_laneIdExit();
		vector<string> get_laneIdLink();
		
		int get_laneIndex(string lane_id);
		string get_laneType(string lane_id);
		string get_laneDirection(string lane_id);
		double get_laneLength(string lane_id);
		double get_laneSpeedLimit(string lane_id);

		vector<double> get_laneLeftBoundary(string lane_id);
		vector<double> get_laneRightBoundary(string lane_id);
		vector<double> get_laneUpBoundary(string lane_id);
		vector<double> get_laneDownBoundary(string lane_id);

		bool hasConflict(string lane_id1, string lane_id2);

        int get_lanePriority(string lane_id);

};

#endif