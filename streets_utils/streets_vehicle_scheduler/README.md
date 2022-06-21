# Streets Vehicle Scheduler Library

## Introduction
This library is intended to contain the vehicle scheduling business logic for **CARMA Streets**. This library consists of two main classes: the **intersection_schedule** struct and the **vehicle_scheduler** abstract class.

### Intersection Schedule 
The intersection schedule is the data object that holds the **vehicle_schedule(s)** for all vehicles in the intersection. Each **vehicle_schedule** contains the **vehicle ID** `v_id`, the current planned **stopping time** `st`, the current planned **entering time** `et`, the current planned **departure time** `dt`, the current planned **departure position** `dp`, and a boolean flag indicating whether the vehicle has **access** to the intersection `access`. Below is an sample JSON
```
{
  "metadata":{
    "timestamp": 12312,
    "intersection_type":"stop_controlled"
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
The `vehicle_scheduler` class is an abstract class meant to be extended to implement custom scheduling logic (see `all_stop_vehicle_scheduler`). This class has some protected members that store information about intersection geometry and other information commonly necessary for scheduling decisions. To implement custom scheduling business logic simply extend this class and implement the virtual method `schedule_vehicles` which takes in a map of `vehicles` with vehicle string IDs as keys and an empty `intersection_schedule`. Using pass by reference, the implemented method should populate this intersection schedule with schedules for all the relavent vehicles in the intersection.
