| CicleCI Build Status | Sonar Code Quality |
|----------------------|---------------------|
|[![CircleCI](https://circleci.com/gh/usdot-fhwa-stol/carma-streets.svg?style=svg)](https://circleci.com/gh/usdot-fhwa-stol/carma-streets)  	|  [![Quality Gate Status](https://sonarcloud.io/api/project_badges/measure?project=usdot-fhwa-stol_carma-streets&metric=alert_status)](https://sonarcloud.io/dashboard?id=usdot-fhwa-stol_carma-streets)  | 
# DockerHub Release Builds
| Scheduling Service | Message Services | Intersection Model | Signal Opt Service | Tsc Service |
|-----|-----|-----|-----|-----|
[![Docker Cloud Build Status](https://img.shields.io/docker/cloud/build/usdotfhwastol/scheduling_service?label=scheduling%20service)](https://hub.docker.com/repository/docker/usdotfhwastol/scheduling_service) | [![Docker Cloud Build Status](https://img.shields.io/docker/cloud/build/usdotfhwastol/message_services?label=message%20services)](https://hub.docker.com/repository/docker/usdotfhwastol/message_services)   	|  [ ![Docker Cloud Build Status](https://img.shields.io/docker/cloud/build/usdotfhwastol/intersection_model?label=intersection%20model)](https://hub.docker.com/repository/docker/usdotfhwastol/intersection_model)	| [![Docker Cloud Build Status](https://img.shields.io/docker/cloud/build/usdotfhwastol/signal_opt_service?label=signal%20opt%20service)](https://hub.docker.com/repository/docker/usdotfhwastol/signal_opt_service)  	| [![Docker Cloud Build Status](https://img.shields.io/docker/cloud/build/usdotfhwastol/tsc_service?label=tsc%20service&logoColor=%232496ED)](https://hub.docker.com/repository/docker/usdotfhwastol/tsc_service) | 
# DockerHub Release Candidate Builds
| Scheduling Service | Message Services | Intersection Model | Signal Opt Service | Tsc Service |
|----|----|----|----|----|
[![Docker Cloud Build Status](https://img.shields.io/docker/cloud/build/usdotfhwastolcandidate/scheduling_service?label=tsc%20service&logoColor=%232496ED)](https://hub.docker.com/repository/docker/usdotfhwastolcandidate/scheduling_service) | [![Docker Cloud Build Status](https://img.shields.io/docker/cloud/build/usdotfhwastolcandidate/message_services?label=message%20services)](https://hub.docker.com/repository/docker/usdotfhwastolcandidate/message_services)   	|  [ ![Docker Cloud Build Status](https://img.shields.io/docker/cloud/build/usdotfhwastolcandidate/intersection_model?label=intersection%20model)](https://hub.docker.com/repository/docker/usdotfhwastolcandidate/intersection_model)	| [![Docker Cloud Build Status](https://img.shields.io/docker/cloud/build/usdotfhwastolcandidate/signal_opt_service?label=signal%20opt%20service)](https://hub.docker.com/repository/docker/usdotfhwastolcandidate/signal_opt_service)  	| [![Docker Cloud Build Status](https://img.shields.io/docker/cloud/build/usdotfhwastolcandidate/tsc_service?label=tsc%20service&logoColor=%232496ED)](https://hub.docker.com/repository/docker/usdotfhwastolcandidate/tsc_service) | 
# DockerHub Develop Builds
| Scheduling Service | Message Services | Intersection Model | Signal Opt Service | Tsc Service |
|-----|-----|-----|-----|-----|
[![Docker Cloud Build Status](https://img.shields.io/docker/cloud/build/usdotfhwastoldev/scheduling_service?label=scheduling%20service)](https://hub.docker.com/repository/docker/usdotfhwastoldev/scheduling_service) | [![Docker Cloud Build Status](https://img.shields.io/docker/cloud/build/usdotfhwastoldev/message_services?label=message%20services)](https://hub.docker.com/repository/docker/usdotfhwastoldev/message_services)   	|  [ ![Docker Cloud Build Status](https://img.shields.io/docker/cloud/build/usdotfhwastoldev/intersection_model?label=intersection%20model)](https://hub.docker.com/repository/docker/usdotfhwastoldev/intersection_model)	| [![Docker Cloud Build Status](https://img.shields.io/docker/cloud/build/usdotfhwastoldev/signal_opt_service?label=signal%20opt%20service)](https://hub.docker.com/repository/docker/usdotfhwastoldev/signal_opt_service)  	| [![Docker Cloud Build Status](https://img.shields.io/docker/cloud/build/usdotfhwastoldev/tsc_service?label=tsc%20service&logoColor=%232496ED)](https://hub.docker.com/repository/docker/usdotfhwastoldev/tsc_service) | 

CARMA Streets is a component of CARMA ecosystem, which enables such a coordination among different transportation users. This component provides an interface for CDA participants to interact with the road infrastructure. CARMA Streets is also an edge-computing unit that improves the efficiency and performance of the Transportation Systems Management and Operations (TSMO) strategies.

## Architecture
CARMA Streets architecture is based on a scalable services and layered architecture pattern that allows for easy deployment.  Service components are packaged to contain one or more modules (classes) that represent a specific reusable function (e.g., decode a particular ASN.1 message) or an independently deployable business function (e.g., control interface to a signal controller). Services interact with each other via lightweight messaging service (e.g., Kafka) which allows for them be deployed either together or distributed for scalability and performance. A high-level abstract view of the architecture to communicate the design pattern is shown in Upcoming Figure. A more detailed Unified Modeling Language class and packaging diagrams to define the interfaces between services and layers and their interactions will be developed and documented here during implementation following an Agile Development Methodology.

## Deployment
Docker is the primary deployment mechanism to containerize one or more services. The CARMA Streets application and other major frameworks such as Kafka will run in their own separate containers. This document will be updated with a detailed Docker deployment strategy during later design phases. 

# CARMAPlatform
The primary carma-streets repository can be found [here](https://github.com/usdot-fhwa-stol/carma-streets) and is part of the [USDOT FHWA STOL](https://github.com/usdot-fhwa-stol/)
github organization. Documentation on how the carma-streets  functions, how it will evolve over time, and how you can contribute can be found at the above links as well

## Contribution
Welcome to the CARMA contributing guide. Please read this guide to learn about our development process, how to propose pull requests and improvements, and how to build and test your changes to this project. [CARMA Contributing Guide](https://github.com/usdot-fhwa-stol/carma-platform/blob/develop/Contributing.md) 

## Code of Conduct 
Please read our [CARMA Code of Conduct](https://github.com/usdot-fhwa-stol/carma-platform/blob/develop/Code_of_Conduct.md) which outlines our expectations for participants within the CARMA community, as well as steps to reporting unacceptable behavior. We are committed to providing a welcoming and inspiring community for all and expect our code of conduct to be honored. Anyone who violates this code of conduct may be banned from the community.

## Attribution
The development team would like to acknowledge the people who have made direct contributions to the design and code in this repository. [CARMA Attribution](https://github.com/usdot-fhwa-stol/carma-platform/blob/develop/ATTRIBUTION.md) 

## License
By contributing to the Federal Highway Administration (FHWA) Connected Automated Research Mobility Applications (CARMA), you agree that your contributions will be licensed under its Apache License 2.0 license. [CARMA License](https://github.com/usdot-fhwa-stol/carma-platform/blob/develop/docs/License.md) 

## Contact
Please click on the CARMA logo below to visit the Federal Highway Adminstration(FHWA) CARMA website.

[![CARMA Image](https://raw.githubusercontent.com/usdot-fhwa-stol/CARMAPlatform/develop/docs/image/CARMA_icon.png)](https://highways.dot.gov/research/research-programs/operations/CARMA)



