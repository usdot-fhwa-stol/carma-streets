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
            "message_count":12332,"
            status":"3e21f0923e21f092",
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

## Including Library