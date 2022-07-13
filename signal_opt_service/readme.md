## Singal Optimization Service
### Introduction
The SO (Signal Optimization) service is the CARMA-Streets service responsible for optimizing traffic signal controller phase order and duration. The SO service is responsible for fixing a configurable amount of future phases on the traffic signal controller. On each phase transition (Yellow and Red clearance interval), the SO service will determine a new phase and duration to append to the list of fixed phases. The SO service does this by monitoring traffic signal state and planned state, vehicles within the intersection communication area, and the latest intersection geometry. 

The SO service depends on message service, intersection model and TSC (Traffic Signal Controller) service. It subscribes to message service to get the vehicle status and intent information, calls the [vehicle list library](https://github.com/usdot-fhwa-stol/carma-streets/tree/develop/streets_utils/streets_vehicle_list) to persist the vehicle. It sends HTTP GET requests to intersection model for configured time period to get the latest intersection geometry update, including the signal group ids for each link lanelet. It subscribes to the TSC service outgoing J2735 based JSON SPaT kafka topic to get information about current and future signal state. The JSON SPaT information is processed and persisted using the [streets_signal_phase_and_timing ](https://github.com/usdot-fhwa-stol/carma-streets/tree/develop/streets_utils/streets_signal_phase_and_timing) library .

## Configuration parameters
### Kafka topic names
| Topic Name      | Description |
| ------------    | ----------- |
| tsc_spat_msg_in | The topic is used to transfer message between signal optimization and  TSC (Traffic Signal Controller) services. The message is carma-streets internal [spat messages in JSON format.](https://github.com/usdot-fhwa-stol/carma-streets/tree/develop/streets_utils/streets_signal_phase_and_timing) 
| vehicle_status_intent_output | This topic is used to transfer message between signal optimization and message services. The message format refers to [confluence page.](https://usdot-carma.atlassian.net/wiki/spaces/CRMTSMO/pages/2182873096/CARMA+Streets+Message+Data+Collection) 

### Additional parameters
| Parameter Name | Description |
| -------------- | ----------- |
| sleep_millisecs | The number of milliseconds the current iteration should sleep for.| 
| int_client_request_attempts | The maximum numbers of loop to send HTTP request to intersection model to get the latest intersection geometry information.|

## Implementation
The SO service consist of two main classes. The first is the `signal_opt_messages_worker`. This is a worker class the holds the pointers to all the persisted data. This includes:
- `intersection_info` (intersection geometry), 
- `spat` (traffic signal current and future state information)
- `vehicle_list` (vehicle currently in the intersection).

 `add_vehicle_update` takes a string JSON status_and_intent message as a parameter and simply calls `vehicle_list::process_update(const std::string &json)` . This will allow the `vehicle_list` to process a `status_and_intent` message and update its map of vehicles to reflect the information from this update. 

`update_spat` takes a string JSON modified SPaT message (see streets_utils/streets_signal_phase_and_timing documentation) and calls from `fromJson(const std::string &json)`. This will cause the `spat` object to read and reflect the updated information.

`request_intersection_info` will poll the intersection_info REST endpoint on the **intersection_model** service for the intersection geometry information until it receives valid information, including signal group id's for every link lanelet in the intersection. This method is currently only called on startup.

This worker class contains methods to update this persisted data and the get shared pointers to it. The second class is the `signal_opt_service`. This class initializes and instance of the `signal_opt_message_worker', reads in configuration parameters from the `manifest.json` and creates threads for kafka consumers for the **status_and_intent** and **spat** messages. These kafka consumers will call the `signal_opt_messages_worker` methods to update the stored persistence objects : `vehicle_list` and `spat`.