
/**
* CARMA Streets scheduling algorithm designed for stop-controlled intersections:
* 
*
* This service will continuously consume vehicle information from a designed Kafka topic, will schedule vehicles
* and estimate their critical time steps, and finally, will produce the scheduling results into another Kafka topic.
*
* This service is designed for cooperation Class D C-ADS equipped vehicles.
*
* Four different state based on the algorithm design is defined for each D C-ADS equipped vehicles:
*	- Entering Vehicle (EV): Vehicles that are approaching the intersection and cannot stop at the stop bar in the next time step
*	- Ready-to-Depart Vehicles (RDV): Vehicles that are stopped at the stop bars and are ready to enter the intersection box. 
*		Note that, if there is more than one vehicle from an entry lane stopped behind the intersection box, only the leading vehicle 
*		will be considered as RDV and the rest will be still considered as EVs. Each RDV is associated with a fixed order index in the RDV departure sequence.
*	- Departing Vehicles (DV): Vehicles that are moving within the intersection box. Note that, vehicles’ cooperation class determines whether one or more 
*		vehicles can use the intersection box simultaneously.
*	- Leaving Vehicles (LV): Vehicles that already departed the intersection box 
*
* Notes: 
*	- the consumer and producer functions must be added.
*	- the function for identifying each vehicle's lane index and the Map class information must be updated.
*
* 
*/

#include "Header_v1_1.h"


void csConsumer_v1_0(Setting& setting, Vehicles& vehicles, Scheduling& scheduling, Map map);
void csInformationUpdate_v1_1(Setting& setting, Vehicles& vehicles, Scheduling& scheduling, Map map, vector<string> list_json_files);
void csCtseClassD_v1_0(Setting& setting, Vehicles& vehicles, Scheduling& scheduling, Map map);
void csProducer_v1_0(Setting& setting, Vehicles& vehicles, Scheduling& scheduling, Map map);


int main() {

	Setting setting;
	Vehicles vehicles;
	Scheduling scheduling;
	Map map;	
	
	vector<string> list_json_files;
	//vector<Json::Value> list_json_files;


	/* 
	* add a consumer thread:
	*	the consumer must continuesly consume vehicle information from the designed Kafka topic.
	*	the consumer will add the consumed json file name (or the json file itself) to the list of json files.
	*/
	csConsumer_v1_0(setting, vehicles, scheduling, map);


	/* while the shutdown switch is off */
	while (setting.shutdown_switch == 0) {

		/* while the scheduling switch is on */
		while (setting.scheduling_switch == 1) {
			
			/* run the scheduling algorithm every setting.scheduling_frequency sec (e.g, 1.0 sec) */
			//if (setting.last_schedule + setting.scheduling_frequency <= time(NULL)) {
			if (setting.last_schedule + setting.scheduling_frequency <= double(duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count()) / 1000) {
				
				//setting.scheduling_start_time = time(NULL);
				setting.scheduling_start_time = double(duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count()) / 1000;

				/* obtain a list of json file names from the consumer */
				/* Notes:
				*	- in here, the service must retrieve the json file list from the consumer and clear the list.
				*/
				//list_json_files.clear();
				//list_json_files = 

				/* 
				* read the json files from the json file list and update the vehicle information class 
				* update the scheduling information class and get ready for scheduling vehicles.
				*/
				csInformationUpdate_v1_1(setting, vehicles, scheduling, map, list_json_files);

				/* schedule vehicles and estimate the vehicles' critical time steps */
				csCtseClassD_v1_0(setting, vehicles, scheduling, map);
				
				/* produce the scheduling decision into a Kafka topic */
				csProducer_v1_0(setting, vehicles, scheduling, map);
			}
		}

	}

}