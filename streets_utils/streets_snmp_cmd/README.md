# Streets SNMP Command library
## Introduction
This CARMA-streets library is meant to define the streets SNMP command and converts external schedule into SNMP commands.

## Streets SNMP command structure
External MMITSS Roadside processor will communicate with CARMA Streets TSC service via Kafka Message Broker, and send the phase control schedule to the TSC service. Thereafter, the TSC service converts the phase control schedule to Streets SNMP commands and communicate with Traffic Signal Controller to update the controller signal phase and schedule. 
The Streets SNMP command definition:
### <b>Parameter description</b>
#### snmp_cmd_struct definition
| Prameter Name    | Data Type          | Description |
| -------------    | -------------      | ----------- |
| set_val_         | snmp_response_obj  | SNMP response object populated by the SNMP response.|
| start_time_      | uint64_t           | Start execution time of the SNMP command. |
| control_type_    | PHASE_CONTROL_TYPE | The type of SNMP command control sent to TSC. |

#### snmp_response_obj definition
| Prameter Name   | Data Type         | Description |
| -------------   | -------------     | ----------- |
| val_int         | int64_t           | Integer value sent to TSC via SNMP command.|
| val_string      | std::vector<char> | String value sent to TSC via SNMP command. |
| type            | RESPONSE_TYPE     | The type of value being requested or set, on the TSC |


## Including library
```
find_package( streets_snmp_cmd_lib REQUIRED )
...
target_link_libraries( <target> PUBLIC streets_snmp_cmd_lib::streets_snmp_cmd_lib )
```
