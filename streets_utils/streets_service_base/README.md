# Streets Service Base Library

## Introduction

This is the `streets_service_base` library meant to be the base of each new CARMA-Streets service. It contains a extensible templated singleton implementation that allows for the creation of singleton scoped objects in CARMA-Streets services with static access. This means that by extending this class you can implement a class of which there can only ever be a single instance and which is statically retrievable using the `streets_singleton::get_singleton()` method. This library also includes an implemented singleton `streets_configuration` class which standardizes how CARMA-Streets services will read configuration parameters from `manifest.json` configuration files. This `streets_configuration` singleton parses the `manifest.json` configuration file and then allows for static retrieval of configuration parameters using the `get_int_config()`,`get_double_config`,`get_string_config()`, and `get_bool_config()` methods.

## Streets Singleton

Library offers access to the `streets_singleton`  class. This is a templated, extensible class that stores and offers static retrieval of a single instance of itself, which is lazily initialized (not initialized until retrieved for the first time). To ensure that new instances of this class can not be created the constructors are deleted or hidden using private or protected access.


## Streets Configuration
Library creates `streets_configuration` singleton which standardizes the `manifest.json` configuration file parsing and configuration parameter retrieval. Extending `streets_singleton` allows `streets_configuration` to be limited to singleton scope ( single instance ) and offer static methods for configuration parameter retrieval. `streets_configuration` also parses some service required configurations like **loglevel** and **service_name** to create and configure a `spdlog::async_loggerr` with a  `spdlog::sinks::daily_file_sink_mt` and a `spdlog::sinks::stdout_color_sink_mt`.

Example `manifest.json` configuration file.
```
{
    "service_name": "test_service",
    "loglevel": "info",
    "configurations": [
        {
            "name": "param_1",
            "value": 1,
            "description": "Test Parameter 1",
            "type": "INTEGER"
        },
        {
            "name": "param_2",
            "value": "Second Parameter",
            "description": "Test Parameter 2",
            "type": "STRING"
        },
        {
            "name": "param_3",
            "value": 123.2,
            "description": "Test Parameter 3",
            "type": "DOUBLE"
        },
        {
            "name": "param_4",
            "value": true,
            "description": "Test Parameter 3",
            "type": "BOOL"
        }
    ]
}
```

## Include streets_service_base_lib::streets_service_base_lib

Streets Service Base `CMakeList.txt` includes an install target which will install this library as a CMake package. The library along with it's dependencies can then be included by simply using the find_package() instruction.

```
find_package(streets_service_base_lib COMPONENT streets_service_base_lib)
...
target_link_library( target PUBLIC streets_service_base_lib::streets_service_base_lib)
```

