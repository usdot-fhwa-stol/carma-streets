#pragma once
#ifndef intersection_client_H
#define intersection_client_H

#include <string>
#include <fstream>
#include <vector>
#include <unordered_map>
#include <math.h>

#include "spdlog/spdlog.h"
#include "spdlog/cfg/env.h"

#include <iostream>
#include "OAIDefaultApi.h"

using namespace OpenAPI;
using namespace std;

struct lane_information {

    /* lane id */
    string id;

    /* lane index */
    int index;

    /* lane type (entry, exit, connection link) */
    string type;

    /* lane direction (right, straight, left) */
    string direction;

    /* lane length */
    double length;

    /* the speed limit on the lane */
    double speed_limit;

    /* lane priority (defined for connection links)
    *  note: a lower integer number indicates a higher priority) 
    *  example: straight = 1, left = 2, right = 3
    */
    int priority;

    // /* the list of lane ids that are connected to the beginning of the subject lane */
    // vector<string> from_id;

    // /* the list of lane ids that are connected to the end of the subject lane */
    // vector<string> to_id;

    /* the list of lane ids that has conflicting direction with the subject lane */
    vector<string> conflicting_lane_id;

};

class intersection_client : public QObject
{
    private:
        
        bool is_running_indicator;

        /* intersection name */
        string intersection_name;

        /* intersection id */
        int intersection_id;

        /* number of lanes */
        int lane_count;

		/* list of lanes and their information */
		unordered_map<string, lane_information> lane_info;

		/* list of all lane ids */
		vector<string> lane_id_all;

		/* list of entry lane ids */
		vector<string> lane_id_entry;

		/* list of exit lane ids */
		vector<string> lane_id_exit;
		
		/* list of connection link ids */
		vector<string> lane_id_link;

		/* a 2D matrix that shows whether two lanes has conflicting directions or not
		*  note: it takes lane indexes as inputs rather than lane ids
		*/
		vector<vector<int>> lane_conflict_status;

    public:
        intersection_client(/* args */){};
        ~intersection_client(){};
        void call();

        bool is_running();
        string get_intersectionName();
        int get_intersectionId();
        int get_laneCount();

		vector<string> get_laneIdAll();
		vector<string> get_laneIdEntry();
		vector<string> get_laneIdExit();
		vector<string> get_laneIdLink();
		
		int get_laneIndex(string lane_id);
		string get_laneType(string lane_id);
		double get_laneLength(string lane_id);
		double get_laneSpeedLimit(string lane_id);

		bool hasConflict(string lane_id1, string lane_id2);

        int get_lanePriority(string lane_id);
};

#endif