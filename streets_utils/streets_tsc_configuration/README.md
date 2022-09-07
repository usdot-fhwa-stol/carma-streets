# Streets TSC Configuration Library

## Introduction
This CARMA-Streets library is meant to handle JSON serialization and deserialization for the CARMA-Streets internal tsc_configuration_state JSON message. Below is a sample tsc_configuration_state JSON message created by this library.
```
{
    "tsc_config_list":[
        {
            "signal_group_id": 1, 
            "yellow_change_duration":1000,
            "red_clearance":500,
            "concurrent_signal_groups":[5,6]
        },
        {
            "signal_group_id": 2,
            "yellow_change_duration":2000,
            "red_clearance":300,
            "concurrent_signal_groups":[5,6]
        },
        {
            "signal_group_id": 7,
            "yellow_change_duration":2000,
            "red_clearance":300
        },
}
```
The third entry in the list shows a signal group with no concurrent phases. The concurrent_signal groups field is an optional argument. In case of a single ring structure there are not expected to be any concurrent groups and a json message can be constructed without it.

# TSC Configuration State Information
Traffic signal controller configuration represents the setup on the physical device. In order to access that information SNMP requests need to be sent to the system. In order to control the current phases/signal_groups on the controller, this information is required to understand ring-barrier setups as well as the duration of yellow and red clearance, which cannot be controlled directly.The tsc_config_state message consists of this information for all **active vehicle phases/signal_groups**.

### Parameter Description

Signal Group ID: signal group id identifier for J2735 which is mapped to phase number in NTCIP

Yellow_change_duration: Yellow signal duration for phase in milliseconds

Red_clearance: Red clearace time for phase in milliseconds

Concurrent_signal_groups: Signal Groups in the same barrier or concurrent group excluding signal groups from same ring


## Including Library
To include this library in your CARMA-Streets service simply add the following to your CMakeList.txt.
```
find_package( streets_tsc_configuration_lib REQUIRED )
...
target_link_libraries( <target> PUBLIC streets_tsc_configuration_lib )
