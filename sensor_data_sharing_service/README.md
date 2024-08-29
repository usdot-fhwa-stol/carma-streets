# Sensor Data Sharing Service

## Introduction

This is the **CARMA Streets** service responsible for consuming [detected_objects_msg](../streets_utils/streets_messages/DetectedObjectsMessage.md) and publishing [sensor_data_sharing_msg](../streets_utils/streets_messages/SensorDataSharingMessage.md) in accordance with the J3224 specification. This service is built on the [streets_service_base_lanelet_aware](../streets_service_base_lanelet_aware/README.md) base image to include lanelet2 dependencies used for map coordinate frame translations.
> [!IMPORTANT]
> Initial implementation of this service will not support the BSM functionality described in the J3224 specification and will also not support any data fusion from multiple infrastructure sensors.

## Configuration

### Environment Variables

The **Sensor Data Sharing Service** uses the following environment variables to retreive CARMA Streets system level configuration information.

**SIMULATION_MODE** : Boolean flag indicating whether the service is being run in a simulation or real-world enviroment. In simulation mode, this service use an external signal from the kafka `time_sync` topic as time.

**SENSOR_JSON_FILE_PATH**: Path to a sensor configuration file with the following format. Used for retrieving a reference location and projection for received cartesian detection data
> [!NOTE]
> Sensor reference or "ref" can either be defined as a "CARTESIAN" offset from a provided lanelet2 osm map (first example) or a "WGS84" location (second example). The "CARTESIAN" offset is only recommended for simulation testing as it adds a dependency on a lanelet2 map. For real-world deployments it is recommended that the "WGS84" location is provided.

**CARTESIAN**:

```json
[
  {
    "sensorId": "sensor_1",
    "type": "SemanticLidar",
    "ref": {
      "type": "CARTESIAN",
      "location": {
        "x": 1.0, /* in meters */
        "y": 2.0, /* in meters */
        "z": -3.2 /* in meters */
      },
       "orientation": { 
        "yaw": 0.0,
        "pitch": 0.0,
        "roll": 0.0
        }
    }
  } 
]
```

or

**WGS84**:

```json
[
  {
    "sensorId": "sensor_1",
    "type": "SemanticLidar",
    "ref": {
      "type": "WGS84",
      "lat": 38.9549716548523, /* in degrees */
      "lon": -77.14935313519123, /* in degrees */
      "elevation": 5.5 /* in meters */
    }
  } 
]
```

**LANELET2_MAP**: Only required for "CARTESIAN" sensor references. This is a file path to a lanelet2 osm map for which the cartesian offset is relavent.

### Service Configuration

**sdsm_producer_topic**: Kafka topic to which the sensor data sharing service will produce SDSMs.
**detection_consumer_topic**: Kafka topic from which the sensor data sharing service will consume Detected Objects.
**sensor_configuration_file_path**: Path to sensor configuration information that includes sensor location and orientation.
**bootstrap_server**: Kafka Broker Server Address.
**sensor_id**: Unique id of sensor for which to publish SDSMS
> [!NOTE]
> While the sensor configuration file described above currently supports multiple sensors, the **Sensor Data Sharing Service** currently is only able to publish detections from a single source. The sensor_id service configuration will check incoming detections for sensor id and only publish SDSMs containing detections from this sensor. Future work to incorporate sensor fusion is being considered to allow the sensor data sharing service to consume detections from multiple sources and fuse them.

## Covariance to confidence conversion

![image](https://github.com/usdot-fhwa-stol/carma-streets/assets/77466294/b5f4d827-7768-40ba-8674-3115ff4a338c)

For covariance to confidence/accuracy translation assumed the following. All confidence/accuracy measurements in SDSM assume 95% confidence interval ( from J3224 documentation). Assuming normal distribution of measurements. Diagonal in covariance matrix is variance for each component. SD (Standard deviation) is square root of variance. 2 standard deviations covers 95 % of sample in normal distribution
![image](https://github.com/usdot-fhwa-stol/carma-streets/assets/77466294/4a6e9875-2f87-4a8f-b1a0-33a6769439ac)

## Sensor Data Sharing speed and heading

For heading and speed we use following coordinate frame (NED) as described in the J3224 specifications.

![Alt text](docs/image.png)

We assume incoming detections are in the ENU cordinate frame. The `detected_object_enu_to_ned_converter` handles conversion of position and velocity in detection to NED coordinate frame.

![Alt text](docs/ned_enu.png)

