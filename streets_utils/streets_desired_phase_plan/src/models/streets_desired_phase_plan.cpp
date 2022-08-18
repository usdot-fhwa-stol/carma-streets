#include "streets_desired_phase_plan.h"

namespace streets_desired_phase_plan
{

    std::string streets_desired_phase_plan::toJson() const
    {
        rapidjson::Document doc;
        auto allocator = doc.GetAllocator();
        // Create streets_desired_phase_plan JSON value
        rapidjson::Value streets_desired_phase_plan_value(rapidjson::kObjectType);
        // Populate streets_desired_phase_plan JSON
        streets_desired_phase_plan_value.AddMember("timestamp", timestamp, allocator);
        rapidjson::Value desired_phase_plan_value(rapidjson::kArrayType);
        for (auto desired_phase : desired_phase_plan)
        {
            rapidjson::Value sg2green_phase_timing_value(rapidjson::kObjectType);
            rapidjson::Value signal_groups_value(rapidjson::kArrayType);
            for (const auto &sg : desired_phase.signal_groups)
            {
                rapidjson::Value sg_value;
                sg_value.SetInt(sg);
                signal_groups_value.PushBack(sg_value, allocator);
            }
            sg2green_phase_timing_value.AddMember("signal_groups", signal_groups_value, allocator);
            sg2green_phase_timing_value.AddMember("start_time", desired_phase.start_time, allocator);
            sg2green_phase_timing_value.AddMember("end_time", desired_phase.end_time, allocator);
            desired_phase_plan_value.PushBack(sg2green_phase_timing_value, allocator);
        }
        streets_desired_phase_plan_value.AddMember("desired_phase_plan", desired_phase_plan_value, allocator);

        rapidjson::StringBuffer buffer;
        try
        {
            rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
            streets_desired_phase_plan_value.Accept(writer);
        }
        catch (const std::exception &e)
        {
            throw streets_desired_phase_plan_exception(e.what());
        }
        return buffer.GetString();
    }

    void streets_desired_phase_plan::fromJson(const std::string &json)
    {
        rapidjson::Document doc;
        doc.Parse(json);
        if (doc.HasParseError())
        {
            throw streets_desired_phase_plan_exception("streets_desired_phase_plan message JSON is misformatted. JSON parsing failed!");
        }

        if (doc.HasMember("timestamp") && doc.FindMember("timestamp")->value.IsUint64())
        {
            timestamp = (uint64_t)doc["timestamp"].GetUint64();
        }
        else
        {
            throw streets_desired_phase_plan_exception("streets_desired_phase_plan message is missing required timestamp property!");
        }

        if (doc.FindMember("desired_phase_plan")->value.IsArray())
        {
            for (const auto &desired_phase_value : doc["desired_phase_plan"].GetArray())
            {
                signal_group2green_phase_timing sg2gpt;
                if (desired_phase_value.HasMember("signal_groups") && desired_phase_value.FindMember("signal_groups")->value.IsArray())
                {
                    for (const auto &sg : desired_phase_value["signal_groups"].GetArray())
                    {
                        sg2gpt.signal_groups.push_back(sg.GetInt());
                    }
                }
                else
                {
                    throw streets_desired_phase_plan_exception("streets_desired_phase_plan message is missing required signal_groups property!");
                }

                if (desired_phase_value.HasMember("start_time") && desired_phase_value.FindMember("start_time")->value.IsUint64())
                {
                    sg2gpt.start_time = (uint64_t)desired_phase_value["start_time"].GetUint64();
                }
                else
                {
                    throw streets_desired_phase_plan_exception("streets_desired_phase_plan message is missing required start_time property!");
                }

                if (desired_phase_value.HasMember("end_time") && desired_phase_value.FindMember("end_time")->value.IsUint64())
                {
                    sg2gpt.end_time = (uint64_t)desired_phase_value["end_time"].GetUint64();
                }
                else
                {
                    throw streets_desired_phase_plan_exception("streets_desired_phase_plan message is missing required end_time property!");
                }
                desired_phase_plan.push_back(sg2gpt);
            }
        }
        else
        {
            throw streets_desired_phase_plan_exception("streets_desired_phase_plan message is missing required desired_phase_plan property!");
        }
    }
}