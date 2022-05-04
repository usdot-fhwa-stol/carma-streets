#include "vehicle.h"

namespace streets_vehicles {


    vehicle& vehicle::operator=(const vehicle &veh) {
        this->id = veh.get_id();
        this->length = veh.get_length();
        this->min_gap =  veh.get_min_gap();
        this->reaction_time =veh.get_reaction_t();
        this->accel_max = veh.get_accel_max();
        this->decel_max = veh.get_decel_max();
        this->entry_lane_id = veh.get_entry_lane_id();
        this->link_id =veh.get_link_id();
        this->exit_lane_id = veh.get_exit_lane_id();
        this->lane_id = veh.get_cur_lane_id();
        this->access = veh.get_access();
        this->departure_position = veh.get_departure_position();
        this->st_actual = veh.get_actual_st();
        this->et_actual = veh.get_actual_et();
        this->dt_actual = veh.get_actual_dt();
        this->timestamp = veh.get_cur_time();
        this->distance = veh.get_cur_distance();
        this->speed = veh.get_cur_speed();
        this->acceleration = veh.get_cur_accel();
        this->state = veh.get_cur_state();
        this->future_info = veh.get_future_info();
        return *this;
    }

    std::string vehicle::get_id() const {return id;};

    void vehicle::set_id(const std::string &id) {
        std::unique_lock<std::mutex> lock(config_lock); 
        this->id = id;
    }

    double vehicle::get_length() const {return length;}

    void vehicle::set_length(const double length) {
        std::unique_lock<std::mutex> lock(config_lock);
        this->length = length;
    }

    double vehicle::get_min_gap() const {return min_gap;}

    void vehicle::set_min_gap( const double min_gap) {
        std::unique_lock<std::mutex> lock(config_lock);
        this->min_gap = min_gap;
    }

    double vehicle::get_reaction_t() const {return reaction_time;}

    void vehicle::set_reaction_time( const double reaction_time) {
        std::unique_lock<std::mutex> lock(config_lock);
        this->reaction_time = reaction_time;
    }

    double vehicle::get_accel_max() const {return accel_max;}

    void vehicle::set_accel_max( const double accel_max) {
        std::unique_lock<std::mutex> lock(config_lock);
        this->accel_max = accel_max;
    }

    double vehicle::get_decel_max() const {return decel_max;}

    void vehicle::set_decel_max( const double decel_max ) {
        std::unique_lock<std::mutex> lock(config_lock);
        this->decel_max = decel_max;
    }

    int vehicle::get_entry_lane_id() const {return entry_lane_id;}

    void vehicle::set_entry_lane_id( const int entry_lane_id ) {
        std::unique_lock<std::mutex> lock(config_lock);
        this->entry_lane_id = entry_lane_id;
    }

    int vehicle::get_link_id() const {return link_id;}

    void vehicle::set_link_id( const int link_id ) {
        std::unique_lock<std::mutex> lock(config_lock);
        this->link_id = link_id;
    }

    std::string vehicle::get_direction() const {return direction;}

    void vehicle::set_direction(const std::string &direction) {
        std::unique_lock<std::mutex> lock(config_lock);
        this->direction = direction;
    }

    int vehicle::get_link_priority() const {return link_priority;}

    void vehicle::set_link_priority(const int link_priority) {
        std::unique_lock<std::mutex> lock(config_lock);
        this->link_priority = link_priority;
    }

    int vehicle::get_exit_lane_id() const {return exit_lane_id;}

    void vehicle::set_exit_lane_id( const int exit_lane_id ) {
        std::unique_lock<std::mutex> lock(config_lock);
        this->exit_lane_id = exit_lane_id;
    }

    bool vehicle::get_access() const {return access;}

    void vehicle::set_access( const bool access ) {
        std::unique_lock<std::mutex> lock(config_lock);
        this->access = access;
    } 
    int vehicle::get_departure_position() const {return departure_position;}

    void vehicle::set_departure_position( const int departure_position){
        std::unique_lock<std::mutex> lock(config_lock);
        this->departure_position = departure_position;
    }

    double vehicle::get_actual_st() const {return st_actual;}

    void vehicle::set_actual_st( const double st_actual ) {
        std::unique_lock<std::mutex> lock(config_lock);
        this->st_actual = st_actual;
    }

    double vehicle::get_actual_et() const {return et_actual;}

    void vehicle::set_actual_et( const double et_actual ) {
        std::unique_lock<std::mutex> lock(config_lock);
        this->et_actual = et_actual;
    }

    double vehicle::get_actual_dt() const {return dt_actual;}

    void vehicle::set_actual_dt( const double dt_actual ) {
        std::unique_lock<std::mutex> lock(config_lock);
        this->dt_actual = dt_actual;
    }
    double vehicle::get_cur_time() const {return timestamp;}

    void vehicle::set_cur_time( const double timestamp ) {
        std::unique_lock<std::mutex> lock(config_lock);
        this->timestamp = timestamp;
    }

    double vehicle::get_cur_distance() const {return distance;}

    void vehicle::set_cur_distance( const double distance ) {
        std::unique_lock<std::mutex> lock(config_lock);
        this->distance =  distance;
    }

    double vehicle::get_cur_speed() const {return speed;}

    void vehicle::set_cur_speed( const double speed ) {
        std::unique_lock<std::mutex> lock(config_lock);
        this->speed = speed;
    }

    double vehicle::get_cur_accel() const {return acceleration;}

    void vehicle::set_cur_accel( const double accel ) {
        std::unique_lock<std::mutex> lock(config_lock);
        this->acceleration = accel;
    }

    int vehicle::get_cur_lane_id() const {return lane_id;}

    void vehicle::set_cur_lane_id( const int lane_id ) {
        std::unique_lock<std::mutex> lock(config_lock);
        this->lane_id =  lane_id;
    }

    vehicle_state vehicle::get_cur_state() const {return state;}

    void vehicle::set_cur_state( const vehicle_state &state ) {
        std::unique_lock<std::mutex> lock(config_lock);
        this->state = state;
    }

    std::vector<future_information> vehicle::get_future_info() const {return future_info;}

    void vehicle::set_future_info( const std::vector<future_information> &future_info ) {
        std::unique_lock<std::mutex> lock(config_lock);
        this->future_info = future_info;
    }
}







