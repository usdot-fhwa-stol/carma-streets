#include "status_intent_processor.h"

namespace streets_vehicles {
    void status_intent_processor::process_status_intent(const rapidjson::Document &doc, vehicle &vehicle ) const{
        if ( doc.IsObject()) {
            from_json( doc.GetObject(), vehicle );
        }
        else{
            throw status_intent_processing_exception("Status and Intent message is messing update object!");
        }
    };

   

    std::string status_intent_processor::get_vehicle_id(const std::string &status_intent_msg, rapidjson::Document &doc) const {
        doc.Parse(status_intent_msg.c_str());
        if (doc.HasParseError()){
            SPDLOG_ERROR("Error  : {0} Offset: {1} ", doc.GetParseError(), doc.GetErrorOffset());
            throw status_intent_processing_exception("Status and Intent message has JSON parse error!");
        }
        if ( doc.FindMember("payload")->value.IsObject() && doc.FindMember("payload")->value.GetObject().FindMember("v_id")->value.IsString()  ) {
            return doc.FindMember("payload")->value.GetObject().FindMember("v_id")->value.GetString();
        }
        else {
            throw status_intent_processing_exception("Status and Intent message has missing/incorrect \"v_id\"!");
        }
    }
}