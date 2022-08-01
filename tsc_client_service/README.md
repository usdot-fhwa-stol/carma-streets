# Traffic Signal Controller Service Library

## Introduction

This is the **TSC Service** (**Traffic Signal Controller Service**) meant to be a communication interface between CARMA-Streets and a **Traffic Signal Controller** (TSC). The service uses **net-snmp** library to implement a client that can GET and SET specified OIDs from a TSC. Current implementation allows the client to establish communication with a configurable host through snmp versions 1 and 2. 

The configurable parameters are defined in the `manifest.json` and include **ip**, **port** for the snmp server. Currently snmp version 3 is *not* supported as it requires authentication. Most signal controllers only support NTCIP 1202 v2 currently.

The **TSC Service** has several responsibilies. The first is to make **SNMP GET** calls to the **TSC** on startup to get configuration information which includes:
- Phase Number (NTCIP) to Signal Group (J2735) Mapping for all phases (Pedestrian and Vehicle) (Found in the ChannelTable)
- Phase Sequence for Rings 1 and 2 (Vehicle Phases)(Found in SequenceTable)
- Phase red clearance , yellow change, and minimum/maximun green times (Found in PhaseTable)
This information is requested and stored in the `tsc_state` object, which on intialization, uses an `snmp_client` to query this information from the TSC.

The `snmp_client` is a class which encapsulates the **net-snmp** connection logic and converts SNMP responses to their `std::string` or `int` equivalents. The constructor requires **host**, **port**, **version**, **community** and **timeout** information to initialize a connection. To make a request simply use the `process_snmp_request` method. It requires 




