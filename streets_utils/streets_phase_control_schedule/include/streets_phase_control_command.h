#include <stdint.h>
#include <string>

namespace streets_phase_control_schedule
{
    // Define Command Actions (Used in phase controls)
    enum class COMMAND_TYPE
    {
        CALL_VEH_PHASES = 1,
        CALL_PED_PHASES = 2,
        FORCEOFF_PHASES = 3,
        HOLD_VEH_PHASES = 4,
        OMIT_VEH_PHASES = 5,
        OMIT_PED_PHASES = 6
    };

    class streets_phase_control_command
    {
    private:
        COMMAND_TYPE command_type;
        int command_phase;
        float command_start_time;
        float command_end_time;

    public:
        streets_phase_control_command(/* args */);
        ~streets_phase_control_command();
        bool set_command_type(const std::string& command_type_str);
    };

}