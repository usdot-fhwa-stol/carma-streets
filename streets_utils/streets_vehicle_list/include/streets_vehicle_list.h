#pragma once

#include "streets_vehicle.h"

using namespace street_vehicles;

namespace street_vehicles {
    class streets_vehicle_list {
        protected:
            std::unordered_map<std::string, streets_vehicle> vehicles;

        public:
            streets_vehicle_list();
            std::vector<streets_vehicle> get_vehicles();
            std::vector<streets_vehicle> get_vehicles_for_entry_lane(int lane_id);
            void add_vehicle(const streets_vehicle &vehicle);
            void remove_vehicle (const std::string &v_id);


    };
}