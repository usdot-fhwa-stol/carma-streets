#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <gtest/gtest_prod.h>
#include "spat.h"
#include "signal_phase_and_timing_exception.h"
#include "streets_desired_phase_plan_arbitrator.h"
#include <spdlog/spdlog.h>
#include <chrono>
#include "vehicle.h"
#include "vehicle_list.h"
#include "status_intent_processing_exception.h"
#include "status_intent_processor.h"
#include "signalized_status_intent_processor.h"

namespace streets_desired_phase_plan_arbitrator
{

    class test_streets_desired_phase_plan_arbitrator : public ::testing::Test
    {
    public:
        std::shared_ptr<signal_phase_and_timing::spat> spat_msg_ptr;
        std::shared_ptr<signal_phase_and_timing::spat> spat_msg_two_ptr;
        std::shared_ptr<signal_phase_and_timing::spat> spat_msg_three_ptr;
        std::shared_ptr<std::unordered_map<int, streets_tsc_configuration::signal_group_configuration>> sg_yellow_duration_red_clearnace_map_ptr;
        uint16_t current_hour_in_tenths_secs;
        std::shared_ptr<OpenAPI::OAIIntersection_info> intersection_info;

    protected:
        void SetUp() override
        {
            sg_yellow_duration_red_clearnace_map_ptr = std::make_shared<std::unordered_map<int, streets_tsc_configuration::signal_group_configuration>>();
            std::chrono::system_clock::time_point now = std::chrono::system_clock::now();
            std::chrono::milliseconds epochMs = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch());
            uint64_t epoch_timestamp = epochMs.count();

            // Create mock intersection info
            OpenAPI::OAIIntersection_info info;
            std::string json_info = "{\"departure_lanelets\":[{ \"id\":162, \"length\":41.60952439839113, \"speed_limit\":11.176}, { \"id\":164, \"length\":189.44565302601367, \"speed_limit\":11.176 }, { \"id\":168, \"length\":34.130869420842046, \"speed_limit\":11.176 } ], \"entry_lanelets\":[ { \"id\":167, \"length\":195.73023157287864, \"speed_limit\":11.176, \"connecting_lanelet_ids\": [155, 169] }, { \"id\":171, \"length\":34.130869411176431136, \"speed_limit\":11.176, \"connecting_lanelet_ids\": [160, 161] }, { \"id\":163, \"length\":41.60952435603712, \"speed_limit\":11.176 , \"connecting_lanelet_ids\": [156, 165]} ], \"id\":9001, \"link_lanelets\":[{ \"conflict_lanelet_ids\":[ 161 ], \"id\":169, \"length\":15.85409574709938, \"speed_limit\":11.176, \"signal_group_id\":1 }, { \"conflict_lanelet_ids\":[ 165, 156, 161 ], \"id\":155, \"length\":16.796388658952235, \"speed_limit\":4.4704, \"signal_group_id\":1 }, { \"conflict_lanelet_ids\":[ 155, 161, 160 ], \"id\":165, \"length\":15.853947840111768943, \"speed_limit\":11.176, \"signal_group_id\":3 }, { \"conflict_lanelet_ids\":[ 155 ], \"id\":156, \"length\":9.744590320260139, \"speed_limit\":11.176, \"signal_group_id\":3 }, { \"conflict_lanelet_ids\":[ 169, 155, 165 ], \"id\":161, \"length\":16.043077028554038, \"speed_limit\":11.176, \"signal_group_id\":2 }, { \"conflict_lanelet_ids\":[ 165 ], \"id\":160, \"length\":10.295559117055083, \"speed_limit\":11.176, \"signal_group_id\":2 } ], \"name\":\"WestIntersection\"}";
            info.fromJson(QString::fromStdString(json_info));
            intersection_info = std::make_shared<OpenAPI::OAIIntersection_info>(info);

            auto hours_since_epoch = std::chrono::duration_cast<std::chrono::hours>(now.time_since_epoch()).count();
            current_hour_in_tenths_secs = (epoch_timestamp - hours_since_epoch * 3600 * 1000) / 100;

            // Create mock spat
            spat_msg_ptr = std::make_shared<signal_phase_and_timing::spat>();
            spat_msg_two_ptr = std::make_shared<signal_phase_and_timing::spat>();
            spat_msg_three_ptr = std::make_shared<signal_phase_and_timing::spat>();

            signal_phase_and_timing::intersection_state intersection_state;
            signal_phase_and_timing::intersection_state intersection_state_two;
            signal_phase_and_timing::intersection_state intersection_state_three;

            signal_phase_and_timing::movement_state state_1;
            state_1.signal_group = 1;
            signal_phase_and_timing::movement_event event_1;
            event_1.event_state = signal_phase_and_timing::movement_phase_state::protected_movement_allowed; // Green
            event_1.timing.start_time = current_hour_in_tenths_secs;
            event_1.timing.min_end_time = current_hour_in_tenths_secs + 10;
            state_1.state_time_speed.push_back(event_1);
            intersection_state.states.push_back(state_1);

            state_1.state_time_speed.front().event_state = signal_phase_and_timing::movement_phase_state::stop_and_remain; // Red
            intersection_state_two.states.push_back(state_1);
            intersection_state_three.states.push_back(state_1);

            streets_tsc_configuration::signal_group_configuration tsc_config_1;
            int signal_group_1 = 1;
            tsc_config_1.red_clearance = 0;
            tsc_config_1.yellow_change_duration = 0;
            sg_yellow_duration_red_clearnace_map_ptr->insert({signal_group_1, tsc_config_1});

            signal_phase_and_timing::movement_state state_2;
            state_2.signal_group = 2;
            signal_phase_and_timing::movement_event event_2;
            event_2.event_state = signal_phase_and_timing::movement_phase_state::stop_and_remain; // Red
            event_2.timing.start_time = current_hour_in_tenths_secs;
            event_2.timing.min_end_time = current_hour_in_tenths_secs + 10;
            state_2.state_time_speed.push_back(event_2);
            intersection_state.states.push_back(state_2);
            intersection_state_three.states.push_back(state_2);

            state_2.state_time_speed.front().event_state = signal_phase_and_timing::movement_phase_state::protected_clearance; // Yellow
            intersection_state_two.states.push_back(state_2);

            streets_tsc_configuration::signal_group_configuration tsc_config_2;
            int signal_group_2 = 2;
            tsc_config_2.red_clearance = 0;
            tsc_config_2.yellow_change_duration = 0;
            sg_yellow_duration_red_clearnace_map_ptr->insert({signal_group_2, tsc_config_2});

            signal_phase_and_timing::movement_state state_3;
            state_3.signal_group = 3;
            signal_phase_and_timing::movement_event event_3;
            event_3.event_state = signal_phase_and_timing::movement_phase_state::stop_and_remain; // Red
            event_3.timing.start_time = current_hour_in_tenths_secs;
            event_3.timing.min_end_time = current_hour_in_tenths_secs + 10;
            state_3.state_time_speed.push_back(event_3);
            intersection_state.states.push_back(state_3);
            intersection_state_two.states.push_back(state_3);
            intersection_state_three.states.push_back(state_3);

            streets_tsc_configuration::signal_group_configuration tsc_config_3;
            int signal_group_3 = 3;
            tsc_config_3.red_clearance = 0;
            tsc_config_3.yellow_change_duration = 0;
            sg_yellow_duration_red_clearnace_map_ptr->insert({signal_group_3, tsc_config_3});

            signal_phase_and_timing::movement_state state_4;
            state_4.signal_group = 4;
            signal_phase_and_timing::movement_event event_4;
            event_4.event_state = signal_phase_and_timing::movement_phase_state::stop_and_remain; // Red
            event_4.timing.start_time = current_hour_in_tenths_secs;
            event_4.timing.min_end_time = current_hour_in_tenths_secs + 10;
            state_4.state_time_speed.push_back(event_4);
            intersection_state.states.push_back(state_4);
            intersection_state_two.states.push_back(state_4);
            intersection_state_three.states.push_back(state_4);

            streets_tsc_configuration::signal_group_configuration tsc_config_4;
            int signal_group_4 = 4;
            tsc_config_4.red_clearance = 0;
            tsc_config_4.yellow_change_duration = 0;
            sg_yellow_duration_red_clearnace_map_ptr->insert({signal_group_4, tsc_config_4});

            signal_phase_and_timing::movement_state state_5;
            state_5.signal_group = 5;
            signal_phase_and_timing::movement_event event_5;
            event_5.event_state = signal_phase_and_timing::movement_phase_state::protected_movement_allowed; // Green
            event_5.timing.start_time = current_hour_in_tenths_secs;
            event_5.timing.min_end_time = current_hour_in_tenths_secs + 10;
            state_5.state_time_speed.push_back(event_5);
            intersection_state.states.push_back(state_5);

            state_5.state_time_speed.front().event_state = signal_phase_and_timing::movement_phase_state::stop_and_remain; // Red
            intersection_state_two.states.push_back(state_5);
            intersection_state_three.states.push_back(state_5);

            streets_tsc_configuration::signal_group_configuration tsc_config_5;
            int signal_group_5 = 5;
            tsc_config_5.red_clearance = 0;
            tsc_config_5.yellow_change_duration = 0;
            sg_yellow_duration_red_clearnace_map_ptr->insert({signal_group_5, tsc_config_5});

            signal_phase_and_timing::movement_state state_6;
            state_6.signal_group = 6;
            signal_phase_and_timing::movement_event event_6;
            event_6.event_state = signal_phase_and_timing::movement_phase_state::stop_and_remain; // Red
            event_6.timing.start_time = current_hour_in_tenths_secs;
            event_6.timing.min_end_time = current_hour_in_tenths_secs + 10;
            state_6.state_time_speed.push_back(event_6);
            intersection_state.states.push_back(state_6);
            intersection_state_three.states.push_back(state_6);

            state_6.state_time_speed.front().event_state = signal_phase_and_timing::movement_phase_state::protected_clearance; // Yellow
            intersection_state_two.states.push_back(state_6);

            streets_tsc_configuration::signal_group_configuration tsc_config_6;
            int signal_group_6 = 6;
            tsc_config_6.red_clearance = 0;
            tsc_config_6.yellow_change_duration = 0;
            sg_yellow_duration_red_clearnace_map_ptr->insert({signal_group_6, tsc_config_6});

            signal_phase_and_timing::movement_state state_7;
            state_7.signal_group = 7;
            signal_phase_and_timing::movement_event event_7;
            event_7.event_state = signal_phase_and_timing::movement_phase_state::stop_and_remain; // RED
            event_7.timing.start_time = current_hour_in_tenths_secs;
            event_7.timing.min_end_time = current_hour_in_tenths_secs + 10;
            state_7.state_time_speed.push_back(event_7);
            intersection_state.states.push_back(state_7);
            intersection_state_two.states.push_back(state_7);
            intersection_state_three.states.push_back(state_7);

            streets_tsc_configuration::signal_group_configuration tsc_config_7;
            int signal_group_7 = 7;
            tsc_config_7.red_clearance = 0;
            tsc_config_7.yellow_change_duration = 0;
            sg_yellow_duration_red_clearnace_map_ptr->insert({signal_group_7, tsc_config_7});

            signal_phase_and_timing::movement_state state_8;
            state_8.signal_group = 8;
            signal_phase_and_timing::movement_event event_8;
            event_8.event_state = signal_phase_and_timing::movement_phase_state::stop_and_remain; // Red
            event_8.timing.start_time = current_hour_in_tenths_secs;
            event_8.timing.min_end_time = current_hour_in_tenths_secs + 10;
            state_8.state_time_speed.push_back(event_8);
            intersection_state.states.push_back(state_8);
            intersection_state_two.states.push_back(state_8);
            intersection_state_three.states.push_back(state_8);

            streets_tsc_configuration::signal_group_configuration tsc_config_8;
            int signal_group_8 = 8;
            tsc_config_8.red_clearance = 0;
            tsc_config_8.yellow_change_duration = 0;
            sg_yellow_duration_red_clearnace_map_ptr->insert({signal_group_8, tsc_config_8});

            spat_msg_ptr->intersections.push_back(intersection_state);
            spat_msg_two_ptr->intersections.push_back(intersection_state_two);
            spat_msg_three_ptr->intersections.push_back(intersection_state_three);
        }

        /**
         * @brief Helper method to load vehicle status and intent updates into a vector
         * of strings using a filepath as a parameter.
         *
         * @param filepath to json file of vehicle status and intent updates for testing.
         * @return std::vector<std::string>
         */
        std::vector<std::string> load_vehicle_update(const std::string &filepath)
        {
            std::ifstream file(filepath);
            if (!file.is_open())
            {
                throw streets_vehicles::status_intent_processing_exception("Unable to open status and intent message update file " + filepath + " !");
            }
            // Add file contents to stream and parse stream into Document
            rapidjson::IStreamWrapper isw(file);
            rapidjson::Document doc;
            doc.ParseStream(isw);
            if (doc.HasParseError())
            {
                SPDLOG_ERROR("Error  : {0} Offset: {1} ", doc.GetParseError(), doc.GetErrorOffset());
                throw streets_vehicles::status_intent_processing_exception("Document parse error!");
            }
            file.close();
            std::vector<std::string> updates;
            rapidjson::StringBuffer buffer;
            rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
            if (doc.FindMember("vehicle_updates")->value.IsArray())
            {
                for (auto &update : doc.FindMember("vehicle_updates")->value.GetArray())
                {
                    if (update.IsObject())
                    {
                        buffer.Clear();
                        writer.Reset(buffer);
                        update.Accept(writer);
                        std::string up = buffer.GetString();
                        updates.push_back(up);
                    }
                }
                return updates;
            }
        }
    };

    TEST_F(test_streets_desired_phase_plan_arbitrator, select_optimal_dpp)
    {
        auto arbitrator = std::make_shared<streets_desired_phase_plan_arbitrator>();
        std::chrono::system_clock::time_point now = std::chrono::system_clock::now();
        std::chrono::milliseconds epochMs = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch());
        uint64_t epoch_timestamp = epochMs.count();

        std::vector<streets_desired_phase_plan::streets_desired_phase_plan> dpp_list;
        // Create mock desired phase plan list
        std::string streets_desired_phase_plan_str_1 = "{\"timestamp\":12121212121,\"desired_phase_plan\":[{\"signal_groups\":[1,5],\"start_time\":" + std::to_string(epoch_timestamp) + ",\"end_time\":" + std::to_string(epoch_timestamp + 10000) + "},{\"signal_groups\":[2,6],\"start_time\":" + std::to_string(epoch_timestamp + 10000) + ",\"end_time\":" + std::to_string(epoch_timestamp + 20000) + "},{\"signal_groups\":[3,7],\"start_time\":" + std::to_string(epoch_timestamp + 20000) + ",\"end_time\":" + std::to_string(epoch_timestamp + 30000) + "}]}";
        auto desired_phase_plan1_ptr = std::make_shared<streets_desired_phase_plan::streets_desired_phase_plan>();
        desired_phase_plan1_ptr->fromJson(streets_desired_phase_plan_str_1);

        std::string streets_desired_phase_plan_str_2 = "{\"timestamp\":12121212121,\"desired_phase_plan\":[{\"signal_groups\":[1,5],\"start_time\":" + std::to_string(epoch_timestamp) + ",\"end_time\":" + std::to_string(epoch_timestamp + 10000) + "},{\"signal_groups\":[2,6],\"start_time\":" + std::to_string(epoch_timestamp + 10000) + ",\"end_time\":" + std::to_string(epoch_timestamp + 20000) + "},{\"signal_groups\":[3,7],\"start_time\":" + std::to_string(epoch_timestamp + 20000) + ",\"end_time\":" + std::to_string(epoch_timestamp + 40000) + "}]}";
        auto desired_phase_plan2_ptr = std::make_shared<streets_desired_phase_plan::streets_desired_phase_plan>();
        desired_phase_plan2_ptr->fromJson(streets_desired_phase_plan_str_2);
        dpp_list.push_back(*desired_phase_plan1_ptr);
        dpp_list.push_back(*desired_phase_plan2_ptr);
        // Initialize empty vehicle list
        streets_vehicles::vehicle_list veh_list;

        // Initialize buffer params
        uint64_t initial_green_buffer = 1000;
        uint64_t final_green_buffer = 1000;

        // Current spat should only contain the ONLY one current movement event for each movement state.
        for (auto movement_state : spat_msg_ptr->intersections.front().states)
        {
            ASSERT_EQ(8, spat_msg_ptr->intersections.front().states.size());
            ASSERT_EQ(1, movement_state.state_time_speed.size());
        }

        try
        {
            arbitrator->select_optimal_dpp(dpp_list, intersection_info, spat_msg_ptr, sg_yellow_duration_red_clearnace_map_ptr, veh_list, initial_green_buffer, final_green_buffer);
        }
        catch (const streets_desired_phase_plan_arbitrator_exception &e)
        {
            ASSERT_STREQ(e.what(), "Vehicle schedule cannot be empty.");
        }

        // It should make a local copy and do not update the pass in spat message spat_msg_ptr
        for (auto movement_state : spat_msg_ptr->intersections.front().states)
        {
            ASSERT_EQ(8, spat_msg_ptr->intersections.front().states.size());
            ASSERT_EQ(1, movement_state.state_time_speed.size());
        }
    }

    TEST_F(test_streets_desired_phase_plan_arbitrator, update_spat_with_candidate_dpp)
    {
        auto arbitrator = std::make_shared<streets_desired_phase_plan_arbitrator>();
        std::string streets_desired_phase_plan_str_1 = "{\"timestamp\":12121212121,\"desired_phase_plan\":[{\"signal_groups\":[1,5],\"start_time\":1660747993,\"end_time\":1660757998},{\"signal_groups\":[2,6],\"start_time\":1660749993,\"end_time\":1660749098},{\"signal_groups\":[3,7],\"start_time\":1660750993,\"end_time\":1660750998},{\"signal_groups\":[4,8],\"start_time\":1660757993,\"end_time\":1660757998}]}";
        auto desired_phase_plan_ptr = std::make_shared<streets_desired_phase_plan::streets_desired_phase_plan>();
        desired_phase_plan_ptr->fromJson(streets_desired_phase_plan_str_1);
        auto invalid_spat_msg_ptr = std::make_shared<signal_phase_and_timing::spat>();
        ASSERT_TRUE(invalid_spat_msg_ptr->intersections.empty());
        signal_phase_and_timing::intersection_state intersection;
        invalid_spat_msg_ptr->intersections.push_back(intersection);
        ASSERT_TRUE(invalid_spat_msg_ptr->intersections.front().states.empty());
        arbitrator->update_spat_with_candidate_dpp(*invalid_spat_msg_ptr, *desired_phase_plan_ptr, sg_yellow_duration_red_clearnace_map_ptr);
        ASSERT_FALSE(invalid_spat_msg_ptr->intersections.empty());
        ASSERT_TRUE(invalid_spat_msg_ptr->intersections.front().states.empty());

        // Current spat should only contain the ONLY one current movement event for each movement state.
        for (auto movement_state : spat_msg_ptr->intersections.front().states)
        {
            ASSERT_EQ(8, spat_msg_ptr->intersections.front().states.size());
            ASSERT_EQ(1, movement_state.state_time_speed.size());
        }

        std::chrono::system_clock::time_point now = std::chrono::system_clock::now();
        std::chrono::milliseconds epochMs = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch());
        uint64_t epoch_timestamp = epochMs.count();

        // Process valid desired phase plan and update the desired phase plan for TSC service
        std::string streets_desired_phase_plan_str_2 = "{\"timestamp\":12121212121,\"desired_phase_plan\":[{\"signal_groups\":[1,5],\"start_time\":" + std::to_string(epoch_timestamp) + ",\"end_time\":" + std::to_string(epoch_timestamp + 10000) + "},{\"signal_groups\":[2,6],\"start_time\":" + std::to_string(epoch_timestamp + 10000) + ",\"end_time\":" + std::to_string(epoch_timestamp + 20000) + "},{\"signal_groups\":[3,7],\"start_time\":" + std::to_string(epoch_timestamp + 20000) + ",\"end_time\":" + std::to_string(epoch_timestamp + 30000) + "},{\"signal_groups\":[4,8],\"start_time\":" + std::to_string(epoch_timestamp + 30000) + ",\"end_time\":" + std::to_string(epoch_timestamp + 40000) + "}]}";
        auto desired_phase_plan2_ptr = std::make_shared<streets_desired_phase_plan::streets_desired_phase_plan>();
        desired_phase_plan2_ptr->fromJson(streets_desired_phase_plan_str_2);

        /****
         * START: Test Scenario one:  There are two green phases [1,5] in the current SPAT movement event.
         * ***/
        // Add future movement events
        arbitrator->update_spat_with_candidate_dpp(*spat_msg_ptr, *desired_phase_plan2_ptr, sg_yellow_duration_red_clearnace_map_ptr);

        // SPAT movement events should be updated with candidate desired phase plan
        for (auto movement_state : spat_msg_ptr->intersections.front().states)
        {
            ASSERT_EQ(8, spat_msg_ptr->intersections.front().states.size());
            ASSERT_TRUE(movement_state.state_time_speed.size() > 1);
        }
        for (auto movement_state : spat_msg_ptr->intersections.front().states)
        {
            int sg = (int)movement_state.signal_group;
            SPDLOG_DEBUG("\n");
            if (sg == 1 || sg == 5)
            {
                SPDLOG_DEBUG("current_hour_in_tenths_secs {0}", current_hour_in_tenths_secs);
                SPDLOG_DEBUG("signal group =  {0}", sg);
                SPDLOG_DEBUG("phase_state \t start_time \t min_end_time");
                for (auto movement_event : movement_state.state_time_speed)
                {
                    std::string state_name = "";
                    switch (movement_event.event_state)
                    {
                    case signal_phase_and_timing::movement_phase_state::stop_and_remain:
                        if (movement_event.timing.start_time == current_hour_in_tenths_secs)
                        {
                            ASSERT_EQ(current_hour_in_tenths_secs, movement_event.timing.start_time);
                            ASSERT_EQ(current_hour_in_tenths_secs, movement_event.timing.min_end_time); // start with red, and change red end time to green start time
                        }
                        else
                        {
                            ASSERT_EQ(current_hour_in_tenths_secs + 100, movement_event.timing.start_time);
                            ASSERT_EQ(current_hour_in_tenths_secs + 400, movement_event.timing.min_end_time); // last 30 secs
                        }

                        state_name = "red";
                        break;
                    case signal_phase_and_timing::movement_phase_state::protected_clearance:
                        ASSERT_EQ(current_hour_in_tenths_secs + 100, movement_event.timing.start_time);
                        ASSERT_EQ(current_hour_in_tenths_secs + 100, movement_event.timing.min_end_time); // last 0 secs as desired yellow duration and red clearance are initial 0.
                        state_name = "yellow";
                        break;
                    case signal_phase_and_timing::movement_phase_state::protected_movement_allowed:
                        ASSERT_EQ(current_hour_in_tenths_secs, movement_event.timing.start_time);
                        ASSERT_EQ(current_hour_in_tenths_secs + 100, movement_event.timing.min_end_time); // last 10 secs
                        state_name = "green";
                        break;

                    default:
                        break;
                    }
                    SPDLOG_DEBUG("{0} \t\t {1} \t\t {2}", state_name, movement_event.timing.start_time, movement_event.timing.min_end_time);
                }
            }
            else if (sg == 2 || sg == 6)
            {
                SPDLOG_DEBUG("current_hour_in_tenths_secs {0}", current_hour_in_tenths_secs);
                SPDLOG_DEBUG("signal group =  {0}", sg);
                SPDLOG_DEBUG("phase_state \t start_time \t min_end_time");
                for (auto movement_event : movement_state.state_time_speed)
                {
                    std::string state_name = "";
                    switch (movement_event.event_state)
                    {
                    case signal_phase_and_timing::movement_phase_state::stop_and_remain:
                        if (movement_event.timing.start_time == current_hour_in_tenths_secs)
                        {
                            ASSERT_EQ(current_hour_in_tenths_secs, movement_event.timing.start_time);
                            ASSERT_EQ(current_hour_in_tenths_secs + 100, movement_event.timing.min_end_time); // start with red, and change red end time to green start time
                        }
                        else
                        {
                            ASSERT_EQ(current_hour_in_tenths_secs + 200, movement_event.timing.start_time);
                            ASSERT_EQ(current_hour_in_tenths_secs + 400, movement_event.timing.min_end_time); // last 20 secs from the last red
                        }
                        state_name = "red";
                        break;
                    case signal_phase_and_timing::movement_phase_state::protected_clearance:
                        ASSERT_EQ(current_hour_in_tenths_secs + 200, movement_event.timing.start_time);
                        ASSERT_EQ(current_hour_in_tenths_secs + 200, movement_event.timing.min_end_time); // last 0 secs as desired yellow duration and red clearance are initial 0.
                        state_name = "yellow";
                        break;
                    case signal_phase_and_timing::movement_phase_state::protected_movement_allowed:
                        ASSERT_EQ(current_hour_in_tenths_secs + 100, movement_event.timing.start_time);
                        ASSERT_EQ(current_hour_in_tenths_secs + 200, movement_event.timing.min_end_time); // last 10 secs, but it starts with second desired green
                        state_name = "green";
                        break;

                    default:
                        break;
                    }
                    SPDLOG_DEBUG("{0} \t\t {1} \t\t {2}", state_name, movement_event.timing.start_time, movement_event.timing.min_end_time);
                }
            }
            else if (sg == 3)
            {
                SPDLOG_DEBUG("current_hour_in_tenths_secs {0}", current_hour_in_tenths_secs);
                SPDLOG_DEBUG("signal group =  {0}", sg);
                SPDLOG_DEBUG("phase_state \t start_time \t min_end_time");
                for (auto movement_event : movement_state.state_time_speed)
                {
                    std::string state_name = "";
                    switch (movement_event.event_state)
                    {
                    case signal_phase_and_timing::movement_phase_state::stop_and_remain:
                        if (movement_event.timing.start_time == current_hour_in_tenths_secs)
                        {
                            ASSERT_EQ(current_hour_in_tenths_secs, movement_event.timing.start_time);
                            ASSERT_EQ(current_hour_in_tenths_secs + 200, movement_event.timing.min_end_time); // start with red, and change red end time to green start time
                        }
                        else
                        {
                            ASSERT_EQ(current_hour_in_tenths_secs + 300, movement_event.timing.start_time);
                            ASSERT_EQ(current_hour_in_tenths_secs + 400, movement_event.timing.min_end_time); // last 10 secs from the last red.
                        }
                        state_name = "red";
                        break;
                    case signal_phase_and_timing::movement_phase_state::protected_clearance:
                        ASSERT_EQ(current_hour_in_tenths_secs + 300, movement_event.timing.start_time);
                        ASSERT_EQ(current_hour_in_tenths_secs + 300, movement_event.timing.min_end_time); // last 0 secs as desired yellow duration and red clearance are initial 0.
                        state_name = "yellow";
                        break;
                    case signal_phase_and_timing::movement_phase_state::protected_movement_allowed:
                        ASSERT_EQ(current_hour_in_tenths_secs + 200, movement_event.timing.start_time);
                        ASSERT_EQ(current_hour_in_tenths_secs + 300, movement_event.timing.min_end_time); // last 10 secs, but it starts with third desired green
                        state_name = "green";
                        break;

                    default:
                        break;
                    }
                    SPDLOG_DEBUG("{0} \t\t {1} \t\t {2}", state_name, movement_event.timing.start_time, movement_event.timing.min_end_time);
                }
            }
            else if (sg == 4 || sg == 8)
            {
                SPDLOG_DEBUG("current_hour_in_tenths_secs {0}", current_hour_in_tenths_secs);
                SPDLOG_DEBUG("signal group =  {0}", sg);
                SPDLOG_DEBUG("phase_state \t start_time \t min_end_time");
                for (auto movement_event : movement_state.state_time_speed)
                {
                    std::string state_name = "";
                    switch (movement_event.event_state)
                    {
                    case signal_phase_and_timing::movement_phase_state::stop_and_remain:
                        if (movement_event.timing.start_time == current_hour_in_tenths_secs)
                        {
                            ASSERT_EQ(current_hour_in_tenths_secs, movement_event.timing.start_time);
                            ASSERT_EQ(current_hour_in_tenths_secs + 300, movement_event.timing.min_end_time); // start with red, and change red end time to green start time
                        }
                        else
                        {
                            ASSERT_EQ(current_hour_in_tenths_secs + 400, movement_event.timing.start_time);
                            ASSERT_EQ(current_hour_in_tenths_secs + 400, movement_event.timing.min_end_time); // last 10 secs from the last red
                        }
                        state_name = "red";
                        break;
                    case signal_phase_and_timing::movement_phase_state::protected_clearance:
                        ASSERT_EQ(current_hour_in_tenths_secs + 400, movement_event.timing.start_time);
                        ASSERT_EQ(current_hour_in_tenths_secs + 400, movement_event.timing.min_end_time); // last 0 secs as desired yellow duration and red clearance are initial 0.
                        state_name = "yellow";
                        break;
                    case signal_phase_and_timing::movement_phase_state::protected_movement_allowed:
                        ASSERT_EQ(current_hour_in_tenths_secs + 300, movement_event.timing.start_time);
                        ASSERT_EQ(current_hour_in_tenths_secs + 400, movement_event.timing.min_end_time); // last 10 secs, but it starts with fourth desired green
                        state_name = "green";
                        break;

                    default:
                        break;
                    }
                    SPDLOG_DEBUG("{0} \t\t {1} \t\t {2}", state_name, movement_event.timing.start_time, movement_event.timing.min_end_time);
                }
            }
        }
        /****
         * END: Test Scenario one
         * ***/

        // Current spat should only contain the ONLY one current movement event for each movement state.
        /****
         * START: Test Scenario two: There are two Yellow phases [1,5] in the current SPAT movement event
         * ***/
        // Add future movement events
        arbitrator->update_spat_with_candidate_dpp(*spat_msg_two_ptr, *desired_phase_plan2_ptr, sg_yellow_duration_red_clearnace_map_ptr);
        for (auto movement_state : spat_msg_two_ptr->intersections.front().states)
        {
            int sg = (int)movement_state.signal_group;
            SPDLOG_DEBUG("\n");
            if (sg == 1 || sg == 5)
            {
                SPDLOG_DEBUG("current_hour_in_tenths_secs {0}", current_hour_in_tenths_secs);
                SPDLOG_DEBUG("signal group =  {0}", sg);
                SPDLOG_DEBUG("phase_state \t start_time \t min_end_time");
                for (auto movement_event : movement_state.state_time_speed)
                {
                    std::string state_name = "";
                    switch (movement_event.event_state)
                    {
                    case signal_phase_and_timing::movement_phase_state::stop_and_remain:
                        if (movement_event.timing.start_time == current_hour_in_tenths_secs)
                        {
                            ASSERT_EQ(current_hour_in_tenths_secs, movement_event.timing.start_time);
                            ASSERT_EQ(current_hour_in_tenths_secs, movement_event.timing.min_end_time); // start with red, and change red end time to green start time
                        }
                        else
                        {
                            ASSERT_EQ(current_hour_in_tenths_secs + 100, movement_event.timing.start_time);
                            ASSERT_EQ(current_hour_in_tenths_secs + 400, movement_event.timing.min_end_time); // last 30 secs
                        }
                        state_name = "red";
                        break;
                    case signal_phase_and_timing::movement_phase_state::protected_clearance:
                        ASSERT_EQ(current_hour_in_tenths_secs + 100, movement_event.timing.start_time);
                        ASSERT_EQ(current_hour_in_tenths_secs + 100, movement_event.timing.min_end_time); // last 0 secs as desired yellow duration and red clearance are initial 0.
                        state_name = "yellow";
                        break;
                    case signal_phase_and_timing::movement_phase_state::protected_movement_allowed:
                        ASSERT_EQ(current_hour_in_tenths_secs, movement_event.timing.start_time);
                        ASSERT_EQ(current_hour_in_tenths_secs + 100, movement_event.timing.min_end_time); // last 10 secs
                        state_name = "green";
                        break;

                    default:
                        break;
                    }
                    SPDLOG_DEBUG("{0} \t\t {1} \t\t {2}", state_name, movement_event.timing.start_time, movement_event.timing.min_end_time);
                }
            }
            else if (sg == 2 || sg == 6)
            {
                SPDLOG_DEBUG("current_hour_in_tenths_secs {0}", current_hour_in_tenths_secs);
                SPDLOG_DEBUG("signal group =  {0}", sg);
                SPDLOG_DEBUG("phase_state \t start_time \t min_end_time");
                for (auto movement_event : movement_state.state_time_speed)
                {
                    std::string state_name = "";
                    switch (movement_event.event_state)
                    {
                    case signal_phase_and_timing::movement_phase_state::stop_and_remain: // Red
                        if (movement_event.timing.start_time == current_hour_in_tenths_secs)
                        {
                            ASSERT_EQ(current_hour_in_tenths_secs, movement_event.timing.start_time);
                            ASSERT_EQ(current_hour_in_tenths_secs + 100, movement_event.timing.min_end_time); // start with yellow (current yellow duration intial =0), and change red start time = yellow start time and end time to green start time
                        }
                        else
                        {
                            ASSERT_EQ(current_hour_in_tenths_secs + 200, movement_event.timing.start_time);
                            ASSERT_EQ(current_hour_in_tenths_secs + 400, movement_event.timing.min_end_time); // last 20 secs from the last red
                        }
                        state_name = "red";
                        break;
                    case signal_phase_and_timing::movement_phase_state::protected_clearance: // Yellow
                        if (movement_event.timing.start_time == current_hour_in_tenths_secs)
                        {
                            ASSERT_EQ(current_hour_in_tenths_secs, movement_event.timing.start_time);
                            ASSERT_EQ(current_hour_in_tenths_secs, movement_event.timing.min_end_time); // last 0 secs as current yellow duration is initial 0.
                        }
                        else
                        {
                            ASSERT_EQ(current_hour_in_tenths_secs + 200, movement_event.timing.start_time);
                            ASSERT_EQ(current_hour_in_tenths_secs + 200, movement_event.timing.min_end_time); // last 0 secs as current yellow duration is initial 0.
                        }
                        state_name = "yellow";
                        break;
                    case signal_phase_and_timing::movement_phase_state::protected_movement_allowed: // Green
                        ASSERT_EQ(current_hour_in_tenths_secs + 100, movement_event.timing.start_time);
                        ASSERT_EQ(current_hour_in_tenths_secs + 200, movement_event.timing.min_end_time); // last 10 secs, but it starts with second desired green
                        state_name = "green";
                        break;

                    default:
                        break;
                    }
                    SPDLOG_DEBUG("{0} \t\t {1} \t\t {2}", state_name, movement_event.timing.start_time, movement_event.timing.min_end_time);
                }
            }
            else if (sg == 3)
            {
                SPDLOG_DEBUG("current_hour_in_tenths_secs {0}", current_hour_in_tenths_secs);
                SPDLOG_DEBUG("signal group =  {0}", sg);
                SPDLOG_DEBUG("phase_state \t start_time \t min_end_time");
                for (auto movement_event : movement_state.state_time_speed)
                {
                    std::string state_name = "";
                    switch (movement_event.event_state)
                    {
                    case signal_phase_and_timing::movement_phase_state::stop_and_remain:
                        if (movement_event.timing.start_time == current_hour_in_tenths_secs)
                        {
                            ASSERT_EQ(current_hour_in_tenths_secs, movement_event.timing.start_time);
                            ASSERT_EQ(current_hour_in_tenths_secs + 200, movement_event.timing.min_end_time); // start with red, and change red end time to green start time
                        }
                        else
                        {
                            ASSERT_EQ(current_hour_in_tenths_secs + 300, movement_event.timing.start_time);
                            ASSERT_EQ(current_hour_in_tenths_secs + 400, movement_event.timing.min_end_time); // last 10 secs from the last red.
                        }
                        state_name = "red";
                        break;
                    case signal_phase_and_timing::movement_phase_state::protected_clearance:
                        ASSERT_EQ(current_hour_in_tenths_secs + 300, movement_event.timing.start_time);
                        ASSERT_EQ(current_hour_in_tenths_secs + 300, movement_event.timing.min_end_time); // last 0 secs as desired yellow duration and red clearance are initial 0.
                        state_name = "yellow";
                        break;
                    case signal_phase_and_timing::movement_phase_state::protected_movement_allowed:
                        ASSERT_EQ(current_hour_in_tenths_secs + 200, movement_event.timing.start_time);
                        ASSERT_EQ(current_hour_in_tenths_secs + 300, movement_event.timing.min_end_time); // last 10 secs, but it starts with third desired green
                        state_name = "green";
                        break;

                    default:
                        break;
                    }
                    SPDLOG_DEBUG("{0} \t\t {1} \t\t {2}", state_name, movement_event.timing.start_time, movement_event.timing.min_end_time);
                }
            }
            else if (sg == 4 || sg == 8)
            {
                SPDLOG_DEBUG("current_hour_in_tenths_secs {0}", current_hour_in_tenths_secs);
                SPDLOG_DEBUG("signal group =  {0}", sg);
                SPDLOG_DEBUG("phase_state \t start_time \t min_end_time");
                for (auto movement_event : movement_state.state_time_speed)
                {
                    std::string state_name = "";
                    switch (movement_event.event_state)
                    {
                    case signal_phase_and_timing::movement_phase_state::stop_and_remain:
                        if (movement_event.timing.start_time == current_hour_in_tenths_secs)
                        {
                            ASSERT_EQ(current_hour_in_tenths_secs, movement_event.timing.start_time);
                            ASSERT_EQ(current_hour_in_tenths_secs + 300, movement_event.timing.min_end_time); // start with red, and change red end time to green start time
                        }
                        else
                        {
                            ASSERT_EQ(current_hour_in_tenths_secs + 400, movement_event.timing.start_time);
                            ASSERT_EQ(current_hour_in_tenths_secs + 400, movement_event.timing.min_end_time); // last 10 secs from the last red
                        }
                        state_name = "red";
                        break;
                    case signal_phase_and_timing::movement_phase_state::protected_clearance:
                        ASSERT_EQ(current_hour_in_tenths_secs + 400, movement_event.timing.start_time);
                        ASSERT_EQ(current_hour_in_tenths_secs + 400, movement_event.timing.min_end_time); // last 0 secs as desired yellow duration and red clearance are initial 0.
                        state_name = "yellow";
                        break;
                    case signal_phase_and_timing::movement_phase_state::protected_movement_allowed:
                        ASSERT_EQ(current_hour_in_tenths_secs + 300, movement_event.timing.start_time);
                        ASSERT_EQ(current_hour_in_tenths_secs + 400, movement_event.timing.min_end_time); // last 10 secs, but it starts with fourth desired green
                        state_name = "green";
                        break;

                    default:
                        break;
                    }
                    SPDLOG_DEBUG("{0} \t\t {1} \t\t {2}", state_name, movement_event.timing.start_time, movement_event.timing.min_end_time);
                }
            }
        }
        /****
         * END: Test Scenario two
         * ***/

        // Current spat should only contain the ONLY one current movement event for each movement state.
        /****
         * START: Test Scenario three: They are all Red in the current SPAT movement events
         * ***/
        // Add future movement events

        arbitrator->update_spat_with_candidate_dpp(*spat_msg_three_ptr, *desired_phase_plan2_ptr, sg_yellow_duration_red_clearnace_map_ptr);
        for (auto movement_state : spat_msg_three_ptr->intersections.front().states)
        {
            int sg = (int)movement_state.signal_group;
            SPDLOG_DEBUG("\n");
            if (sg == 1 || sg == 5)
            {
                SPDLOG_DEBUG("current_hour_in_tenths_secs {0}", current_hour_in_tenths_secs);
                SPDLOG_DEBUG("signal group =  {0}", sg);
                SPDLOG_DEBUG("phase_state \t start_time \t min_end_time");
                for (auto movement_event : movement_state.state_time_speed)
                {
                    std::string state_name = "";
                    switch (movement_event.event_state)
                    {
                    case signal_phase_and_timing::movement_phase_state::stop_and_remain: // Red
                        if (movement_event.timing.start_time == current_hour_in_tenths_secs)
                        {
                            ASSERT_EQ(current_hour_in_tenths_secs, movement_event.timing.start_time);
                            ASSERT_EQ(current_hour_in_tenths_secs, movement_event.timing.min_end_time); // start with red, and change red end time to green start time
                        }
                        else
                        {
                            ASSERT_EQ(current_hour_in_tenths_secs + 100, movement_event.timing.start_time);
                            ASSERT_EQ(current_hour_in_tenths_secs + 400, movement_event.timing.min_end_time); // last 30 secs
                        }
                        state_name = "red";
                        break;
                    case signal_phase_and_timing::movement_phase_state::protected_clearance: // Yellow
                        ASSERT_EQ(current_hour_in_tenths_secs + 100, movement_event.timing.start_time);
                        ASSERT_EQ(current_hour_in_tenths_secs + 100, movement_event.timing.min_end_time); // last 0 secs as desired yellow duration and red clearance are initial 0.
                        state_name = "yellow";
                        break;
                    case signal_phase_and_timing::movement_phase_state::protected_movement_allowed: // Green
                        ASSERT_EQ(current_hour_in_tenths_secs, movement_event.timing.start_time);
                        ASSERT_EQ(current_hour_in_tenths_secs + 100, movement_event.timing.min_end_time); // last 10 secs
                        state_name = "green";
                        break;

                    default:
                        break;
                    }
                    SPDLOG_DEBUG("{0} \t\t {1} \t\t {2}", state_name, movement_event.timing.start_time, movement_event.timing.min_end_time);
                }
            }
            else if (sg == 2 || sg == 6)
            {
                SPDLOG_DEBUG("current_hour_in_tenths_secs {0}", current_hour_in_tenths_secs);
                SPDLOG_DEBUG("signal group =  {0}", sg);
                SPDLOG_DEBUG("phase_state \t start_time \t min_end_time");
                for (auto movement_event : movement_state.state_time_speed)
                {
                    std::string state_name = "";
                    switch (movement_event.event_state)
                    {
                    case signal_phase_and_timing::movement_phase_state::stop_and_remain: // Red
                        if (movement_event.timing.start_time == current_hour_in_tenths_secs)
                        {
                            ASSERT_EQ(current_hour_in_tenths_secs, movement_event.timing.start_time);
                            ASSERT_EQ(current_hour_in_tenths_secs + 100, movement_event.timing.min_end_time); // start with yellow (current yellow duration intial =0), and change red start time = yellow start time and end time to green start time
                        }
                        else
                        {
                            ASSERT_EQ(current_hour_in_tenths_secs + 200, movement_event.timing.start_time);
                            ASSERT_EQ(current_hour_in_tenths_secs + 400, movement_event.timing.min_end_time); // last 20 secs from the last red
                        }
                        state_name = "red";
                        break;
                    case signal_phase_and_timing::movement_phase_state::protected_clearance: // Yellow
                        if (movement_event.timing.start_time == current_hour_in_tenths_secs)
                        {
                            ASSERT_EQ(current_hour_in_tenths_secs, movement_event.timing.start_time);
                            ASSERT_EQ(current_hour_in_tenths_secs, movement_event.timing.min_end_time); // last 0 secs as current yellow duration is initial 0.
                        }
                        else
                        {
                            ASSERT_EQ(current_hour_in_tenths_secs + 200, movement_event.timing.start_time);
                            ASSERT_EQ(current_hour_in_tenths_secs + 200, movement_event.timing.min_end_time); // last 0 secs as current yellow duration is initial 0.
                        }
                        state_name = "yellow";
                        break;
                    case signal_phase_and_timing::movement_phase_state::protected_movement_allowed: // Green
                        ASSERT_EQ(current_hour_in_tenths_secs + 100, movement_event.timing.start_time);
                        ASSERT_EQ(current_hour_in_tenths_secs + 200, movement_event.timing.min_end_time); // last 10 secs, but it starts with second desired green
                        state_name = "green";
                        break;

                    default:
                        break;
                    }
                    SPDLOG_DEBUG("{0} \t\t {1} \t\t {2}", state_name, movement_event.timing.start_time, movement_event.timing.min_end_time);
                }
            }
            else if (sg == 3)
            {
                SPDLOG_DEBUG("current_hour_in_tenths_secs {0}", current_hour_in_tenths_secs);
                SPDLOG_DEBUG("signal group =  {0}", sg);
                SPDLOG_DEBUG("phase_state \t start_time \t min_end_time");
                for (auto movement_event : movement_state.state_time_speed)
                {
                    std::string state_name = "";
                    switch (movement_event.event_state)
                    {
                    case signal_phase_and_timing::movement_phase_state::stop_and_remain:
                        if (movement_event.timing.start_time == current_hour_in_tenths_secs)
                        {
                            ASSERT_EQ(current_hour_in_tenths_secs, movement_event.timing.start_time);
                            ASSERT_EQ(current_hour_in_tenths_secs + 200, movement_event.timing.min_end_time); // start with red, and change red end time to green start time
                        }
                        else
                        {
                            ASSERT_EQ(current_hour_in_tenths_secs + 300, movement_event.timing.start_time);
                            ASSERT_EQ(current_hour_in_tenths_secs + 400, movement_event.timing.min_end_time); // last 10 secs from the last red.
                        }
                        state_name = "red";
                        break;
                    case signal_phase_and_timing::movement_phase_state::protected_clearance:
                        ASSERT_EQ(current_hour_in_tenths_secs + 300, movement_event.timing.start_time);
                        ASSERT_EQ(current_hour_in_tenths_secs + 300, movement_event.timing.min_end_time); // last 0 secs as desired yellow duration and red clearance are initial 0.
                        state_name = "yellow";
                        break;
                    case signal_phase_and_timing::movement_phase_state::protected_movement_allowed:
                        ASSERT_EQ(current_hour_in_tenths_secs + 200, movement_event.timing.start_time);
                        ASSERT_EQ(current_hour_in_tenths_secs + 300, movement_event.timing.min_end_time); // last 10 secs, but it starts with third desired green
                        state_name = "green";
                        break;

                    default:
                        break;
                    }
                    SPDLOG_DEBUG("{0} \t\t {1} \t\t {2}", state_name, movement_event.timing.start_time, movement_event.timing.min_end_time);
                }
            }
            else if (sg == 4 || sg == 8)
            {
                SPDLOG_DEBUG("current_hour_in_tenths_secs {0}", current_hour_in_tenths_secs);
                SPDLOG_DEBUG("signal group =  {0}", sg);
                SPDLOG_DEBUG("phase_state \t start_time \t min_end_time");
                for (auto movement_event : movement_state.state_time_speed)
                {
                    std::string state_name = "";
                    switch (movement_event.event_state)
                    {
                    case signal_phase_and_timing::movement_phase_state::stop_and_remain:
                        if (movement_event.timing.start_time == current_hour_in_tenths_secs)
                        {
                            ASSERT_EQ(current_hour_in_tenths_secs, movement_event.timing.start_time);
                            ASSERT_EQ(current_hour_in_tenths_secs + 300, movement_event.timing.min_end_time); // start with red, and change red end time to green start time
                        }
                        else
                        {
                            ASSERT_EQ(current_hour_in_tenths_secs + 400, movement_event.timing.start_time);
                            ASSERT_EQ(current_hour_in_tenths_secs + 400, movement_event.timing.min_end_time); // last 10 secs from the last red
                        }
                        state_name = "red";
                        break;
                    case signal_phase_and_timing::movement_phase_state::protected_clearance:
                        ASSERT_EQ(current_hour_in_tenths_secs + 400, movement_event.timing.start_time);
                        ASSERT_EQ(current_hour_in_tenths_secs + 400, movement_event.timing.min_end_time); // last 0 secs as desired yellow duration and red clearance are initial 0.
                        state_name = "yellow";
                        break;
                    case signal_phase_and_timing::movement_phase_state::protected_movement_allowed:
                        ASSERT_EQ(current_hour_in_tenths_secs + 300, movement_event.timing.start_time);
                        ASSERT_EQ(current_hour_in_tenths_secs + 400, movement_event.timing.min_end_time); // last 10 secs, but it starts with fourth desired green
                        state_name = "green";
                        break;

                    default:
                        break;
                    }
                    SPDLOG_DEBUG("{0} \t\t {1} \t\t {2}", state_name, movement_event.timing.start_time, movement_event.timing.min_end_time);
                }
            }
        }
        /****
         * END: Test Scenario three
         * ***/
    }

    TEST_F(test_streets_desired_phase_plan_arbitrator, calculate_vehicle_schedules)
    {
        auto arbitrator = std::make_shared<streets_desired_phase_plan_arbitrator>();
        auto schedule_ptr = std::make_shared<streets_vehicle_scheduler::signalized_intersection_schedule>();
        // Initialize vehicle list
        streets_vehicles::vehicle_list veh_list;
        // Initialize buffer params
        uint64_t initial_green_buffer = 2000;
        uint64_t final_green_buffer = 2000;
        try
        {
            arbitrator->calculate_vehicle_schedules(schedule_ptr, *spat_msg_ptr, veh_list, intersection_info, initial_green_buffer, final_green_buffer);
        }
        catch (const streets_desired_phase_plan_arbitrator_exception &e)
        {
            ASSERT_STREQ(e.what(), "Vehicle schedule cannot be empty.");
        }
        // Schedule should be empty is thre is no vehicle.
        ASSERT_EQ(schedule_ptr->vehicle_schedules.size(), 0);

        // Load Vehicle Update
        std::vector<std::string> updates = load_vehicle_update("../test/test_data/updates_signalized.json");

        // Update vehicle list
        streets_vehicles::vehicle veh;

        // Set signalized_status_intent_processor processor for vehicle ist
        veh_list.set_processor(std::make_shared<streets_vehicles::signalized_status_intent_processor>());
        auto processor = std::dynamic_pointer_cast<streets_vehicles::signalized_status_intent_processor>(veh_list.get_processor());
        processor->set_stopping_distance(1.0);
        processor->set_stopping_speed(0.1);
        veh_list.get_processor()->set_timeout(3.154e11);

        // Print timeout in days.
        SPDLOG_DEBUG("Set timeout to {0} days !", veh_list.get_processor()->get_timeout() / (1000 * 60 * 60 * 24));
        for (auto &update : updates)
        {
            SPDLOG_DEBUG("Processing Update {0} ", update);
            veh_list.process_update(update);
        }
        SPDLOG_INFO("Processed all updates!");
        ASSERT_EQ(veh_list.get_vehicles().size(), 2);
        arbitrator->calculate_vehicle_schedules(schedule_ptr, *spat_msg_ptr, veh_list, intersection_info, initial_green_buffer, final_green_buffer);
        // Schedule should updated as there is vehicle.
        // ASSERT_EQ(2, schedule_ptr->vehicle_schedules.size());
    }

    TEST_F(test_streets_desired_phase_plan_arbitrator, calculate_delay_measure)
    {
        auto arbitrator = std::make_shared<streets_desired_phase_plan_arbitrator>();
        auto schedule_ptr = std::make_shared<streets_vehicle_scheduler::signalized_intersection_schedule>();
        std::chrono::system_clock::time_point now = std::chrono::system_clock::now();
        std::chrono::milliseconds epochMs = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch());
        uint64_t epoch_timestamp = epochMs.count();

        // Create mock desired phase plan list
        std::vector<streets_desired_phase_plan::streets_desired_phase_plan> dpp_list;
        std::string streets_desired_phase_plan_str_1 = "{\"timestamp\":12121212121,\"desired_phase_plan\":[{\"signal_groups\":[1,5],\"start_time\":" + std::to_string(epoch_timestamp) + ",\"end_time\":" + std::to_string(epoch_timestamp + 10000) + "},{\"signal_groups\":[2,6],\"start_time\":" + std::to_string(epoch_timestamp + 10000) + ",\"end_time\":" + std::to_string(epoch_timestamp + 20000) + "},{\"signal_groups\":[3,7],\"start_time\":" + std::to_string(epoch_timestamp + 20000) + ",\"end_time\":" + std::to_string(epoch_timestamp + 30000) + "}]}";
        auto desired_phase_plan1_ptr = std::make_shared<streets_desired_phase_plan::streets_desired_phase_plan>();
        desired_phase_plan1_ptr->fromJson(streets_desired_phase_plan_str_1);

        std::string streets_desired_phase_plan_str_2 = "{\"timestamp\":12121212121,\"desired_phase_plan\":[{\"signal_groups\":[1,5],\"start_time\":" + std::to_string(epoch_timestamp) + ",\"end_time\":" + std::to_string(epoch_timestamp + 10000) + "},{\"signal_groups\":[2,6],\"start_time\":" + std::to_string(epoch_timestamp + 10000) + ",\"end_time\":" + std::to_string(epoch_timestamp + 20000) + "},{\"signal_groups\":[3,7],\"start_time\":" + std::to_string(epoch_timestamp + 20000) + ",\"end_time\":" + std::to_string(epoch_timestamp + 40000) + "}]}";
        auto desired_phase_plan2_ptr = std::make_shared<streets_desired_phase_plan::streets_desired_phase_plan>();
        desired_phase_plan2_ptr->fromJson(streets_desired_phase_plan_str_2);
        dpp_list.push_back(*desired_phase_plan1_ptr);
        dpp_list.push_back(*desired_phase_plan2_ptr);

        streets_vehicle_scheduler::signalized_vehicle_schedule vehicle_schedule_1;
        vehicle_schedule_1.v_id = "DOT-111";
        vehicle_schedule_1.et = epoch_timestamp + 20000;
        vehicle_schedule_1.eet = epoch_timestamp + 10000;
        schedule_ptr->vehicle_schedules.push_back(vehicle_schedule_1);

        streets_vehicle_scheduler::signalized_vehicle_schedule vehicle_schedule_2;
        vehicle_schedule_2.v_id = "DOT-112";
        vehicle_schedule_2.et = epoch_timestamp + 30000;
        vehicle_schedule_2.eet = epoch_timestamp + 20000;
        schedule_ptr->vehicle_schedules.push_back(vehicle_schedule_2);
        int desired_phase_plan_index = 0;
        float delay_measure_1 = arbitrator->calculate_delay_measure(schedule_ptr, dpp_list[desired_phase_plan_index]);
        desired_phase_plan_index++;
        ASSERT_EQ(1, delay_measure_1);
        float delay_measure_2 = arbitrator->calculate_delay_measure(schedule_ptr, dpp_list[desired_phase_plan_index]);
        ASSERT_EQ(20000, delay_measure_2);
    }

    TEST_F(test_streets_desired_phase_plan_arbitrator, identify_ddp_by_delay_measures)
    {
        auto arbitrator = std::make_shared<streets_desired_phase_plan_arbitrator>();
        auto schedule_ptr = std::make_shared<streets_vehicle_scheduler::signalized_intersection_schedule>();
        std::chrono::system_clock::time_point now = std::chrono::system_clock::now();
        std::chrono::milliseconds epochMs = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch());
        uint64_t epoch_timestamp = epochMs.count();

        // Create mock desired phase plan list
        std::vector<streets_desired_phase_plan::streets_desired_phase_plan> dpp_list;
        std::string streets_desired_phase_plan_str_1 = "{\"timestamp\":1111111111,\"desired_phase_plan\":[{\"signal_groups\":[1,5],\"start_time\":" + std::to_string(epoch_timestamp) + ",\"end_time\":" + std::to_string(epoch_timestamp + 10000) + "},{\"signal_groups\":[2,6],\"start_time\":" + std::to_string(epoch_timestamp + 10000) + ",\"end_time\":" + std::to_string(epoch_timestamp + 20000) + "},{\"signal_groups\":[3,7],\"start_time\":" + std::to_string(epoch_timestamp + 20000) + ",\"end_time\":" + std::to_string(epoch_timestamp + 30000) + "}]}";
        auto desired_phase_plan1_ptr = std::make_shared<streets_desired_phase_plan::streets_desired_phase_plan>();
        desired_phase_plan1_ptr->fromJson(streets_desired_phase_plan_str_1);

        std::string streets_desired_phase_plan_str_2 = "{\"timestamp\":2222222222,\"desired_phase_plan\":[{\"signal_groups\":[1,5],\"start_time\":" + std::to_string(epoch_timestamp) + ",\"end_time\":" + std::to_string(epoch_timestamp + 10000) + "},{\"signal_groups\":[2,6],\"start_time\":" + std::to_string(epoch_timestamp + 10000) + ",\"end_time\":" + std::to_string(epoch_timestamp + 20000) + "},{\"signal_groups\":[3,7],\"start_time\":" + std::to_string(epoch_timestamp + 20000) + ",\"end_time\":" + std::to_string(epoch_timestamp + 40000) + "}]}";
        auto desired_phase_plan2_ptr = std::make_shared<streets_desired_phase_plan::streets_desired_phase_plan>();
        desired_phase_plan2_ptr->fromJson(streets_desired_phase_plan_str_2);
        dpp_list.push_back(*desired_phase_plan1_ptr);
        dpp_list.push_back(*desired_phase_plan2_ptr);

        streets_vehicle_scheduler::signalized_vehicle_schedule vehicle_schedule_1;
        vehicle_schedule_1.v_id = "DOT-111";
        vehicle_schedule_1.et = epoch_timestamp + 20000;
        vehicle_schedule_1.eet = epoch_timestamp + 10000;
        schedule_ptr->vehicle_schedules.push_back(vehicle_schedule_1);

        streets_vehicle_scheduler::signalized_vehicle_schedule vehicle_schedule_2;
        vehicle_schedule_2.v_id = "DOT-112";
        vehicle_schedule_2.et = epoch_timestamp + 30000;
        vehicle_schedule_2.eet = epoch_timestamp + 20000;
        schedule_ptr->vehicle_schedules.push_back(vehicle_schedule_2);

        //Calculate delay measures
        int desired_phase_plan_index = 0;
        std::unordered_map<int, float> ddp_index_delay_measure_mappings;
        float delay_measure_1 = arbitrator->calculate_delay_measure(schedule_ptr, dpp_list[desired_phase_plan_index]);
        ASSERT_EQ(1, delay_measure_1);
        ddp_index_delay_measure_mappings.insert({desired_phase_plan_index, delay_measure_1});
        desired_phase_plan_index++;

        float delay_measure_2 = arbitrator->calculate_delay_measure(schedule_ptr, dpp_list[desired_phase_plan_index]);
        ASSERT_EQ(20000, delay_measure_2);
        ddp_index_delay_measure_mappings.insert({desired_phase_plan_index, delay_measure_2});

        //Identify highest delay measures and find final desired phase plan
        auto final_dpp = arbitrator->identify_ddp_by_delay_measures(dpp_list, ddp_index_delay_measure_mappings);        
        if (delay_measure_1 >= delay_measure_2)
        {
            ASSERT_EQ(streets_desired_phase_plan_str_1, final_dpp.toJson());
        }
        else
        {
            ASSERT_EQ(streets_desired_phase_plan_str_2, final_dpp.toJson());
        }
    }

}