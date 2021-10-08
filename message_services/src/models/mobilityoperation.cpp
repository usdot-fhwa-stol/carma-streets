#include <iostream>
#include <sstream>
#include <map>

#include <boost/algorithm/string.hpp>
#include "mobilityoperation.h"

namespace message_services
{

    namespace models
    {

        mobilityoperation::mobilityoperation() : strategy(""), strategy_params(""), header()
        {
        }

        mobilityoperation::~mobilityoperation()
        {
        }

        void mobilityoperation::fromJsonObject(const rapidjson::Value &obj)
        {
            if (obj.IsObject())
            {
                for (rapidjson::Value::ConstMemberIterator obj_itr = obj.MemberBegin(); obj_itr != obj.MemberEnd(); obj_itr++)
                {
                    if (obj_itr->value.IsObject() && boost::iequals(std::string(obj_itr->name.GetString()), std::string("header")))
                    {
                        mobility_header_t header;

                        for (rapidjson::Value::ConstMemberIterator header_itr = obj_itr->value.MemberBegin(); header_itr != obj_itr->value.MemberEnd(); header_itr++)
                        {
                            if (std::string(header_itr->name.GetString()) == std::string("sender_id"))
                            {
                                header.sender_id = header_itr->value.GetString();
                            }

                            if (std::string(header_itr->name.GetString()) == std::string("sender_bsm_id"))
                            {
                                header.sender_bsm_id = header_itr->value.GetString();
                            }

                            if (std::string(header_itr->name.GetString()) == std::string("recipient_id"))
                            {
                                header.recipient_id = (header_itr->value.GetString());
                            }

                            if (std::string(header_itr->name.GetString()) == std::string("plan_id"))
                            {
                                header.plan_id = (header_itr->value.GetString());
                            }
                            if (std::string(header_itr->name.GetString()) == std::string("timestamp"))
                            {
                                header.timestamp = (header_itr->value.GetUint64());
                            }
                        }
                        setHeader(header);
                    }

                    if (obj_itr->value.IsString() && boost::iequals(std::string(obj_itr->name.GetString()), std::string("strategy")))
                    {
                        setStrategy(obj_itr->value.GetString());
                    }

                    if (obj_itr->value.IsString() && boost::iequals(std::string(obj_itr->name.GetString()), std::string("strategy_params")))
                    {
                        setStrategy_params(obj_itr->value.GetString());
                    }
                }
            }
        }

        bool mobilityoperation::asJsonObject(rapidjson::Writer<rapidjson::StringBuffer> *writer) const
        {
            try
            {
                writer->StartObject();
                writer->Key("header");
                writer->StartObject();
                writer->Key("sender_id");
                writer->String(this->getHeader().sender_id.c_str());
                writer->Key("recipient_id");
                writer->String(this->getHeader().recipient_id.c_str());
                writer->Key("plan_id");
                writer->String(this->getHeader().plan_id.c_str());
                writer->Key("timestamp");
                writer->Uint64(this->getHeader().timestamp);
                writer->EndObject();
                writer->Key("strategy_params");
                writer->String(this->getStrategy_params().c_str());
                writer->Key("strategy");
                writer->String(this->getStrategy().c_str());
                writer->EndObject();
                return true;
            }
            catch (std::exception &ex)
            {
                spdlog::critical("mobilityoperation model: {0} ", ex.what());
                return false;
            }
        }

        std::string mobilityoperation::get_value_from_strategy_params(std::string key) const
        {
            const char *delimiter = ",:";
            std::string key_value_str = "";
            std::vector<std::string> key_value_v = {};
            std::stringstream strategy_params_stream(this->strategy_params);
            //strategy_params are key value pairs separate by comma
            while (std::getline(strategy_params_stream, key_value_str, *delimiter))
            {
                std::stringstream key_value_str_stream(key_value_str);
                std::string key_or_value = "";

                while (std::getline(key_value_str_stream, key_or_value, *(delimiter + 1)))
                {

                    key_value_v.push_back(key_or_value);
                    if (key_or_value == key)
                    {
                        return key_value_v.back();
                    }
                }

                if (key_value_v.size() > 0)
                {
                    key_value_v.clear();
                }
            }
            return strategy_params_stream;
        }

        std::ostream &operator<<(std::ostream &out, mobilityoperation &mobilityoperation_obj)
        {
            std::cout << std::fixed;
            std::cout << std::setprecision(6);
            std::cout << "mobilityoperation model: sender_id =  " << mobilityoperation_obj.header.sender_id
                      << ", sender_bsm_id =" << mobilityoperation_obj.header.sender_bsm_id
                      << ", timestamp = " << mobilityoperation_obj.header.timestamp
                      << ", sender id = " << mobilityoperation_obj.header.sender_id << ", strategy= " << mobilityoperation_obj.strategy
                      << ", strategy_params= " << mobilityoperation_obj.strategy_params << std::endl;
        }

        mobility_header_t mobilityoperation::getHeader() const
        {
            return this->header;
        }
        void mobilityoperation::setHeader(mobility_header_t header)
        {
            this->header = header;
        }
        std::string mobilityoperation::getStrategy_params() const
        {
            return this->strategy_params;
        }
        void mobilityoperation::setStrategy_params(std::string strategy_params)
        {
            this->strategy_params = strategy_params;
        }
        std::string mobilityoperation::getStrategy() const
        {
            return this->strategy;
        }
        void mobilityoperation::setStrategy(std::string strategy)
        {
            this->strategy = strategy;
        }

    }
}
