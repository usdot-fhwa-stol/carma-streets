#include <gtest/gtest.h>
#include <spdlog/spdlog.h>
#include <rapidjson/writer.h>
#include <rapidjson/stringbuffer.h>
#include "spat.h"
#include "signal_phase_and_timing_exception.h"
#include "threads.h"

using namespace signal_phase_and_timing;

class test_intersection_state : public ::testing::Test
{
public:
    intersection_state iss;
    uint32_t moy;     // Minute of the year
    uint16_t msOfMin; // Millisecond of the minute
    uint64_t epoch_timestamp; //Current epoch timestamp
    uint64_t epoch_intersection_state_timestamp; //Epoch minutes + intersection state timestamp

protected:
    void SetUp() override
    {
        std::chrono::system_clock::time_point now = std::chrono::system_clock::now();
        time_t tt = std::chrono::system_clock::to_time_t(now);
        struct tm *utctime = gmtime(&tt);

        // Calculate the minute of the year
        moy = utctime->tm_min + (utctime->tm_hour * 60) + (utctime->tm_yday * 24 * 60);
        iss.moy = moy;

        // Calculate the millisecond of the minute
        std::chrono::milliseconds epochMs = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch());
        epoch_timestamp = epochMs.count();
        SPDLOG_INFO("epoch_timestamp {0}", epoch_timestamp );        
        std::chrono::seconds epochS = std::chrono::duration_cast<std::chrono::seconds>(epochMs);
        msOfMin = 1000 * (epochS.count() % 60) + (epochMs.count() % 1000);
        iss.time_stamp = msOfMin;
        SPDLOG_INFO("Millisecond of the minute {0}", msOfMin );        

        // Get the epoch minutes 
        std::chrono::minutes epochMin = std::chrono::duration_cast<std::chrono::minutes>(now.time_since_epoch());
        SPDLOG_INFO("Epoch minutes {0}", epochMin.count());

        // Epoch minutes + milliseconds of current minutes
        epoch_intersection_state_timestamp = epochMin.count() * iss.MIN_TO_SECONDS*iss.SECOND_TO_MILLISECONDS + msOfMin;
        SPDLOG_INFO("epoch_intersection_state_timestamp {0}", epoch_intersection_state_timestamp );
    }
};

TEST_F(test_intersection_state, fromJson)
{
    rapidjson::Value json(rapidjson::kObjectType);
    rapidjson::Document doc;
    auto allocator = doc.GetAllocator();
    json.AddMember("time_stamp", 1000, allocator);
    json.AddMember("id", 1, allocator);
    json.AddMember("revision", 1, allocator);
    json.AddMember("moy", 12, allocator);
    json.AddMember("status", "1", allocator);
    rapidjson::Value states_list(rapidjson::kArrayType);
    json.AddMember("states", states_list, allocator);

    intersection_state iss;
    iss.fromJson(json);
    ASSERT_EQ(1000, iss.time_stamp);
}

TEST_F(test_intersection_state, toJson)
{
    intersection_state iss;
    rapidjson::Document doc;
    auto allocator = doc.GetAllocator();
    iss.time_stamp = 1000;
    iss.id= 1;
    iss.revision = 1;
    iss.moy= 123;
    iss.status = "1";
    movement_state mms;
    iss.states.push_back(mms);
    auto json = iss.toJson(allocator);

    ASSERT_EQ(1000, json["time_stamp"].GetInt());
}

TEST_F(test_intersection_state, convert_min_mills2epoch_ts)
{
    ASSERT_EQ(epoch_timestamp, iss.convert_min_mills2epoch_ts(moy, iss.time_stamp));
    ASSERT_EQ(epoch_intersection_state_timestamp, iss.convert_min_mills2epoch_ts(moy, iss.time_stamp));
}

TEST_F(test_intersection_state, get_epoch_timestamp)
{
    ASSERT_EQ(epoch_timestamp, iss.get_epoch_timestamp());
    ASSERT_EQ(epoch_intersection_state_timestamp, iss.get_epoch_timestamp());
}
