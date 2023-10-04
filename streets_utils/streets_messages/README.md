# Streets Messages Library

## Introduction

This CARMA-Streets library contains will be the new location for storing both CARMA Streets message data types as well as the logic to serialize and deserialize these messages
> [!IMPORTANT]\
> This is an improvement but also a deviation from how messages were previously processed in which message data types were couple to serialization. To remove this coupling new library will separate message data types from functions that serialize/deserialize them. This provides multiple benefits. By separating the logic the serialization logic from the message data types this improves maintainability by allowing us to potentially change serialization without any direct impact on message types or logic using message data. It also reduces the number of packages dependent on external libraries related to serialization like rapidjson.