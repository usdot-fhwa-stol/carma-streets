# Build Scripts
## Introduction
The directory contains different scripts used for the build process of **CARMA Streets** service images and base images.
## Install Streets Service Base Dependencies
This script is used to install some common dependencies of **CARMA Streets** services. It assumes that build tools like a compiler, cmake, and other basic C++ build and test dependencies are already installed. Currently we use this script to install **CARMA Streets** service dependencies on top of `carma-builds-64x` (https://github.com/usdot-fhwa-stol/carma-builds) images to create base images for **CARMA Streets** services.
## Install Lanelet2 dependencies
This script installs [lanelet2](https://github.com/fzi-forschungszentrum-informatik/Lanelet2) along with its depedencies. **CARMA Streets** services that require spatial understanding of the surrounding roadway use [lanelet2](https://github.com/fzi-forschungszentrum-informatik/Lanelet2) maps and libraries. This script is used in conjuction with the [streets_service_base_lanelet_aware](../streets_service_base_lanelet_aware/README.md) Dockerfile to create a base image which includes these dependencies.