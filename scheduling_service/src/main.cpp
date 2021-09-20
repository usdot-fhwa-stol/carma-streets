#include "kafka_client.h"
#include <thread>    
#include <chrono>  
#include <vector>
#include <unordered_map>

#include "configuration.h"
#include "osm.h"
#include "vehicle.h"
#include "sorting.h"
#include "scheduling.h"

#include "spdlog/spdlog.h"
#include "spdlog/cfg/env.h"
#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"

using namespace std;
using namespace rapidjson;
using namespace chrono;


//configuration config("configuration.json");
configuration config;
osm localmap("osm.json");
unordered_map<string, vehicle> list_veh;


void consumer_update(const char* paylod){
    
    Document message;
    message.Parse(paylod);
    
    string veh_id = message["payload"]["v_id"].GetString();

    if (!list_veh.count(veh_id)){
        string cur_lane_type = localmap.get_laneType(message["payload"]["cur_lane_id"].GetString());
        if (cur_lane_type == "entry" || cur_lane_type == "link"){
            list_veh[veh_id] = vehicle();
        }
    }
    
    if (list_veh.count(veh_id)){
        list_veh[veh_id].update(message);
        if (list_veh[veh_id].get_curState() == "LV"){
            list_veh.erase(veh_id);
        }
    }

}

void scheduling_func(unordered_map<string, vehicle> list_veh, Document& document){

    scheduling schedule(list_veh);

    /* estimate the departure times (DTs) of DVs */
    for (auto & vehicle_index : schedule.get_indexDVs()){
        double dt = schedule.get_timeList()[vehicle_index] + schedule.get_clearTimeList()[vehicle_index];
        schedule.set_dt(vehicle_index, dt);
    }
    
    /* create a list of vehicles and initially fill it with DVs indexes */
	vector<int> listS = schedule.get_indexDVs();
	sort(listS.begin(), listS.end(), sorting<int>(schedule.get_departPosIndexList(), "dec"));
    sort(listS.begin(), listS.end(), sorting<double>(schedule.get_dtList(), "dec"));

    /* create a list of RDVs and sort them based on their departure sequence index */
    vector<int> listRDV = schedule.get_indexRDVs();
    int count_RDV = listRDV.size();
    sort(listRDV.begin(), listRDV.end(), sorting<int>(schedule.get_departPosIndexList(), "asc"));

    /* estimate earliest entering and departure times for RDV */
	vector<double> listS_minET(schedule.get_vehicleIdList().size());
	vector<double> listS_minDT(schedule.get_vehicleIdList().size());
    for (int n = 0; n < listS.size(); ++n){
        int vehicle_index = listS[n];
        listS_minET[vehicle_index] = schedule.get_etList()[vehicle_index];
		listS_minDT[vehicle_index] = schedule.get_dtList()[vehicle_index];
    }
    for (int n = 0; n < listRDV.size(); ++n){
        int vehicle_index1 = listRDV[n];
        string vehicle_id1 = schedule.get_vehicleIdList()[vehicle_index1];
        string link_id1 = list_veh[vehicle_id1].get_linkID();
        double et = config.get_curSchedulingT() + config.get_schedulingDelta();
        for (int m = 0; m < listS.size(); ++m){
            int vehicle_index2 = listS[m];
            string vehicle_id2 = schedule.get_vehicleIdList()[vehicle_index2];
            string link_id2 = list_veh[vehicle_id2].get_linkID();
            if (localmap.hasConflict(link_id1, link_id2) == true){
                et = max(et, listS_minDT[vehicle_index2]);
            }
        }
        double dt = et + schedule.get_clearTimeList()[vehicle_index1];
        listS_minET[vehicle_index1] = et;
        listS_minDT[vehicle_index1] = dt;
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
    int count = listS.size();
    while(count_RDV > 0){

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
		for (int n = 0; n < listRDV.size(); ++n){

            int vehicle_index1 = listRDV[n];
            string vehicle_id1 = schedule.get_vehicleIdList()[vehicle_index1];
            string link_id1 = list_veh[vehicle_id1].get_linkID();
            
            int break_indicator = 0;
			listQ.clear();
            for (int m = 0; m < listRDV.size(); ++m) {
				int vehicle_index2 = listRDV[m];
                string vehicle_id2 = schedule.get_vehicleIdList()[vehicle_index2];
				if (vehicle_index1 != vehicle_index2) {
                    listQ.push_back(schedule.get_departPosIndexList()[vehicle_index2] + list_veh[vehicle_id2].get_flexET() - (count + 1));
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

                double st1 = schedule.get_stList()[vehicle_index1];
                double et1 = schedule.get_etList()[vehicle_index1];
                double dt1 = schedule.get_dtList()[vehicle_index1];
                double delay = et1 - st1;
                for (int m = 0; m < listRDV.size(); ++m){
                    int vehicle_index2 = listRDV[m];
                    string vehicle_id2 = schedule.get_vehicleIdList()[vehicle_index2];
                    string link_id2 = list_veh[vehicle_id2].get_linkID();
                    if (vehicle_index1 != vehicle_index2){
                        double et2;
                        if (localmap.hasConflict(link_id1, link_id2) == true){
                            et2 = max(listS_minDT[vehicle_index2], et1);
                        }
                        else{
                            et2 = max(listS_minET[vehicle_index2], et1);
                        }
                        delay += et2 - schedule.get_stList()[vehicle_index2];
                    }
                }

				listOptionsIndex.push_back(count_options);
				listOptionsVehicleIndex.push_back(vehicle_index1);
				listOptionsListRDVIndex.push_back(n);
                listOptionsPosition.push_back(schedule.get_departPosIndexList()[vehicle_index1]);
				listOptionsST.push_back(st1);
				listOptionsET.push_back(et1);
				listOptionsDT.push_back(dt1);
				listOptionsDelay.push_back(delay);
				count_options += 1;
			}

        }

        sort(listOptionsIndex.begin(), listOptionsIndex.end(), sorting<int>(listOptionsPosition, "asc"));
		sort(listOptionsIndex.begin(), listOptionsIndex.end(), sorting<double>(listOptionsET, "asc"));
		sort(listOptionsIndex.begin(), listOptionsIndex.end(), sorting<double>(listOptionsDT, "asc"));
		sort(listOptionsIndex.begin(), listOptionsIndex.end(), sorting<double>(listOptionsDelay, "asc"));


        int index_opt = listOptionsIndex[0];
		int vehicle_index = listOptionsVehicleIndex[index_opt];
        string vehicle_id = schedule.get_vehicleIdList()[vehicle_index];
		string link_id = list_veh[vehicle_id].get_linkID();
		int index_list_RDV = listOptionsListRDVIndex[index_opt];
		double et = listOptionsET[index_opt];
		double dt = listOptionsDT[index_opt];
        schedule.set_et(vehicle_index, et);
        schedule.set_dt(vehicle_index, dt);
        schedule.set_departPosIndex(vehicle_index, count);
        // change ET flexibility !!!


        /* update unscheduled RDVs' earliest entering and departure time */
		for (int n = 0; n < listRDV.size(); ++n) {
			int vehicle_index1 = listRDV[n];
			if (vehicle_index1 != vehicle_index) {
                string vehicle_id1 = schedule.get_vehicleIdList()[vehicle_index1];
                string link_id1 = list_veh[vehicle_id1].get_linkID();
                if (localmap.hasConflict(link_id, link_id1) == true){
                    listS_minET[vehicle_index1] = max(listS_minET[vehicle_index1], dt);
                }
                else{
                    listS_minET[vehicle_index1] = max(listS_minET[vehicle_index1], et);
                }
				listS_minDT[vehicle_index1] = listS_minET[vehicle_index1] + schedule.get_clearTimeList()[vehicle_index1];
			}
		}

        /*  */
        if (et <= config.get_curSchedulingT() + config.get_schedulingDelta()){
            schedule.set_access(vehicle_index, "true");
            for (int n = 0; n < schedule.get_indexDVs().size(); ++n){
                int vehicle_index1 = schedule.get_indexDVs()[n];
                string vehicle_id1 = schedule.get_vehicleIdList()[vehicle_index1];
                string link_id1 = list_veh[vehicle_id1].get_linkID();
                if (localmap.hasConflict(link_id, link_id1) == true){
                    schedule.set_access(vehicle_index, "false");
                    break;
                }
            }
        }

		listRDV.erase(listRDV.begin() + index_list_RDV);
		count_RDV -= 1;

    }


    /* scheduling EVs */
    for (int n = 0; n < listRDV.size(); ++n) {
		int vehicle_index = listRDV[n];
		listS.push_back(vehicle_index);
	}
    sort(listS.begin(), listS.end(), sorting<int>(schedule.get_linkPriorityList(), "dec"));
    sort(listS.begin(), listS.end(), sorting<double>(schedule.get_stList(), "dec"));
    sort(listS.begin(), listS.end(), sorting<double>(schedule.get_etList(), "dec"));
    sort(listS.begin(), listS.end(), sorting<double>(schedule.get_dtList(), "dec"));
    vector<vector<int>> listEV = schedule.get_indexEVs();
    int count_EV = 0;
    for (int i = 0; i < localmap.get_laneIdEntry().size(); ++i){
        count_EV += listEV.size();
    }

    while (count_EV > 0){
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
        for (int i = 0; i < localmap.get_laneIdEntry().size(); ++i){
            string lane_id = localmap.get_laneIdEntry()[i];
            if (listEV[i].size() > 0){
                
                int vehicle_index1 = listEV[i][0];
                string vehicle_id1 = schedule.get_vehicleIdList()[vehicle_index1];
                string link_id1 = list_veh[vehicle_id1].get_linkID();

                double st = schedule.get_estList()[vehicle_index1];
                if (listS.size() > 0){
                    for (int n = 0; n < listS.size(); ++n){
                        int vehicle_index2 = listS[n];
                        string vehicle_id2 = schedule.get_vehicleIdList()[vehicle_index2];
                        //string link_id2 = list_veh[vehicle_id2].get_linkID();
                        if (list_veh[vehicle_id1].get_entryLaneID() == list_veh[vehicle_id2].get_entryLaneID()){
                            st = ceil(max(st, schedule.get_dtList()[vehicle_index2]));
                            break;
                        }
                    }
                }

                double et = st + 0.1;
                if (listS.size() > 0){
                    for (int n = 0; n < listS.size(); ++n){
                        int vehicle_index2 = listS[n];
                        if (st < schedule.get_dtList()[vehicle_index2]){
                            string vehicle_id2 = schedule.get_vehicleIdList()[vehicle_index2];
                            string link_id2 = list_veh[vehicle_id2].get_linkID();
                            if (localmap.hasConflict(link_id1, link_id2) == true){
                                et = max(et, schedule.get_dtList()[vehicle_index2]);
                            }
                        }
                        else{
                            break;
                        }
                    }     
                }

                double dt = et + schedule.get_clearTimeList()[vehicle_index1];

				listOptionsIndex.push_back(count_options);
				listOptionsVehicleIndex.push_back(vehicle_index1);
				listOptionsST.push_back(st);
				listOptionsET.push_back(et);
				listOptionsDT.push_back(dt);
				listOptionsLinkPriority.push_back(schedule.get_linkPriorityList()[vehicle_index1]);
				listOptionsLaneIndex.push_back(i);

				count_options += 1;
                
            }
        }

        sort(listOptionsIndex.begin(), listOptionsIndex.end(), sorting<double>(listOptionsDT, "asc"));
        sort(listOptionsIndex.begin(), listOptionsIndex.end(), sorting<double>(listOptionsET, "asc"));
        sort(listOptionsIndex.begin(), listOptionsIndex.end(), sorting<int>(listOptionsLinkPriority, "asc"));
        sort(listOptionsIndex.begin(), listOptionsIndex.end(), sorting<double>(listOptionsST, "asc"));

        int index_opt = listOptionsIndex[0];
		int vehicle_index = listOptionsVehicleIndex[index_opt];
		double st = listOptionsST[index_opt];
		double et = listOptionsET[index_opt];
		double dt = listOptionsDT[index_opt];
		int lane_list_index = listOptionsLaneIndex[index_opt];
		
        schedule.set_st(vehicle_index, st);
        schedule.set_et(vehicle_index, et);
        schedule.set_dt(vehicle_index, dt);

        listS.insert(listS.begin(), vehicle_index);
        sort(listS.begin(), listS.end(), sorting<int>(schedule.get_linkPriorityList(), "dec"));
        sort(listS.begin(), listS.end(), sorting<double>(schedule.get_stList(), "dec"));
        sort(listS.begin(), listS.end(), sorting<double>(schedule.get_etList(), "dec"));
        sort(listS.begin(), listS.end(), sorting<double>(schedule.get_dtList(), "dec"));
		
		listEV[lane_list_index].erase(listEV[lane_list_index].begin() + 0);
		count_EV -= 1;

    }

    /*
    * create the json file that includes the scheduling decision
    */
    for (int n = 0; n < schedule.get_vehicleIdList().size(); ++n){
        string vehicle_id = schedule.get_vehicleIdList()[n];
        document["schedule"].AddMember("v_id", vehicle_id, document.GetAllocator());
        document["schedule"].AddMember("st", schedule.get_stList()[n], document.GetAllocator());
        document["schedule"].AddMember("et", schedule.get_etList()[n], document.GetAllocator());
        document["schedule"].AddMember("dt", schedule.get_dtList()[n], document.GetAllocator());
        document["schedule"].AddMember("departure_position", schedule.get_departPosIndexList()[n], document.GetAllocator());
        if (schedule.get_accessList()[n] == true){
            document["schedule"].AddMember("access", 1, document.GetAllocator());
        } 
        else{
            document["schedule"].AddMember("access", 0, document.GetAllocator());
        }
    }
    
}



void call_consumer_thread()
{
   kafka_clients::kafka_client *client = new kafka_clients::kafka_client();
    std::string file_name= "../manifest.json";
    rapidjson::Document doc_json = client->read_json_file(file_name); 
    std::string bootstrap_server = client->get_value_by_doc(doc_json, "BOOTSTRAP_SERVER");
    std::string group_id = client->get_value_by_doc(doc_json, "GROUP_ID");
    std::string topic = client->get_value_by_doc(doc_json, "CONSUMER_TOPIC");
    kafka_clients::kafka_consumer_worker *consumer_worker = client->create_consumer(bootstrap_server,topic,group_id);
    delete client;
    
    if(!consumer_worker->init())
    {
        spdlog::critical("kafka consumer initialize error");
    }
    else
    {
        consumer_worker->subscribe();
        if(!consumer_worker->is_running())
        {
            spdlog::critical("consumer_worker is not running");
        }
        
        while (consumer_worker->is_running()) 
        {
            const char* paylod= consumer_worker->consume(1000);
            
            if(strlen(paylod) > 0)
            {
                spdlog::info("Consumed message payload: {0}", paylod);
                
                
                /* 
                * update function for updating the stored vehicle status and intents:
                *   note: 
                *   
                */
                consumer_update(paylod);
            }
        }
        
        consumer_worker->stop();
    }     
    free(consumer_worker);
    return;    
}

void call_scheduling_thread(){
    
    kafka_clients::kafka_client *client = new kafka_clients::kafka_client(); 
                  
    std::string file_name="../manifest.json";
    rapidjson::Document doc_json = client->read_json_file(file_name);           
    std::string bootstrap_server =  client->get_value_by_doc(doc_json, "BOOTSTRAP_SERVER");
    std::string topic = client->get_value_by_doc(doc_json, "PRODUCER_TOPIC");
    kafka_clients::kafka_producer_worker *producer_worker  = client->create_producer(bootstrap_server, topic);
    delete client;

    char str_msg[]="";           
    if(!producer_worker->init())
    {
        spdlog::critical("kafka producer initialize error");
    }
    else
    {        
        while (true) 
        {   
            
            if (duration<double>(system_clock::now().time_since_epoch()).count() - config.get_lastSchedulingT() >= config.get_schedulingDelta()){
                
                config.set_curSchedulingT(duration<double>(system_clock::now().time_since_epoch()).count());
                auto t = system_clock::now() + milliseconds(int(config.get_schedulingDelta()*1000));

                // copy list_veh
                unordered_map<string, vehicle> list_veh_copy = list_veh;

                // run the scheduling service
                Document document;
                document.AddMember("intersection_type", "stop_controlled", document.GetAllocator());
                document.AddMember("schedule", Value(kObjectType), document.GetAllocator());
                if (list_veh_copy.size() > 0){
                    scheduling_func(list_veh_copy, document);
                }
                StringBuffer buffer;
                Writer<StringBuffer> writer(buffer);
                document.Accept(writer);
                string msg_to_send = buffer.GetString();

                /* produce the scheduling plan to kafka */
                //string msg_to_send = "\"This is a test\"";
                //this_thread::sleep_for(std::chrono::seconds(5));
                producer_worker->send(msg_to_send);

                // update the previous scheduling time and sleep until next schedule
                config.set_lastSchedulingT(config.get_curSchedulingT());
                if (system_clock::now() < t){
                    this_thread::sleep_until(t);
                }
            }

        }
        producer_worker->stop();
    }
    free(producer_worker);
    
    return;

}


int main(int argc,char** argv)
{

    thread consumer(call_consumer_thread);
    thread scheduling(call_scheduling_thread);

}