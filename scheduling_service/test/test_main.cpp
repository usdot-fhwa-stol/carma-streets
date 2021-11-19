#include "gtest/gtest.h"
#include "OAIDefaultApi.h"
#include <QTimer>
#include <QEventLoop>
#include <QCoreApplication>
#include "OAIHelpers.h"

int main(int argc, char **argv)
{
    QCoreApplication a(argc, argv);
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}