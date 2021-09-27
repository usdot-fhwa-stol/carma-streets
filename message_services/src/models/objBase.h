#pragma once
#include <rapidjson/document.h>
#include <string>
#include <rapidjson/istreamwrapper.h>
#include <fstream>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/writer.h>
#include <spdlog/spdlog.h>
#include <iostream>

#include "rapidjson/rapidjson.h"
#include "rapidjson/document.h"     // rapidjson's DOM-style API
#include "rapidjson/stringbuffer.h" // wrapper of C stream for prettywriter as output
#include "rapidjson/prettywriter.h"
namespace message_services
{

    namespace models
    {
        class objBase
        {
        public:
            virtual bool asJsonObject(rapidjson::Writer<rapidjson::StringBuffer> *writer) const = 0;

            virtual std::string asJson() const;

            virtual bool fromJson(const std::string &jsonString);

            virtual void fromJsonObject(const rapidjson::Value &obj) = 0;
        };

    }
}