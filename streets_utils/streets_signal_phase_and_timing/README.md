# Streets Signal Phase and Timing Library

## Introduction
This CARMA-Streets library is meant to handle JSON serialization and deserialization for the CARMA-Streets internal SPaT JSON message. The message structure is heavily based on the J2735 SPaT message but does not contain any regional extension data since we are currently not using it. Below is a sample SPaT JSON message created by this library.
```
{
    "timestamp":0,
    "name":"",
    "intersections":[
        {
            "name":"West Intersection",
            "id":1909,
            "revision":123,
            "status":"01001001001",
            "moy":34232,
            "time_stamp":130,
            "enabled_lanes":[1,3,5],
            "states":[
                {
                    "movement_name":"Right Turn",
                    "signal_group":4,
                    "state_time_speed":[
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
                                    "class":5
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
The two relevant portions of the SPaT JSON message for managing traffic signal **states** MovementList and the **state-time-speed** MovementEventList. The **states** MovementList is a list of MovementStates. Each describes the state of a particular movement in the intersection. These can be identified by their **signal_group_id** which is the unique identifier for the signal group that controls this movement. Inside each MovementState there is a **state-time-speed** MovementEventList. This is a list of MovementEvents. Each MovementEvent has information about the phase as a MovementPhaseState enumeration and timing information about the start and end time of the described phase. Each MovementEventList can hold 16 possible MovementEvents (limitation of J2735 not this data type) to describe a current phase and up to 15 future phases. Each MovementEvent contains **timing** information which will include at minimum, **start_time** and **min_end_time**. These fields will represent the start and end time for each phase and are represented as tenths of a second in the current or next hour in units of 1/10th second from UTC time.A range of 0~36000 covers one hour.The values 35991..35999 are used when a leap second occurs.The value 36000 is used to indicate time >3600 seconds. 36001 is to be used when value undefined or unknown. Note that this is NOT expressed in GPS time or in local time Below is an example JSON for the described objects.(J2735 TimeMark definition).
```
"states":[
    {
        "signal_group":1,
        "state_time_speed":[
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
        "state_time_speed":[
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