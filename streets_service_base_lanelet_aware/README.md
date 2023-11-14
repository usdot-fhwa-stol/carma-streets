# Streets Service Base Lanelet Aware
## Introduction
This base image is intented for **CARMA Streets** services that require [lanelet2](https://github.com/fzi-forschungszentrum-informatik/Lanelet2) for spatial understanding of the surrouding area. Currently both **Message Service** and **Intersection Model** both depend on [lanelet2](https://github.com/fzi-forschungszentrum-informatik/Lanelet2) for this purpose. The new propose **Sensor Data Sharing Service** will also need lanelet2 for map transforms and for eventual perception. To allow all three services and any future services easy access to this dependency, this new **Streets Service Base Lanelet Aware** image exist.
> [!IMPORTANT]\
> Currently this base image is only being used for the **Sensor Data Sharing Service** but both the **Intersection_Model** and **Message Service** should be ported to this base image as well.

## Usage
Simply use the following to extend this image
```
FROM usdotfhwastoldev/streets_service_base_lanelet_aware:<tag>
```
The image also contains the environment variable **LANELET2_MAP**, which is assigned a default value of `/home/carma-streets/MAP/Intersection.osm`, but can be overwritten using the docker-compose environment variable section of the service description. An example is shown belo
```
 lanlet_aware_service:
    image: usdotfhwastoldev/lanlet_aware_service:develop
    environment:
      LANELET2_MAP: /new/path/to/lanelet2/map.osm
```