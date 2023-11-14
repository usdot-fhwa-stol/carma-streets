# Streets Service Base
## Introduction
This base image is intended for any basic **CARMA Streets** service. The **streets service base** image installs our logging library [spdlog](https://github.com/gabime/spdlog), kafka client library [librdkafka](https://github.com/confluentinc/librdkafka), and our JSON parsing/serializing library (rapidjson)[https://miloyip.github.io/rapidjson/] on top of the `carma-builds-x64` image to provide common CARMA Streets dependencies.The `carma-builds-x64` image is built from the (carma-builds)[https://github.com/usdot-fhwa-stol/carma-builds] github repository. This repository creates images in different ubuntu distributions and CPU architectures that function as build environments for C++ applications. Additionally, the **streets serivce base** image adds our debian package repository `http://s3.amazonaws.com/stol-apt-repository` to apt and installs (carma-time-lib)[https://github.com/usdot-fhwa-stol/carma-time-lib], the library we use to allow our CARMA Streets services to function both in simulation and real-world. This base image should be used for any CARMA Streets services.

> [!IMPORTANT]\
> Currently this base image is only being used for the **Sensor Data Sharing Service** but the **Signal Optimization Service**, **Scheduling Service** and the **TSC Service** should all be updated to build off this base image.

## Usage
Simply use the following to extend this image
```
FROM usdotfhwastoldev/streets_service_base:<ubuntu_distro_codename>
```
To build this image, use a docker build command similar to the one below
```
docker build -t usdotfhwastoldev/streets_service_base:<ubuntu-distro-codename> -f streets_service_base/Dockerfile --build-arg=UBUNTU_CODENAME=<ubuntu-distro-codename> .
```