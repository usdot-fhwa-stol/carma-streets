#include "spat.h"

namespace signal_phase_and_timing{

    std::string spat::toJson() {
        rapidjson::Document doc;
        auto allocator = doc.GetAllocator();
        // Create SPaT JSON value
        rapidjson::Value spat(rapidjson::kObjectType);
        // Read lock
        std::shared_lock lock(spat_lock);
        // Populate SPat JSON
        spat.AddMember("time_stamp", timestamp, allocator);
        spat.AddMember( "name", name, allocator);
        if ( !intersections.empty() ) {
            rapidjson::Value list(rapidjson::kArrayType);
            for (const auto &intersection : intersections ) {
                list.PushBack(intersection.toJson(allocator), allocator);
            }
            spat.AddMember("intersections", list, allocator);
        }else {
            throw signal_phase_and_timing_exception("SPaT message is missing required intersections property!");
        }
        rapidjson::StringBuffer buffer;
        try {
            rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
            spat.Accept(writer);
        }
        catch( const std::exception &e ) {
            throw signal_phase_and_timing_exception(e.what());
        }
        return buffer.GetString();
    }

    void spat::fromJson(const std::string &json )  {
        rapidjson::Document doc;
        doc.Parse(json);
        if (doc.HasParseError()) {
            throw signal_phase_and_timing_exception("SPaT message JSON is misformatted. JSON parsing failed!");  
        }
        // Write lock
        std::unique_lock  lock(spat_lock);
        if ( doc.HasMember("time_stamp") && doc.FindMember("time_stamp")->value.IsUint64()) {
            timestamp =  (uint32_t) doc["time_stamp"].GetUint64(); // OPTIONAL in J2735 SPaT definition
        } 
        if (doc.FindMember("name")->value.IsString() ) {
            name = doc["name"].GetString();  // OPTIONAL see J2735 SPaT definition
        }
        if ( doc.FindMember("intersections")->value.IsArray() ) {
            // REQUIRED see J2735 SPaT definition
            // Clear intersection state list in case it is populated.
            intersections.clear();
            for ( const auto &intersection : doc["intersections"].GetArray() ){
                intersection_state cur_state;
                cur_state.fromJson(intersection);
                intersections.push_back(cur_state);
            }
        }
        else {
            throw signal_phase_and_timing_exception("SPaT message is missing required intersections property!");
        }
            
        
    }

    void spat::update( ntcip::ntcip_1202_ext &ntcip_data, bool use_ntcip_timestamp ){
        if ( phase_to_signal_group.empty() || intersections.front().name.empty() || intersections.front().id == 0) {
            throw signal_phase_and_timing_exception("Before updating SPAT with NTCIP information spat::initialize() must be called! See README documentation!");
        }
        // Write lock
        std::unique_lock lock(spat_lock);
        if ( use_ntcip_timestamp ) {
            set_timestamp_ntcip( ntcip_data.get_timestamp_seconds_of_day(), ntcip_data.spat_timestamp_msec);
        } 
        else {
            set_timestamp_local();
        }
        update_intersection_state( ntcip_data );
        
        
        
    }

    void spat::initialize_intersection(const std::string &intersection_name, const int intersection_id, const std::unordered_map<int,int> &_phase_number_to_signal_group ) {
        // Write lock
        std::unique_lock lock(spat_lock);
        if (!intersections.empty()) {
            intersections.clear();
        }
        intersection_state cur_state;
        cur_state.name = intersection_name;
        cur_state.id =  (uint16_t) intersection_id;
        phase_to_signal_group = _phase_number_to_signal_group;
        cur_state.initialize_movement_states( phase_to_signal_group );
        intersections.push_back(cur_state);
    }

    void spat::set_timestamp_ntcip(const uint32_t second_of_day, const uint16_t millisecond_of_second ) {
        if ( !intersections.empty()) {
            intersection_state &intersection = intersections.front();
            intersection.set_timestamp_ntcip(second_of_day, millisecond_of_second);
            timestamp = intersection.moy;
        }
        else {
            throw signal_phase_and_timing_exception("Intersection State List is empty! Cannot populate timestamp information!");
        }
    }

    void spat::set_timestamp_local() {
        if ( !intersections.empty() ) {
            intersection_state &intersection = intersections.front();
            intersection.set_timestamp_local();
            timestamp =  intersection.moy;
        }
        else {
            throw signal_phase_and_timing_exception("Intersection State List is empty! Cannot populate timestamp information!");
        }
    }

    void spat::update_intersection_state( ntcip::ntcip_1202_ext &ntcip_data ) {
        if ( !intersections.empty() ) {
            intersection_state &intersection = intersections.front();
            intersection.update_movements(ntcip_data, phase_to_signal_group);
            // Update Intersection Status
            intersection.status = ntcip_data.spat_intersection_status;
            // From V2X-Hub TODO: Investigate
            intersection.revision = 1;
        }
        else {
            throw signal_phase_and_timing_exception("Intersection State List is empty! Cannot populate status information!");
        }
    }

    intersection_state spat::get_intersection() {
        std::shared_lock lock(spat_lock);
        if (intersections.empty())
            throw signal_phase_and_timing_exception("No intersection included currently in SPaT!"); 
        return intersections.front();

    }

    bool spat::operator==(const spat &other) const{
        return timestamp == other.timestamp && name == other.name && intersections == other.intersections;
    }

    bool spat::operator!=(const spat &other) const{
        return !operator==(other);
    }
}