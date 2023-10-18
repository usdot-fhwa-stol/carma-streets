# Streets Messages Library

## Introduction

This CARMA-Streets library will be the new location for storing both CARMA Streets message data types as well as the logic to serialize and deserialize these messages. Message data types will be stored in directories with the message name as the directory name similar to the current `sensor_data_sharing_msg`. Then functions will hold the JSON serialization and deserialization logic for each message to seperate concerns betweening handling and using the messages and serializing/deserializing the message for sending and receiving.
> [!IMPORTANT]\
> Currently this library only contains the sensor_data sharing message and its JSON serialization logic. Will attempt to part existing and future messages into this library.
## Serialization and Deserialization

CARMA Streets messages are currently all using JSON for serialization for sending via a Apache Kafka broker. To parse or write the JSON we are currently using the [rapidjson library](https://github.com/Tencent/rapidjson). In addition, we have implemented some helper functions to parse optional fields from JSON using the [std::optional](https://en.cppreference.com/w/cpp/utility/optional) class template implemented in our [json_utils library](https://github.com/usdot-fhwa-stol/carma-streets/tree/develop/streets_utils/json_utils).

## Some future planned improvements to this library include:
- Porting existing CARMA Streets message JSON serialization/deserialization into library functions and out of message data type definitions
- introduce "compile time inheritance" for serialization and deserialization to improve ease of use (https://github.com/usdot-fhwa-stol/carma-streets/issues/356)


> [!IMPORTANT]\
> This is an improvement but also a deviation from how messages were previously processed in which message data types were couple to serialization. To remove this coupling new library will separate message data types from functions that serialize/deserialize them. This provides multiple benefits. By separating the logic the serialization logic from the message data types this improves maintainability by allowing us to potentially change serialization without any direct impact on message types or logic using message data. It also reduces the number of packages dependent on external libraries related to serialization like rapidjson.