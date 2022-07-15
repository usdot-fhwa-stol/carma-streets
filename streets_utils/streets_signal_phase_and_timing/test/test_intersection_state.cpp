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
    long moy;     // Minute of the year
    long msOfMin; // Millisecond of the minute
    uint64_t epoch_timestamp;

protected:
    void SetUp() override
    {
        std::chrono::system_clock::time_point now = std::chrono::system_clock::now();
        time_t tt = std::chrono::system_clock::to_time_t(now);
        struct tm *utctime = gmtime(&tt);

        // Calculate the minute of the year
        moy = utctime->tm_min + (utctime->tm_hour * 60) + (utctime->tm_yday * 24 * 60);

        // Calculate the millisecond of the minute
        std::chrono::milliseconds epochMs = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch());
        epoch_timestamp = epochMs.count();
        SPDLOG_INFO("epoch_timestamp {0}", epoch_timestamp );
        std::chrono::seconds epochS = std::chrono::duration_cast<std::chrono::seconds>(epochMs);
        msOfMin = 1000 * (epochS.count() % 60) + (epochMs.count() % 1000);
        iss.time_stamp = msOfMin;
        SPDLOG_INFO("Millisecond of the minute {0}", msOfMin );
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
    ASSERT_EQ(epoch_timestamp, iss.convert_min_mills2epoch_ts(iss.time_stamp));
}

TEST_F(test_intersection_state, get_epoch_timestamp)
{
    ASSERT_EQ(epoch_timestamp, iss.get_epoch_timestamp());
}
