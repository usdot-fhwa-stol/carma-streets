#pragma once
#include <iostream>
#include "OAIDefaultApi.h"
using namespace OpenAPI;

class intersection_client : public QObject
{
    public:
    intersection_client(/* args */){};
    ~intersection_client(){};
    void call();
};
