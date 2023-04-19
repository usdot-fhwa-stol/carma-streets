# Streets Service Base Library

## Introduction

This is the `streets_service_base` library meant to be the base of each new CARMA-Streets service. This library exposes two major classes. The first is `streets_clock_singleton`, which is a `streets_singleton` that manages as instance of CarmaClock (see https://github.com/usdot-fhwa-stol/carma-time-lib ). The second is `streets_service`, which is meant to be a base class for all CARMA-Streets services that offers a framework for standing up new CARMA-Streets services.  

## streets_clock_singleton
The streets_clock_singleton is a streets_singleton which manages and instance of CarmaClock (see https://github.com/usdot-fhwa-stol/carma-time-lib ). It offers static access to the CarmaClock time wrapper, which is meant to allow CARMA-Streets services to configurable use system time or an external source for time.

## streets_service
The street_service class is a base class that sets up method to help standup new CARMA-Streets Services. This includes protected methods to initialize kafka consumers and producers, methods to start service threads and methods to intialize service members. The virtual `initialize` method initializes streets_singletons streets_clock_singleton and streets_configuration based on environment variables:
**SIMULATION_MODE**: TRUE will initialize streets_clock_singleton in simulation mode which will allow for time updating from external source via static update() method.FALSE will initialize streets_clock_singleton in realtime mode which will make streets_clock_singleton call system time.
**CONFIG_FILE_PATH** : String relative path from executable or absolute path to JSON configuration file. This is used to initialize streets_configuration singleton.
The initialize() and start() methods is meant to be overriden as follows:
protected:
    bool initialize() override {
        // Initializes streets_clock singleton, streets_configuration singleton and kafka consumer for simulation time
        streets_service::initialize();
    }

    void start() override {
        // Starts thread to use kafka time consumer to consume time sync messages and update streets_clock_singleton
        streets_service::start();
    }

## Include streets_service_base_lib
Streets Service Base `CMakeList.txt` includes an install target which will install this library as a CMake package. The library along with it's dependencies can then be included by simply using the find_package() instruction.

```
find_package(streets_service_base_lib COMPONENT streets_service_base_lib)
...
target_link_library( target PUBLIC streets_service_base_lib::streets_service_base_lib)
```

