#ifndef scheduling_H
#define scheduling_H

#include <string>
#include <iostream>
#include <vector>
#include "json.hpp"
#include "osm.h"
#include "vehicle.h"
#include "sorting.h"
using namespace std;
using json = nlohmann::json;

class scheduling{
	private:
		
		vector<string> veh_id;
		unordered_map<string, int> veh_index;

		//vector<time_t> time;
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

	public:

		scheduling(unordered_map<string, vehicle> list_veh, osm localmap);


		vector<string> get_vehicleIdList(){return veh_id;}
		unordered_map<string, int> get_vehicleIndexList(){return veh_index;}

		vector<double> get_timeList(){return time;}
		vector<double> get_latList(){return lat;}
		vector<double> get_lngList(){return lng;}
		vector<double> get_speedList(){return speed;}
		vector<double> get_accelList(){return acceleration;}
		vector<string> get_laneIdList(){return lane_id;}
		vector<string> get_stateList(){return state;}

		vector<int> get_departPosIndexList(){return departurePosition_index;}
		vector<double> get_distanceList(){return distance;}
		vector<double> get_clearTimeList(){return clearance_time;}
		vector<double> get_estList(){return est;}
		vector<double> get_stList(){return st;}
		vector<double> get_etList(){return et;}
		vector<double> get_dtList(){return dt;}

		vector<vector<int>> get_indexEVs(){return index_EVs;}
		vector<int> get_indexRDVs(){return index_RDVs;}
		vector<int> get_indexDVs(){return index_DVs;}
		vector<int> get_indexLVs(){return index_LVs;}


		void set_st(int v_index, double st_added){st[v_index] = st_added;}
		void set_et(int v_index, double et_added){et[v_index] = et_added;}
		void set_dt(int v_index, double dt_added){dt[v_index] = dt_added;}


};

/*
vehicle::vehicle(){

} */


scheduling::scheduling(unordered_map<string, vehicle> list_veh, osm localmap){

	/*
	veh_id.clear();
	veh_index.clear();

	time.clear();
	lat.clear();
	lng.clear();
	speed.clear();
	acceleration.clear();
	lane_id.clear();
	state.clear();

	departurePosition_index.clear();
	distance.clear();
	clearance_time.clear();
	est.clear();
	st.clear();
	et.clear();
	dt.clear();

	index_EVs.clear();
	index_RDVs.clear();
	index_DVs.clear();
	index_LVs.clear();
	*/
	index_EVs.resize(localmap.get_laneIdEntry().size());
	for (auto& element : list_veh){
		
		veh_id.push_back(element.first);
		int vehicle_index = veh_id.size() - 1;
		veh_index[element.first] = vehicle_index;
		
		time.push_back(element.second.get_curTime());
		lat.push_back(element.second.get_curLat());
		lng.push_back(element.second.get_curLng());
		speed.push_back(element.second.get_curSpeed());
		acceleration.push_back(element.second.get_curAccel());
		lane_id.push_back(element.second.get_curLaneID());
		state.push_back(element.second.get_curState());

		departurePosition_index.push_back(element.second.get_departurePosition());
		distance.push_back(element.second.get_curDistance());
		clearance_time.push_back(-1);							// !!!
		est.push_back(-1);										// !!!
		st.push_back(element.second.get_actualST());
		et.push_back(element.second.get_actualET());
		dt.push_back(element.second.get_actualDT());


		/* estimate the vehicle location, speed, state, etc. at a given future timestamp! */
		if (time.back() < config.get_curSchedulingT() + config.get_schedulingDelta()){
			for (int i = 0; i < element.second.get_futureTime().size(); ++i){
				if (element.second.get_futureTime()[i] >= config.get_curSchedulingT() + config.get_schedulingDelta() || 
				i == element.second.get_futureTime().size() - 1){
					
					time.back() = element.second.get_futureTime()[i];
					lat.back() = element.second.get_futureLat()[i];
					lng.back() = element.second.get_futureLng()[i];
					lane_id.back() = element.second.get_futureLaneID()[i];
					distance.back() = element.second.get_futureDistance()[i];
					speed.back() = element.second.get_futureSpeed()[i];
					acceleration.back() = element.second.get_futureAccel()[i];

					if (state.back() == "EV"){
						if (lane_id.back() != element.second.get_entryLaneID()){
							lane_id.back() = element.second.get_entryLaneID();
							distance.back() = 0.1;
						}
					} else if (state.back() == "RDV"){
						if (lane_id.back() != element.second.get_entryLaneID()){
							lane_id.back() = element.second.get_entryLaneID();
							distance.back() = 0.1;
						}
					} else if (state.back() == "DV"){
						if (lane_id.back() == element.second.get_entryLaneID()){
							lane_id.back() = element.second.get_linkID();
							distance.back() = localmap.get_laneLength(lane_id.back());
						} else if (lane_id.back() != element.second.get_linkID()){
							state.back() = "LV";
							dt.back() = time.back();
						}
					}

					break;

				}
			}
		}


		/* if the vehicle is in the EV state, estimate its earliest stopping time */
		if (state.back() == "EV"){

			double dx1 = max(distance.back(), 0.01);
			double dx2 = -pow(speed.back(), 2) / (2 * element.second.get_decelMax());
			double dx3 = ((pow(localmap.get_laneSpeedLimit(lane_id.back()), 2) - pow(speed.back(), 2)) / (2 * element.second.get_accelMax())) - 
				(pow(localmap.get_laneSpeedLimit(lane_id.back()), 2) / (2 * element.second.get_decelMax()));
			double t_a;
			double t_d;
			double t_cr;
			if (dx1 < dx2){
				double d_emergency = -pow(speed.back(), 2) / (2 * dx1);
				t_a = 0.0;
				t_d = - speed.back() / d_emergency;
				t_cr = 0.0;
			} 
			else if (dx1 < dx3){
				double v_mid = sqrt(element.second.get_decelMax() * ((2 * dx1 * element.second.get_accelMax()) + pow(speed.back(), 2)) / 
					(element.second.get_decelMax() - element.second.get_accelMax()));
				t_a = (v_mid - speed.back()) / element.second.get_accelMax();
				t_d = -v_mid / element.second.get_decelMax();
				t_cr = 0.0;
			}
			else{
				double v_mid = localmap.get_laneSpeedLimit(lane_id.back());
				t_a = (v_mid - speed.back()) / element.second.get_accelMax();
				t_d = -v_mid / element.second.get_decelMax();
				t_cr = (dx1 - dx3) / v_mid;
			}
			est.back() = time.back() + max(t_a + t_cr + t_d, 0.01);
		}


		/* calculate the clearance time required by the vehicle to pass through its associated connection link*/
		if (state.back() == "EV" || state.back() == "RDV") {
			if (pow(localmap.get_laneSpeedLimit(element.second.get_linkID()), 2) / (2 * element.second.get_accelMax()) < localmap.get_laneLength(element.second.get_linkID())){
				clearance_time.back() = (localmap.get_laneLength(element.second.get_linkID()) / localmap.get_laneSpeedLimit(element.second.get_linkID())) + 
					(localmap.get_laneSpeedLimit(element.second.get_linkID()) / (2 * element.second.get_accelMax()));
			} else{
				clearance_time.back() = sqrt((2 * localmap.get_laneLength(element.second.get_linkID())) / element.second.get_accelMax());
			}
			clearance_time.back() = ceil(clearance_time.back());
		}
		else if (state.back() == "DV"){
			double dx = (pow(localmap.get_laneSpeedLimit(element.second.get_linkID()), 2) - pow(speed.back(), 2)) / (2 * element.second.get_accelMax());
			if (distance.back() < dx){
				clearance_time.back() = (sqrt(pow(speed.back(), 2) + (2 * element.second.get_accelMax() * distance.back())) - speed.back()) / element.second.get_accelMax();
			} else{
				clearance_time.back() = ((localmap.get_laneSpeedLimit(element.second.get_linkID()) - speed.back()) / element.second.get_accelMax()) + 
					((distance.back() - dx) / localmap.get_laneSpeedLimit(element.second.get_linkID()));
			}
			clearance_time.back() = ceil(clearance_time.back());
		}


		/* create the EV, RDV, DV, and LV lists */
		if (state.back() == "EV"){
			for (int i = 0; i < index_EVs.size(); ++i){
				if (lane_id.back() == localmap.get_laneIdEntry()[i]){
					index_EVs[i].push_back(vehicle_index);
					break;
				}
			}
		}
		else if (state.back() == "RDV"){
			index_RDVs.push_back(vehicle_index);
		}
		else if (state.back() == "DV"){
			index_DVs.push_back(vehicle_index);
		}
		else if (state.back() == "LV"){
			index_LVs.push_back(vehicle_index);
		}

	}

	sort(index_DVs.begin(), index_DVs.end(), sorting<int>(departurePosition_index, "asc"));
	for (int n = 0; n < index_DVs.size(); ++n){
		int vehicle_index = index_DVs[n];
		departurePosition_index[vehicle_index] = n + 1;
	}
	//sort(index_RDVs.begin(), index_RDVs.end(), sorting(), "asc"));
	sort(index_RDVs.begin(), index_RDVs.end(), sorting<double>(st, "asc"));
	sort(index_RDVs.begin(), index_RDVs.end(), sorting<int>(departurePosition_index, "asc"));
	for (int n = 0; n < index_RDVs.size(); ++n) {
		int vehicle_index = index_RDVs[n];
		departurePosition_index[vehicle_index] = index_DVs.size() + n + 1;
	}
	for (int i = 0; i < index_EVs.size(); ++i) {
		sort(index_EVs[i].begin(), index_EVs[i].end(), sorting<double>(distance, "asc"));
	}

}



#endif