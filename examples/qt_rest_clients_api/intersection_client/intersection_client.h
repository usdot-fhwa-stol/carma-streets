#pragma once
#include <iostream>
#include "intersection_client_api_lib/OAIDefaultApi.h"
using namespace OpenAPI;

class intersection_client : public QObject
{
    public:
    intersection_client(/* args */){};
    ~intersection_client(){};
    void call();
};
