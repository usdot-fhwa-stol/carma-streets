#include <gtest/gtest.h>
#include <spdlog/spdlog.h>

#include "signal_opt_processing_worker.h"
#include "signalized_status_intent_processor.h"
namespace signal_opt_service
{

    class test_signal_opt_processing_worker : public ::testing::Test
    {
    public:
        std::shared_ptr<signal_phase_and_timing::spat> spat_msg_ptr;
        std::shared_ptr<signal_phase_and_timing::spat> spat_msg_two_ptr;
        std::shared_ptr<signal_phase_and_timing::spat> spat_msg_three_ptr;
        std::shared_ptr<streets_tsc_configuration::tsc_configuration_state> tsc_state;
        uint16_t current_hour_in_tenths_secs;
        std::shared_ptr<OpenAPI::OAIIntersection_info> intersection_info;

    protected:
        void SetUp() override
        {
            tsc_state = std::make_shared<streets_tsc_configuration::tsc_configuration_state>();
            std::chrono::system_clock::time_point now = std::chrono::system_clock::now();
            std::chrono::milliseconds epochMs = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch());
            uint64_t epoch_timestamp = epochMs.count();

            // Create mock intersection info
            std::string json_info = "{\"departure_lanelets\":[{ \"id\":11, \"length\":41.609524398391133, \"speed_limit\":11.176}, { \"id\":12, \"length\":189.445653026013676, \"speed_limit\":11.176 }, { \"id\":13, \"length\":34.130869420842046, \"speed_limit\":11.176 }, { \"id\":14, \"length\":50.123213235343123, \"speed_limit\":11.176 }], \"entry_lanelets\":[{ \"id\":1, \"length\":195.73023157287864, \"speed_limit\":8.0, \"connecting_lanelet_ids\": [21, 22] }, { \"id\":2, \"length\":34.13086941117643, \"speed_limit\":8.0, \"connecting_lanelet_ids\": [23] }, { \"id\":3, \"length\":41.60952435603712, \"speed_limit\":8.0 , \"connecting_lanelet_ids\": [24, 25]}, { \"id\":4, \"length\":53.19846216254821, \"speed_limit\":8.0 , \"connecting_lanelet_ids\": [26]}, { \"id\":5, \"length\":58.19846216254821, \"speed_limit\":8.0 , \"connecting_lanelet_ids\": [27, 28]}, { \"id\":6, \"length\":57.19846216254821, \"speed_limit\":8.0 , \"connecting_lanelet_ids\": [29]}, { \"id\":7, \"length\":63.19846216254821, \"speed_limit\":8.0 , \"connecting_lanelet_ids\": [30, 31]}, { \"id\":8, \"length\":46.19846216254821, \"speed_limit\":8.0 , \"connecting_lanelet_ids\": [32]}], \"id\":9001, \"link_lanelets\":[{ \"conflict_lanelet_ids\":[24, 26, 29, 30, 31, 32], \"id\":21, \"length\":15.85409574709938, \"speed_limit\":8.0, \"signal_group_id\":1 }, { \"conflict_lanelet_ids\":[24, 29], \"id\":22, \"length\":16.79638865895223, \"speed_limit\":4.4, \"signal_group_id\":1 }, { \"conflict_lanelet_ids\":[24, 26, 27, 28, 30, 32], \"id\":23, \"length\":16.04307702855403, \"speed_limit\":8.0, \"signal_group_id\":6 }, { \"conflict_lanelet_ids\":[21, 22, 23, 27, 29, 32], \"id\":24, \"length\":10.29555911705508, \"speed_limit\":4.4, \"signal_group_id\":3 }, { \"conflict_lanelet_ids\":[27, 32], \"id\":25, \"length\":15.85394784011176, \"speed_limit\":8.0, \"signal_group_id\":3 }, { \"conflict_lanelet_ids\":[21, 23, 27, 29, 30, 31], \"id\":26, \"length\":9.74459032026013, \"speed_limit\":4.4, \"signal_group_id\":8 }, { \"conflict_lanelet_ids\":[23, 24, 25, 26, 30, 32], \"id\":27, \"length\":13.64738192837192, \"speed_limit\":8.0, \"signal_group_id\":5 }, { \"conflict_lanelet_ids\":[23, 30], \"id\":28, \"length\":8.18273610098126, \"speed_limit\":4.4, \"signal_group_id\":5 }, { \"conflict_lanelet_ids\":[21, 22, 24, 26, 30, 32], \"id\":29, \"length\":15.85394784011176, \"speed_limit\":8.0, \"signal_group_id\":2 }, { \"conflict_lanelet_ids\":[21, 23, 26, 27, 28, 29], \"id\":30, \"length\":9.74459032026013, \"speed_limit\":4.4, \"signal_group_id\":7 }, { \"conflict_lanelet_ids\":[21, 26], \"id\":31, \"length\":13.64738192837192, \"speed_limit\":8.0, \"signal_group_id\":7 }, { \"conflict_lanelet_ids\":[21, 23, 24, 25, 27, 29], \"id\":32, \"length\":8.18273610098126, \"speed_limit\":4.4, \"signal_group_id\":4 }], \"name\":\"WestIntersection\"}"; 
            intersection_info = std::make_shared<OpenAPI::OAIIntersection_info>();
            intersection_info->fromJson(QString::fromStdString(json_info));
            SPDLOG_DEBUG("Intersection information consists of {0} entry lanes {1} linking lanes and {2} departure lanes.",
                         intersection_info->getEntryLanelets().size(),
                         intersection_info->getLinkLanelets().size(),
                         intersection_info->getDepartureLanelets().size());

            auto hours_since_epoch = std::chrono::duration_cast<std::chrono::hours>(now.time_since_epoch()).count();
            current_hour_in_tenths_secs = (epoch_timestamp - hours_since_epoch * 3600 * 1000) / 100;

            // Create sample spat
            spat_msg_ptr = std::make_shared<signal_phase_and_timing::spat>();
            spat_msg_two_ptr = std::make_shared<signal_phase_and_timing::spat>();
            spat_msg_three_ptr = std::make_shared<signal_phase_and_timing::spat>();

            signal_phase_and_timing::intersection_state intersection_state;
            signal_phase_and_timing::intersection_state intersection_state_two;
            signal_phase_and_timing::intersection_state intersection_state_three;

            signal_phase_and_timing::movement_state state_1;
            state_1.signal_group = 1;
            signal_phase_and_timing::movement_event event_1_1;
            event_1_1.event_state = signal_phase_and_timing::movement_phase_state::protected_movement_allowed; // Green
            event_1_1.timing.set_start_time(epoch_timestamp);
            event_1_1.timing.set_min_end_time(epoch_timestamp + 10000);
            state_1.state_time_speed.push_back(event_1_1);
            signal_phase_and_timing::movement_event event_1_2;
            event_1_2.event_state = signal_phase_and_timing::movement_phase_state::protected_clearance; // Yellow
            event_1_2.timing.set_start_time(epoch_timestamp + 10000);
            event_1_2.timing.set_min_end_time(epoch_timestamp + 13000);
            state_1.state_time_speed.push_back(event_1_2);
            signal_phase_and_timing::movement_event event_1_3;
            event_1_3.event_state = signal_phase_and_timing::movement_phase_state::stop_and_remain; // Red
            event_1_3.timing.set_start_time(epoch_timestamp + 13000);
            event_1_3.timing.set_min_end_time(epoch_timestamp + 15000);
            state_1.state_time_speed.push_back(event_1_3);
            intersection_state.states.push_back(state_1);            

            streets_tsc_configuration::signal_group_configuration tsc_config_1;
            tsc_config_1.signal_group_id = 1;
            tsc_config_1.red_clearance = 0;
            tsc_config_1.yellow_change_duration = 0;
            tsc_state->tsc_config_list.push_back(tsc_config_1);

            signal_phase_and_timing::movement_state state_2;
            state_2.signal_group = 2;
            signal_phase_and_timing::movement_event event_2;
            event_2.event_state = signal_phase_and_timing::movement_phase_state::stop_and_remain; // Red
            event_2.timing.set_start_time(epoch_timestamp);
            event_2.timing.set_min_end_time(epoch_timestamp + 15000);
            state_2.state_time_speed.push_back(event_2);
            intersection_state.states.push_back(state_2);

            streets_tsc_configuration::signal_group_configuration tsc_config_2;
            tsc_config_2.signal_group_id = 2;
            tsc_config_2.red_clearance = 0;
            tsc_config_2.yellow_change_duration = 0;
            tsc_state->tsc_config_list.push_back(tsc_config_2);

            signal_phase_and_timing::movement_state state_3;
            state_3.signal_group = 3;
            signal_phase_and_timing::movement_event event_3;
            event_3.event_state = signal_phase_and_timing::movement_phase_state::stop_and_remain; // Red
            event_3.timing.set_start_time(epoch_timestamp);
            event_3.timing.set_min_end_time(epoch_timestamp + 15000);
            state_3.state_time_speed.push_back(event_3);
            intersection_state.states.push_back(state_3);

            streets_tsc_configuration::signal_group_configuration tsc_config_3;
            tsc_config_3.signal_group_id = 3;
            tsc_config_3.red_clearance = 0;
            tsc_config_3.yellow_change_duration = 0;
            tsc_state->tsc_config_list.push_back(tsc_config_3);

            signal_phase_and_timing::movement_state state_4;
            state_4.signal_group = 4;
            signal_phase_and_timing::movement_event event_4;
            event_4.event_state = signal_phase_and_timing::movement_phase_state::stop_and_remain; // Red
            event_4.timing.set_start_time(epoch_timestamp);
            event_4.timing.set_min_end_time(epoch_timestamp + 15000);
            state_4.state_time_speed.push_back(event_4);
            intersection_state.states.push_back(state_4);

            streets_tsc_configuration::signal_group_configuration tsc_config_4;
            tsc_config_4.signal_group_id = 4;
            tsc_config_4.red_clearance = 0;
            tsc_config_4.yellow_change_duration = 0;
            tsc_state->tsc_config_list.push_back(tsc_config_4);

            signal_phase_and_timing::movement_state state_5;
            state_5.signal_group = 5;
            signal_phase_and_timing::movement_event event_5_1;
            event_5_1.event_state = signal_phase_and_timing::movement_phase_state::protected_movement_allowed; // Green
            event_5_1.timing.set_start_time(epoch_timestamp);
            event_5_1.timing.set_min_end_time(epoch_timestamp + 10000);
            state_5.state_time_speed.push_back(event_5_1);
            signal_phase_and_timing::movement_event event_5_2;
            event_5_2.event_state = signal_phase_and_timing::movement_phase_state::protected_clearance; // Yellow
            event_5_2.timing.set_start_time(epoch_timestamp + 10000);
            event_5_2.timing.set_min_end_time(epoch_timestamp + 13000);
            state_5.state_time_speed.push_back(event_5_2);
            signal_phase_and_timing::movement_event event_5_3;
            event_5_3.event_state = signal_phase_and_timing::movement_phase_state::stop_and_remain; // Red
            event_5_3.timing.set_start_time(epoch_timestamp + 13000);
            event_5_3.timing.set_min_end_time(epoch_timestamp + 15000);
            state_5.state_time_speed.push_back(event_5_3);
            intersection_state.states.push_back(state_5);

            streets_tsc_configuration::signal_group_configuration tsc_config_5;
            tsc_config_5.signal_group_id = 5;
            tsc_config_5.red_clearance = 0;
            tsc_config_5.yellow_change_duration = 0;
            tsc_state->tsc_config_list.push_back(tsc_config_5);

            signal_phase_and_timing::movement_state state_6;
            state_6.signal_group = 6;
            signal_phase_and_timing::movement_event event_6;
            event_6.event_state = signal_phase_and_timing::movement_phase_state::stop_and_remain; // Red
            event_6.timing.set_start_time(epoch_timestamp);
            event_6.timing.set_min_end_time(epoch_timestamp + 15000);
            state_6.state_time_speed.push_back(event_6);
            intersection_state.states.push_back(state_6);

            streets_tsc_configuration::signal_group_configuration tsc_config_6;
            tsc_config_6.signal_group_id = 6;
            tsc_config_6.red_clearance = 0;
            tsc_config_6.yellow_change_duration = 0;
            tsc_state->tsc_config_list.push_back(tsc_config_6);

            signal_phase_and_timing::movement_state state_7;
            state_7.signal_group = 7;
            signal_phase_and_timing::movement_event event_7;
            event_7.event_state = signal_phase_and_timing::movement_phase_state::stop_and_remain; // RED
            event_7.timing.set_start_time(epoch_timestamp);
            event_7.timing.set_min_end_time(epoch_timestamp + 15000);
            state_7.state_time_speed.push_back(event_7);
            intersection_state.states.push_back(state_7);

            streets_tsc_configuration::signal_group_configuration tsc_config_7;
            tsc_config_7.signal_group_id = 7;
            tsc_config_7.red_clearance = 0;
            tsc_config_7.yellow_change_duration = 0;
            tsc_state->tsc_config_list.push_back(tsc_config_7);

            signal_phase_and_timing::movement_state state_8;
            state_8.signal_group = 8;
            signal_phase_and_timing::movement_event event_8;
            event_8.event_state = signal_phase_and_timing::movement_phase_state::stop_and_remain; // Red
            event_8.timing.set_start_time(epoch_timestamp);
            event_8.timing.set_min_end_time(epoch_timestamp + 15000);
            state_8.state_time_speed.push_back(event_8);
            intersection_state.states.push_back(state_8);

            streets_tsc_configuration::signal_group_configuration tsc_config_8;
            tsc_config_8.signal_group_id = 8;
            tsc_config_8.red_clearance = 0;
            tsc_config_8.yellow_change_duration = 0;
            tsc_state->tsc_config_list.push_back(tsc_config_8);

            intersection_state.time_stamp = 130;
            intersection_state.id = 1909;
            intersection_state.moy = 34232;
            spat_msg_ptr->set_intersection(intersection_state);
            spat_msg_two_ptr->set_intersection(intersection_state_two);
            spat_msg_three_ptr->set_intersection(intersection_state_three);
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
                        auto unix_timestamp = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
                        auto itr = update.FindMember("metadata")->value.FindMember("timestamp");
                        unix_timestamp -= 10;
                        itr->value.SetInt64(unix_timestamp);
                        buffer.Clear();
                        writer.Reset(buffer);
                        update.Accept(writer);
                        std::string up = buffer.GetString();
                        updates.push_back(up);
                    }
                }
                return updates;
            }
            else {
                SPDLOG_ERROR("vehicle update is not array!");
                throw streets_vehicles::status_intent_processing_exception("vehicle update is not array!");
            }
        }
    };

    TEST_F(test_signal_opt_processing_worker, select_optimal_dpp)
    {
        auto so_processing_worker = std::make_shared<signal_opt_processing_worker>();
        std::chrono::system_clock::time_point now = std::chrono::system_clock::now();
        std::chrono::milliseconds epochMs = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch());
        uint64_t epoch_timestamp = epochMs.count();

        // Initialize empty vehicle list
        auto veh_list_ptr = std::make_shared<streets_vehicles::vehicle_list>();

        // Set signalized_status_intent_processor processor for vehicle ist
        veh_list_ptr->set_processor(std::make_shared<streets_vehicles::signalized_status_intent_processor>());
        auto processor = std::dynamic_pointer_cast<streets_vehicles::signalized_status_intent_processor>(veh_list_ptr->get_processor());
        processor->set_stopping_distance(1.0);
        processor->set_stopping_speed(0.1);
        veh_list_ptr->get_processor()->set_timeout(3.154e11);
        ASSERT_EQ(0, veh_list_ptr->get_vehicles().size());

        // Initialize buffer params
        streets_signal_optimization::streets_desired_phase_plan_generator_configuration dpp_config;
        dpp_config.initial_green_buffer = 1000;
        dpp_config.final_green_buffer = 1000;
        dpp_config.et_inaccuracy_buffer = 2000;
        dpp_config.queue_max_time_headway = 3000;
        dpp_config.so_radius = 200;
        dpp_config.min_green = 5000;
        dpp_config.max_green = 120000;
        dpp_config.desired_future_move_group_count = 2;

        // create a movement_group shared pointer
        auto movement_group_list = std::make_shared<streets_signal_optimization::movement_groups>();
        streets_signal_optimization::movement_group mg1;
        mg1.name = "movement_group 1";
        mg1.signal_groups = {1, 5};
        movement_group_list->groups.push_back(mg1);
        streets_signal_optimization::movement_group mg2;
        mg2.name = "movement_group 2";
        mg2.signal_groups = {1, 6};
        movement_group_list->groups.push_back(mg2);
        streets_signal_optimization::movement_group mg3;
        mg3.name = "movement_group 3";
        mg3.signal_groups = {2, 5};
        movement_group_list->groups.push_back(mg3);
        streets_signal_optimization::movement_group mg4;
        mg4.name = "movement_group 4";
        mg4.signal_groups = {2, 6};
        movement_group_list->groups.push_back(mg4);
        streets_signal_optimization::movement_group mg5;
        mg5.name = "movement_group 5";
        mg5.signal_groups = {3, 7};
        movement_group_list->groups.push_back(mg5);
        streets_signal_optimization::movement_group mg6;
        mg6.name = "movement_group 6";
        mg6.signal_groups = {3, 8};
        movement_group_list->groups.push_back(mg6);
        streets_signal_optimization::movement_group mg7;
        mg7.name = "movement_group 7";
        mg7.signal_groups = {4, 7};
        movement_group_list->groups.push_back(mg7);
        streets_signal_optimization::movement_group mg8;
        mg8.name = "movement_group 8";
        mg8.signal_groups = {4, 8};
        movement_group_list->groups.push_back(mg8);
        
        SPDLOG_INFO(spat_msg_ptr->toJson());

        // Current spat should only contain the ONLY one current movement event for each movement state.
        ASSERT_EQ(8, spat_msg_ptr->get_intersection().states.size());
        for (auto movement_state : spat_msg_ptr->get_intersection().states)
        {
            if (movement_state.signal_group == 1 || movement_state.signal_group == 5) {
                ASSERT_EQ(3, movement_state.state_time_speed.size());
            }
            else {
                ASSERT_EQ(1, movement_state.state_time_speed.size());
            }
        }

        so_processing_worker->configure_dpp_optimizer(dpp_config);
        try
        {
            so_processing_worker->select_optimal_dpp(intersection_info, spat_msg_ptr, tsc_state, veh_list_ptr, movement_group_list, dpp_config);
        }
        catch (const streets_signal_optimization::streets_desired_phase_plan_arbitrator_exception &e)
        {
            ASSERT_STREQ(e.what(), "Vehicle schedule cannot be empty. Vehicles maybe outside of the radius.");
        }

        // It should make a local copy and do not update the pass in spat message spat_msg_ptr
        ASSERT_EQ(8, spat_msg_ptr->get_intersection().states.size());
        for (auto movement_state : spat_msg_ptr->get_intersection().states)
        {
            if (movement_state.signal_group == 1 || movement_state.signal_group == 5) {
                ASSERT_EQ(3, movement_state.state_time_speed.size());
            }
            else {
                ASSERT_EQ(1, movement_state.state_time_speed.size());
            }
        }
        // Load Vehicle Update
        std::vector<std::string> updates = load_vehicle_update("../test/test_data/updates_signalized.json");

        // Print timeout in days.
        SPDLOG_DEBUG("Set timeout to {0} days !", veh_list_ptr->get_processor()->get_timeout() / (1000 * 60 * 60 * 24));
        for (auto &update : updates)
        {
            veh_list_ptr->process_update(update);
        }
        ASSERT_EQ(veh_list_ptr->get_vehicles().size(), 2);
        auto chosen_dpp = so_processing_worker->select_optimal_dpp(intersection_info, spat_msg_ptr, tsc_state, veh_list_ptr, movement_group_list, dpp_config);
        ASSERT_TRUE(chosen_dpp.desired_phase_plan.size() == 2);


        
    }
}