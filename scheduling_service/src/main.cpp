#define RAPIDJSON_HAS_STDSTRING 1
#include "kafka_client.h"
#include <thread>    
#include <chrono>  
#include <vector>
#include <unordered_map>
#include <set>

#include "configuration.h"
#include "vehicle.h"
#include "sorting.h"
#include "scheduling.h"
#include "csv_logger.h"

#include "spdlog/spdlog.h"
#include "spdlog/cfg/env.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"

#include "OAIDefaultApi.h"
#include <QTimer>
#include "intersection_client.h"
#include <QEventLoop>
#include <QCoreApplication>
#include "OAIHelpers.h"

using namespace std;
using namespace rapidjson;
using namespace chrono;
using namespace OpenAPI;

configuration config;
intersection_client localmap;
unordered_map<string, vehicle> list_veh;
set<string> list_veh_confirmation;
set<string> list_veh_removal;
std::mutex worker_mtx;



void consumer_update(const char* payload){
    rapidjson::Document message;
    message.SetObject();
    message.Parse(payload);
    
    /* if the received message does not have payload, it cannot be processed! */
    if (message.HasMember("payload")){
        if (message["payload"].HasMember("v_id")){

            string veh_id = message["payload"]["v_id"].GetString(); 

            /* check if the vehicle is included in the list_veh. if not, include it. */
            if (!list_veh.count(veh_id)){

                if (message["payload"].HasMember("cur_lane_id")){
                    string cur_lane_type = localmap.get_laneType(to_string(message["payload"]["cur_lane_id"].GetInt()));
                    if (cur_lane_type == "entry" || cur_lane_type == "link"){
                        list_veh[veh_id] = vehicle();
                    }
                }
                else{
                    spdlog::critical("Vehicle {0}'s current lane id is missing in the received status and intent update!", veh_id);
                }
            }

            /* update the vehicle status and intent information */
            if (list_veh.count(veh_id)){
                /* adding a check to not read the messages with wrong BSM */
                if (list_veh[veh_id].message_hasError(message, localmap) == false){
                    list_veh[veh_id].update(message, localmap, config);       
                    if (list_veh[veh_id].get_curState() == "LV"){
                        spdlog::info("Vehicle {0} has departed the intersection box (i.e., is in LV state) and been removed from list_veh", veh_id);
                        list_veh.erase(veh_id);
                    }
                }
            }
        }
        else{
            spdlog::critical("vehicle id is missing in the received status and intent update!");
        }
    }
    else{
        spdlog::critical("payload is missing in the received status and intent update!");
    }

}

rapidjson::Value scheduling_func(unordered_map<string, vehicle> list_veh, Document::AllocatorType& allocator, std::unique_ptr<csv_logger> &logger, double &last_schedule){
    std::unique_lock<std::mutex> lck(worker_mtx);
    scheduling schedule(list_veh, list_veh_confirmation, localmap, config, list_veh_removal);
    lck.unlock();
    if ( config.isScheduleLoggerEnabled() ) {
        logger->log_line( schedule.toCSV() ); 
    }

    last_schedule = schedule.get_timestamp();
    /* estimate the departure times (DTs) of DVs */
    for (const auto vehicle_index : schedule.get_indexDVs()){
        double dt = schedule.get_timeList()[vehicle_index] + schedule.get_clearTimeList()[vehicle_index];
        schedule.set_dt(vehicle_index, dt);
    }
    
    /* create a list of vehicles and initially fill it with DVs indexes */
	vector<int> listS = schedule.get_indexDVs();
	sort(listS.begin(), listS.end(), sorting<int>(schedule.get_departPosIndexList(), "dec"));
    sort(listS.begin(), listS.end(), sorting<double>(schedule.get_dtList(), "dec"));

    /* create a list of RDVs and sort them based on their departure sequence index */
    vector<int> listRDV = schedule.get_indexRDVs();
    auto count_RDV = (int)listRDV.size();
    sort(listRDV.begin(), listRDV.end(), sorting<int>(schedule.get_departPosIndexList(), "asc"));

    /* estimate earliest entering and departure times for RDV */
	vector<double> listS_minET(schedule.get_vehicleIdList().size());
	vector<double> listS_minDT(schedule.get_vehicleIdList().size());
    for (int n = 0; n < (int)listS.size(); ++n){
        int vehicle_index = listS[n];
        listS_minET[vehicle_index] = schedule.get_etList()[vehicle_index];
		listS_minDT[vehicle_index] = schedule.get_dtList()[vehicle_index];
    }
    for (int n = 0; n < (int)listRDV.size(); ++n){
        int vehicle_index1 = listRDV[n];
        string vehicle_id1 = schedule.get_vehicleIdList()[vehicle_index1];
        string link_id1 = list_veh[vehicle_id1].get_linkID();
        double et = schedule.get_timestamp() + config.get_schedulingDelta();
        for (int m = 0; m < (int)listS.size(); ++m){
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
    auto count = (int)listS.size();
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
		for (int n = 0; n < (int)listRDV.size(); ++n){

            int vehicle_index1 = listRDV[n];
            string vehicle_id1 = schedule.get_vehicleIdList()[vehicle_index1];
            string link_id1 = list_veh[vehicle_id1].get_linkID();
            
            int break_indicator = 0;
			listQ.clear();
            for (int m = 0; m < (int)listRDV.size(); ++m) {
				int vehicle_index2 = listRDV[m];
                string vehicle_id2 = schedule.get_vehicleIdList()[vehicle_index2];
				if (vehicle_index1 != vehicle_index2) {
                    listQ.push_back(schedule.get_departPosIndexList()[vehicle_index2] + list_veh[vehicle_id2].get_flexET() - (count + 1));
				}
			}
            sort(listQ.begin(), listQ.end());
			for (int m = 0; m < (int)listRDV.size() - 1; ++m) {
				if (listQ[m] < m) {
					break_indicator = 1;
					break;
				}
			}

            if (break_indicator == 0) {

                double st1 = schedule.get_stList()[vehicle_index1];
                double et1 = listS_minET[vehicle_index1];
                double dt1 = listS_minDT[vehicle_index1];
                double delay = et1 - st1;
                for (int m = 0; m < (int)listRDV.size(); ++m){
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
		for (int n = 0; n < (int)listRDV.size(); ++n) {
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

        /* if the vehicle's entering time is set to the next scheduling time step, give access to the vehicle */
        if (et <= schedule.get_timestamp() + config.get_schedulingDelta()){
            bool vehicle_access_indicator = true;
            for (int n = 0; n < (int)schedule.get_indexDVs().size(); ++n){
                int vehicle_index1 = schedule.get_indexDVs()[n];
                string vehicle_id1 = schedule.get_vehicleIdList()[vehicle_index1];
                string link_id1 = list_veh[vehicle_id1].get_linkID();
                if (localmap.hasConflict(link_id, link_id1) == true){
                    vehicle_access_indicator = false;
                    break;
                }
            }
            schedule.set_access(vehicle_index, vehicle_access_indicator);    
        }

		listRDV.erase(listRDV.begin() + index_list_RDV);
		count_RDV -= 1;

    }

    /* scheduling EVs */
    for (int n = 0; n < (int)listRDV.size(); ++n) {
		int vehicle_index = listRDV[n];
		listS.push_back(vehicle_index);
	}
    sort(listS.begin(), listS.end(), sorting<int>(schedule.get_linkPriorityList(), "dec"));
    sort(listS.begin(), listS.end(), sorting<double>(schedule.get_stList(), "dec"));
    sort(listS.begin(), listS.end(), sorting<double>(schedule.get_etList(), "dec"));
    sort(listS.begin(), listS.end(), sorting<double>(schedule.get_dtList(), "dec"));
    vector<vector<int>> listEV = schedule.get_indexEVs();
    int count_EV = 0;
    for (int i = 0; i < (int)localmap.get_laneIdEntry().size(); ++i){
        count_EV += listEV[i].size();
    }

    while (count_EV > 0){
        
        /*
		* for each entry lane, estimate the earliest feasible stopping time of the preceding unscheduled EV. 
        * pick the one with the earliest estimated stopping time.
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
        for (int i = 0; i < (int)localmap.get_laneIdEntry().size(); ++i){
            string lane_id = localmap.get_laneIdEntry()[i];
            if (listEV[i].size() > 0){
                
                int vehicle_index1 = listEV[i][0];
                string vehicle_id1 = schedule.get_vehicleIdList()[vehicle_index1];
                string link_id1 = list_veh[vehicle_id1].get_linkID();

                double st = max(schedule.get_estList()[vehicle_index1], schedule.get_timestamp() + config.get_schedulingDelta());
                if (!listS.empty()){
                    for (int n = 0; n < (int)listS.size(); ++n){
                        int vehicle_index2 = listS[n];
                        string vehicle_id2 = schedule.get_vehicleIdList()[vehicle_index2];
                        if (list_veh[vehicle_id1].get_entryLaneID() == list_veh[vehicle_id2].get_entryLaneID()){
                            st = ceil(max(st, schedule.get_dtList()[vehicle_index2]));
                            break;
                        }
                    }
                }

                double et = st + 0.1;
                if (!listS.empty()){
                    for (int n = 0; n < (int)listS.size(); ++n){
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

    // Create vehicle schedule plan : 
    //  "schedule": [
    //    {
    //             "v_id": "DOT-508",
    //             "st": "21121212",
    //             "et": "12121212",
    //             "dt": "12121212",
    //             "dp": 2,
    //             "access": 0
    //         } , ... ]
    Value schedule_plan;
    if ((int)schedule.get_vehicleIdList().size() > 0) {
        schedule_plan.SetArray();
        for (int n = 0; n < (int)schedule.get_vehicleIdList().size(); ++n){
            std::string vehicle_id = schedule.get_vehicleIdList()[n];
            Value veh_sched(kObjectType);
            veh_sched.AddMember("v_id", vehicle_id, allocator);

            /* the units of the critical time points (i.e., st, et, dt) in the scheduling service is second,
            *  but each vehicle need to receive these time points in milisecond. Therefore, a conversion from second to milisecond is added here!
            */
            veh_sched.AddMember("st", u_int64_t(schedule.get_stList()[n]*1000), allocator);
            veh_sched.AddMember("et", u_int64_t(schedule.get_etList()[n]*1000), allocator);
            veh_sched.AddMember("dt", u_int64_t(schedule.get_dtList()[n]*1000), allocator);
            veh_sched.AddMember("dp", schedule.get_departPosIndexList()[n], allocator);
            if (schedule.get_accessList()[n] == true){
                veh_sched.AddMember("access", 1, allocator);
                if (list_veh[vehicle_id].get_access() == false){
                    list_veh_confirmation.insert(vehicle_id);
                }
            } 
            else{
                veh_sched.AddMember("access", 0, allocator);
            }
            schedule_plan.PushBack(veh_sched, allocator);
        }
    }

    return schedule_plan;
      
}



void call_consumer_thread()
{
  
    auto client = std::make_shared<kafka_clients::kafka_client>();
    std::string file_name= "../manifest.json";
    rapidjson::Document doc_json = client->read_json_file(file_name); 
    std::string bootstrap_server = client->get_value_by_doc(doc_json, "BOOTSTRAP_SERVER");
    std::string group_id = client->get_value_by_doc(doc_json, "GROUP_ID");
    std::string topic = client->get_value_by_doc(doc_json, "CONSUMER_TOPIC");
    auto consumer_worker = client->create_consumer(bootstrap_server,topic,group_id);
   

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
            /* remove those vehicles with old updates */
            if (list_veh_removal.size() > 0){
                for (auto veh_id : list_veh_removal){
                    list_veh.erase(veh_id);
                }
                std::unique_lock<std::mutex> lck(worker_mtx);
                list_veh_removal.clear();
                lck.unlock();
            } 
            const std::string payload = consumer_worker->consume(1000);

            if(payload.length() > 0)
            {

                
                /* 
                * update function for updating the stored vehicle status and intents:
                *   note: 
                *   
                */
                std::unique_lock<std::mutex> lck(worker_mtx); //Lock the list_veh for vehicle's update
                consumer_update(payload.c_str());

                
            } //The unique lock is automatically released when it is out of scope
        }
        
        consumer_worker->stop();
    }     
    return;    
}

void call_scheduling_thread(){

    auto client = std::make_shared<kafka_clients::kafka_client>();         
    std::string file_name="../manifest.json";
    rapidjson::Document doc_json = client->read_json_file(file_name);           
    std::string bootstrap_server =  client->get_value_by_doc(doc_json, "BOOTSTRAP_SERVER");
    std::string topic = client->get_value_by_doc(doc_json, "PRODUCER_TOPIC");
    auto producer_worker  = client->create_producer(bootstrap_server, topic);
     // Holds timestamp for last schedule sent
    double last_schedule;

    // Create logger
    auto logger = std::unique_ptr<csv_logger>(new csv_logger( config.get_scheduleLogPath(), config.get_scheduleLogFilename(), config.get_scheduleLogMaxsize() ));
    
    char str_msg[]="";           
    if(!producer_worker->init())
    {
        spdlog::critical("kafka producer initialize error");
    }
    else
    {        
        
        int sch_count = 0;
        while (true) 
        {   
            
            if (duration<double>(system_clock::now().time_since_epoch()).count() - last_schedule >= config.get_schedulingDelta()){
                
                spdlog::info("schedule number #{0}", sch_count);

                auto t = system_clock::now() + milliseconds(int(config.get_schedulingDelta()*1000));
                std::unique_lock<std::mutex> lck(worker_mtx);
                unordered_map<string, vehicle> list_veh_copy = list_veh;
                lck.unlock(); //Temporarily unlock the list_veh
                // Create scheduling JSON
                //  
                //    {
                //      "metadata": {
                //          "timestamp": 123123123,
                //          "intersection_type": "stop_controlled"
                //       },
                //       "payload": { "schedule": [ ..., ...]}    (see scheduling_func)
                //     }
                Document document;
                document.SetObject();

                Document::AllocatorType &allocator = document.GetAllocator();

                Value metadata(kObjectType);
                
                /* the unit of timestamp here is milliseconds without decimal places */
                auto timestamp = u_int64_t(duration<double>(system_clock::now().time_since_epoch()).count()*1000);

                metadata.AddMember("timestamp", timestamp, allocator);
                metadata.AddMember("intersection_type", "Carma/stop_controlled_intersection",allocator);
                document.AddMember("metadata", metadata, allocator);

                Value schedule;
                if (!list_veh_copy.empty()){
                    schedule = scheduling_func(list_veh_copy, allocator, logger, last_schedule);
                }
                document.AddMember("payload", schedule, allocator);

                StringBuffer buffer;
                Writer<StringBuffer> writer(buffer);
                document.Accept(writer);
                string msg_to_send = buffer.GetString();

                /* produce the scheduling plan to kafka */
                producer_worker->send(msg_to_send);

                // update the previous scheduling time and sleep until next schedule
                if (system_clock::now() < t){
                    this_thread::sleep_until(t);
                }

                sch_count += 1;

            } //The unique lock is automatically released when it is out of scope

        }
        producer_worker->stop();

    }

    return;

}

int main(int argc,char** argv)
{
    QCoreApplication a(argc, argv);
    localmap.call();
    boost::thread consumer{call_consumer_thread};
    boost::thread scheduling{call_scheduling_thread};
    consumer.join();
    scheduling.join();
    return 0;

}
