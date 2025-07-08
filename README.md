| CI Build Status | Sonar Code Quality |
|----------------------|---------------------|
|[![CI](https://github.com/usdot-fhwa-stol/carma-streets/actions/workflows/ci.yml/badge.svg)](https://github.com/usdot-fhwa-stol/carma-streets/actions/workflows/ci.yml) | [![Quality Gate Status](https://sonarcloud.io/api/project_badges/measure?project=usdot-fhwa-stol_carma-streets&metric=alert_status)](https://sonarcloud.io/dashboard?id=usdot-fhwa-stol_carma-streets) |
# DockerHub Release Builds
| Scheduling Service | Message Services | Intersection Model | Signal Opt Service | Tsc Service |
|-----|-----|-----|-----|-----|
[![Docker Cloud Build Status](https://img.shields.io/docker/cloud/build/usdotfhwastol/scheduling_service?label=scheduling%20service)](https://hub.docker.com/repository/docker/usdotfhwastol/scheduling_service) | [![Docker Cloud Build Status](https://img.shields.io/docker/cloud/build/usdotfhwastol/message_services?label=message%20services)](https://hub.docker.com/repository/docker/usdotfhwastol/message_services)   	|  [ ![Docker Cloud Build Status](https://img.shields.io/docker/cloud/build/usdotfhwastol/intersection_model?label=intersection%20model)](https://hub.docker.com/repository/docker/usdotfhwastol/intersection_model)	| [![Docker Cloud Build Status](https://img.shields.io/docker/cloud/build/usdotfhwastol/signal_opt_service?label=signal%20opt%20service)](https://hub.docker.com/repository/docker/usdotfhwastol/signal_opt_service)  	| [![Docker Cloud Build Status](https://img.shields.io/docker/cloud/build/usdotfhwastol/tsc_service?label=tsc%20service&logoColor=%232496ED)](https://hub.docker.com/repository/docker/usdotfhwastol/tsc_service) | 
# DockerHub Release Candidate Builds
| Scheduling Service | Message Services | Intersection Model | Signal Opt Service | Tsc Service | Sensor Data Sharing Service
|----|----|----|----|----|----|
[![Docker Cloud Build Status](https://img.shields.io/docker/cloud/build/usdotfhwastolcandidate/scheduling_service?label=tsc%20service&logoColor=%232496ED)](https://hub.docker.com/repository/docker/usdotfhwastolcandidate/scheduling_service) | [![Docker Cloud Build Status](https://img.shields.io/docker/cloud/build/usdotfhwastolcandidate/message_services?label=message%20services)](https://hub.docker.com/repository/docker/usdotfhwastolcandidate/message_services)   	|  [ ![Docker Cloud Build Status](https://img.shields.io/docker/cloud/build/usdotfhwastolcandidate/intersection_model?label=intersection%20model)](https://hub.docker.com/repository/docker/usdotfhwastolcandidate/intersection_model)	| [![Docker Cloud Build Status](https://img.shields.io/docker/cloud/build/usdotfhwastolcandidate/signal_opt_service?label=signal%20opt%20service)](https://hub.docker.com/repository/docker/usdotfhwastolcandidate/signal_opt_service)  	| [![Docker Cloud Build Status](https://img.shields.io/docker/cloud/build/usdotfhwastolcandidate/tsc_service?label=tsc%20service&logoColor=%232496ED)](https://hub.docker.com/repository/docker/usdotfhwastolcandidate/tsc_service) | ![Docker Cloud Build Status](https://img.shields.io/docker/cloud/build/usdotfhwastolcandidate/sensor_data_sharing_service?label=sensor_data_sharing_%20service&logoColor=%232496ED)|
# DockerHub Develop Builds
| Scheduling Service | Message Services | Intersection Model | Signal Opt Service | Tsc Service | Sensor Data Sharing Service
|-----|-----|-----|-----|-----|-----|
[![Docker Cloud Build Status](https://img.shields.io/docker/cloud/build/usdotfhwastoldev/scheduling_service?label=scheduling%20service)](https://hub.docker.com/repository/docker/usdotfhwastoldev/scheduling_service) | [![Docker Cloud Build Status](https://img.shields.io/docker/cloud/build/usdotfhwastoldev/message_services?label=message%20services)](https://hub.docker.com/repository/docker/usdotfhwastoldev/message_services)   	|  [ ![Docker Cloud Build Status](https://img.shields.io/docker/cloud/build/usdotfhwastoldev/intersection_model?label=intersection%20model)](https://hub.docker.com/repository/docker/usdotfhwastoldev/intersection_model)	| [![Docker Cloud Build Status](https://img.shields.io/docker/cloud/build/usdotfhwastoldev/signal_opt_service?label=signal%20opt%20service)](https://hub.docker.com/repository/docker/usdotfhwastoldev/signal_opt_service)  	| [![Docker Cloud Build Status](https://img.shields.io/docker/cloud/build/usdotfhwastoldev/tsc_service?label=tsc%20service&logoColor=%232496ED)](https://hub.docker.com/repository/docker/usdotfhwastoldev/tsc_service) | ![Docker Cloud Build Status](https://img.shields.io/docker/cloud/build/usdotfhwastoldev/sensor_data_sharing_service?label=sensor_data_sharing_%20service&logoColor=%232496ED)|
# CARMA Streets Base Image Builds
[![Build and Push Base Images](https://github.com/usdot-fhwa-stol/carma-streets/actions/workflows/build_streets_base_images.yml/badge.svg)](https://github.com/usdot-fhwa-stol/carma-streets/actions/workflows/build_streets_base_images.yml)

CARMA Streets is a component of CARMA ecosystem, which enables such a coordination among different transportation users. This component provides an interface for CDA participants to interact with the road infrastructure. CARMA Streets is also an edge-computing unit that improves the efficiency and performance of the Transportation Systems Management and Operations (TSMO) strategies.

## Architecture
CARMA Streets architecture is based on a scalable services and layered architecture pattern that allows for easy deployment.  Service components are packaged to contain one or more modules (classes) that represent a specific reusable function (e.g., decode a particular ASN.1 message) or an independently deployable business function (e.g., control interface to a signal controller). Services interact with each other via lightweight messaging service (e.g., Kafka) which allows for them be deployed either together or distributed for scalability and performance. A high-level abstract view of the architecture to communicate the design pattern is shown in Upcoming Figure. A more detailed Unified Modeling Language class and packaging diagrams to define the interfaces between services and layers and their interactions will be developed and documented here during implementation following an Agile Development Methodology.

## Deployment
Docker is the primary deployment mechanism to containerize one or more services. The CARMA Streets application and other major frameworks such as Kafka will run in their own separate containers. The following are required for deployment of CARMA Streets.
- Docker 
- Docker Compose

To deploy CARMA Streets we currently use Docker Compose, which is a container orchestration tool for defining and running multi-container appilcations. For instruction on installing Docker Compose please refer to their [installation guide](https://docs.docker.com/compose/install/).

### Initialization Script

For those less familiar with Docker, Docker Compose and their CLIs (Command Line Interfaces) we have provided a script, which does the following:
- Install V2X Hub
  - Setup V2X Hub Docker Environment
  - Pull V2X Hub Docker Images
- Install CARMA Streets
  - Setup CARMA Streets Docker Environment
  - Pull CARMA Streets Docker Images
- Deploy CARMA Streets
  - Add V2X Hub Admin Portal user
  - Run Docker Compose 

Simply run `./initialization.sh` and answer the prompts to deploy CARMA Streets.

> [!NOTE]  
> While this script is the preferred method for deploying CARMA Streets, especially from scratch, for those more experience with Docker, Docker Compose, V2X Hub and CARMA Streets, CARMA Streets can also be deployed directly using the Docker Compose CLI.


#### V2X Hub
Included as part of the CARMA Streets deployment is V2X Hub. V2X Hub is the hardware interface to an RSU (Road Side Unit) and enables CARMA Streets to have V2X (Vehicle to Everything) communication via the J2735 Message Set and CV2X radio communication. Once running V2X Hub needs to be configured to do the following in order for CARMA Streets to have V2X communication cababilites:
- Connect to RSU
- Connect to CARMA Streets
For information V2X Hub including configuration information please visit [V2X Hub GitHub Repository](https://github.com/usdot-fhwa-OPS/V2X-Hub/blob/develop/README.md)

#### Docker Environment

CARMA Streets Docker Compose deployment heavily relies on (Docker Compose Environment Variables)[https://docs.docker.com/compose/how-tos/environment-variables/set-environment-variables/]. We use these for the following:
- Setting Software Version
- Setting functionality to deploy
- Setting Deployment Specific Configurations


##### Setting Software Version

By default our Docker Compose deployment configuration will pull down develop images for both V2X Hub and CARMA Streets. The `initialization.sh` script will prompted the user to select a version of CARMA Streets to Deploy, which will override the default value by creating and populating a `.env` file, used by Docker Compose. To modify this value either edit the `.env` file directly or run the script. Below is a list of version specific environment variables and descriptions:

**OPS_TAG**: The version of V2X Hub to deploy (default: develop)
**STOL_ORG** & **STOL_TAG** : The version of CARMA Streets to deploy (default: usdotfhwastoldev/<image_name>:develop). To use a release update `STOL_ORG=usdotfhwastol` and `STOL_TAG=<release-version>`.

##### Setting Functionality to Deploy

TODO

##### Setting Deployment Specific Configurations

TODO

## Development
This repository includes configurations for [devcontainer](https://code.visualstudio.com/docs/devcontainers/containers) VSCode extension. This extension allows us to standup a containerized development environment. More information about the CARMA Streets Dev Container Setup can be found [here](.devcontainer/README.md).

## Base Images
To make creating new CARMA Streets services easier and to make our CI/CD more efficient, we have introduced new CARMA Streets base images. These images can be used as a starting point and common build/runtime environment for CARMA Streets services. There are currently two CARMA Streets base images , for which documentation and Dockerfiles can be found [Streets Service Base](streets_service_base/README.md) and [Streets Service Base Lanelet Aware](streets_service_base_lanelet_aware/README.md).

# CARMAStreets
The primary carma-streets repository can be found [here](https://github.com/usdot-fhwa-stol/carma-streets) and is part of the [USDOT FHWA STOL](https://github.com/usdot-fhwa-stol/)
github organization. Documentation on how the carma-streets  functions, how it will evolve over time, and how you can contribute can be found at the above links, as well as on the [Doxygen Source Code Documentation](https://usdot-fhwa-stol.github.io/documentation/carma-streets/).

## Data Collection
**CARMA Streets** services have several source from which data about usecase performance can be pulled. **Kafka Message Topics** can be pulled using the `collect_kafka_logs.py` script and includes message traffic on a configurable list of topics. Service log files can be found in a given **CARMA Streets** service `logs/` directory. The `collect_service_logs.sh` adds all log files in this directory to a zip file and then deletes the originals. Some **CARMA Streets** services can configurable log performance in `.csv` file in the `logs/` directory.

### Collect Kafka Logs
This script uses `docker exec` to ssh into a running kafka container. Then using kafka container scripts to read all kafka data from a list of provided topics. 
```
usage: collect_kafka_logs.py [-h] [--outdir OUTDIR] [--start_timestamp START_TIMESTAMP] [--end_timestamp END_TIMESTAMP] [--start_hours_ago START_HOURS_AGO]
                             [--end_hours_ago END_HOURS_AGO] [--topics TOPICS [TOPICS ...]] [--timeout TIMEOUT] [--zip ZIP]
                             [--container_names CONTAINER_NAMES [CONTAINER_NAMES ...]]

Script to grab data from kafka

options:
  -h, --help            show this help message and exit
  --outdir OUTDIR       Folder name for the resulting folder logs are placed in
  --start_timestamp START_TIMESTAMP
                        Unix timestamp (seconds) for the first message to grab. Exclusive with start_hours_ago.
  --end_timestamp END_TIMESTAMP
                        Unix timestamp (seconds) for the last message to grab. Exclusive with end_hours_ago.
  --start_hours_ago START_HOURS_AGO
                        float hours before current time to grab first message. Exclusive with start_timestamp.
  --end_hours_ago END_HOURS_AGO
                        float hours before current time to grab last message. Exclusive with start_timestamp.
  --topics TOPICS [TOPICS ...]
                        list of topics to grab data from
  --timeout TIMEOUT     timeout for receiving messages on a topic, default is 5 seconds
  --zip ZIP             bool flag. When set to true, folder is compressed into a zip file.
  --container_names CONTAINER_NAMES [CONTAINER_NAMES ...]
                        list of kafka containers to grab data from
```
### Collection Service Logs
This script collects all **CARMA Streets** service log files, adds them to a zip file.
```
usage: collect_service_logs.sh [-h | --help] [--output <zip_file_name>.zip]
                                     [-c | --clear]

options:
-h, --help                Show usage
--output, -o              Name of the resulting zip file container CARMA Streets Service logs
--clear, -c               Adding this flag will delete log directories after creating zip.
```
## Contribution
Welcome to the CARMA contributing guide. Please read this guide to learn about our development process, how to propose pull requests and improvements, and how to build and test your changes to this project. [CARMA Contributing Guide](https://github.com/usdot-fhwa-stol/carma-platform/blob/develop/Contributing.md) 

## Code of Conduct 
Please read our [CARMA Code of Conduct](https://github.com/usdot-fhwa-stol/carma-platform/blob/develop/Code_of_Conduct.md) which outlines our expectations for participants within the CARMA community, as well as steps to reporting unacceptable behavior. We are committed to providing a welcoming and inspiring community for all and expect our code of conduct to be honored. Anyone who violates this code of conduct may be banned from the community.

## Attribution
The development team would like to acknowledge the people who have made direct contributions to the design and code in this repository. [CARMA Attribution](https://github.com/usdot-fhwa-stol/carma-platform/blob/develop/ATTRIBUTION.md) 

## License
By contributing to the Federal Highway Administration (FHWA) Connected Automated Research Mobility Applications (CARMA), you agree that your contributions will be licensed under its Apache License 2.0 license. [CARMA License](https://github.com/usdot-fhwa-stol/carma-platform/blob/develop/docs/License.md) 

## Contact
Please click on the CARMA logo below to visit the Federal Highway Adminstration(FHWA) CARMA website. For technical support from the CARMA team, please contact the CARMA help desk at CAVSupportServices@dot.gov.

[![CARMA Image](https://raw.githubusercontent.com/usdot-fhwa-stol/CARMAPlatform/develop/docs/image/CARMA_icon.png)](https://highways.dot.gov/research/research-programs/operations/CARMA)



