# Streets Service Base Library

## Introduction

This is the `streets_service_base` library meant to be the base of each new CARMA-Streets service. 
## Include streets_service_base_lib::streets_service_base_lib

Streets Service Base `CMakeList.txt` includes an install target which will install this library as a CMake package. The library along with it's dependencies can then be included by simply using the find_package() instruction.

```
find_package(streets_service_base_lib COMPONENT streets_service_base_lib)
...
target_link_library( target PUBLIC streets_service_base_lib::streets_service_base_lib)
```

