#include "serializers/sensor_data_sharing_msg_json_serializer.hpp"


namespace streets_utils::messages{
    std::string to_json(const sensor_data_sharing_msg &msg) {
        rapidjson::Document doc;
        rapidjson::Value sdsm_json(rapidjson::kObjectType);
        sdsm_json.AddMember("msg_cnt", msg._msg_count, doc.GetAllocator());
        sdsm_json.AddMember("source_id", msg._source_id, doc.GetAllocator());
        sdsm_json.AddMember("equipement_type", static_cast<int>(msg._equipment_type), doc.GetAllocator());
        // Construct SDSM Time Stamp JSON Object
        auto time_stamp_json = create_timestamp(msg._time_stamp, doc.GetAllocator());
        sdsm_json.AddMember("sdsm_time_stamp", time_stamp_json, doc.GetAllocator());
        // Construct reference position JSON Object
        auto position_3d_json = create_position_3d( msg._ref_positon, doc.GetAllocator() );
        sdsm_json.AddMember("ref_pos", position_3d_json, doc.GetAllocator());
        sdsm_json.AddMember("ref_pos_xy_conf", rapidjson::Value(rapidjson::kObjectType), doc.GetAllocator());
        if ( msg._ref_position_elavation_confidence.has_value() )
            sdsm_json.AddMember("ref_pos_el_conf", msg._ref_position_elavation_confidence.value(), doc.GetAllocator());
        // Construct object list
        sdsm_json.AddMember("objects", create_detected_object_list(msg._objects, doc.GetAllocator()), doc.GetAllocator());

        rapidjson::StringBuffer buffer;
        rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
        sdsm_json.Accept(writer);
    
        return buffer.GetString();
    }

    rapidjson::Value create_timestamp(const time_stamp &val, rapidjson::Document::AllocatorType &allocator){
        rapidjson::Value time_stamp_json(rapidjson::kObjectType);
        time_stamp_json.AddMember("second", val.second, allocator);
        time_stamp_json.AddMember("minute", val.minute, allocator);
        time_stamp_json.AddMember("hour", val.hour, allocator);
        time_stamp_json.AddMember("day", val.day, allocator);
        time_stamp_json.AddMember("month", val.month, allocator);
        time_stamp_json.AddMember("year", val.year, allocator);
        time_stamp_json.AddMember("offset", val.offset, allocator);
        return time_stamp_json;
    }

    rapidjson::Value create_position_3d(const position_3d &val, rapidjson::Document::AllocatorType &allocator){
        rapidjson::Value position_3d_json(rapidjson::kObjectType);
        position_3d_json.AddMember("long", val._longitude, allocator);
        position_3d_json.AddMember("lat", val._latitude, allocator);
        if ( val._elavation.has_value() ) {
            position_3d_json.AddMember("elevation", val._elavation.value(), allocator);
        }
        return position_3d_json;
    }

    rapidjson::Value create_detected_object_list(const std::vector<detected_object_data> &val, rapidjson::Document::AllocatorType &allocator ){
        rapidjson::Value detected_object_list_json(rapidjson::kArrayType);
        for (const auto detected_obect : val) {
            // Create and push detected object data
            detected_object_list_json.PushBack(create_detected_object_data(detected_obect, allocator), allocator);
            
        }
        return detected_object_list_json;
    }

    rapidjson::Value create_detected_object_data(const detected_object_data &val, rapidjson::Document::AllocatorType &allocator){
        rapidjson::Value detected_object_data_json(rapidjson::kObjectType);
        // Create Common Data
        detected_object_data_json.AddMember("detected_object_common_data",create_detected_object_data_common(val._detected_object_common_data,allocator), allocator );
        // Create Optional Data
        if ( val._detected_object_optional_data.has_value() )
            detected_object_data_json.AddMember(
                    "detected_object_data_optional", 
                    create_detected_object_data_optional(val._detected_object_optional_data.value(), allocator), 
                    allocator);
        return detected_object_data_json;
    }


    rapidjson::Value create_detected_object_data_common(const detected_object_data_common &val, rapidjson::Document::AllocatorType &allocator){
        rapidjson::Value detected_object_data_common_json(rapidjson::kObjectType);
        detected_object_data_common_json.AddMember("object_type", static_cast<uint>(val._object_type), allocator);
        detected_object_data_common_json.AddMember("object_id", val._object_id, allocator);
        detected_object_data_common_json.AddMember("obj_type_cfd", val._classification_confidence, allocator);
        detected_object_data_common_json.AddMember("measurement_time", val._time_measurement_offset, allocator);
        detected_object_data_common_json.AddMember("time_confidence", static_cast<uint>(val._time_confidence), allocator);
        // Create Position 
        detected_object_data_common_json.AddMember(
                    "pos", 
                    create_position_3d( val._position_offset, allocator), 
                    allocator);
        // Create Position Confidence
        detected_object_data_common_json.AddMember("pos_confidence", create_position_confidence_set(val._pos_confidence, allocator), allocator );
        detected_object_data_common_json.AddMember("speed", val._speed, allocator);
        detected_object_data_common_json.AddMember("speed_confidence", static_cast<uint>(val._speed_confidence), allocator);
        if ( val._speed_z.has_value())
            detected_object_data_common_json.AddMember("speed_z", val._speed_z.value(), allocator);
        if ( val._speed_z_confidence.has_value())
            detected_object_data_common_json.AddMember(
                    "speed_confidence_z", 
                    static_cast<uint>(val._speed_z_confidence.value()),
                    allocator);
        detected_object_data_common_json.AddMember("heading", val._heading, allocator);
        detected_object_data_common_json.AddMember("heading_conf", static_cast<uint>(val._heading_confidence), allocator);
        if (val._acceleration_confidence.has_value() ) {
            detected_object_data_common_json.AddMember(
                    "acc_cfd_x", 
                    static_cast<uint>(val._acceleration_confidence.value()._lateral_confidence),
                    allocator);
            detected_object_data_common_json.AddMember(
                    "acc_cfd_y", 
                    static_cast<uint>(val._acceleration_confidence.value()._longitudinal_confidence),
                    allocator);
            detected_object_data_common_json.AddMember(
                    "acc_cfd_z", 
                    static_cast<uint>(val._acceleration_confidence.value()._vertical_confidence),
                    allocator);
            detected_object_data_common_json.AddMember(
                    "acc_cfd_yaw", 
                    static_cast<uint>(val._acceleration_confidence.value()._yaw_rate_confidence),
                    allocator);
        }
        if (val._acceleration_4_way.has_value()) {
            // Create accel_4_way
            detected_object_data_common_json.AddMember(
                    "accel_4_way", 
                    create_accelaration_set_4_way(val._acceleration_4_way.value(), allocator), 
                    allocator);
        }
        return detected_object_data_common_json;
    }

    rapidjson::Value create_accelaration_set_4_way(const acceleration_set_4_way &val, rapidjson::Document::AllocatorType &allocator) {
        rapidjson::Value accelaration_set_4_way_json(rapidjson::kObjectType);
        accelaration_set_4_way_json.AddMember("lat", val._lateral_accel, allocator);
        accelaration_set_4_way_json.AddMember("long", val._longitudinal_accel, allocator);
        accelaration_set_4_way_json.AddMember("vert", val._vertical_accel, allocator);
        accelaration_set_4_way_json.AddMember("yaw", val._yaw_rate, allocator);
        return accelaration_set_4_way_json;
    }

    rapidjson::Value create_position_3d(const position_offset &val, rapidjson::Document::AllocatorType &allocator){
        rapidjson::Value position_3d_json(rapidjson::kObjectType);
        position_3d_json.AddMember("offset_x", val._offset_x, allocator);
        position_3d_json.AddMember("offset_y", val._offset_y, allocator);
        position_3d_json.AddMember("offset_z", val._offset_z, allocator);
        return position_3d_json;
    }

    rapidjson::Value create_position_confidence_set(const position_confidence_set &val, rapidjson::Document::AllocatorType &allocator) {
        rapidjson::Value position_confidence_json(rapidjson::kObjectType);
        position_confidence_json.AddMember("pos",static_cast<uint>(val._position_confidence), allocator);
        position_confidence_json.AddMember("elavation", static_cast<uint>(val._elavation_confidence), allocator);
        return position_confidence_json;
    }
    rapidjson::Value create_detected_object_data_optional(const std::variant<detected_obstacle_data, detected_vehicle_data, detected_vru_data> &val, rapidjson::Document::AllocatorType &allocator){
        rapidjson::Value detected_object_data_optional_json(rapidjson::kObjectType);
        return detected_object_data_optional_json;
    }


}