## install the intersection client library from carma-utils
```
cd streets_utils/streets_api/intersection_client_api
mkdir build
cmake .. && make
make install
rm -r build
```

## Add the library and it depdencies in current CMakeLists.txt
```
target_link_libraries(${PROJECT_NAME} PRIVATE intersection_client_api_lib)
target_link_libraries(${PROJECT_NAME} PRIVATE Qt5::Core Qt5::Network )
```

## Add the include file in current CMakeLists.txt
The default installation folder for streets_utils/streets_api/intersection_client_api .h files are located in /usr/local/include folder.

```
include_directories("/usr/local/include")
```