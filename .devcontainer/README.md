# CARMA Streets Dev Container Setup
## Introduction
This is the setup for the VSCode extension [devcontainer](https://code.visualstudio.com/docs/devcontainers/containers) extension, which allows developers to standup a containerized development environment with the VS Code IDE. The current configuration includes the installation of common VSCode plugins for C++, CMake, Python, Mark Down and more. A complete list of VS Code installed plugins can be found in the `./devcontainer/devcontainer.json` under `"customizations.vscode.extensions` attribute. The setup also stands up the CARMA Streets [docker compose file](../docker-compose.yml) , which provides your development environment with easy access to Kafka and CARMA Streets services for integration testing of functionality.
## Suggested Structure of CARMA Street Service
```bash
├── <streets_service_name>
│   ├── src/                        # Source Code
│   ├── include/                    # Headers
│   ├── manifest.json               # CARMA Streets Service configuration file
│   ├── CMakeLists.txt              # CMake build file
│   ├── README.md                   # Documentation about Service
│   ├── build.sh                    # Build script to build streets_util libraries for service and service itself
|   ├── install_dependencies.sh     # Optional file to install external dependencies not included in base image
│   └── Dockerfile                  # Dockerfile to build streets serivce
```
## Setup for new CARMA Streets Service
* Select a streets base image ([streets base images](../README.md#base-images)) and uncomment appropriate lines in `.devcontainer/devcontainer.json` and `.devcontainer/docker-compose-devcontainer.yml` (See comments in files)
* Use ctrl-shift-P (Command Pallette) and select *Dev Containers: Open Folder in Container*.
* Use VS Code terminals to build new streets service, preferably using a `build.sh` script under your new service directory(see [Sensor Data Sharing Service](../sensor_data_sharing_service/README.md)).
## Setup for existing CARMA Streets Service
* Select a streets base image ([streets base images](../README.md#base-images)) and uncomment appropriate lines in `.devcontainer/devcontainer.json` and `.devcontainer/docker-compose-devcontainer.yml` (See comments in files)
* Comment out existing service inside `docker-compose.yml`
* Use ctrl-shift-P (Command Pallette) and select *Dev Containers: Open Folder in Container*.
* Use VS Code terminals to build existing streets service, preferably using a `build.sh` script under your new service directory(see [Sensor Data Sharing Service](../sensor_data_sharing_service/README.md)).


