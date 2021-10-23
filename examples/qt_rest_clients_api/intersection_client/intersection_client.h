#pragma once
#include <iostream>
#include "OAIDefaultApi.h"
using namespace OpenAPI;

class intersection_client : public QObject
{
    Q_OBJECT
public slots:
    void call();
};
