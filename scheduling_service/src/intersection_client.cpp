
#include "intersection_client.h"
#include "OAIDefaultApi.h"
#include <QTimer>
#include <QEventLoop>
#include "OAIHelpers.h"

void intersection_client::call()
{
  OAIDefaultApi apiInstance;

  QEventLoop loop;
  connect(&apiInstance, &OAIDefaultApi::getIntersectionInfoSignal, [&](OAIIntersection_info int_info)
          {

            // qDebug() << "Intersection name: " <<  int_info.getName();
            // qDebug() << "Intersection id: "   << int_info.getId();
            intersection_name = int_info.getName().toStdString();
            intersection_id = int_info.getId();

            /* entry lanes */
            int count = 0;
            QList<OpenAPI::OAILanelet_info> entry_lanelets = int_info.getEntryLanelets(); 
            for(QList<OpenAPI::OAILanelet_info>::iterator itr = entry_lanelets.begin(); itr != entry_lanelets.end(); itr++ )
            {
              // qDebug() << "entry lanelet ID: "   << itr->getId();
              // qDebug() << "Speed Limit (m/s): " << itr->getSpeedLimit() * 0.44704;
              // qDebug() << "Length (m): " << itr->getLength();
              lane_information li;
              li.id = to_string(itr->getId());
              li.index = count;
              li.type = "entry";
              li.direction = "straight";
              li.length = itr->getLength();
              li.speed_limit = itr->getSpeedLimit() * 0.44704;
              li.priority = -1;
              lane_info[li.id] = li;
              lane_id_all.push_back(li.id);
	          	lane_id_entry.push_back(li.id);
              count += 1;
            }

            /* exit\departure lanes */
            QList<OpenAPI::OAILanelet_info> departure_lanelets = int_info.getDepartureLanelets();
            for(QList<OpenAPI::OAILanelet_info>::iterator itr = departure_lanelets.begin(); itr != departure_lanelets.end(); itr++ )
            {
              // qDebug() << "departure lanelet ID: "   << itr->getId();
              // qDebug() << "Speed Limit (m/s): " << itr->getSpeedLimit() * 0.44704;
              // qDebug() << "Length (m): " << itr->getLength();
              lane_information li;
              li.id = to_string(itr->getId());
              li.index = count;
              li.type = "exit";
              li.direction = "straight";
              li.length = itr->getLength();
              li.speed_limit = itr->getSpeedLimit() * 0.44704;
              li.priority = -1;
              lane_info[li.id] = li;
              lane_id_all.push_back(li.id);
	          	lane_id_exit.push_back(li.id);
              count += 1;
            } 

            /* link lanes */
            QList<OpenAPI::OAILanelet_info> link_lanelets = int_info.getLinkLanelets(); 
            for(QList<OpenAPI::OAILanelet_info>::iterator itr = link_lanelets.begin(); itr != link_lanelets.end(); itr++ )
            {
              // qDebug() << "link lanelet ID: "   << itr->getId();
              // qDebug() << "Speed Limit (m/s): " << itr->getSpeedLimit() * 0.44704;
              // qDebug() << "Length (m): " << itr->getLength();
              lane_information li;
              li.id = to_string(itr->getId());
              li.index = count;
              li.type = "link";
              li.direction = "unknown";
              // li.direction = itr->getDirection();
              li.length = itr->getLength();
              li.speed_limit = itr->getSpeedLimit() * 0.44704;

              li.priority = 1;
              // if (li.direction == "straight"){
              //   li.priority = 1;
              // }
              // else if (li.direction == "left"){
              //   li.priority = 2;
              // }
              // else if (li.direction == "right"){
              //   li.priority = 3;
              // }
              
              // qDebug() << "Conflict lanelets: ";
              QList<qint32> conflict_lanelets = itr->getConflictLaneletIds();
              for(QList<qint32>::iterator inner_itr = conflict_lanelets.begin(); inner_itr != conflict_lanelets.end(); inner_itr++ )
              {
                    // qDebug() <<  *inner_itr;
                    li.conflicting_lane_id.push_back(to_string(*inner_itr));
              }
              lane_info[li.id] = li;
              lane_id_all.push_back(li.id);
	          	lane_id_link.push_back(li.id);
              count += 1;
            }

            lane_count = count;

            lane_conflict_status.resize(lane_count, vector<int>(lane_count));
            for (string lane_id1 : lane_id_link){
              // qDebug() << "lane ID: " << QString::fromStdString(lane_id1);
              for (string lane_id2 : lane_info[lane_id1].conflicting_lane_id){
                // qDebug() << "has conflict with: " << QString::fromStdString(lane_id2);
                lane_conflict_status[lane_info[lane_id1].index][lane_info[lane_id2].index] = 1;
              }
              lane_conflict_status[lane_info[lane_id1].index][lane_info[lane_id1].index] = 1;
            }

            loop.quit(); });

  connect(&apiInstance, &OAIDefaultApi::getIntersectionInfoSignalE, [&](OAIIntersection_info, QNetworkReply::NetworkError, QString error_str)
          {
            qDebug() << "Error happened while issuing request : " << error_str;
            loop.quit(); });

  apiInstance.getIntersectionInfo();

  QTimer::singleShot(5000, &loop, &QEventLoop::quit);
  loop.exec();
}

/* */
string intersection_client::get_intersectionName(){return intersection_name;}

/* */
int intersection_client::get_intersectionId(){return intersection_id;}

/* */
int intersection_client::get_laneCount(){return lane_count;}

/* */
vector<string> intersection_client::get_laneIdAll(){return lane_id_all;}

/* */
vector<string> intersection_client::get_laneIdEntry(){return lane_id_entry;}

/* */
vector<string> intersection_client::get_laneIdExit(){return lane_id_exit;}

/* */
vector<string> intersection_client::get_laneIdLink(){return lane_id_link;}

/* */
int intersection_client::get_laneIndex(string lane_id){return lane_info[lane_id].index;}

/* */
string intersection_client::get_laneType(string lane_id){return lane_info[lane_id].type;}

/* */
string intersection_client::get_laneDirection(string lane_id){return lane_info[lane_id].direction;}

/* */
double intersection_client::get_laneLength(string lane_id){return lane_info[lane_id].length;}

/* */
double intersection_client::get_laneSpeedLimit(string lane_id){return lane_info[lane_id].speed_limit;}

/* */
bool intersection_client::hasConflict(string lane_id1, string lane_id2){
	if(lane_conflict_status[lane_info[lane_id1].index][lane_info[lane_id2].index] == 1){
		return true;
	} else{
		return false;
	}
}

/* */
int intersection_client::get_lanePriority(string lane_id){return lane_info[lane_id].priority;}
