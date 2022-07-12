## Singal Optimization Service
### Monitoring
* The SO (Signal Optimization) service is the CARMA-Streets service responsible for optimizing traffic signal controller phase order and duration. The SO service is responsible for fixing a configurable amount of future phases on the traffic signal controller. On each phase transition (Yellow and Red clearance interval), the SO service will determine a new phase and duration to append to the list of fixed phases. The SO service does this by monitoring traffic signal state and planned state, vehicles within the intersection communication area, and the latest intersection geometry. <br/>

* The SO service depends on message service, intersection model and TSC (Traffic Signal Controller) service. It subscribes to message service to get the vehicle status and intent information, calls the [vehicle list library](https://github.com/usdot-fhwa-stol/carma-streets/tree/develop/streets_utils/streets_vehicle_list) to persist the vehicle. It sends HTTP GET requests to intersection model for configured time period to get the latest intersection geometry update, including the signal group ids for each link lanelet. It subscribes to the TSC service outgoing J2735 based JSON SPaT kafka topic to get information about current and future signal state. The JSON SPaT information is processed and persisted using the [streets_signal_phase_and_timing ](https://github.com/usdot-fhwa-stol/carma-streets/tree/develop/streets_utils/streets_signal_phase_and_timing) library .

## Configuration parameters
### Kafka topic names
| Topic Name      | Description |
| ------------    | ----------- |
| tsc_spat_msg_in | The topic is used to transfer message between signal optimization and traffic signal client services. The message is carma-streets internal [spat messages in JSON format.](https://github.com/usdot-fhwa-stol/carma-streets/tree/develop/streets_utils/streets_signal_phase_and_timing) 
| vehicle_status_intent_output | This topic is used to transfer message between signal optimization and message services. The message format refers to [confluence page.](https://usdot-carma.atlassian.net/wiki/spaces/CRMTSMO/pages/2182873096/CARMA+Streets+Message+Data+Collection) 

### Additional parameters
| Parameter Name | Description |
| -------------- | ----------- |
| sleep_millisecs | The number of milliseconds the current iteration should sleep for.| 
| int_client_request_attempts | The maximum numbers of loop to send HTTP request to intersection model to get the latest intersection geometry information.|