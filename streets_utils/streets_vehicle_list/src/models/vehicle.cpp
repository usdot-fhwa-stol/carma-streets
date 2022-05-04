#include "vehicle.h"

namespace streets_vehicles {

    
    vehicle& vehicle::operator=(const vehicle &veh) {
        _id = veh.get_id();
        _length = veh.get_length();
        _min_gap =  veh.get_min_gap();
        _reaction_time =veh.get_reaction_t();
        _accel_max = veh.get_accel_max();
        _decel_max = veh.get_decel_max();
        _entry_lane_id = veh.get_entry_lane_id();
        _link_id =veh.get_link_id();
        _exit_lane_id = veh.get_exit_lane_id();
        _lane_id = veh.get_cur_lane_id();
        _access = veh.get_access();
        _departure_position = veh.get_departure_position();
        _st_actual = veh.get_actual_st();
        _et_actual = veh.get_actual_et();
        _dt_actual = veh.get_actual_dt();
        _timestamp = veh.get_cur_time();
        _distance = veh.get_cur_distance();
        _speed = veh.get_cur_speed();
        _acceleration = veh.get_cur_accel();
        _state = veh.get_cur_state();
        _future_info = veh.get_future_info();
        return *this;
    }

    std::string vehicle::get_id() const {return _id;};

    void vehicle::set_id(const std::string &id) {
        std::unique_lock<std::mutex> lock(_config_lock); 
        _id = id;
    }

    double vehicle::get_length() const {return _length;}

    void vehicle::set_length(const double length) {
        std::unique_lock<std::mutex> lock(_config_lock);
        _length = length;
    }

    double vehicle::get_min_gap() const {return _min_gap;}

    void vehicle::set_min_gap( const double min_gap) {
        std::unique_lock<std::mutex> lock(_config_lock);
        _min_gap = min_gap;
    }

    double vehicle::get_reaction_t() const {return _reaction_time;}

    void vehicle::set_reaction_time( const double reaction_time) {
        std::unique_lock<std::mutex> lock(_config_lock);
        _reaction_time = reaction_time;
    }

    double vehicle::get_accel_max() const {return _accel_max;}

    void vehicle::set_accel_max( const double accel_max) {
        std::unique_lock<std::mutex> lock(_config_lock);
        _accel_max = accel_max;
    }

    double vehicle::get_decel_max() const {return _decel_max;}

    void vehicle::set_decel_max( const double decel_max ) {
        std::unique_lock<std::mutex> lock(_config_lock);
        _decel_max = decel_max;
    }

    int vehicle::get_entry_lane_id() const {return _entry_lane_id;}

    void vehicle::set_entry_lane_id( const int entry_lane_id ) {
        std::unique_lock<std::mutex> lock(_config_lock);
        _entry_lane_id = entry_lane_id;
    }

    int vehicle::get_link_id() const {return _link_id;}

    void vehicle::set_link_id( const int link_id ) {
        std::unique_lock<std::mutex> lock(_config_lock);
        _link_id = link_id;
    }

    std::string vehicle::get_direction() const {return _direction;}

    void vehicle::set_direction(const std::string &direction) {
        std::unique_lock<std::mutex> lock(_config_lock);
        _direction = direction;
    }

    int vehicle::get_link_priority() const {return _link_priority;}

    void vehicle::set_link_priority(const int link_priority) {
        std::unique_lock<std::mutex> lock(_config_lock);
        _link_priority = link_priority;
    }

    int vehicle::get_exit_lane_id() const {return _exit_lane_id;}

    void vehicle::set_exit_lane_id( const int exit_lane_id ) {
        std::unique_lock<std::mutex> lock(_config_lock);
        _exit_lane_id = exit_lane_id;
    }

    bool vehicle::get_access() const {return _access;}

    void vehicle::set_access( const bool access ) {
        std::unique_lock<std::mutex> lock(_config_lock);
        _access = access;
    } 
    int vehicle::get_departure_position() const {return _departure_position;}

    void vehicle::set_departure_position( const int departure_position){
        std::unique_lock<std::mutex> lock(_config_lock);
        _departure_position = departure_position;
    }

    double vehicle::get_actual_st() const {return _st_actual;}

    void vehicle::set_actual_st( const double st_actual ) {
        std::unique_lock<std::mutex> lock(_config_lock);
        _st_actual = st_actual;
    }

    double vehicle::get_actual_et() const {return _et_actual;}

    void vehicle::set_actual_et( const double et_actual ) {
        std::unique_lock<std::mutex> lock(_config_lock);
        _et_actual = et_actual;
    }

    double vehicle::get_actual_dt() const {return _dt_actual;}

    void vehicle::set_actual_dt( const double dt_actual ) {
        std::unique_lock<std::mutex> lock(_config_lock);
        _dt_actual = dt_actual;
    }
    double vehicle::get_cur_time() const {return _timestamp;}

    void vehicle::set_cur_time( const double timestamp ) {
        std::unique_lock<std::mutex> lock(_config_lock);
        _timestamp = timestamp;
    }

    double vehicle::get_cur_distance() const {return _distance;}

    void vehicle::set_cur_distance( const double distance ) {
        std::unique_lock<std::mutex> lock(_config_lock);
        _distance =  distance;
    }

    double vehicle::get_cur_speed() const {return _speed;}

    void vehicle::set_cur_speed( const double speed ) {
        std::unique_lock<std::mutex> lock(_config_lock);
        _speed = speed;
    }

    double vehicle::get_cur_accel() const {return _acceleration;}

    void vehicle::set_cur_accel( const double accel ) {
        std::unique_lock<std::mutex> lock(_config_lock);
        _acceleration = accel;
    }

    int vehicle::get_cur_lane_id() const {return _lane_id;}

    void vehicle::set_cur_lane_id( const int lane_id ) {
        std::unique_lock<std::mutex> lock(_config_lock);
        _lane_id =  lane_id;
    }

    vehicle_state vehicle::get_cur_state() const {return _state;}

    void vehicle::set_cur_state( const vehicle_state &state ) {
        std::unique_lock<std::mutex> lock(_config_lock);
        _state = state;
    }

    std::vector<future_information> vehicle::get_future_info() const {return _future_info;}

    void vehicle::set_future_info( const std::vector<future_information> &future_info ) {
        std::unique_lock<std::mutex> lock(_config_lock);
        _future_info = future_info;
    }
}







