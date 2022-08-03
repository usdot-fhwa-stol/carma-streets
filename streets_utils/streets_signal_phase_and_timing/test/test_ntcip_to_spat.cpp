#include <gtest/gtest.h>
#include <spdlog/spdlog.h>
#include <fstream>
#include <map>
#include <chrono>


#include "spat.h"
#include "ntcip_1202_ext.h"

using namespace signal_phase_and_timing;
using namespace ntcip;

class test_ntcip_to_spat : public ::testing::Test {
    public:
        std::shared_ptr<spat> spat_ptr;

        ntcip_1202_ext spat_ntcip_data;

        std::ifstream file;

        std::unordered_map<int,int> phase_to_signal_group = {
                {1,8},{2,7},{3,6},{4,5},{5,4},{6,3},{7,2},{8,1}
        };

        /**
         * @brief Read next line of ntcip_spat_data. Each line represents a single ntcip
         * spat message in string hex format. This string hex is convert to bytes and
         * read them into the spat_ntcip_data object.
         */
        void read_next_line(){
            std::string line;
            if ( std::getline( file, line)) {
                std::vector<char> buf = hex_to_bytes(line); 
                std::memcpy(&spat_ntcip_data, buf.data(), line.size()/2);
            } else {
                SPDLOG_WARN("Reached in of ntcip_spat_data.txt file !");
            }
        }
        
        /**
         * @brief Conver string hex to char byte vector
         * 
         * @param hex string hex to conver
         * @return std::vector<char> byte vector
         */
        std::vector<char> hex_to_bytes(const std::string& hex) {
            std::vector<char> bytes;
            for (unsigned int i = 0; i < hex.length(); i += 2) {
                std::string byteString = hex.substr(i, 2);
                char byte = (char) strtol(byteString.c_str(), NULL, 16);
                bytes.push_back(byte);
            }
            return bytes;
        }

    protected:
        void SetUp() override {
            spat_ptr = std::make_shared<spat>();
            file.open("../test/test_data/ntcip_spat_data.txt");
            spat_ptr->initialize_intersection("Test Intersection", 12902, phase_to_signal_group );
        }


        void TearDown() override {
            file.close();
        }
};

TEST_F( test_ntcip_to_spat, test_update) {
    // Assert Initiliazition added a single intersection with correct name and id
    ASSERT_TRUE(!spat_ptr->intersections.empty());
    intersection_state &intersection =  spat_ptr->intersections.front();
    ASSERT_EQ( intersection.name , "Test Intersection" );
    ASSERT_EQ( intersection.id , 12902 );
    ASSERT_EQ( intersection.states.size(), phase_to_signal_group.size());

    read_next_line();
    SPDLOG_INFO("NTCIP SPat {0}", spat_ntcip_data.to_string());
    spat_ptr->update( spat_ntcip_data, false);
    std::chrono::system_clock::time_point now = std::chrono::system_clock::now();
    time_t tt = std::chrono::system_clock::to_time_t(now);
    tm utc_tm = *gmtime(&tt);
    uint32_t moy = utc_tm.tm_yday*60*24 + utc_tm.tm_hour*60 + utc_tm.tm_min;
    ASSERT_EQ( intersection.moy, moy );
    // Get phase number 6
    movement_state &state_6 =  intersection.get_movement(phase_to_signal_group.find(6)->second);
    ASSERT_EQ( state_6.signal_group, phase_to_signal_group.find(6)->second );

    movement_event &event_cur_6 = state_6.state_time_speed.front(); 
    ASSERT_EQ( event_cur_6.event_state, movement_phase_state::protected_movement_allowed);
    ASSERT_EQ( event_cur_6.timing.min_end_time,intersection.convert_offset(0) );
    ASSERT_EQ( event_cur_6.timing.max_end_time,intersection.convert_offset(50) );
    // Get phase number 4
    movement_state &state_4 =  intersection.get_movement(phase_to_signal_group.find(4)->second);
    ASSERT_EQ( state_4.signal_group, phase_to_signal_group.find(4)->second);

    movement_event &event_cur_4 = state_4.state_time_speed.front(); 
    ASSERT_EQ( event_cur_4.event_state, movement_phase_state::stop_and_remain);

    ASSERT_EQ( event_cur_4.timing.min_end_time,intersection.convert_offset(50) );
    ASSERT_EQ( event_cur_4.timing.max_end_time,intersection.convert_offset(100) );

    read_next_line();
    
    SPDLOG_INFO("NTCIP SPat {0}", spat_ntcip_data.to_string());
    spat_ptr->update( spat_ntcip_data, false);
    now = std::chrono::system_clock::now();
    tt = std::chrono::system_clock::to_time_t(now);
    utc_tm = *gmtime(&tt);
    moy = utc_tm.tm_yday*60*24 + utc_tm.tm_hour*60 + utc_tm.tm_min;
    ASSERT_EQ( intersection.moy, moy );
    // Get phase number 6
    state_6 =  intersection.get_movement(phase_to_signal_group.find(6)->second);
    ASSERT_EQ( state_6.signal_group, phase_to_signal_group.find(6)->second );

    event_cur_6 = state_6.state_time_speed.front(); 
    ASSERT_EQ( event_cur_6.event_state, movement_phase_state::stop_and_remain);
    ASSERT_EQ( event_cur_6.timing.min_end_time,intersection.convert_offset(89) );
    ASSERT_EQ( event_cur_6.timing.max_end_time,intersection.convert_offset(209) );
    // Get phase number 4
    state_4 =  intersection.get_movement(phase_to_signal_group.find(4)->second);
    ASSERT_EQ( state_4.signal_group, phase_to_signal_group.find(4)->second);

    event_cur_4 = state_4.state_time_speed.front(); 
    ASSERT_EQ( event_cur_4.event_state, movement_phase_state::protected_movement_allowed);
    ASSERT_EQ( event_cur_4.timing.min_end_time,intersection.convert_offset(39) );
    ASSERT_EQ( event_cur_4.timing.max_end_time,intersection.convert_offset(159) );

    read_next_line();

    spat_ptr->update( spat_ntcip_data, false);
    now = std::chrono::system_clock::now();
    tt = std::chrono::system_clock::to_time_t(now);
    utc_tm = *gmtime(&tt);
    moy = utc_tm.tm_yday*60*24 + utc_tm.tm_hour*60 + utc_tm.tm_min;
    ASSERT_EQ( intersection.moy, moy );
    // Get phase number 6
    state_6 =  intersection.get_movement(phase_to_signal_group.find(6)->second);
    ASSERT_EQ( state_6.signal_group, phase_to_signal_group.find(6)->second );

    event_cur_6 = state_6.state_time_speed.front(); 
    ASSERT_EQ( event_cur_6.event_state, movement_phase_state::stop_and_remain);
    ASSERT_EQ( event_cur_6.timing.min_end_time,intersection.convert_offset(88) );
    ASSERT_EQ( event_cur_6.timing.max_end_time,intersection.convert_offset(208) );
    // Get phase number 4
    state_4 =  intersection.get_movement(phase_to_signal_group.find(4)->second);
    ASSERT_EQ( state_4.signal_group, phase_to_signal_group.find(4)->second);

    event_cur_4 = state_4.state_time_speed.front(); 
    ASSERT_EQ( event_cur_4.event_state, movement_phase_state::protected_movement_allowed);
    ASSERT_EQ( event_cur_4.timing.min_end_time,intersection.convert_offset(38) );
    ASSERT_EQ( event_cur_4.timing.max_end_time,intersection.convert_offset(158) );
}

TEST_F( test_ntcip_to_spat, test_update_tsc_timestamp) {
    // Assert Initiliazition added a single intersection with correct name and id
    ASSERT_TRUE(!spat_ptr->intersections.empty());
    intersection_state &intersection =  spat_ptr->intersections.front();
    ASSERT_EQ( intersection.name , "Test Intersection" );
    ASSERT_EQ( intersection.id , 12902 );
    ASSERT_EQ( intersection.states.size(), phase_to_signal_group.size());

    read_next_line();
    SPDLOG_INFO("NTCIP SPat {0}", spat_ntcip_data.to_string());
    spat_ptr->update( spat_ntcip_data, true);
    std::chrono::system_clock::time_point now = std::chrono::system_clock::now();
    time_t tt = std::chrono::system_clock::to_time_t(now);
    tm utc_tm = *gmtime(&tt);
    uint32_t moy = utc_tm.tm_yday*60*24 + utc_tm.tm_hour*60 + utc_tm.tm_min;
    ASSERT_FALSE( intersection.moy == moy );
}