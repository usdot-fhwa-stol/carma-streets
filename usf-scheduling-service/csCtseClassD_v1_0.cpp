
/**/

#include "Header_v1_1.h"

struct SortAscending_i
{
	const vector<int> & value_vector;

	SortAscending_i(const vector<int> & val_vec) :
		value_vector(val_vec) {}

	bool operator()(int i1, int i2)
	{
		return value_vector[i1] < value_vector[i2];
	}
};

struct SortDecending_i
{
	const vector<int> & value_vector;

	SortDecending_i(const vector<int> & val_vec) :
		value_vector(val_vec) {}

	bool operator()(int i1, int i2)
	{
		return value_vector[i1] > value_vector[i2];
	}
};

struct SortAscending_d
{
	const vector<double> & value_vector;

	SortAscending_d(const vector<double> & val_vec) :
		value_vector(val_vec) {}

	bool operator()(int i1, int i2)
	{
		return value_vector[i1] < value_vector[i2];
	}
};

struct SortDecending_d
{
	const vector<double> & value_vector;

	SortDecending_d(const vector<double> & val_vec) :
		value_vector(val_vec) {}

	bool operator()(int i1, int i2)
	{
		return value_vector[i1] > value_vector[i2];
	}
};

void csCtseClassD_v1_0(Setting& setting, Vehicles& vehicles, Scheduling& scheduling, Map map) {

	int vehicle_index;
	int vehicle_index_1;
	int lane_index;
	int lane_index_1;
	int link_index;
	int link_index_1;
	int count;
	int break_indicator;
	int index_opt;
	int index_list_RDV;
	string vehicle_id;
	double EST;
	double ST;
	double ET;
	double ET_1;
	double DT;
	double delay;

	/* estimate the departure times (DTs) of DVs */
	for (int n = 0; n < scheduling.index_DVs.size(); ++n) {
		vehicle_index = scheduling.index_DVs[n];
		scheduling.dt[vehicle_index] = scheduling.time[vehicle_index] + scheduling.clearance_time[vehicle_index];
	}

	/* create a list of vehicles and initially fill it with DVs indexes */
	vector<int> listS = scheduling.index_DVs;
	sort(listS.begin(), listS.end(), SortDecending_i(vehicles.departurePosition_index));
	sort(listS.begin(), listS.end(), SortDecending_d(scheduling.dt));

	/* create a list of RDVs and sort them based on their departure sequence index */
	vector<int> listRDV = scheduling.index_RDVs;
	int count_RDV = listRDV.size();
	/*
	sort(listRDV.begin(), listRDV.end(), SortAscending_i(vehicles.link_priority));
	sort(listRDV.begin(), listRDV.end(), SortAscending_d(scheduling.st));
	for (int n = 0; n < listRDV.size(); ++n) {
		vehicle_index = listRDV[n];
		if (scheduling.departurePosition_index[vehicle_index] == -1) {
			scheduling.departurePosition_index[vehicle_index] = listS.size() + n + 1;
		}
	}
	*/
	sort(listRDV.begin(), listRDV.end(), SortAscending_i(scheduling.departurePosition_index));

	/* estimate earliest entering and departure times for RDV */
	vector<double> listS_minET(scheduling.index.size());
	vector<double> listS_minDT(scheduling.index.size());
	for (int n = 0; n < listS.size(); ++n) {
		vehicle_index = listS[n];
		listS_minET[vehicle_index] = scheduling.et[vehicle_index];
		listS_minDT[vehicle_index] = scheduling.dt[vehicle_index];
	}
	for (int n = 0; n < listRDV.size(); ++n) {
		vehicle_index = listRDV[n];
		link_index = vehicles.link_index[vehicle_index];
		ET = setting.scheduling_start_time + 2 * setting.scheduling_frequency;				// !!!
		for (int m = 0; m < listS.size(); ++m) {
			vehicle_index_1 = listS[m];
			link_index_1 = vehicles.link_index[vehicle_index_1];
			if (map.link_conflict[link_index][link_index_1] == 1) {
				ET = max(ET, listS_minDT[vehicle_index_1]);
			}
		}
		DT = ET + scheduling.clearance_time[vehicle_index];
		listS_minET[vehicle_index] = ET;
		listS_minDT[vehicle_index] = DT;
	}

	/* scheduling RDVs */
	vector<int> listOptionsIndex;
	vector<int> listOptionsVehicleIndex;
	vector<int> listOptionsListRDVIndex;
	vector<int> listOptionsPosition;
	vector<int> listOptionsLinkPriority;
	vector<int> listOptionsLaneIndex;
	vector<double> listOptionsST;
	vector<double> listOptionsET;
	vector<double> listOptionsDT;
	vector<double> listOptionsDelay;
	vector<int> listQ;
	int count_options;
	count = listS.size();
	while (count_RDV > 0) {

		count += 1;
		listOptionsIndex.clear();
		listOptionsVehicleIndex.clear();
		listOptionsListRDVIndex.clear();
		listOptionsPosition.clear();
		listOptionsST.clear();
		listOptionsET.clear();
		listOptionsDT.clear();
		listOptionsDelay.clear();

		/* 
		* calculate the delay caused by fixing an RDV in the departure sequence. pick the one that causes the least delay. 
		*/
		count_options = 0;
		for (int n = 0; n < listRDV.size(); ++n) {
			vehicle_index = listRDV[n];
			link_index = vehicles.link_index[vehicle_index];

			break_indicator = 0;
			listQ.clear();
			for (int m = 0; m < listRDV.size(); ++m) {
				vehicle_index_1 = listRDV[m];
				if (vehicle_index != vehicle_index_1) {
					listQ.push_back(scheduling.departurePosition_index[vehicle_index_1] + vehicles.flexibility_ET[vehicle_index_1] - (count + 1));
				}
			}
			sort(listQ.begin(), listQ.end());
			for (int m = 0; m < listRDV.size() - 1; ++m) {
				if (listQ[m] < m) {
					break_indicator = 1;
					break;
				}
			}
			if (break_indicator == 0) {
				ST = scheduling.st[vehicle_index];
				ET = listS_minET[vehicle_index];
				DT = listS_minDT[vehicle_index];
				delay = ET - ST;
				for (int m = 0; m < listRDV.size(); ++m) {
					vehicle_index_1 = listRDV[m];
					link_index_1 = vehicles.link_index[vehicle_index_1];
					if (vehicle_index != vehicle_index_1) {
						if (map.link_conflict[link_index][link_index_1] == 1) {
							ET_1 = max(listS_minET[vehicle_index_1], DT);
						}
						else {
							ET_1 = max(listS_minET[vehicle_index_1], ET);
						}

						delay += ET_1 - scheduling.st[vehicle_index_1];
					}
				}

				listOptionsIndex.push_back(count_options);
				listOptionsVehicleIndex.push_back(vehicle_index);
				listOptionsListRDVIndex.push_back(n);
				listOptionsPosition.push_back(scheduling.departurePosition_index[vehicle_index]);
				listOptionsST.push_back(ST);
				listOptionsET.push_back(ET);
				listOptionsDT.push_back(DT);
				listOptionsDelay.push_back(delay);
				count_options += 1;
			}
		}
		sort(listOptionsIndex.begin(), listOptionsIndex.end(), SortAscending_i(listOptionsPosition));
		sort(listOptionsIndex.begin(), listOptionsIndex.end(), SortAscending_d(listOptionsET));
		sort(listOptionsIndex.begin(), listOptionsIndex.end(), SortAscending_d(listOptionsDT));
		sort(listOptionsIndex.begin(), listOptionsIndex.end(), SortAscending_d(listOptionsDelay));

		index_opt = listOptionsIndex[0];
		vehicle_index = listOptionsVehicleIndex[index_opt];
		link_index = vehicles.link_index[vehicle_index];
		index_list_RDV = listOptionsListRDVIndex[index_opt];
		ET = listOptionsET[index_opt];
		DT = listOptionsDT[index_opt];
		scheduling.et[vehicle_index] = ET;
		scheduling.dt[vehicle_index] = DT;
		vehicles.flexibility_ET[vehicle_index] -= count - scheduling.departurePosition_index[vehicle_index]; // !!!
		scheduling.departurePosition_index[vehicle_index] = count;

		/* update unscheduled RDVs' earliest entering and departure time */
		for (int n = 0; n < listRDV.size(); ++n) {
			vehicle_index_1 = listRDV[n];
			if (vehicle_index_1 != vehicle_index) {
				link_index_1 = vehicles.link_index[vehicle_index_1];
				if (map.link_conflict[link_index][link_index_1] == 1) {
					listS_minET[vehicle_index_1] = max(listS_minET[vehicle_index_1], DT);
				}
				else {
					listS_minET[vehicle_index_1] = max(listS_minET[vehicle_index_1], ET);
				}
				listS_minDT[vehicle_index_1] = listS_minET[vehicle_index_1] + scheduling.clearance_time[vehicle_index_1];
			}
		}

		/*
		if (ET == setting.scheduling_start_time + 2 * setting.scheduling_frequency) {
			vehicles.access[vehicle_index] = "Y";
		} */
		/*  */
		if (ET == setting.scheduling_start_time + 2 * setting.scheduling_frequency) {
			vehicles.access[vehicle_index] = "Y";
			for (int n = 0; n < scheduling.index_DVs.size(); ++n) {
				vehicle_index_1 = scheduling.index_DVs[n];
				link_index_1 = vehicles.link_index[vehicle_index_1];
				if (map.link_conflict[link_index][link_index_1] == 1) {
					vehicles.access[vehicle_index] = "N";
					break;
				}
			}
		}

		listRDV.erase(listRDV.begin() + index_list_RDV);
		count_RDV -= 1;
	}


	/* scheduling EVs */
	for (int n = 0; n < listRDV.size(); ++n) {
		vehicle_index = listRDV[n];
		listS.push_back(vehicle_index);
	}
	sort(listS.begin(), listS.end(), SortDecending_i(vehicles.link_priority));
	sort(listS.begin(), listS.end(), SortDecending_d(scheduling.st));
	sort(listS.begin(), listS.end(), SortDecending_d(scheduling.et));
	sort(listS.begin(), listS.end(), SortDecending_d(scheduling.dt));
	vector<vector<int>> listEV = scheduling.index_EVs;
	int count_EV = 0;
	for (int i = 0; i < map.index_entryLane.size(); ++i) {
		count_EV += listEV[i].size();
	}
	while (count_EV > 0) {

		/*
		* for each entry lane, estimate the earliest stopping time of the preceding unscheduled EV. pick the one with the earliest estimated stopping time.
		* estimate its entering and departure times, and remove the vehicle from the EV list. 
		*/
		listOptionsIndex.clear();
		listOptionsVehicleIndex.clear();
		listOptionsST.clear();
		listOptionsET.clear();
		listOptionsDT.clear();
		listOptionsLinkPriority.clear();
		listOptionsLaneIndex.clear();
		count_options = 0;
		for (int i = 0; i < map.index_entryLane.size(); ++i) {
			lane_index = map.index_entryLane[i];
			if (listEV[i].size() > 0) {
				
				vehicle_index = listEV[i][0];
				link_index = vehicles.link_index[vehicle_index];

				ST = scheduling.est[vehicle_index];
				if (listS.size() > 0) {
					for (int n = 0; n < listS.size(); ++n) {
						vehicle_index_1 = listS[n];
						if (vehicles.entryLane_index[vehicle_index] == vehicles.entryLane_index[vehicle_index_1]) {
							ST = max(ST, scheduling.dt[vehicle_index_1]);
							break;
						}
					}
				}
				ST = ceil(ST);

				ET = ST + 0.1; // ???
				if (listS.size() > 0) {
					for (int n = 0; n < listS.size(); ++n) {
						vehicle_index_1 = listS[n];
						if (ST < scheduling.dt[vehicle_index_1]) {
							link_index_1 = vehicles.link_index[vehicle_index_1];
							if (map.link_conflict[link_index][link_index_1] == 1) {
								ET = max(ET, scheduling.dt[vehicle_index_1]);
								break;
							}
						}
						else {
							break;
						}
					}
				}
				
				DT = ET + scheduling.clearance_time[vehicle_index];

				listOptionsIndex.push_back(count_options);
				listOptionsVehicleIndex.push_back(vehicle_index);
				listOptionsST.push_back(ST);
				listOptionsET.push_back(ET);
				listOptionsDT.push_back(DT);
				listOptionsLinkPriority.push_back(vehicles.link_priority[vehicle_index]);
				listOptionsLaneIndex.push_back(i);

				count_options += 1;
			}
		}
		sort(listOptionsIndex.begin(), listOptionsIndex.end(), SortAscending_d(listOptionsDT));
		sort(listOptionsIndex.begin(), listOptionsIndex.end(), SortAscending_d(listOptionsET));
		sort(listOptionsIndex.begin(), listOptionsIndex.end(), SortAscending_i(listOptionsLinkPriority));
		sort(listOptionsIndex.begin(), listOptionsIndex.end(), SortAscending_d(listOptionsST));
		
		index_opt = listOptionsIndex[0];
		vehicle_index = listOptionsVehicleIndex[index_opt];
		lane_index = vehicles.entryLane_index[vehicle_index];
		ST = listOptionsST[index_opt];
		ET = listOptionsET[index_opt];
		DT = listOptionsDT[index_opt];
		int lane_list_index = listOptionsLaneIndex[index_opt];

		scheduling.st[vehicle_index] = ST;
		scheduling.et[vehicle_index] = ET;
		scheduling.dt[vehicle_index] = DT;

		listS.insert(listS.begin(), vehicle_index);
		sort(listS.begin(), listS.end(), SortDecending_i(vehicles.link_priority));
		sort(listS.begin(), listS.end(), SortDecending_d(scheduling.st));
		sort(listS.begin(), listS.end(), SortDecending_d(scheduling.et));
		sort(listS.begin(), listS.end(), SortDecending_d(scheduling.dt));
		
		listEV[lane_list_index].erase(listEV[lane_list_index].begin() + 0);
		count_EV -= 1;

	}



}