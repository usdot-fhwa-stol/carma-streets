#pragma once

#include <spdlog/spdlog.h>



namespace streets_signal_optimization {

    struct desired_phase_plan_generator_configuration {

        /**
         * @brief The configurable time interval at the beginning of a green phase in milliseconds that is considered for estimating 
         * vehicles' entering times. The earliest possible entering time during a phase is set to the beginning of the green phase plus 
         * the initial_green_buffer.
         */
        uint64_t initial_green_buffer;

        /**
         * @brief The configurable time interval at the end of a green phase in milliseconds that is considered for estimating vehicles' 
         * entering times. The latest possible entering time during a phase is set to the end of the green phase minus the final_green_buffer.
         */
        uint64_t final_green_buffer;

        /**
         * @brief The configurable time interval in milliseconds defined to account for entering time (ET) estimation inaccuracy. To find 
         * the required green duration for dissipating a queue from an entry lane, the end of the green is set to the estimated ET of the 
         * last vehicle in the queue plus this configurable parameter plus final_green_buffer.
         */
        uint64_t et_inaccuracy_buffer;

        /**
         * @brief The configurable time headway in milliseconds that defines the maximum allowed time headway between the estimated 
         * entering time (ET) of two consecutive vehicles within a queue. This parameter is defined to find the last vehicle in the queue 
         * from an entry lane.
         */
        uint64_t queue_max_time_headway;

        /**
         * @brief The configurable distance in meters defined as the radius of the signal optimization (SO) area. The signal optimization 
         * service only considers vehicles that are within the signal optimization area.
         */
        double so_radius;

        /**
         * @brief The configurable minimum green duration in milliseconds.
         */
        uint64_t min_green;

        /**
         * @brief The configurable maximum green duration in milliseconds.
         */
        uint64_t max_green;
        
        /**
         * @brief The configurable parameter that defines the desired number of fixed future movement groups in the spat.
         */            
        uint8_t desired_future_move_group_count;


    };

}