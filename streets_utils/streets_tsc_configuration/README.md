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
        }
}
```

# TSC Configuration State Information
Traffic signal controller configuration represents the setup on the physical device. In order to access that information SNMP requests need to be sent to the system. In order to control the current phases/signal_groups on the controller, this information is required to understand ring-barrier setups 
as well as the duration of yellow and red clearance, which cannot be controlled directly.
The tsc_config_state message consists of this information for all **active vehicle phases/signal_groups**.


## Including Library
To include this library in your CARMA-Streets service simply add the following to your CMakeList.txt.
```
find_package( streets_tsc_configuration_lib REQUIRED )
...
target_link_libraries( <target> PUBLIC streets_tsc_configuration_lib )
