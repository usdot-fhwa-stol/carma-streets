#include <gtest/gtest.h>
#include <streets_clock_singleton.h>
#include <thread>
#include <future>


using namespace streets_service;
using namespace std::chrono;
namespace streets_service{


   class test_streets_clock : public testing::Test {
        protected:
            void SetUp() {
               SPDLOG_INFO("Setup");
            }
            void TearDown() {
                SPDLOG_INFO("Tear Down");
            }
            int time_comparison_threshold_ms = 20;
        
    };
    /**
     * @brief This test case is meant to execute the streets_clock_singleton functionality in simulation mode to block a single calling thread
     * requesting any time before the carma-clock object has been initialized by a time sync message from simulation and correctly unblock/wake up
     * the thread once the carma clock is initialized
     * 
    */
    TEST_F( test_streets_clock, test_simulation_mode_single_thread_initialization) {
        streets_clock_singleton::create(true);
        // Providing a seed value
        srand((unsigned) time(NULL));
        // Get a random number
        int random = rand();
        int old_val = random;
        auto start = std::chrono::system_clock::now();
        // Set at 100 ms
        auto sleep_duration = 100;
        // Thread to simulate first incoming time sync message
        std::thread t1([sleep_duration]() {
            std::this_thread::sleep_for(std::chrono::milliseconds(sleep_duration));
            streets_clock_singleton::update(0);
        });
        // Calling thread
        EXPECT_EQ(streets_clock_singleton::time_in_ms(), 0);
        // Measure how long calling thread was blocked 
        auto end = std::chrono::system_clock::now();
        auto elapsed_time = end-start;
        // Assume duration is sleep duration of update thread +/- 1ms
        EXPECT_NEAR( sleep_duration, std::chrono::duration_cast<milliseconds>(elapsed_time).count(), time_comparison_threshold_ms );
        t1.join();
    }
    /**
     * @brief This test case is meant to execute the streets_clock_singleton functionality in simulation mode to block a single calling thread
     * using the sleep_until method and unblock/waking up the thread once the sleep_until value has been reached or passed.
     * 
    */
    TEST_F( test_streets_clock, test_simulation_mode_single_thread_sleep_until) {
        streets_clock_singleton::create(true);
        // Initialize streets_clock_singleton to initial value
        streets_clock_singleton::update(0);
        // Providing a seed value
        srand((unsigned) time(NULL));
        // Get a random number
        int random = rand();
        int old_val = random;
        // Measure start of real time duration
        auto start = std::chrono::system_clock::now();
        // Set at 100 ms
        auto sleep_duration = 100;
        // Thread to simulate incoming time sync update
        std::thread t2([random, sleep_duration]() {
            std::this_thread::sleep_for(std::chrono::milliseconds(sleep_duration));
            streets_clock_singleton::update(random);
        });
        // Should block until streets_clock_singleton is updated to value.
        streets_clock_singleton::sleep_until(random);
        // Measure end of real time duration
        auto end = std::chrono::system_clock::now();
        auto elapsed_time = end-start;
        // Assume duration is sleep duration of update thread +/- 1ms
        EXPECT_NEAR( sleep_duration, duration_cast<milliseconds>(elapsed_time).count(), time_comparison_threshold_ms );
        EXPECT_EQ(streets_clock_singleton::time_in_ms(), random);
        t2.join();

    };

    /**
     * @brief This test case is meant to execute the streets_clock_singleton functionality in simulation mode to block  multiple calling threads
     * requesting any time before the carma-clock object has been initialized by a time sync message and correctly unblocking/waking up all
     * waiting threads once the carma clock is initialized
     * 
    */
    TEST_F( test_streets_clock, test_simulation_mode_multiple_thread_initialization) {
        streets_clock_singleton::create(true);
        // Providing a seed value
        srand((unsigned) time(NULL));
        // Get a random number
        int random = rand();
        int old_val = random;
        auto start = std::chrono::system_clock::now();
        int t1_duration_ms, t2_duration_ms, t3_duration_ms;
        // Set at 100 ms
        auto sleep_duration = 100;
        // Thread to simulate first incoming time sync message
        std::thread t1([start, &t1_duration_ms]() {
            //Blocked until streets_clock_singleton is initialized with a value
            streets_clock_singleton::time_in_ms();
            auto t1_end = std::chrono::system_clock::now();
            t1_duration_ms = std::chrono::duration_cast<std::chrono::milliseconds>(t1_end-start).count();

        });
        std::thread t2([start, &t2_duration_ms]() {
            //Blocked until streets_clock_singleton is initialized with a value
            streets_clock_singleton::time_in_ms();
            auto t2_end = std::chrono::system_clock::now();
            t2_duration_ms = std::chrono::duration_cast<std::chrono::milliseconds>(t2_end-start).count();


        });
        std::thread t3([start, &t3_duration_ms]() {
            //Blocked until streets_clock_singleton is initialized with a value
            streets_clock_singleton::time_in_ms();
            auto t3_end = std::chrono::system_clock::now();
            t3_duration_ms = std::chrono::duration_cast<std::chrono::milliseconds>(t3_end-start).count();

        });

        // Simulate time sync message update
        std::this_thread::sleep_for(std::chrono::milliseconds(sleep_duration));
        streets_clock_singleton::update(0);
        t1.join();
        t2.join();
        t3.join();
        // Confirm update was succesful
        EXPECT_EQ(streets_clock_singleton::time_in_ms(), 0);

        // // Assume duration is sleep duration of update is equal to thread duration +/- 1ms
        EXPECT_NEAR( sleep_duration, t1_duration_ms, time_comparison_threshold_ms );
        EXPECT_NEAR( sleep_duration, t2_duration_ms, time_comparison_threshold_ms );
        EXPECT_NEAR( sleep_duration, t3_duration_ms, time_comparison_threshold_ms );
        
    }
    /**
     * @brief  This test case is meant to test the functionality of the streets_clock_singleton to correctly handle multiple time updates
     * in simulation mode.
     */
    TEST_F(test_streets_clock, test_sim_mode_multiple_updates) {
        streets_clock_singleton::create(true);
        // Initialize streets_clock_singleton to initial value
        streets_clock_singleton::update(0);
        // Providing a seed value
        srand((unsigned) time(NULL));
        // Get a random number
        int random = rand();
        int old_val = random;
        streets_clock_singleton::update(random);
        EXPECT_EQ(streets_clock_singleton::time_in_ms(), random);
        // Simulate random size timestep
        random += rand();
        streets_clock_singleton::update(random);
        EXPECT_EQ(streets_clock_singleton::time_in_ms(), random);
        EXPECT_NE(old_val, random);
    }
    /**
     * @brief This test case is to test the streets_clock_singleton functionality when not in simulation mode. When not in simulation mode the streets
     * clock singleton should just return system time.
     */
    TEST_F(test_streets_clock, test_real_time) {
        streets_clock_singleton::create(false);
        EXPECT_NEAR(streets_clock_singleton::time_in_ms(), duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count(), time_comparison_threshold_ms);
        // allow time to change
        sleep(1);
        EXPECT_NEAR(streets_clock_singleton::time_in_ms(), duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count(), time_comparison_threshold_ms);
        auto cur_time = streets_clock_singleton::time_in_ms();
        streets_clock_singleton::sleep_for(2000);
        EXPECT_NEAR(cur_time+2000, duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count(), time_comparison_threshold_ms);

        cur_time = streets_clock_singleton::time_in_ms();
        auto sleep_until_time = cur_time + 3000;
        streets_clock_singleton::sleep_until(sleep_until_time);
        EXPECT_NEAR(sleep_until_time,  duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count(), time_comparison_threshold_ms);

    }
}