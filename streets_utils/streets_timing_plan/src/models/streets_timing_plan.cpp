#include "streets_timing_plan.h"
#include <iostream>

namespace streets_timing_plan
{
    void streets_timing_plan::fromJson(const std::string &json)
    {
        rapidjson::Document doc;
        doc.Parse(json);
        if (doc.HasParseError())
        {
            throw streets_timing_plan_exception("streets_timing_plan message JSON is misformatted.");
        }

        if (doc.HasMember("MsgType") && doc.FindMember("MsgType")->value.IsString())
        {
            std::string value = doc.FindMember("MsgType")->value.GetString();
            if (value != TIMING_PLAN_MSG_TYPE)
            {
                throw streets_timing_plan_exception("streets_timing_plan requires MsgType property value (= " + TIMING_PLAN_MSG_TYPE + "), but received " + value + " instead.");
            }
        }
        else
        {
            throw streets_timing_plan_exception("streets_timing_plan is missing required MsgType property.");
        }

        if (!doc.HasMember("TimingPlan") || !doc.FindMember("TimingPlan")->value.IsObject())
        {
            throw streets_timing_plan_exception("streets_timing_plan is missing required TimingPlan property.");
        }

        auto timing_plan_value = doc.FindMember("TimingPlan")->value.GetObject();
        if (timing_plan_value.HasMember("NoOfPhase") && timing_plan_value.FindMember("NoOfPhase")->value.IsUint64())
        {
            number_of_phase = timing_plan_value.FindMember("NoOfPhase")->value.GetUint64();
        }
        else
        {
            throw streets_timing_plan_exception("streets_timing_plan is missing required NoOfPhase property.");
        }

        if (timing_plan_value.HasMember("PhaseNumber") && timing_plan_value.FindMember("PhaseNumber")->value.IsArray())
        {
            for (const auto &itr : timing_plan_value.FindMember("PhaseNumber")->value.GetArray())
            {
                phase_number_v.push_back(itr.GetInt64());
            }
        }
        else
        {
            throw streets_timing_plan_exception("streets_timing_plan is missing required PhaseNumber property.");
        }

        if (timing_plan_value.HasMember("PedWalk") && timing_plan_value.FindMember("PedWalk")->value.IsArray())
        {
            for (const auto &itr : timing_plan_value.FindMember("PedWalk")->value.GetArray())
            {
                pedestrian_walk_v.push_back(itr.GetInt64());
            }
        }
        else
        {
            throw streets_timing_plan_exception("streets_timing_plan  is missing required PedWalk property.");
        }

        if (timing_plan_value.HasMember("PedClear") && timing_plan_value.FindMember("PedClear")->value.IsArray())
        {
            for (const auto &itr : timing_plan_value.FindMember("PedClear")->value.GetArray())
            {
                pedestrian_clear_v.push_back(itr.GetInt64());
            }
        }
        else
        {
            throw streets_timing_plan_exception("streets_timing_plan is missing required PedClear property.");
        }

        if (timing_plan_value.HasMember("MinGreen") && timing_plan_value.FindMember("MinGreen")->value.IsArray())
        {
            for (const auto &itr : timing_plan_value.FindMember("MinGreen")->value.GetArray())
            {
                min_green_v.push_back(itr.GetInt64());
            }
        }
        else
        {
            throw streets_timing_plan_exception("streets_timing_plan is missing required MinGreen property.");
        }

        if (timing_plan_value.HasMember("Passage") && timing_plan_value.FindMember("Passage")->value.IsArray())
        {
            for (const auto &itr : timing_plan_value.FindMember("Passage")->value.GetArray())
            {
                passage_v.push_back(itr.GetDouble());
            }
        }
        else
        {
            throw streets_timing_plan_exception("streets_timing_plan is missing required Passage property.");
        }

        if (timing_plan_value.HasMember("MaxGreen") && timing_plan_value.FindMember("MaxGreen")->value.IsArray())
        {
            for (const auto &itr : timing_plan_value.FindMember("MaxGreen")->value.GetArray())
            {
                max_green_v.push_back(itr.GetInt64());
            }
        }
        else
        {
            throw streets_timing_plan_exception("streets_timing_plan is missing required MaxGreen property.");
        }

        if (timing_plan_value.HasMember("YellowChange") && timing_plan_value.FindMember("YellowChange")->value.IsArray())
        {
            for (const auto &itr : timing_plan_value.FindMember("YellowChange")->value.GetArray())
            {
                yellow_change_v.push_back(itr.GetDouble());
            }
        }
        else
        {
            throw streets_timing_plan_exception("streets_timing_plan  is missing required YellowChange property.");
        }

        if (timing_plan_value.HasMember("RedClear") && timing_plan_value.FindMember("RedClear")->value.IsArray())
        {
            for (const auto &itr : timing_plan_value.FindMember("RedClear")->value.GetArray())
            {
                red_clear_v.push_back(itr.GetDouble());
            }
        }
        else
        {
            throw streets_timing_plan_exception("streets_timing_plan  is missing required RedClear property.");
        }

        if (timing_plan_value.HasMember("PhaseRing") && timing_plan_value.FindMember("PhaseRing")->value.IsArray())
        {
            for (const auto &itr : timing_plan_value.FindMember("PhaseRing")->value.GetArray())
            {
                phase_ring_v.push_back(itr.GetInt64());
            }
        }
        else
        {
            throw streets_timing_plan_exception("streets_timing_plan  is missing required PhaseRing property.");
        }
    }

    rapidjson::Document streets_timing_plan::toJson() const
    {
        // document is the root of a JSON message
        rapidjson::Document document;

        // Define the document as an object
        document.SetObject();

        // Must pass an allocator when the object may need to allocate memory
        rapidjson::Document::AllocatorType &allocator = document.GetAllocator();

        document.AddMember("MsgType", TIMING_PLAN_MSG_TYPE, allocator);

        // Create a rapidjson timing plan object ytpe
        rapidjson::Value timing_plan_obj_value(rapidjson::kObjectType);
        timing_plan_obj_value.AddMember("NoOfPhase", number_of_phase, allocator);

        // Create a rapidjson array type for number of phases
        rapidjson::Value phase_number_array_value(rapidjson::kArrayType);

        // Add an array of phase number to JSON array
        for (const auto &phase_num : phase_number_v)
        {
            phase_number_array_value.PushBack(phase_num, allocator);
        }
        timing_plan_obj_value.AddMember("PhaseNumber", phase_number_array_value, allocator);

        // Create a rapidjson array type for pedestrian walk
        rapidjson::Value pede_walk_array_value(rapidjson::kArrayType);

        // Add an array of pedestrian walk to rapidjson array
        for (const auto &ped_walk : pedestrian_walk_v)
        {
            pede_walk_array_value.PushBack(ped_walk, allocator);
        }
        timing_plan_obj_value.AddMember("PedWalk", pede_walk_array_value, allocator);

        // Create a rapidjson array type for pedestrian clear
        rapidjson::Value pede_clear_array_value(rapidjson::kArrayType);

        // Add an array of pedestrian clear to rapidjson array
        for (const auto &pede_clear : pedestrian_clear_v)
        {
            pede_clear_array_value.PushBack(pede_clear, allocator);
        }
        timing_plan_obj_value.AddMember("PedClear", pede_clear_array_value, allocator);

        // Create a rapidjson array type for minimum green
        rapidjson::Value min_green_array_value(rapidjson::kArrayType);

        // Add an array of pedestrian clear to rapidjson array
        for (const auto &min_green : min_green_v)
        {
            min_green_array_value.PushBack(min_green, allocator);
        }
        timing_plan_obj_value.AddMember("MinGreen", min_green_array_value, allocator);

        // Create a rapidjson array type for passage
        rapidjson::Value passage_array_value(rapidjson::kArrayType);

        // Add an array of pedestrian clear to rapidjson array
        for (const auto &passage : passage_v)
        {
            passage_array_value.PushBack(passage, allocator);
        }
        timing_plan_obj_value.AddMember("Passage", passage_array_value, allocator);

        // Create a rapidjson array type for maximum green
        rapidjson::Value max_green_array_value(rapidjson::kArrayType);

        // Add an array of pedestrian clear to rapidjson array
        for (const auto &max_green : max_green_v)
        {
            max_green_array_value.PushBack(max_green, allocator);
        }
        timing_plan_obj_value.AddMember("MaxGreen", max_green_array_value, allocator);

        // Create a rapidjson array type for yellow change
        rapidjson::Value yellow_change_array_value(rapidjson::kArrayType);

        // Add an array of pedestrian clear to rapidjson array
        for (const auto &yellow_change : yellow_change_v)
        {
            yellow_change_array_value.PushBack(yellow_change, allocator);
        }
        timing_plan_obj_value.AddMember("YellowChange", yellow_change_array_value, allocator);

        // Create a rapidjson array type for red clear
        rapidjson::Value red_clear_array_value(rapidjson::kArrayType);

        // Add an array of pedestrian clear to rapidjson array
        for (const auto &red_clear : red_clear_v)
        {
            red_clear_array_value.PushBack(red_clear, allocator);
        }
        timing_plan_obj_value.AddMember("RedClear", red_clear_array_value, allocator);

        // Create a rapidjson array type for phase ring
        rapidjson::Value phase_ring_array_value(rapidjson::kArrayType);

        // Add an array of pedestrian clear to rapidjson array
        for (const auto &phase_ring : phase_ring_v)
        {
            phase_ring_array_value.PushBack(phase_ring, allocator);
        }
        timing_plan_obj_value.AddMember("PhaseRing", phase_ring_array_value, allocator);
        document.AddMember("TimingPlan", timing_plan_obj_value, allocator);
        return document;
    }
}