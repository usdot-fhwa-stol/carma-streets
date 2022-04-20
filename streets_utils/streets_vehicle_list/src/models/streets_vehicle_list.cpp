#include "streets_vehicle_list.h"

namespace streets_vehicles {

    streets_vehicle_list::streets_vehicle_list() {
    }

    std::vector<streets_vehicle> streets_vehicle_list::get_vehicles_for_entry_lane(int lane_id ) {
        std::vector<streets_vehicle> vehicles_in_entry_lane;
        for ( auto it = vehicles.begin(); it != vehicles.end(); it ++ ) {
            streets_vehicle veh =it->second;
        }
        return vehicles_in_entry_lane;
    }



    
}