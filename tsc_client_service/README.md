# Streets Service Base Library

## Introduction

This is `tsc_client_service` meant to be a communication interface between CARMA-Streets and a Traffic Signal Controller (TSC). The service uses net-snmp library to implement a client that can GET and SET specified OIDs 
from a TSC. Current implementation allows the client to establish communication with a configurable host through snmp versions 1 and 2, which don't require authentication. 
The configurable parameters are defined in the `manifest.json` and include ip, port for the server as well as snmp specific configurations. Which setting version 3 is still an option, it will most likely not succeed since that version requires additional logic to be setup for authentication.




