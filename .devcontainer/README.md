# CARMA Streets Dev Container Setup
## Introduction
This is the setup for the VSCode extension [devcontainer](https://code.visualstudio.com/docs/devcontainers/containers) extension, which allows developers to standup a Docker container for a more controlled development environment, and connect the VSCode IDE to this docker container. The setup also stands up the CARMA Streets [docker compose file](../docker-compose.yml) , which provides your development environment with easy access to Kafka and depedent CARMA Streets services for integration testing of functionality.
## Setup for new CARMA Streets Service
* Select a streets base image ([streets base images](../README.md#base-images)) and uncomment appropriate lines in `.devcontainer/devcontainer.json` and `.devcontainer/docker-compose-devcontainer.yml` (See comments in files)
* Use ctrl-shift-P (Command Pallette) and select *Dev Containers: Open Folder in Container*.
* Use VS Code terminals to build new streets service, preferably using a `build.sh` script under your new service directory(see [Sensor Data Sharing Service](../sensor_data_sharing_service/README.md)).
## Setup for existing CARMA Streets Service
* Select a streets base image ([streets base images](../README.md#base-images)) and uncomment appropriate lines in `.devcontainer/devcontainer.json` and `.devcontainer/docker-compose-devcontainer.yml` (See comments in files)
* Comment out existing service inside `docker-compose.yml`
* Use ctrl-shift-P (Command Pallette) and select *Dev Containers: Open Folder in Container*.
* Use VS Code terminals to build existing streets service, preferably using a `build.sh` script under your new service directory(see [Sensor Data Sharing Service](../sensor_data_sharing_service/README.md)).

