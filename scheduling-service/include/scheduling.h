#ifndef scheduling_H
#define scheduling_H

#include <string>
#include <iostream>
#include <vector>
#include "json.hpp"
#include "configuration.h"
#include "osm.h"
#include "vehicle.h"
#include "sorting.h"
using namespace std;
using json = nlohmann::json;

class scheduling{
	private:
		
		vector<string> veh_id;
		unordered_map<string, int> veh_index;

		vector<double> time;
		vector<double> lat;
		vector<double> lng;
		vector<double> speed;
		vector<double> acceleration;
		vector<string> lane_id;
		vector<string> state;

		vector<int> departurePosition_index;
		vector<double> distance;
		vector<double> clearance_time;
		vector<double> est;
		vector<double> st;
		vector<double> et;
		vector<double> dt;

		vector<vector<int>> index_EVs;
		vector<int> index_RDVs;
		vector<int> index_DVs;
		vector<int> index_LVs;

		vector<bool> access;
		vector<int> link_priority;

	public:

		scheduling(unordered_map<string, vehicle> list_veh);


		vector<string> get_vehicleIdList();
		unordered_map<string, int> get_vehicleIndexList();

		vector<double> get_timeList();
		vector<double> get_latList();
		vector<double> get_lngList();
		vector<double> get_speedList();
		vector<double> get_accelList();
		vector<string> get_laneIdList();
		vector<string> get_stateList();

		vector<int> get_departPosIndexList();
		vector<double> get_distanceList();
		vector<double> get_clearTimeList();
		vector<double> get_estList();
		vector<double> get_stList();
		vector<double> get_etList();
		vector<double> get_dtList();

		vector<vector<int>> get_indexEVs();
		vector<int> get_indexRDVs();
		vector<int> get_indexDVs();
		vector<int> get_indexLVs();

		vector<bool> get_accessList();
		vector<int> get_linkPriorityList();


		void set_st(int v_index, double st_added);
		void set_et(int v_index, double et_added);
		void set_dt(int v_index, double dt_added);
		void set_departPosIndex(int v_index, int pos_index);
		void set_access(int v_index, bool access_value);
};


#endif