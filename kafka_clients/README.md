# Kafka Clients
## Introduction 
[librdkafka_github]: https://github.com/confluentinc/librdkafka
This is a library that wraps  [librdkafka][librdkafka_github] C++ kafka client library. It consists of three main classes: 
`kafka_client`: Class used to setup Kafka connection and create producers and consumers.
`kafka_producer_worker`: Kafka producer. Constructor takes in kafka broker, topic to produce to, and an optional parameter for partition.
`kafka_consumer_worker`: Kafka consumer. Constructor takes in kafka broker, topic to subscribe to, and consumer group to join.
More [Kafka Documentation](https://kafka.apache.org/documentation/) is avaiable online.
## Usage
To include in you CMake project simply find the `kafka_clients_lib` package and link it to your target. Note: this package requires that [librdkafka][librdkafka_github] is installed. It also requires that [spdlog](https://github.com/gabime/spdlog) is installed.
```cmake
find_package(kafka_clients_lib REQUIRED)
target_link_libraries(<TARGET>
  PUBLIC
    kafka_clients_lib::kafka_clients_lib
)
```