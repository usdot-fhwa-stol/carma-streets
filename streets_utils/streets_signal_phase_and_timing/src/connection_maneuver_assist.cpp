#include "connection_maneuver_assist.h"

namespace signal_phase_and_timing {
    
    rapidjson::Value connection_maneuver_assist::toJson(rapidjson::Document::AllocatorType &allocator) const {
        // Create connection maneuver assist JSON value
        rapidjson::Value manuever(rapidjson::kObjectType);
        manuever.AddMember("connection_id", connection_id, allocator);
        manuever.AddMember("queue_length", queue_length, allocator);
        manuever.AddMember("available_storage_length", available_storage_length, allocator);
        manuever.AddMember("wait_on_stop", wait_on_stop, allocator);
        manuever.AddMember("ped_bicycle_detect", ped_bicycle_detect, allocator);
        SPDLOG_INFO("Connection Maneuver Assist JSON Created");
        return manuever;
    }

    void connection_maneuver_assist::fromJson( const rapidjson::Value &val ) {
        if ( val.IsObject() ){
            if ( val.FindMember("connection_id")->value.IsInt() ) {
                connection_id =  val["connection_id"].GetInt();
            }
            if ( val.FindMember("queue_length")->value.IsInt() ) {
                queue_length =  val["queue_length"].GetInt();
            }
            if ( val.FindMember("available_storage_length")->value.IsInt() ) {
                available_storage_length =  val["available_storage_length"].GetInt();
            }
            if ( val.FindMember("wait_on_stop")->value.IsBool() ) {
                wait_on_stop =  val["wait_on_stop"].GetBool();
            }
            if ( val.FindMember("ped_bicycle_detect")->value.IsBool() ) {
                ped_bicycle_detect =  val["ped_bicycle_detect"].GetBool();
            }
        }
    }

    bool connection_maneuver_assist::operator==(const connection_maneuver_assist &compare ) const{
        return connection_id == compare.connection_id && queue_length == compare.queue_length 
            && available_storage_length == compare.available_storage_length && wait_on_stop == compare.wait_on_stop
            && ped_bicycle_detect == compare.ped_bicycle_detect;
    }

    bool connection_maneuver_assist::operator!=(const connection_maneuver_assist &compare) const{
        return !operator==(compare);
    }
}