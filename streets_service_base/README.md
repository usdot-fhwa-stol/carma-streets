# Streets Service Base
## Introduction
This base image is intented any basic **CARMA Streets** service. It installs our logging library [spdlog](https://github.com/gabime/spdlog), kafka client library [librdkafka](https://github.com/confluentinc/librdkafka), and our JSON parsing/serializing library (rapidjson)[https://miloyip.github.io/rapidjson/]. Additional this base image adds our debian package repository `http://s3.amazonaws.com/stol-apt-repository` to apt and installs (carma-time-lib)[https://github.com/usdot-fhwa-stol/carma-time-lib], the library we use to allow our CARMA Streets services to function both in simulation and real-world. This base image should be used for any CARMA Streets services. It is built off the (carma-builds-x64)[https://github.com/usdot-fhwa-stol/carma-builds] image, which is a image that generates an ubuntu linux environment with build tools for C++ installed. Both `streets_service_base` and `carma-builds-x64` have images tagged based on the ubuntu distribution they are built off to allow services flexibility in choosing a ubuntu distribution.

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