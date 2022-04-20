#include "streets_vehicle.h"

namespace street_vehicles {
    std::string streets_vehicle::get_id() const {return id;};

    void streets_vehicle::set_id(const std::string &id) {
        std::unique_lock<std::mutex>(config_lock); 
        this->id = id;
    }

    double streets_vehicle::get_length() const {return length;}

    void streets_vehicle::set_length(double length) {
        std::unique_lock<std::mutex>(config_lock);
        this->length = length;
    }

    double streets_vehicle::get_min_gap() const {return min_gap;}

    void streets_vehicle::set_min_gap( double min_gap) {
        std::unique_lock<std::mutex>(config_lock);
        this->min_gap = min_gap;
    }

    double streets_vehicle::get_reaction_t() const {return reaction_time;}

    void streets_vehicle::set_reaction_time(double reaction_time) {
        std::unique_lock<std::mutex>(config_lock);
        this->reaction_time = reaction_time;
    }

    double streets_vehicle::get_accel_max() const {return accel_max;}

    void streets_vehicle::set_accel_max( double accel_max) {
        std::unique_lock<std::mutex>(config_lock);
        this->accel_max = accel_max;
    }

    double streets_vehicle::get_decel_max() const {return decel_max;}

    void streets_vehicle::set_decel_max( double decel_max ) {
        std::unique_lock<std::mutex>(config_lock);
        this->decel_max = decel_max;
    }

    int streets_vehicle::get_entry_lane_id() const {return entry_lane_id;}

    void streets_vehicle::set_entry_lane_id( int entry_lane_id ) {
        std::unique_lock<std::mutex>(config_lock);
        this->entry_lane_id = entry_lane_id;
    }

    int streets_vehicle::get_link_id() const {return link_id;}

    void streets_vehicle::set_link_id( int link_id ) {
        std::unique_lock<std::mutex>(config_lock);
        this->link_id = link_id;
    }

    int streets_vehicle::get_exit_lane_id() const {return exit_lane_id;}

    void streets_vehicle::set_exit_lane_id( int exit_lane_id ) {
        std::unique_lock<std::mutex>(config_lock);
        this->exit_lane_id = exit_lane_id;
    }

    bool streets_vehicle::get_access() const {return access;}

    void streets_vehicle::set_access( bool access ) {
        std::unique_lock<std::mutex>(config_lock);
        this->access = access;
    } 
    int streets_vehicle::get_departure_position() const {return departure_position;}

    void streets_vehicle::set_departure_position( int departure_position){
        std::unique_lock<std::mutex>(config_lock);
        this->departure_position = departure_position;
    }

    double streets_vehicle::get_actual_st() const {return st_actual;}

    void streets_vehicle::set_actual_st( double st_actual ) {
        std::unique_lock<std::mutex>(config_lock);
        this->st_actual = st_actual;
    }

    double streets_vehicle::get_actual_et() const {return et_actual;}

    void streets_vehicle::set_actual_et( double et_actual ) {
        std::unique_lock<std::mutex>(config_lock);
        this->et_actual = et_actual;
    }

    double streets_vehicle::get_actual_dt() const {return dt_actual;}

    void streets_vehicle::set_actual_dt( double dt_actual ) {
        std::unique_lock<std::mutex>(config_lock);
        this->dt_actual = dt_actual;
    }
    double streets_vehicle::get_cur_time() const {return timestamp;}

    void streets_vehicle::set_cur_time( double timestamp ) {
        std::unique_lock<std::mutex>(config_lock);
        this->timestamp = timestamp;
    }

    double streets_vehicle::get_cur_distance() const {return distance;}

    void streets_vehicle::set_cur_distance( double distance ) {
        std::unique_lock<std::mutex>(config_lock);
        this->distance =  distance;
    }

    double streets_vehicle::get_cur_speed() const {return speed;}

    void streets_vehicle::set_cur_speed( double speed ) {
        std::unique_lock<std::mutex>(config_lock);
        this->speed = speed;
    }

    double streets_vehicle::get_cur_accel() const {return acceleration;}

    void streets_vehicle::set_cur_accel( double accel ) {
        std::unique_lock<std::mutex>(config_lock);
        this->acceleration = accel;
    }

    int streets_vehicle::get_cur_lane_id() const {return lane_id;}

    void streets_vehicle::set_cur_lane_id( int lane_id ) {
        std::unique_lock<std::mutex>(config_lock);
        this->lane_id =  lane_id;
    }

    vehicle_state streets_vehicle::get_cur_state() const {return state;}

    void streets_vehicle::set_cur_state( const vehicle_state &state ) {
        std::unique_lock<std::mutex>(config_lock);
        this->state = state;
    }

    std::vector<future_information> streets_vehicle::get_future_info() const {return future_info;}

    void streets_vehicle::set_future_info( const std::vector<future_information> &future_info ) {
        std::unique_lock<std::mutex>(config_lock);
        this->future_info = future_info;
    }
}







