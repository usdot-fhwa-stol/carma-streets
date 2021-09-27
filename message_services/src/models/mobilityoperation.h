#pragma once

#include <spdlog/spdlog.h>
#include <iomanip>

#include "objBase.h"
#include "mobilityHeader.h"
namespace message_services
{

    namespace models
    {

        class mobilityoperation : public objBase
        {
            friend std::ostream &operator<<(std::ostream &out, mobilityoperation &mobilityoperation_obj);

        private:
            mobility_header_t header;
            std::string strategy = "";
            std::string strategy_params = "";

        public:
            mobilityoperation(/* args */);
            virtual ~mobilityoperation();

            virtual void fromJsonObject(const rapidjson::Value &obj);
            virtual bool asJsonObject(rapidjson::Writer<rapidjson::StringBuffer> *writer) const;

            std::string get_value_from_strategy_params(std::string key) const;

            std::string getStrategy_params() const;
            void setStrategy_params(std::string strategy_params);
            std::string getStrategy() const;
            void setStrategy(std::string strategy);

            mobility_header_t getHeader() const;
            void setHeader(mobility_header_t header);
        };
    }

}