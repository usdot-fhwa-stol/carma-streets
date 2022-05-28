#include "intersection_client.h"
#include <QTimer>
#include <QEventLoop>
#include "intersection_client_api_lib/OAIHelpers.h"

void intersection_client::call()
{
      OAIDefaultApi apiInstance;

      QEventLoop loop;
      connect(&apiInstance, &OAIDefaultApi::getIntersectionInfoSignal, [&](OAIIntersection_info int_info)
              {
                qDebug() << "Intersection name: " <<  int_info.getName();
                qDebug() << "Intersection id: "   << int_info.getId();

                QList<OpenAPI::OAILanelet_info> link_lanelets = int_info.getLinkLanelets(); 
                QList<OpenAPI::OAILanelet_info>::iterator begin = link_lanelets.begin();
                for(QList<OpenAPI::OAILanelet_info>::iterator itr = begin; itr != link_lanelets.end(); itr++ )
                {
                  qDebug() << "link lanelet ID: "   << itr->getId();
                  qDebug() << "Speed Limit: " << itr->getSpeedLimit();
                  qDebug() << "Conflict lanelets: ";
                  for(auto inner_itr = itr->getConflictLaneletIds().begin(); inner_itr != itr->getConflictLaneletIds().end(); inner_itr++ )
                  {
                        qDebug() <<  *inner_itr;
                  }
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
