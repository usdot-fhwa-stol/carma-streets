#pragma once
#include <spdlog/spdlog.h>

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
                                                const std::shared_ptr<tsc_state> tsc_state_ptr) const;
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
         * @param spat_ptr shared pointer to the current spat information.
         * @param tsc_state_ptr shared pointer to the tsc state information including tsc phase configuration information.
         */
        void fix_upcoming_green(const std::shared_ptr<signal_phase_and_timing::spat> spat_ptr, const std::shared_ptr<tsc_state> tsc_state_ptr) const;

        void fix_upcoming_yell_red(const std::shared_ptr<signal_phase_and_timing::spat> spat_ptr, 
                                    const std::shared_ptr<tsc_state> tsc_state_ptr, 
                                    const std::vector<signal_phase_and_timing::movement_state> green_phases) const;
    };

}
