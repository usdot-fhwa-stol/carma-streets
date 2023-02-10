# Streets Vehicle List Library

## Introduction
The `streets_vehicle_list_lib` is meant to store data objects to store information about vehicles in the intersection received from `status_intent_msgs`. It also enables extensible update parsing business logic through the abstract class `status_intent_processor`, allowing for many different implementations of this business logic isolated from the data it modifies. 

## Data Objects
The `vehicle` struct holds all the kinematic information about vehicles and information about their state and intended path in the intersection. 
The `future_info` struct holds kinematic information for a future point in the vehicles path
The `vehicle_list` contains the a map of all the currently active vehicles in the intersection. Vehicles are active as long as the timeout for updates, configured in the status_intent_processor, has not expired. After the timeout expires vehicles are removed from the map. This object offers thread-safe methods to consume json updates `void process_update(const std::string &update)` and access vehicle information : `std::vector<vehicle> get_vehicles_by_state(const vehicle_state state)`, `std::vector<vehicle> get_vehicles_by_lane(const int lane_id)`, and `std::unordered_map<std::string, vehicle> get_vehicles()`.

## Status Intent Processor
The `vehicle_list` object also contains a `shared_ptr<status_intent_processor>`. The `status_intent_processor` is a abstract class with one pure virtual method `void process_update(const std::string &update)`, which allows inheriting classes to implement custom status_intent message parsing and vehicle list updating business logic. Currently the only implementation of this is the `all_stop_status_intent_processor` which holds logic for considering when vehicles are stopped, determining vehicle state based on kinematic information and other UC1 specific business logic. Eventually we will be able to replace this business logic for UC3 by simply replacing the  `all_stop_status_intent_processor` with another class that inherits from `status_intent_process' like 'signalized_status_intent_processor`. This way we don't need to edit the logic in the class directly and can just set the processor in the `vehicle list` and rely on polymorphism.

## Include streets_vehicle_list
Streets Vehicle List CMakeList.txt includes an install target which will install this library as a CMake package. The library along with it's dependencies can then be included by simply using the `find_package()` instruction.
```
find_package(streets_vehicle_list_lib REQUIRED)
...
target_link_library( target PUBLIC streets_vehicle_list)
```
