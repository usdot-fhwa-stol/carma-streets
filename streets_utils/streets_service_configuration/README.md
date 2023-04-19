# Streets Service Configuration Library

## Introduction

This is the `streets_service_configuration` library meant to provide of each new CARMA-Streets service with utility methods to load configurations. It contains a extensible templated singleton implementation that allows for the creation of singleton scoped objects in CARMA-Streets services with static access. This means that by extending this class you can implement a class of which there can only ever be a single instance and which is statically retrievable using the `streets_singleton::get_singleton()` method. This library also includes an implemented singleton `streets_configuration` class which standardizes how CARMA-Streets services will read configuration parameters from `manifest.json` configuration files. This `streets_configuration` singleton parses the `manifest.json` configuration file and then allows for static retrieval of configuration parameters using the `get_int_config()`,`get_double_config`,`get_string_config()`, and `get_bool_config()` methods.

## Streets Singleton

Library offers access to the `streets_singleton`  class. This is a templated, extensible class that stores and offers static retrieval of a single instance of itself. To ensure that new instances of this class can not be created the copy and assignment constructors are deleted and the regular constructor is hidden using protected access. Before accessing this object using the static `streets_singleton::get_singleton()` method, it must first be initialized using the `streets_singleon::create( Args ...)` method. Args should be the constructor parameter of the class which the singleton will manage an instance of. When implementating a new singleton it is recommended to separately create a class to manage the data and logic and then add this class and it's constructor parameters as the types for the singleton as shown below:  
```
class example_data{

    std::string param1;
    int param2;
    void method_1() {
        // DO SOMETHING
    };
    void example_data(const std::string &_param1, int _param2): param1(_param1), param2(_param2) {};
};
class example_singleton : public streets_singleton<example_data, std::string, int> {

    public:
        //Provide static access to singleton data methon
        static void method_1() {
            auto &inst = get_instance();
            inst.method_1();
        }
    protected;
        // Hide access to get_instance method
        using streets_singleton::get_instance;
};
// Create singleton
example_singleton::create("something", 3);
// Call any singleton method
example_singleton::method_1();

```


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

## Include streets_service_configuration_lib::streets_service_configuration_lib

Streets Service Base `CMakeList.txt` includes an install target which will install this library as a CMake package. The library along with it's dependencies can then be included by simply using the find_package() instruction.

```
find_package(streets_service_configuration_lib COMPONENT streets_service_configuration_lib)
...
target_link_library( target PUBLIC streets_service_configuration_lib::streets_service_configuration_lib)
```

