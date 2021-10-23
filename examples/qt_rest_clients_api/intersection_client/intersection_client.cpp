
#include "OAIDefaultApi.h"
#include "intersection_client.h"
#include <QTimer>
#include <QEventLoop>
#include "OAIHelpers.h"

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
                qDebug() << "Json: " << itr->asJson();
                qDebug() << "ID: "   << itr->getId();
                qDebug() << "Speed Limit: " << itr->getSpeedLimit();
                }
                loop.quit(); 
          });

         connect(&apiInstance, &OAIDefaultApi::getIntersectionInfoSignalE, [&](OAIIntersection_info, QNetworkReply::NetworkError, QString error_str)
          {
                qDebug() << "Error happened while issuing request : " << error_str;
                loop.quit(); 
          });

        apiInstance.getIntersectionInfo();

  QTimer::singleShot(5000, &loop, &QEventLoop::quit);
  loop.exec();
}
