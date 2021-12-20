#ifndef scheduling_H
#define scheduling_H

#include <string>
#include <vector>
#include <algorithm>
#include <math.h>
#include <set>

#include "spdlog/spdlog.h"
#include "spdlog/cfg/env.h"

#include "configuration.h"
#include "vehicle.h"
#include "sorting.h"
#include "intersection_client.h"

using namespace std;


/* !!! must be revised !!! */
class scheduling{
	private:
		
		/* list of vehicle ids */
		vector<string> veh_id;

		/* list of vehicle indexes
		*  note: at the begining of each schedule, CARMA Streets will assign integer parameters to vehicles as their indexes.
		*/
		unordered_map<string, int> veh_index;

		/*** list of vehicle latest timestamps 
		* @brief the unit of timestamp here is second with decimal places.
		*/
		vector<double> time;

		/* list of vehicle speeds */
		vector<double> speed;

		/* list of vehicle accelerations */
		vector<double> acceleration;

		/* list of vehicle lane ids */
		vector<string> lane_id;

		/* list of vehicle states */
		vector<string> state;

		/* list of vehicle departure position indexes */
		vector<int> departurePosition_index;

		/* list of distances between vehicles and the end of their lanes */
		vector<double> distance;

		/* list of vehicle clearance times
		*  clearance time: the time a vehicle needs to pass through its associated connection link
		*/
		vector<double> clearance_time;

		/* list of vehicle accesses */
		vector<bool> access;

		/* list of vehicle link priorities */
		vector<int> link_priority;

		/*** list of vehicle earliest stopping times 
		* @brief the unit of timestamp here is second with decimal places.
		*/
		vector<double> est;

		/*** list of vehicle stopping times 
		* @brief the unit of timestamp here is second with decimal places.
		*/ 
		vector<double> st;

		/*** list of vehicle entering times 
		* @brief the unit of timestamp here is second with decimal places.
		*/
		vector<double> et;

		/*** list of vehicle departure times 
		* @brief the unit of timestamp here is second with decimal places.
		*/
		vector<double> dt;
		
		/* note: the sequence of information in the above vectors will always remain the same.
		*        therefore, the ith element in any of the above vectors corresponds to the vehicle with index i!
		*/

		/*** list of entering vehice (EV) indexes.
		*  @brief EVs are seperated based on their entry lanes
		*/
		vector<vector<int>> index_EVs;

		/* list of ready-to-depart vehicle (RDV) indexes */ 
		vector<int> index_RDVs;

		/* list of departing vehicle (DV) indexes */
		vector<int> index_DVs;

		/* list of leaving vehicle (LV) indexes */
		vector<int> index_LVs;

	public:

		scheduling(unordered_map<string, vehicle> list_veh, set<string> list_veh_confirmation, intersection_client& localmap, configuration& config);

		vector<string> get_vehicleIdList() const;
		unordered_map<string, int> get_vehicleIndexList() const;

		vector<double> get_timeList() const;
		vector<double> get_speedList() const;
		vector<double> get_accelList() const;
		vector<string> get_laneIdList() const;
		vector<string> get_stateList() const;

		vector<int> get_departPosIndexList() const;
		vector<double> get_distanceList() const;
		vector<double> get_clearTimeList() const;
		vector<double> get_estList() const;
		vector<double> get_stList() const;
		vector<double> get_etList() const;
		vector<double> get_dtList() const;

		vector<vector<int>> get_indexEVs() const;
		vector<int> get_indexRDVs() const;
		vector<int> get_indexDVs() const;
		vector<int> get_indexLVs() const;

		vector<bool> get_accessList() const;
		vector<int> get_linkPriorityList() const;


		void set_st(int v_index, double st_added);
		void set_et(int v_index, double et_added);
		void set_dt(int v_index, double dt_added);
		void set_departPosIndex(int v_index, int pos_index);
		void set_access(int v_index, bool access_value);
};


#endif
