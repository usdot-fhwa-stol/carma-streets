/**
 * Intersection Model API
 * No description provided (generated by Openapi Generator https://github.com/openapitools/openapi-generator)
 *
 * The version of the OpenAPI document: 1.0.0
 *
 * NOTE: This class is auto generated by OpenAPI Generator (https://openapi-generator.tech).
 * https://openapi-generator.tech
 * Do not edit the class manually.
 */

#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QVariantMap>


#include "OAIApiRouter.h"
#include "OAIDefaultApiRequest.h"


namespace OpenAPI {

OAIApiRouter::OAIApiRouter() {
    createApiHandlers();
}

OAIApiRouter::~OAIApiRouter(){

}

void OAIApiRouter::createApiHandlers() { 
    mOAIDefaultApiHandler = QSharedPointer<OAIDefaultApiHandler>::create();
}


void OAIApiRouter::setOAIDefaultApiHandler(QSharedPointer<OAIDefaultApiHandler> handler){
    mOAIDefaultApiHandler = handler;
}

void OAIApiRouter::setUpRoutes() {
    
    Routes.insert(QString("%1 %2").arg("GET").arg("/intersection_model/v1/info").toLower(), [this](QHttpEngine::Socket *socket) {
            auto reqObj = new OAIDefaultApiRequest(socket, mOAIDefaultApiHandler);
            reqObj->getIntersectionInfoRequest();
    });
    Routes.insert(QString("%1 %2").arg("GET").arg("/intersection_model/v1/departure_lanelets").toLower(), [this](QHttpEngine::Socket *socket) {
            auto reqObj = new OAIDefaultApiRequest(socket, mOAIDefaultApiHandler);
            reqObj->listDepartureLaneletsRequest();
    });
    Routes.insert(QString("%1 %2").arg("GET").arg("/intersection_model/v1/entry_lanelets").toLower(), [this](QHttpEngine::Socket *socket) {
            auto reqObj = new OAIDefaultApiRequest(socket, mOAIDefaultApiHandler);
            reqObj->listEntryLaneletsRequest();
    });
    Routes.insert(QString("%1 %2").arg("GET").arg("/intersection_model/v1/link_lanelets").toLower(), [this](QHttpEngine::Socket *socket) {
            auto reqObj = new OAIDefaultApiRequest(socket, mOAIDefaultApiHandler);
            reqObj->listLinkLaneletsRequest();
    });
}

void OAIApiRouter::processRequest(QHttpEngine::Socket *socket){
    if( handleRequest(socket) ){
        return;
    }
    if( handleRequestAndExtractPathParam(socket) ){
        return;
    }
    socket->setStatusCode(QHttpEngine::Socket::NotFound);
    if(socket->isOpen()){
        socket->writeHeaders();
        socket->close();
    }
}

bool OAIApiRouter::handleRequest(QHttpEngine::Socket *socket){
    auto reqPath = QString("%1 %2").arg(fromQHttpEngineMethod(socket->method())).arg(socket->path()).toLower();
    if ( Routes.contains(reqPath) ) {
        Routes.value(reqPath).operator()(socket);
        return true;
    }
    return false;
}

bool OAIApiRouter::handleRequestAndExtractPathParam(QHttpEngine::Socket *socket){
    auto reqPath = QString("%1 %2").arg(fromQHttpEngineMethod(socket->method())).arg(socket->path()).toLower();
    {
        auto completePath = QString("%1 %2").arg("GET").arg("/intersection_model/v1/get_conflict_lanelets/{link_lanelet_id}").toLower();
        if ( reqPath.startsWith(completePath.leftRef( completePath.indexOf(QString("/{")))) ) {
            QRegularExpressionMatch match = getRequestMatch( completePath, reqPath );
            if ( match.hasMatch() ){
                QString link_lanelet_id = match.captured(QString("link_lanelet_id").toLower());
                auto reqObj = new OAIDefaultApiRequest(socket, mOAIDefaultApiHandler);
                reqObj->getConflictLaneletsRequest(link_lanelet_id);
                return true;
            }
        }
    }
    return false;
}

}
