# Streets Service Base Library

## Introduction

This is the `streets_service_base` library meant to be the base of each new CARMA-Streets service. It contains a extensible templated singleton implementation that allows for the creation of singleton scoped objects in CARMA-Streets services with static access. This means that by extending this class you can implement an class of which there can only ever be a single instance and which is statically retrievable using the `streets_singleton::get_singleton()` method. This library also includes an implemented singleton `streets_configuration` class which standardizes how CARMA-Streets services will read configuration parameters from `manifest.json` configuration files. This `streets_configuration` singleton parses the `manifest.json` configuration file and then allows for static retrieval of configuration parameters using the `get_int_config()`,`get_double_config`,`get_string_config()`, and `get_bool_config()` methods.

## Streets Singleton
TODO

## Streets Configuration
TODO