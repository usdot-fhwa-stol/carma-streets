# Streets Vehicle Scheduler Library

## Introduction
This library is intended to contain the vehicle scheduling business logic for **CARMA Streets**. This library consists of two main classes: the **intersection_schedule** struct and the **vehicle_scheduler** abstract class.

### Intersection Schedule 
The intersection schedule is the data object that holds the **vehicle_schedule(s)** for all vehicles in the intersection. Each **vehicle_schedule** contains the **vehicle ID** `v_id`, the current planned **stopping time** `st`, the current planned **entering time** `et`, the current planned **departure time** `dt`, the current planned **departure position** `dp`, and a boolean flag indicating whether the vehicle has **access** to the intersection `access`. Below is an sample JSON for a stopped controlled intersection. 
**Note** : The JSON serialization will need to be overriden for **signalized** intersection_type for UC 3.
```
{
  "metadata":{
    "timestamp": 12312, // Timestamp in milliseconds 
    "intersection_type":"stop_controlled" // Type of intersection for which the schedule applies
  },
  "payload": [
    {
      "v_id": "DOT-608",
      "st": 123123,
      "et": 123123,
      "dt": 123213,
      "dp": 2,
      "access": 0
    },
      {
      "v_id": "DOT-513",
      "st": 123123,
      "et": 123123,
      "dt": 123213,
      "dp": 2,
      "access": 0
    }
  ]  
}
```
### Vehicle scheduler
The `vehicle_scheduler` class is an abstract class meant to be extended to implement custom scheduling logic (see `all_stop_vehicle_scheduler`). This class contains a protected members that stores information about intersection geometry (`intersection_info`) . To implement custom scheduling business logic simply extend this class and implement the virtual method `schedule_vehicles` which takes in a map of `vehicles` with vehicle string IDs as keys and an empty `intersection_schedule`. Using pass by reference, the implemented method should populate this intersection schedule with schedules for all the relevant vehicles in the intersection. Below is an example custom vehicle scheduler.

```
class custom_vehicle_scheduler : public vehicle_scheduler {
  ...

  void schedule_vehicles( std::unordered_map<std::string,streets_vehicles::vehicle> &vehicles, intersection_schedule &schedule) override {
    // Custom implementation
    // Populate intersection_schedule with vehicle_schedule(s) for relevant vehicles included in vehicles map

  }
}
```

Below is an example of using a custom vehicle scheduler.
```
intersection_info_ptr = std::make_shared<OpenAPI::OAIIntersection_info>(); // Note: obtaining intersection information requires using intersection_client_api.
                                                                           // Please refer to streets_utils/streets_api/intersection_client_api for 
                                                                           // documentation about retreiving intersection information from intersection model

vehicle_list_ptr = std::make_shared<streets_vehicles::vehicle_list>();     // Note: obtaining vehicle information requires using streets_vehicle_list library.
                                                                           // Please refer to streets_utils/streets_vehicle_list/ for documentation about 
                                                                           //  using vehicle list to consume status and intent msgs
// Create vehicle_scheduler ptr and use custom implementation
std::shared_ptr<streets_vehicle_scheduler::vehicle_scheduler> scheduler_ptr = std::make_shared<streets_vehicle_scheduler::custom_vehicle_scheduler>();
// Set intersection information for scheduler
scheduler_ptr->set_intersection_info(intersection_info_ptr);
// Get current vehicles from vehicle_list
auto veh_map = _vehicle_list_ptr -> get_vehicles();
// schedule vehicles
streets_vehicle_scheduler::intersection_schedule int_schedule = _scheduling_worker->schedule_vehicles(veh_map, _scheduler_ptr);
// Serialize schedule to JSON
std::string msg_to_send = int_schedule.toJson();


```
