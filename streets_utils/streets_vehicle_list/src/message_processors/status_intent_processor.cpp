#include "status_intent_processor.h"

namespace streets_vehicles {
    void status_intent_processor::process_status_intent(const std::string &status_intent_msg, streets_vehicle &vehicle ) const{
        rapidjson::Document document;

        document.Parse(status_intent_msg.c_str());
        if (document.HasParseError()){
            SPDLOG_ERROR("Error  : {0} Offset: {1} ", document.GetParseError(), document.GetErrorOffset());
            throw status_intent_processing_exception("Status and Intent message has JSON parse error!");
        }
        process_status_intent(document, vehicle);
    };

    void status_intent_processor::process_status_intent(const rapidjson::Value &status_intent_msg, streets_vehicle &vehicle) const{
        if ( status_intent_msg.IsObject()) {
            from_json( status_intent_msg.GetObject(), vehicle );
        }
        else{
            throw status_intent_processing_exception("Status and Intent message is messing update object!");
        }
    }
}