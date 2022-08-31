#include <gtest/gtest.h>
#include <spdlog/spdlog.h>
#include <rapidjson/writer.h>
#include <rapidjson/stringbuffer.h>
#include "spat.h"
#include "signal_phase_and_timing_exception.h"

using namespace signal_phase_and_timing;

class test_time_change_detail : public ::testing::Test
{
public:
    time_change_details tcd;
    uint64_t epoch_start_time;
    uint64_t epoch_min_end_time;
    uint64_t epoch_max_end_time;
    uint64_t epoch_next_time;
    int THREE_SECONDS = 3;
    int FIVE_SECONDS = 5;
    int TEN_SECONDS = 10;
protected:    
    void SetUp() override
    {
        typedef std::chrono::duration<int, std::ratio_multiply<std::chrono::hours::period, std::ratio<24>>::type> days;
        auto tp = std::chrono::system_clock::now();
        auto duration = tp.time_since_epoch();
        SPDLOG_INFO("epoch time point: {0}", tp.time_since_epoch().count());

        days d = std::chrono::duration_cast<days>(duration);
        SPDLOG_INFO("epoch days: {0}", d.count());

        auto cur_h = std::chrono::duration_cast<std::chrono::hours>(duration - d);
        SPDLOG_INFO("current hours: {0}", cur_h.count());

        auto cur_m = std::chrono::duration_cast<std::chrono::minutes>(duration - d - cur_h);
        SPDLOG_INFO("current minutes: {0}", cur_m.count());

        auto cur_s = std::chrono::duration_cast<std::chrono::seconds>(duration - d - cur_h - cur_m);
        SPDLOG_INFO("current seconds: {0}", cur_s.count());

        auto cur_ms = std::chrono::duration_cast<std::chrono::milliseconds>(duration - d - cur_h - cur_m - cur_s);
        SPDLOG_INFO("current milliseconds: {0}", cur_ms.count());

        //Start time
        double fractionSeconds = cur_s.count() + (cur_ms.count() / 1000.0);
        long tenth_seconds_at_current_hour = ((cur_m.count() * 60) + fractionSeconds) * 10;
        SPDLOG_INFO("Tenth seconds at current hour: {0}", tenth_seconds_at_current_hour);
        tcd.start_time =  tenth_seconds_at_current_hour;

        auto hours_since_epoch = std::chrono::duration_cast<std::chrono::hours>(duration).count();
        auto hours_sicne_epoch_in_milliseconds = hours_since_epoch * tcd.HOUR_TO_SECONDS * tcd.SECOND_TO_MILLISECONDS;
        epoch_start_time = hours_sicne_epoch_in_milliseconds + tcd.start_time * 100;
        SPDLOG_INFO("Epoch start time: {0}", epoch_start_time);

        //Min end time in THREE_SECONDS
        tcd.min_end_time = tenth_seconds_at_current_hour + THREE_SECONDS * 10;
        epoch_min_end_time = hours_sicne_epoch_in_milliseconds + tcd.min_end_time * 100;
        SPDLOG_INFO("Epoch min end time (in THREE SECONDS): {0}", epoch_min_end_time);

        //Max end time in FIVE_SECONDS
        tcd.max_end_time = tenth_seconds_at_current_hour + FIVE_SECONDS * 10;
        epoch_max_end_time = hours_sicne_epoch_in_milliseconds + tcd.max_end_time * 100;
        SPDLOG_INFO("Epoch max end time (in FIVE SECONDS): {0}", epoch_max_end_time);

        //Next time in TEN_SECONDS
        tcd.next_time = tenth_seconds_at_current_hour + TEN_SECONDS * 10;
        epoch_next_time = hours_sicne_epoch_in_milliseconds +  tcd.next_time * 100;
        SPDLOG_INFO("Epoch next time (in TEN SECONDS): {0}", epoch_next_time);

    }
};

TEST_F(test_time_change_detail, fromJson)
{
    rapidjson::Value json(rapidjson::kObjectType);
    rapidjson::Document doc;
    auto allocator = doc.GetAllocator();
    json.AddMember("start_time", 1000, allocator);
    json.AddMember("min_end_time", 2000, allocator);
    json.AddMember("max_end_time", 3000, allocator);
    json.AddMember("next_time", 4000, allocator);

    time_change_details tcd;
    tcd.fromJson(json);
    ASSERT_EQ(1000, tcd.start_time);
    ASSERT_EQ(2000, tcd.min_end_time);
    ASSERT_EQ(3000, tcd.max_end_time);
    ASSERT_EQ(4000, tcd.next_time);
}

TEST_F(test_time_change_detail, toJson)
{
    time_change_details tcd;
    rapidjson::Document doc;
    auto allocator = doc.GetAllocator();
    tcd.start_time = 1000;
    tcd.min_end_time = 2000;
    tcd.max_end_time = 3000;
    tcd.next_time = 4000;
    auto json = tcd.toJson(allocator);

    ASSERT_EQ(1000, json["start_time"].GetInt());
    ASSERT_EQ(2000, json["min_end_time"].GetInt());
    ASSERT_EQ(3000, json["max_end_time"].GetInt());
    ASSERT_EQ(4000, json["next_time"].GetInt());
}

TEST_F(test_time_change_detail, convert_hour_mills2epoch_ts)
{
    ASSERT_EQ(epoch_start_time, tcd.convert_hour_tenth_secs2epoch_ts(tcd.start_time));
}

TEST_F(test_time_change_detail, get_epoch_start_time)
{
    ASSERT_EQ(epoch_start_time, tcd.get_epoch_start_time());
}

TEST_F(test_time_change_detail, get_epoch_min_end_time)
{
    ASSERT_EQ(epoch_min_end_time, tcd.get_epoch_min_end_time());
}

TEST_F(test_time_change_detail, get_epoch_max_end_time)
{
    ASSERT_EQ(epoch_max_end_time, tcd.get_epoch_max_end_time());
}

TEST_F(test_time_change_detail, get_epoch_next_time)
{
    ASSERT_EQ(epoch_next_time, tcd.get_epoch_next_time());
}

TEST_F(test_time_change_detail, convert_msepoch_to_hour_tenth_secs)
{
    std::chrono::system_clock::time_point now = std::chrono::system_clock::now();
    std::chrono::milliseconds epochMs = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch());
    uint64_t epoch_timestamp = epochMs.count();
    auto hours_since_epoch = std::chrono::duration_cast<std::chrono::hours>(now.time_since_epoch()).count();
    
    ASSERT_EQ((epoch_timestamp - hours_since_epoch*3600*1000)/100, tcd.convert_msepoch_to_hour_tenth_secs(epoch_timestamp));
}

TEST_F(test_time_change_detail, set_start_time)
{
    std::chrono::system_clock::time_point now = std::chrono::system_clock::now();
    std::chrono::milliseconds epochMs = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch());
    auto hours_since_epoch = std::chrono::duration_cast<std::chrono::hours>(now.time_since_epoch()).count();
    uint64_t epoch_timestamp = epochMs.count();
    tcd.set_start_time(epoch_timestamp);
    ASSERT_EQ((epoch_timestamp - hours_since_epoch*3600*1000)/100, tcd.start_time );
}

TEST_F(test_time_change_detail, set_min_end_time)
{
     std::chrono::system_clock::time_point now = std::chrono::system_clock::now();
    std::chrono::milliseconds epochMs = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch());
    auto hours_since_epoch = std::chrono::duration_cast<std::chrono::hours>(now.time_since_epoch()).count();
    uint64_t epoch_timestamp = epochMs.count();
    tcd.set_min_end_time(epoch_timestamp);
    ASSERT_EQ((epoch_timestamp - hours_since_epoch*3600*1000)/100, tcd.min_end_time);
}

TEST_F(test_time_change_detail, set_max_end_time)
{
    std::chrono::system_clock::time_point now = std::chrono::system_clock::now();
    std::chrono::milliseconds epochMs = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch());
    auto hours_since_epoch = std::chrono::duration_cast<std::chrono::hours>(now.time_since_epoch()).count();
    uint64_t epoch_timestamp = epochMs.count();
    tcd.set_max_end_time(epoch_timestamp);
    ASSERT_EQ((epoch_timestamp - hours_since_epoch*3600*1000)/100, tcd.max_end_time);
}

TEST_F(test_time_change_detail, set_next_time)
{
     std::chrono::system_clock::time_point now = std::chrono::system_clock::now();
    std::chrono::milliseconds epochMs = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch());
    auto hours_since_epoch = std::chrono::duration_cast<std::chrono::hours>(now.time_since_epoch()).count();
    uint64_t epoch_timestamp = epochMs.count();
    tcd.set_next_time(epoch_timestamp);
    ASSERT_EQ((epoch_timestamp - hours_since_epoch*3600*1000)/100, tcd.next_time);
}