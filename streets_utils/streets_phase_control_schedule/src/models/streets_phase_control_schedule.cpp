#include "streets_phase_control_schedule.h"

namespace streets_phase_control_schedule
{    
    void streets_phase_control_schedule::fromJson(const std::string &json)
    {
        rapidjson::Document doc;
        doc.Parse(json);
        if (doc.HasParseError())
        {
            throw streets_phase_control_schedule_exception("streets_phase_control_schedule message JSON is misformatted. JSON parsing failed!");
        }

        if (doc.HasMember("timestamp") && doc.FindMember("timestamp")->value.IsUint64())
        {
            
        }
        else
        {
            throw streets_phase_control_schedule_exception("streets_phase_control_schedule message is missing required timestamp property!");
        }

        if (doc.FindMember("desired_phase_plan")->value.IsArray())
        {
           
        }
        else
        {
            throw streets_phase_control_schedule_exception("streets_phase_control_schedule message is missing required desired_phase_plan property!");
        }
    }
}