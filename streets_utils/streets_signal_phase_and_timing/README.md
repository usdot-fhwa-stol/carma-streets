# Streets Signal Phase and Timing Library

## Introduction
This CARMA-Streets library is meant to handle JSON serialization and deserialization for the CARMA-Streets internal SPaT JSON message. The message structure is heavily based on the J2735 SPaT message but does not contain any regional extension data since we are currently not using it. Below is a sample SPaT JSON message created by this library.
```
{
    "time_stamp":130,
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

## Initializing SPaT Object for consuming NTCIP UDP data
This `spat` object contains methods to update based on json SPaT updates or ntcip_1202_ext UDP data. For updating using JSON SPaT messages no initialization is required since each update will contain all the required information. Below is an example of how to consume an update.
```
std::string json; // This string needs to be populated with JSON spat update.
auto spat_ptr = std::make_shared<signal_phase_and_timing::spat>();
// Method to update spat based on SPaT JSON update
spat_ptr->fromJson( json )
```

For consuming **NTCIP** UDP update messages the `spat` requires initialization since it requires mapping information between phase numbers (from NTCIP) to signal group ids (from J2735), intersection name, and intersection id not included in the UDP message. An example below illustrates this process.
```
auto spat_ptr = std::make_shared<signal_phase_and_timing::spat>();

// Initialize methods takes parameters : std::string intersection_name (corresponds to J2735 intersection name from MAP 
// SPaT), int intersection_id (corresponds to J2735 intersection id from MAP and SPaT), std::unordered_hashmap<int,int>
// phase_number_to_signal_group (Map of phase numbers from NTCIP to signal groups defined in MAP. This information can be
// obtained from the channeltable in the NTCIP server on the TSC, see monitor_tsc_state in tsc_service) 

spat_ptr->initialize_intersection( intersection_name, intersection_id, phase_number_to_signal_group)

std::vector<char> udp_packet                            // buffer of udp message( this needs to be populated from UDP socket!)
ntcip::ntcip_1202_ext ntcip_1202_data;                  // struct with byte mapping to values for udp message

// copy buffer into struct
std::memcpy(&ntcip_1202_data, spat_buf.data(), spat_buf.size()); 

spat_ptr->update(ntcip_1202_data, _use_msg_timestamp);  // use struct to update spat, bool flag controls whether to use 
                                                        // host machine unix time(if false) or NTCIP UDP message timestamp 
                                                        // (if true) 
```

## Modifying SPaT Object
The `spat` object has been made thread safe using a internal `shared_mutex` for **read/write** locking behavior. Currently the lock allows concurrent access for **read** (copy constructor) operatons and exclusive access for **write** (changing `spat` object data) operations. 

All fields of `spat` have been made private so to **read** the data inside the `spat` object use the `get_intersection_state()` method which will return and copy of the internal `intersection_state` object that represents the state of the spat at the time this method is called. Since this is a copy of the current state, it will not remain current with the spat data so this method should be called when the data is being used and should be called repeatedly for any calculations that happen on an interval.

In order to **update** the spat object there are currently **3** methods:
- `update(ntcip::ntcip_1202_ext &ntcip_data, bool use_ntcip_timestamp)` is used to update spat using ntcip data from the **Traffic Signal Controller (TSC)**.
- `fromJson(const std::string json)` is used to update spat using a JSON update obtained from kafka.
- `set_intersection(const intersection_state &intersection)` is used to update the spat using an `intersection_state` object. This method can be used for any updates that need to be made to the spat object that come from other sources.
