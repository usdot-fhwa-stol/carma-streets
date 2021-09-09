
#include <string>
#include <fstream>
#include <iostream>
#include <vector>
#include <unordered_map>
#include "json.hpp"

#include "osm.h"

using namespace std;
using json = nlohmann::json;

/* */
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
			} else if (element.value()["type"] == "exit"){
				lane_id_exit.push_back(element.key());
			} else if (element.value()["type"] == "link"){
				lane_id_link.push_back(element.key());
			} else{
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

        for (string lane_id : lane_id_all){
            if (lane_type[lane_id] != "link"){
                lane_priority[lane_id] = -1;
            } else{
                if (lane_direction[lane_id] == "s"){
                    lane_priority[lane_id] = 1;
                } else if (lane_direction[lane_id] == "l"){
                    lane_priority[lane_id] = 2;
                } else if (lane_direction[lane_id] == "r"){
                    lane_priority[lane_id] = 3;
                } else{
                    lane_priority[lane_id] = -1;
                    cout << "unknown lane direction " + lane_direction[lane_id] + " is detected!" << endl;
                }
            }
        }

        cout << "successful" << endl;

	} else{
		cout << "error - json file " + json_file_name + " deos not exist!" << endl;
	}
	file_input.close();
}

/* */
int osm::get_laneCount(){return lane_count;}

/* */
vector<string> osm::get_laneIdAll(){return lane_id_all;}

/* */
vector<string> osm::get_laneIdEntry(){return lane_id_entry;}

/* */
vector<string> osm::get_laneIdExit(){return lane_id_exit;}

/* */
vector<string> osm::get_laneIdLink(){return lane_id_link;}

/* */
int osm::get_laneIndex(string lane_id){return lane_index_all[lane_id];}

/* */
string osm::get_laneType(string lane_id){return lane_type[lane_id];}

/* */
string osm::get_laneDirection(string lane_id){return lane_direction[lane_id];}

/* */
double osm::get_laneLength(string lane_id){return lane_length[lane_id];}

/* */
double osm::get_laneSpeedLimit(string lane_id){return lane_speed_limit[lane_id];}

/* */
vector<double> osm::get_laneLeftBoundary(string lane_id){return lane_left_boundary[lane_id];}

/* */
vector<double> osm::get_laneRightBoundary(string lane_id){return lane_right_boundary[lane_id];}

/* */
vector<double> osm::get_laneUpBoundary(string lane_id){return lane_up_boundary[lane_id];}

/* */
vector<double> osm::get_laneDownBoundary(string lane_id){return lane_down_boundary[lane_id];}

/* */
bool osm::hasConflict(string lane_id1, string lane_id2){
	if(lane_conflict_status[lane_index_all[lane_id1]][lane_index_all[lane_id2]] == 1){
		return true;
	} else{
		return false;
	}
}

/* */
int osm::get_lanePriority(string lane_id){return lane_priority[lane_id];}

