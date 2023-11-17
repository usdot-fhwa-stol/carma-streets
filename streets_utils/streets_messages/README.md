# Streets Messages Library

## Introduction

This CARMA-Streets library will be the new location for storing both CARMA Streets message data types as well as the logic to serialize and deserialize these messages. Message data types will be stored in directories with the message name as the directory name similar to the current `sensor_data_sharing_msg`. Then functions will hold the JSON serialization and deserialization logic for each message to seperate concerns between message serialization and message data.
> [!IMPORTANT]\
> Currently this library only contains the sensor_data sharing message and the detected objects message, with their JSON serialization logic. Will attempt to part existing and future messages into this library.

## Messages
**[Sensor Data Sharing Message](SensorDataSharingMessage.md)** : J3224 Message used for sharing detection level data between vehicles and infrastructure.
**[Detected Objects Message](DetectedObjectsMessage.md)** : Custom Message used for sharing object detection information between sensors and ITS actors
## Serialization and Deserialization

CARMA Streets messages are currently all using JSON for serialization for sending via a Apache Kafka broker. To parse or write the JSON we are currently using the [rapidjson library](https://github.com/Tencent/rapidjson). In addition, we have implemented some helper functions to parse optional fields from JSON using the [std::optional](https://en.cppreference.com/w/cpp/utility/optional) class template implemented in our [json_utils library](https://github.com/usdot-fhwa-stol/carma-streets/tree/develop/streets_utils/json_utils).

## Some future planned improvements to this library include:
- Porting existing CARMA Streets message JSON serialization/deserialization into library functions and out of message data type definitions
- introduce "compile time inheritance" for serialization and deserialization to improve ease of use (https://github.com/usdot-fhwa-stol/carma-streets/issues/356)

> [!IMPORTANT]\
> This is an improvement but also a deviation from how messages were previously processed in which message data types were couple to serialization. To remove this coupling new library will separate message data types from functions that serialize/deserialize them. This provides multiple benefits. By separating the logic the serialization logic from the message data types this improves maintainability by allowing us to potentially change serialization without any direct impact on message types or logic using message data. It also reduces the number of packages dependent on external libraries related to serialization like rapidjson.

## Installation
This library includes a CMake install target that will attempt to install this library, including CMake configuration files under the following general path:

`streets_utils\streets_messages_lib`

Header files will be installed in the CMake default include directory

`include\streets_utils\streets_messages_lib\`

CMake configuration files will be installed under CMake default install directory

`cmake\streets_messages_lib\`

And the library binary will be installed under the CMake default Library directory

`lib\streets_utils\`

## Using Library

To incorporate this library into a CARMA Streets Service simply install the library using `make install` and then use the CMake `find_package(streets_messages REQUIRED)` call to find the install package. To link this library include the streets utils namespace as follows:
```
target_link_libraries(<targetname> PUBLIC streets_utils::streets_messages)
```