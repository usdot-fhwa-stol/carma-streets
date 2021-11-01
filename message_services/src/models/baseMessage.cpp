#include "baseMessage.h"
namespace message_services
{
    namespace models
    {

        std::string baseMessage::asJson() const
        {
            rapidjson::StringBuffer ss;
            rapidjson::Writer<rapidjson::StringBuffer> writer(ss);
            if (this->asJsonObject(&writer))
                return ss.GetString();
            return "";
        }

        bool baseMessage::fromJson(const std::string &jsonString)
        {
            rapidjson::Document doc;
            bool has_parse_error = doc.Parse(jsonString.c_str()).HasParseError() ? true : false;

            if (has_parse_error)
            {
                return false;
            }
            this->fromJsonObject(doc);
            return true;
        }
    }
}