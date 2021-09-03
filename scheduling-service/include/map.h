#ifndef osm_H
#define osm_H

#include <string>
#include <fstream>
#include <iostream>
#include <unordered_map>
#include <json.hpp>
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

	public:

		osm(string json_file_name);

		int get_laneCount(){return lane_count;}
		vector<string> get_laneIdAll(){return lane_id_all;}
		vector<string> get_laneIdEntry(){return lane_id_entry;}
		vector<string> get_laneIdExit(){return lane_id_exit;}
		vector<string> get_laneIdLink(){return lane_id_link;}
		
		int get_laneIndex(string lane_id){return lane_index_all[lane_id];}
		string get_laneType(string lane_id){return lane_type[lane_id];}
		string get_laneDirection(string lane_id){return lane_direction[lane_id];}
		double get_laneLength(string lane_id){return lane_length[lane_id];}
		double get_laneSpeedLimit(string lane_id){return lane_speed_limit[lane_id];}

		vector<double> get_laneLeftBoundary(string lane_id){return lane_left_boundary[lane_id];}
		vector<double> get_laneRightBoundary(string lane_id){return lane_right_boundary[lane_id];}
		vector<double> get_laneUpBoundary(string lane_id){return lane_up_boundary[lane_id];}
		vector<double> get_laneDownBoundary(string lane_id){return lane_down_boundary[lane_id];}

		bool hasConflict(string lane_id1, string lane_id2){
			if(lane_conflict_status[lane_index_all[lane_id1]][lane_index_all[lane_id2]] == 1){
				return true;
			}else{
				return false;
			}
		}

};



osm::osm(string json_file_name){
	ifstream file_input(json_file_name);
	if (file_input){
		
		json json_map;
		file_input >> json_map;

		lane_count = json_map.size();
		int count = 0;
		for (auto& element : json_map.items()){
			
			lane_id_all.push_back(element.key());
			lane_index_all[element.key()] = count;

			lane_type[element.key()] = element.value()["type"];
			lane_direction[element.key()] = element.value()["direction"];
			lane_length[element.key()] = element.value()["length"];
			lane_speed_limit[element.key()] = element.value()["speed_limit"];
			
			lane_left_boundary[element.key()].insert(lane_left_boundary[element.key()].begin(), 
			begin(element.value()["left_boundary"]), end(element.value()["left_boundary"]));
			
			lane_right_boundary[element.key()].insert(lane_right_boundary[element.key()].begin(), 
			begin(element.value()["right_boundary"]), end(element.value()["right_boundary"]));

			lane_up_boundary[element.key()].insert(lane_up_boundary[element.key()].begin(), 
			begin(element.value()["up_boundary"]), end(element.value()["up_boundary"]));

			lane_down_boundary[element.key()].insert(lane_down_boundary[element.key()].begin(), 
			begin(element.value()["down_boundary"]), end(element.value()["down_boundary"]));

			lane_from_id[element.key()].insert(lane_from_id[element.key()].begin(), 
			begin(element.value()["from_id"]), end(element.value()["from_id"]));

			lane_to_id[element.key()].insert(lane_to_id[element.key()].begin(), 
			begin(element.value()["to_id"]), end(element.value()["to_id"]));

			lane_conflicting_lanes[element.key()].insert(lane_conflicting_lanes[element.key()].begin(), 
			begin(element.value()["conflict_lanes"]), end(element.value()["conflict_lanes"]));

			if (element.value()["type"] == "entry"){
				lane_id_entry.push_back(element.key());
			}else if (element.value()["type"] == "exit"){
				lane_id_exit.push_back(element.key());
			}else if (element.value()["type"] == "link"){
				lane_id_link.push_back(element.key());
			}else{
				cout << "unknown lane type " + to_string(element.value()["type"]) + " is detected!" << endl;
			}
			count += 1;
		}

		lane_conflict_status.resize(lane_count, vector<int>(lane_count));
		for (string lane_id1 : lane_id_link){
			for (string lane_id2 : lane_conflicting_lanes[lane_id1]){
				lane_conflict_status[lane_index_all[lane_id1]][lane_index_all[lane_id2]] = 1;
			}
			lane_conflict_status[lane_index_all[lane_id1]][lane_index_all[lane_id1]] = 1;
		}

	}else{
		cout << "error - json file " + json_file_name + " deos not exist!" << endl;
	}
	file_input.close();
}


#endif