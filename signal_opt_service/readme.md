## Singal Optimization Service
### Monitoring
This package provides logic for monitoring traffic signal state and planned state, vehicles within the intersection communication area, and the latest intersection geometry. The signal optimization service depends on message service, intersection model and traffic signal client service. It subscribes to message service to get the vehicle status and intent information, calls the [vehicle list library](https://github.com/usdot-fhwa-stol/carma-streets/tree/develop/streets_utils/streets_vehicle_list) to persist the vehicles in the service instance list variable. It sends HTTP GET requests to intersection model server for configured time period to get the latest intersection geometry update, especially the signal group ids update.  It subscribes to the traffic signal client service to get the latest signal state and call the [traffic singal phase and timing library](https://github.com/usdot-fhwa-stol/carma-streets/tree/develop/streets_utils/streets_signal_phase_and_timing) to update the signal state service instance variable. 

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