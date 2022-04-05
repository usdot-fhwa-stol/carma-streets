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
#include <QDebug>

#include "OAIHelpers.h"
#include "OAIDefaultApiRequest.h"

namespace OpenAPI {

OAIDefaultApiRequest::OAIDefaultApiRequest(QHttpEngine::Socket *s, QSharedPointer<OAIDefaultApiHandler> hdl) : QObject(s), socket(s), handler(hdl) {
    auto headers = s->headers();
    for(auto itr = headers.begin(); itr != headers.end(); itr++) {
        requestHeaders.insert(QString(itr.key()), QString(itr.value()));
    }
}

OAIDefaultApiRequest::~OAIDefaultApiRequest(){
    disconnect(this, nullptr, nullptr, nullptr);
    SPDLOG_DEBUG("OAIDefaultApiRequest::~OAIDefaultApiRequest()");
}

QMap<QString, QString>
OAIDefaultApiRequest::getRequestHeaders() const {
    return requestHeaders;
}

void OAIDefaultApiRequest::setResponseHeaders(const QMultiMap<QString, QString>& headers){
    for(auto itr = headers.begin(); itr != headers.end(); ++itr) {
        responseHeaders.insert(itr.key(), itr.value());
    }
}


QHttpEngine::Socket* OAIDefaultApiRequest::getRawSocket(){
    return socket;
}


void OAIDefaultApiRequest::getConflictLaneletsRequest(const QString& link_lanelet_idstr){
    SPDLOG_INFO("/intersection_model/v1/get_conflict_lanelets/{0}", link_lanelet_idstr.toStdString());
    connect(this, &OAIDefaultApiRequest::getConflictLanelets, handler.data(), &OAIDefaultApiHandler::getConflictLanelets);

    
    qint32 link_lanelet_id;
    fromStringValue(link_lanelet_idstr, link_lanelet_id);
    

    emit getConflictLanelets(link_lanelet_id);
}


void OAIDefaultApiRequest::getIntersectionInfoRequest(){
    SPDLOG_INFO("/intersection_model/v1/info");
    connect(this, &OAIDefaultApiRequest::getIntersectionInfo, handler.data(), &OAIDefaultApiHandler::getIntersectionInfo);

    


    emit getIntersectionInfo();
}


void OAIDefaultApiRequest::listDepartureLaneletsRequest(){
    SPDLOG_INFO("/intersection_model/v1/departure_lanelets");
    connect(this, &OAIDefaultApiRequest::listDepartureLanelets, handler.data(), &OAIDefaultApiHandler::listDepartureLanelets);

    


    emit listDepartureLanelets();
}


void OAIDefaultApiRequest::listEntryLaneletsRequest(){
    SPDLOG_INFO("/intersection_model/v1/entry_lanelets");
    connect(this, &OAIDefaultApiRequest::listEntryLanelets, handler.data(), &OAIDefaultApiHandler::listEntryLanelets);

    


    emit listEntryLanelets();
}


void OAIDefaultApiRequest::listLinkLaneletsRequest(){
    SPDLOG_INFO("/intersection_model/v1/link_lanelets");
    connect(this, &OAIDefaultApiRequest::listLinkLanelets, handler.data(), &OAIDefaultApiHandler::listLinkLanelets);

    


    emit listLinkLanelets();
}



void OAIDefaultApiRequest::getConflictLaneletsResponse(const QList<OAILanelet_info>& res){
    setSocketResponseHeaders();
    QJsonDocument resDoc(::OpenAPI::toJsonValue(res).toArray());
    socket->writeJson(resDoc);
    if(socket->isOpen()){
        socket->close();
    }
}

void OAIDefaultApiRequest::getIntersectionInfoResponse(const OAIIntersection_info& res){
    setSocketResponseHeaders();
    QJsonDocument resDoc(::OpenAPI::toJsonValue(res).toObject());
    socket->writeJson(resDoc);
    if(socket->isOpen()){
        socket->close();
    }
}

void OAIDefaultApiRequest::listDepartureLaneletsResponse(const QList<OAILanelet_info>& res){
    setSocketResponseHeaders();
    QJsonDocument resDoc(::OpenAPI::toJsonValue(res).toArray());
    socket->writeJson(resDoc);
    if(socket->isOpen()){
        socket->close();
    }
}

void OAIDefaultApiRequest::listEntryLaneletsResponse(const QList<OAILanelet_info>& res){
    setSocketResponseHeaders();
    QJsonDocument resDoc(::OpenAPI::toJsonValue(res).toArray());
    socket->writeJson(resDoc);
    if(socket->isOpen()){
        socket->close();
    }
}

void OAIDefaultApiRequest::listLinkLaneletsResponse(const QList<OAILanelet_info>& res){
    setSocketResponseHeaders();
    QJsonDocument resDoc(::OpenAPI::toJsonValue(res).toArray());
    socket->writeJson(resDoc);
    if(socket->isOpen()){
        socket->close();
    }
}


void OAIDefaultApiRequest::getConflictLaneletsError(const QList<OAILanelet_info>& res, QNetworkReply::NetworkError error_type, QString& error_str){
    setSocketResponseHeaders();
    if(error_type == QNetworkReply::NetworkError::ContentAccessDenied)
    {
        socket->writeError(QHttpEngine::Socket::Forbidden,QByteArray(error_str.toUtf8()));
    }
    else
    {
        QJsonDocument resDoc(::OpenAPI::toJsonValue(res).toObject());
        socket->writeJson(resDoc);
    }
    if(socket->isOpen()){
        socket->close();
    }
}

void OAIDefaultApiRequest::getIntersectionInfoError(const OAIIntersection_info& res, QNetworkReply::NetworkError error_type, QString& error_str){
    Q_UNUSED(error_type); // TODO: Remap error_type to QHttpEngine::Socket errors
    setSocketResponseHeaders();
    Q_UNUSED(error_str);  // response will be used instead of error string
    QJsonDocument resDoc(::OpenAPI::toJsonValue(res).toObject());
    socket->writeJson(resDoc);
    if(socket->isOpen()){
        socket->close();
    }
}

void OAIDefaultApiRequest::listDepartureLaneletsError(const QList<OAILanelet_info>& res, QNetworkReply::NetworkError error_type, QString& error_str){
    Q_UNUSED(error_type); // TODO: Remap error_type to QHttpEngine::Socket errors
    setSocketResponseHeaders();
    Q_UNUSED(error_str);  // response will be used instead of error string
    QJsonDocument resDoc(::OpenAPI::toJsonValue(res).toArray());
    socket->writeJson(resDoc);
    if(socket->isOpen()){
        socket->close();
    }
}

void OAIDefaultApiRequest::listEntryLaneletsError(const QList<OAILanelet_info>& res, QNetworkReply::NetworkError error_type, QString& error_str){
    Q_UNUSED(error_type); // TODO: Remap error_type to QHttpEngine::Socket errors
    setSocketResponseHeaders();
    Q_UNUSED(error_str);  // response will be used instead of error string
    QJsonDocument resDoc(::OpenAPI::toJsonValue(res).toArray());
    socket->writeJson(resDoc);
    if(socket->isOpen()){
        socket->close();
    }
}

void OAIDefaultApiRequest::listLinkLaneletsError(const QList<OAILanelet_info>& res, QNetworkReply::NetworkError error_type, QString& error_str){
    Q_UNUSED(error_type); // TODO: Remap error_type to QHttpEngine::Socket errors
    setSocketResponseHeaders();
    Q_UNUSED(error_str);  // response will be used instead of error string
    QJsonDocument resDoc(::OpenAPI::toJsonValue(res).toArray());
    socket->writeJson(resDoc);
    if(socket->isOpen()){
        socket->close();
    }
}


void OAIDefaultApiRequest::sendCustomResponse(QByteArray & res, QNetworkReply::NetworkError error_type){
    Q_UNUSED(error_type); // TODO
    socket->write(res);
    if(socket->isOpen()){
        socket->close();
    }
}

void OAIDefaultApiRequest::sendCustomResponse(QIODevice *res, QNetworkReply::NetworkError error_type){
    Q_UNUSED(error_type);  // TODO
    socket->write(res->readAll());
    if(socket->isOpen()){
        socket->close();
    }
}

}
