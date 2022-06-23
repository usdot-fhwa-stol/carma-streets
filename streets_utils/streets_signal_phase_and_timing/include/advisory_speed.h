#pragma once
#define RAPIDJSON_HAS_STDSTRING 1


#include <rapidjson/rapidjson.h>
#include <rapidjson/document.h>
#include <spdlog/spdlog.h>


namespace signal_phase_and_timing{
    enum class advisory_speed_type{
        none = 0,
        greenwave = 1,
        ecodrive = 2,
        transit = 3
    };

    enum class speed_confidence{
        /**
         * @brief Not equipped or unavailable
         */
        unavailable = 0,
        /**
         * @brief 100 m/s
         */
        pre100ms = 1 ,
        /**
         * @brief 10 m/s
         */
        prec10ms = 2 ,
        /**
         * @brief 5 m/s
         */
        prec5ms = 3 ,
        /**
         * @brief 1 m/s
         */
        prec1ms = 4 ,
        /**
         * @brief 0.1 m/s
         * 
         */
        prec0_1ms = 5,
        /**
         * @brief 0.05 m/s
         */
        prec0_05ms = 6,
        /**
         * @brief 0.01 m/s
         */
        prec0_01ms = 7
    };

    struct advisory_speed{
        /**
         * @brief The type of advisory which this is.
         */
        advisory_speed_type type = advisory_speed_type::none;
        /**
         * @brief Expressed in 0.1 m/s. This element is optional ONLY when superceded by the presence of a regional speed element 
         * found in Reg-AdvisorySpeed entry.
         */
        uint16_t speed_limit = 0;
        /**
         * @brief A confidence value for the above speed.
         */
        speed_confidence confidence = speed_confidence::unavailable;
        /**
         * @brief The distance indicates the region for which the advised speed is recommended, it is specified upstream from the 
         * stop bar along the connected egressing lane (units = meters). 
         */
        uint16_t distance = 0;
        /**
         * @brief Field to describe which class of vehicles the speed advisory applies to. When absent, the advisory applies to all
         * motor vehicles. @see J2735 ASN1 RestrictionClassID definition. Optional
         */
        u_int8_t veh_class = 0;

        /**
         * @brief Serialize Advisory Speed object to rapidjson::Value for writing as JSON string
         * 
         * @return rapidjson::Value serialize Advisory Speed object
         */
        rapidjson::Value toJson(rapidjson::Document::AllocatorType &allocator) const;        /**
         * @brief Deserialize Advisory Speed JSON into Advisory Speed object.
         * 
         * @param val Advisory Speed JSON.
         */
        void fromJson(const rapidjson::Value &val);

        bool operator==(const advisory_speed &compare) const;

        bool operator!=(const advisory_speed &compare) const;
        
    };

    

}