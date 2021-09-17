
#include <string>
#include <vector>
#include <algorithm>
#include <math.h>

#include "configuration.h"
#include "osm.h"
#include "vehicle.h"
#include "sorting.h"
#include "scheduling.h"

using namespace std;

extern configuration config;
extern osm localmap;

/* */
scheduling::scheduling(unordered_map<string, vehicle> list_veh){

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

		access.push_back(element.second.get_access());
		link_priority.push_back(element.second.get_linkPriority());

		/* estimate the vehicle location, speed, state, etc. at a given future timestamp! */
		if (time.back() < config.get_curSchedulingT() + config.get_schedulingDelta()){
			
			for (int i = 0; i < element.second.get_futureInfo().size(); ++i){
				
				if (element.second.get_futureInfo()[i].timestamp >= config.get_curSchedulingT() + config.get_schedulingDelta() || 
					i == element.second.get_futureInfo().size() - 1){

					
					time.back() = element.second.get_futureInfo()[i].timestamp;
					lat.back() = element.second.get_futureInfo()[i].lat;
					lng.back() = element.second.get_futureInfo()[i].lng;
					lane_id.back() = element.second.get_futureInfo()[i].lane_id;
					distance.back() = element.second.get_futureInfo()[i].distance;
					speed.back() = element.second.get_futureInfo()[i].speed;
					acceleration.back() = element.second.get_futureInfo()[i].acceleration;

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

/* */
vector<string> scheduling::get_vehicleIdList(){return veh_id;}

/* */
unordered_map<string, int> scheduling::get_vehicleIndexList(){return veh_index;}

/* */
vector<double> scheduling::get_timeList(){return time;}

/* */
vector<double> scheduling::get_latList(){return lat;}

/* */
vector<double> scheduling::get_lngList(){return lng;}

/* */
vector<double> scheduling::get_speedList(){return speed;}

/* */
vector<double> scheduling::get_accelList(){return acceleration;}

/* */
vector<string> scheduling::get_laneIdList(){return lane_id;}

/* */
vector<string> scheduling::get_stateList(){return state;}

/* */
vector<int> scheduling::get_departPosIndexList(){return departurePosition_index;}

/* */
vector<double> scheduling::get_distanceList(){return distance;}

/* */
vector<double> scheduling::get_clearTimeList(){return clearance_time;}

/* */
vector<double> scheduling::get_estList(){return est;}

/* */
vector<double> scheduling::get_stList(){return st;}

/* */
vector<double> scheduling::get_etList(){return et;}

/* */
vector<double> scheduling::get_dtList(){return dt;}

/* */
vector<vector<int>> scheduling::get_indexEVs(){return index_EVs;}

/* */
vector<int> scheduling::get_indexRDVs(){return index_RDVs;}

/* */
vector<int> scheduling::get_indexDVs(){return index_DVs;}

/* */
vector<int> scheduling::get_indexLVs(){return index_LVs;}

/* */
vector<bool> scheduling::get_accessList(){return access;}

/* */
vector<int> scheduling::get_linkPriorityList(){return link_priority;}

/* */
void scheduling::set_st(int v_index, double st_added){st[v_index] = st_added;}

/* */
void scheduling::set_et(int v_index, double et_added){et[v_index] = et_added;}

/* */
void scheduling::set_dt(int v_index, double dt_added){dt[v_index] = dt_added;}

/* */
void scheduling::set_departPosIndex(int v_index, int pos_index){departurePosition_index[v_index] = pos_index;}

/* */
void scheduling::set_access(int v_index, bool access_value){access[v_index] = access_value;}