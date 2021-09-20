
#include <string>
#include <fstream>
#include <vector>
#include <unordered_map>

#include "spdlog/spdlog.h"
#include "spdlog/cfg/env.h"
#include <rapidjson/document.h>
#include <rapidjson/istreamwrapper.h>

#include "osm.h"

using namespace std;
using namespace rapidjson;

/* !!! must be revised !!! */
osm::osm(string json_file_name){
	
	/* dummy intersection */
	int count = 0;
	
	// entry lanes
	for (int i = 0; i < 4; ++i){
		
		lane_information li;
		li.id = "entry_lane_" + to_string(i);
		li.index = count;
		li.type = "entry";
		li.direction = "straight";
		li.length = 300; // m
		li.speed_limit = 14; // m/s
		li.priority = -1;
		li.from_id = {};
		li.to_id = {};
		li.conflicting_lane_id = {};

		lane_info[li.id] = li;
		lane_id_all.push_back(li.id);
		lane_id_entry.push_back(li.id);

		count += 1;
	}

	// exit lanes
	for (int i = 0; i < 4; ++i){
		
		lane_information li;
		li.id = "exit_lane_" + to_string(i);
		li.index = count;
		li.type = "exit";
		li.direction = "straight";
		li.length = 300; // m
		li.speed_limit = 14; // m/s
		li.priority = -1;
		li.from_id = {};
		li.to_id = {};
		li.conflicting_lane_id = {};

		lane_info[li.id] = li;
		lane_id_all.push_back(li.id);
		lane_id_exit.push_back(li.id);

		count += 1;
	}

	// connection links
	for (int i = 0; i < 4; ++i){
		
		lane_information li;
		li.id = "link_" + to_string(i);
		li.index = count;
		li.type = "link";
		li.direction = "straight";
		li.length = 20; // m
		li.speed_limit = 14; // m/s
		li.priority = 1;
		li.from_id = {};
		li.to_id = {};
		li.conflicting_lane_id = {};

		lane_info[li.id] = li;
		lane_id_all.push_back(li.id);
		lane_id_link.push_back(li.id);

		count += 1;
	}

	lane_count = count;

	for (int i = 0; i < 4; ++i){
		lane_info[lane_id_entry[i]].to_id.push_back(lane_id_link[i]);

		lane_info[lane_id_link[i]].from_id.push_back(lane_id_entry[i]);
		lane_info[lane_id_link[i]].to_id.push_back(lane_id_exit[i]);

		lane_info[lane_id_exit[i]].from_id.push_back(lane_id_link[i]);

		for (int j = 0; j < 4; ++j){
			lane_info[lane_id_link[i]].conflicting_lane_id.push_back(lane_id_link[j]);
		}
	}

	lane_conflict_status.resize(lane_count, vector<int>(lane_count));
	for (string lane_id1 : lane_id_link){
		for (string lane_id2 : lane_info[lane_id1].conflicting_lane_id){
			lane_conflict_status[lane_info[lane_id1].index][lane_info[lane_id2].index] = 1;
		}
		lane_conflict_status[lane_info[lane_id1].index][lane_info[lane_id1].index] = 1;
	}

	/*
	ifstream file_input(json_file_name);
	if (file_input){
		IStreamWrapper isw(file_input);
		Document json_map;
		json_map.ParseStream(isw);
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
			} 
			else{
				if (lane_direction[lane_id] == "s"){
					lane_priority[lane_id] = 1;
				}
				else if (lane_direction[lane_id] == "l"){
					lane_priority[lane_id] = 2;
				}
				else if (lane_direction[lane_id] == "r"){
					lane_priority[lane_id] = 3;
				}
				else{
					lane_priority[lane_id] = -1;
					spdlog::critical("unknown lane direction is detected!");
				}
			}
		}
	} 
	else{
		spdlog::critical("the osm file does not exist!");	
	}
	file_input.close(); 
	*/
	
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
int osm::get_laneIndex(string lane_id){return lane_info[lane_id].index;}

/* */
string osm::get_laneType(string lane_id){return lane_info[lane_id].type;}

/* */
string osm::get_laneDirection(string lane_id){return lane_info[lane_id].direction;}

/* */
double osm::get_laneLength(string lane_id){return lane_info[lane_id].length;}

/* */
double osm::get_laneSpeedLimit(string lane_id){return lane_info[lane_id].speed_limit;}

/* */
bool osm::hasConflict(string lane_id1, string lane_id2){
	if(lane_conflict_status[lane_info[lane_id1].index][lane_info[lane_id2].index] == 1){
		return true;
	} else{
		return false;
	}
}

/* */
int osm::get_lanePriority(string lane_id){return lane_info[lane_id].priority;}

