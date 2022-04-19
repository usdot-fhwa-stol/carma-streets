
#include "intersection_client.h"


void intersection_client::call()
{
  OAIDefaultApi apiInstance;

  QEventLoop loop;
  connect(&apiInstance, &OAIDefaultApi::getIntersectionInfoSignal, [&](OAIIntersection_info int_info)
          {
            
            is_running_indicator = true;
            intersection_name = int_info.getName().toStdString();
            intersection_id = int_info.getId();
            SPDLOG_DEBUG("intersection name: {0}", intersection_name);
            SPDLOG_DEBUG("intersection id: {0}", intersection_id);

            /* entry lanes */
            int count = 0;
            QList<OpenAPI::OAILanelet_info> entry_lanelets = int_info.getEntryLanelets(); 
            for(auto itr = entry_lanelets.begin(); itr != entry_lanelets.end(); itr++ )
            {
              lane_information li;
              li.id = to_string(itr->getId());
              li.index = count;
              li.type = "entry";

              li.length = itr->getLength();
              /* the unit of received speed limit from the intersection model is mile/hr
              *  but the unit of speed limit used in the scheduling_service is meter/sec
              */
              li.speed_limit = itr->getSpeedLimit() * 0.44704;

              /* the unit of received speed limit from the intersection model is mile/hr
              *  but the unit of speed limit used in the scheduling_service is meter/sec
              */
              li.priority = -1;
              lane_info[li.id] = li;
              lane_id_all.push_back(li.id);
	          	lane_id_entry.push_back(li.id);
              count += 1;


              SPDLOG_DEBUG("entry lanelet ID: {0}", li.id);
              SPDLOG_DEBUG("Speed Limit (m/s): {0}", li.speed_limit);
              SPDLOG_DEBUG("Length (m): {0}", li.length);
            }

            /* exit\departure lanes */
            QList<OpenAPI::OAILanelet_info> departure_lanelets = int_info.getDepartureLanelets();
            for(auto itr = departure_lanelets.begin(); itr != departure_lanelets.end(); itr++ )
            {
              lane_information li;
              li.id = to_string(itr->getId());
              li.index = count;
              li.type = "exit";

              li.length = itr->getLength();
              /* the unit of received speed limit from the intersection model is mile/hr
              *  but the unit of speed limit used in the scheduling_service is meter/sec
              */
              li.speed_limit = itr->getSpeedLimit() * 0.44704;

              /* the unit of received speed limit from the intersection model is mile/hr
              *  but the unit of speed limit used in the scheduling_service is meter/sec
              */

              li.priority = -1;
              lane_info[li.id] = li;
              lane_id_all.push_back(li.id);
	          	lane_id_exit.push_back(li.id);
              count += 1;

              SPDLOG_DEBUG("departure lanelet ID: {0}", li.id);
              SPDLOG_DEBUG("Speed Limit (m/s): {0}", li.speed_limit);
              SPDLOG_DEBUG("Length (m): {0}", li.length);

            } 

            /* link lanes */
            QList<OpenAPI::OAILanelet_info> link_lanelets = int_info.getLinkLanelets(); 
            for(auto itr = link_lanelets.begin(); itr != link_lanelets.end(); itr++ )
            {

              lane_information li;
              li.id = to_string(itr->getId());
              li.index = count;
              li.type = "link";

              li.length = itr->getLength();
              /* the unit of received speed limit from the intersection model is mile/hr
              *  but the unit of speed limit used in the scheduling_service is meter/sec
              */
              li.speed_limit = itr->getSpeedLimit() * 0.44704;

              /* each link's priority depends on its direction. but since currently the lane directions are not available, all links are assumed to have the same priority 1 */
              li.priority = 1;
              SPDLOG_DEBUG("departure lanelet ID: {0}", li.id);
              SPDLOG_DEBUG("Speed Limit (m/s): {0}", li.speed_limit);
              SPDLOG_DEBUG("Length (m): {0}", li.length);

              
              SPDLOG_DEBUG("conflict lanelets of lanelet {0}:", li.id);
              QList<qint32> conflict_lanelets = itr->getConflictLaneletIds();
              for(auto inner_itr = conflict_lanelets.begin(); inner_itr != conflict_lanelets.end(); inner_itr++ )
              {
                    li.conflicting_lane_id.push_back(to_string(*inner_itr));
                    SPDLOG_DEBUG("lanelet {0}", li.conflicting_lane_id.back());
              }
              lane_info[li.id] = li;
              lane_id_all.push_back(li.id);
	          	lane_id_link.push_back(li.id);
              count += 1;
            }

            lane_count = count;

            lane_conflict_status.resize(lane_count, vector<int>(lane_count));
            for (string lane_id1 : lane_id_link){
              for (string lane_id2 : lane_info[lane_id1].conflicting_lane_id){
                lane_conflict_status[lane_info[lane_id1].index][lane_info[lane_id2].index] = 1;
              }
              lane_conflict_status[lane_info[lane_id1].index][lane_info[lane_id1].index] = 1;
            }

            loop.quit(); });

  connect(&apiInstance, &OAIDefaultApi::getIntersectionInfoSignalE, [&](OAIIntersection_info, QNetworkReply::NetworkError, QString error_str)
          {
            string error_str_print = error_str.toStdString();
            SPDLOG_CRITICAL("Error happened while issuing request : {0}", error_str_print);
            is_running_indicator = false;
            loop.quit(); });

  apiInstance.getIntersectionInfo();

  QTimer::singleShot(5000, &loop, &QEventLoop::quit);
  loop.exec();
}

/* */
string intersection_client::get_intersectionName() const {return intersection_name;}

bool intersection_client::is_running(){return is_running_indicator;}

/* */
int intersection_client::get_intersectionId() const {return intersection_id;}

/* */
int intersection_client::get_laneCount() const {return lane_count;}

/* */
vector<string> intersection_client::get_laneIdAll() const {return lane_id_all;}

/* */
vector<string> intersection_client::get_laneIdEntry() const {return lane_id_entry;}

/* */
vector<string> intersection_client::get_laneIdExit() const {return lane_id_exit;}

/* */
vector<string> intersection_client::get_laneIdLink() const {return lane_id_link;}

/* */
int intersection_client::get_laneIndex(string const & lane_id) {return lane_info[lane_id].index;}

/* */
string intersection_client::get_laneType(string const & lane_id) {return lane_info[lane_id].type;}

/* */
double intersection_client::get_laneLength(string const & lane_id) {return lane_info[lane_id].length;}

/* */
double intersection_client::get_laneSpeedLimit(string const & lane_id) {return lane_info[lane_id].speed_limit;}

/* */
bool intersection_client::hasConflict(string const & lane_id1, string const & lane_id2){
	if(lane_conflict_status[lane_info[lane_id1].index][lane_info[lane_id2].index] == 1){
		return true;
	} else{
		return false;
	}
}

/* */
int intersection_client::get_lanePriority(string const & lane_id) {return lane_info[lane_id].priority;}
