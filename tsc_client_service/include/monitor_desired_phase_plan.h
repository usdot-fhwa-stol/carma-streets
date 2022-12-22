#pragma once
#include <spdlog/spdlog.h>
#include <gtest/gtest_prod.h>

#include "streets_desired_phase_plan.h"
#include "spat.h"
#include "monitor_tsc_state.h"
#include "monitor_desired_phase_plan_exception.h"
#include "snmp_client.h"
namespace traffic_signal_controller_service
{
    class monitor_desired_phase_plan
    {
    private:
        std::shared_ptr<streets_desired_phase_plan::streets_desired_phase_plan> desired_phase_plan_ptr;

        std::shared_ptr<snmp_client> _snmp_client;
        /**
         * @brief Holds a vector of signal groups that represent the phases that were most recently green. This 
         * vector is used to determine which signal group is currently in red clearance.
         */
        std::vector<signal_phase_and_timing::movement_state> last_green_served;
        // Add friend test fixture to allow access to priveate members.
        friend class test_monitor_desired_phase_plan;
        FRIEND_TEST(test_monitor_desired_phase_plan, test_spat_prediction_no_desired_phase_plan_cur_all_red);
        FRIEND_TEST(test_monitor_desired_phase_plan, test_spat_prediction_no_desired_phase_plan_cur_yellow);
        FRIEND_TEST(test_monitor_desired_phase_plan, update_last_green_served);
        FRIEND_TEST(test_monitor_desired_phase_plan, fix_upcoming_green_exception);


    public:
        /**
         * @brief Construct a new monitor desired phase plan object..
         * 
         * @param client shared pointer to an initialized SNMP client for NTCIP next phase group requests.
         */
        explicit monitor_desired_phase_plan( const std::shared_ptr<snmp_client> client );
        /**
         * @brief Destroy the monitor desired phase plan object
         * 
         */
        ~monitor_desired_phase_plan() = default;
        /**
         * @brief Update desired_phase_plan object with json received string message.
         * 
         * @param payload 
         */
        void update_desired_phase_plan(const std::string& payload);
        /**
         * @brief Update spat movement_event list with desired phase plan information.
         * 
         * @param spat_ptr shared pointer to spat object.
         * @param tsc_state_ptr shared pointer to tsc_state object.
         */
        void update_spat_future_movement_events(std::shared_ptr<signal_phase_and_timing::spat> spat_ptr, 
                                                const std::shared_ptr<tsc_state> tsc_state_ptr) ;
        /**
         * @brief Returns shared pointer to desired_phase_plan object.
         * 
         * @return std::shared_ptr<streets_desired_phase_plan::streets_desired_phase_plan> 
         */
        std::shared_ptr<streets_desired_phase_plan::streets_desired_phase_plan> get_desired_phase_plan_ptr() const;

        /**
         * @brief Method to represent the upcoming green, when no desired phase plan is present and the TSC is currently 
         * in YELLOW CHANGE or RED CLEARANCE. This is indicated by the SPaT not containing any movement states with
         * GREEN/PROTECTED MOVEMENT ALLOWED as the current state. Using the SNMP Client to make a GET request to the 
         * PHASE_STATUS_GROUP_PHASE_NEXT OID (see ntcip_oids.h PHASE_STATUS_GROUP_PHASE_NEXT documentation for more information)
         * we will determine which phases are next and project GREEN, YELLOW and RED events onto the SPaT for this next GREEN
         * using phase configuration information (min green, yellow change, and red clearance) to set timing data. THe projected
         * GREEN phase will be fixed to last the minimum GREEN duration.
         * 
         * @throws monitor_desired_phase_plan_exception if last_green_served is empty
         * @param spat_ptr shared pointer to the current spat information.
         * @param tsc_state_ptr shared pointer to the tsc state information including tsc phase configuration information.
         */
        void fix_upcoming_green(const std::shared_ptr<signal_phase_and_timing::spat> spat_ptr, const std::shared_ptr<tsc_state> tsc_state_ptr) ;
        /**
         * @brief Method to represent yellow change and red clearance when no desired phase plan is present and the TSC is currently
         * GREEN for a given phase. This method assumes the current green will continue for min_green time and adds future events for all
         * signal groups to account for the current green, the following yellow change and red clearance. This method will add future events 
         * with this information to the current spat_ptr data. Upon successful execution a SPaT message that previously only had current state and start time
         * information will included future state information assuming the current green ends at min green and is followed by a yellow change and red clearance.
         * 
         * @throws monitor_desired_phase_plan_exception if more than 2 green phases are passed to this method.
         * 
         * @param spat_ptr SPaT data to include projected future events 
         * @param tsc_state_ptr traffic signal controller configuration information like SG to phase number mapping, min green, yellow change
         *      and red clearance.
         * @param green_phases movement state objects of the phases that are currently green.
         */
        void fix_upcoming_yell_red(const std::shared_ptr<signal_phase_and_timing::spat> spat_ptr, 
                                    const std::shared_ptr<tsc_state> tsc_state_ptr, 
                                    const std::vector<signal_phase_and_timing::movement_state> &green_phases) const;
        /**
         * @brief Method to prune the desired phase plan of any entries for which the end time is less than the current time.
         * 
         * @param desired_phase_plan desired phase plan shared pointer from which to prune expired entries.
         */
        void prune_expired_greens_from_dpp( const std::shared_ptr<streets_desired_phase_plan::streets_desired_phase_plan> &desired_phase_plan ) const;
        /**
         * @brief Method to update last greens served with currently present green movement states.
         * 
         * @throws monitor_desired_phase_plan_exception if greens_phases_present empty.
         * @param green_phases_present Currently present green phases in SPAT.
         */
        void update_last_green_served(const std::vector<signal_phase_and_timing::movement_state> &green_phases_present);
    };

}
