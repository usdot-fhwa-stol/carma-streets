#pragma once

#include <stdexcept>
#include <rapidjson/document.h>

namespace streets_utils::json_utils {
     class json_document_parse_error : public std::runtime_error {
          public:
               explicit json_document_parse_error(const std::string &msg, const rapidjson::Document &doc ) : 
                         std::runtime_error(msg  + "\nError :" +  std::to_string(doc.GetParseError()) + " Offset: " + std::to_string(doc.GetErrorOffset())){};
            
     };
}