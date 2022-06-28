# Streets Signal Phase and Timing Library

## Introduction
This CARMA-Streets library is meant to handle JSON serialization and deserialization for the CARMA-Streets internal SPaT JSON message. The message structure is heavily based on the J2735 SPaT message but does not contain any regional extension data since we are currently not using it. Below is a sample SPaT JSON message created by this library.
```
{
    "timestamp":0,
    "name":"",
    "intersection_state_list":[
        {
            "name":"West Intersection",
            "id":1909,
            "message_count":12332,
            "status":"3e21f0923e21f092",
            "minute_of_the_year":34232,
            "second":13,
            "enabled_lane_list":[1,3,5],
            "states":[
                {
                    "movement_name":"Right Turn",
                    "signal_group_id":4,
                    "movement_event_list":[
                        {
                            "event_state":3,
                            "timing":{
                                "start_time":0,  
                                "min_end_time":0,
                                "max_end_time":0,
                                "likely_time":0,
                                "confidence":0
                            },
                            "speeds":[
                                {
                                    "type":0,
                                    "speed_limit":4,
                                    "speed_confidence":1,
                                    "distance":5,
                                    "vehicle_class":5
                                }
                            ]
                        }],
                        "maneuver_assist_list":[
                            {
                                "connection_id":7,
                                "queue_length":4,
                                "available_storage_length":8,
                                "wait_on_stop":true,
                                "ped_bicycle_detect":false
                            }]
                }],
                "maneuver_assist_list":[
                    {
                        "connection_id":7,
                        "queue_length":4,
                        "available_storage_length":8,
                        "wait_on_stop":true,
                        "ped_bicycle_detect":false
                    }]
        }]
}
```
## SPaT Traffic Signal State Information
The two relevant portions of the SPaT JSON message for managing traffic signal **states** MovementList and the **state-time-speed** MovementEventList. The **states** MovementList is a list of MovementStates. Each describes the state of a particular movement in the intersection. These can be identified by their **signal_group_id** which is the unique identifier for the signal group that controls this movement. Inside each MovementState there is a **state-time-speed** MovementEventList. This is a list of MovementEvents. Each MovementEvent has information about the phase as a MovementPhaseState enumeration and timing information about the start and end time of the described phase. Each MovementEventList can hold 16 possible MovementEvents (limitation of J2735 not this data type) to describe a current phase and up to 15 future phases. Below is an example JSON for the described objects.
```
"states":[
    {
        "signal_group":1,
        "state-time-speed":[
            {
                "event_state":3,  // RED light see movement_phase_state
                "timing":{
                    "start_time":0,  
                    "min_end_time":10000
                }
            },
            {
                "event_state":6,  // Protected GREEN light see movement_phase_state
                "timing":{
                    "start_time":10000,  
                    "min_end_time":25000
                }
            },
            {
                "event_state":8,  // Protected YELLOW light see movement_phase_state
                "timing":{
                    "start_time":25000,  
                    "min_end_time":30000
                }
            }]
    },
     {
        "signal_group":2,
        "state-time-speed":[
            {
                "event_state":6,  // Protect GREEN light see movement_phase_state
                "timing":{
                    "start_time":0,  
                    "min_end_time":5000
                }
            },
            {
                "event_state":8,  // Protected YELLOW light see movement_phase_state
                "timing":{
                    "start_time":5000,  
                    "min_end_time":10000
                }
            },
            {
                "event_state":3,  // RED light see movement_phase_state
                "timing":{
                    "start_time":10000,  
                    "min_end_time":30000
                }
            }]
    }
]
```

## Including Library
To include this library in your CARMA-Streets service simply add the following to your CMakeList.txt.
```
find_package( streets_signal_phase_and_timing_lib REQUIRED )
...
target_link_libraries( <target> PUBLIC streets_signal_phase_and_timing_lib )
```