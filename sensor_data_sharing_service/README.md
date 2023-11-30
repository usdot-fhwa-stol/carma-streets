# Sensor Data Sharing Service
## Introduction
This is the **CARMA Streets** service responsible for consuming [detected_objects_msg](../streets_utils/streets_messages/DetectedObjectsMessage.md) and publishing [sensor_data_sharing_msg](../streets_utils/streets_messages/SensorDataSharingMessage.md) in accordance with the J3224 specification. This service is built on the [streets_service_base_lanelet_aware](../streets_service_base_lanelet_aware/README.md) base image to include lanelet2 dependencies used for map coordinate frame translations.
## Configuration Parameters
**TODO** Saving this part for once I finalize the implementation
> [!IMPORTANT]\
> Initial implementation of this service will not support the BSM functionality described in the J3224 specification and will also not support any data fusion from multiple infrastructure sensors.