
#include <gtest/gtest.h>
#include <spdlog/spdlog.h>

#include "intersection_client.h"

using namespace scheduling_service;

namespace {

    class intersection_client_test : public ::testing::Test {

    protected:
        
        std::shared_ptr<intersection_client> int_client;
        
        /**
         * @brief Test Setup method run before each test.
         * 
         */
        void SetUp() override {
            
            int_client = std::make_shared<intersection_client>();

            SPDLOG_INFO("Test SetUp Complete");
        }
            
        /**
         * @brief Test TearDown method run after each test.
         * 
         */
        void TearDown() override{
            
        }

    };
};

TEST_F(intersection_client_test, update_intersection_info)
{
    unsigned long sleep_millisecs = 1000;
    unsigned long int_client_request_attempts = 10;
    ASSERT_FALSE(int_client->update_intersection_info(sleep_millisecs, int_client_request_attempts));
}


TEST_F(intersection_client_test, request_intersection_info)
{
    ASSERT_FALSE(int_client->request_intersection_info());
}