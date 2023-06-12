# Traffic Signal Controller Service Library

## Introduction

This is the **TSC Service** (**Traffic Signal Controller Service**) meant to be a communication interface between CARMA-Streets and a **Traffic Signal Controller** (TSC). The service uses **net-snmp** library to implement a client that can GET and SET specified OIDs from a TSC. Current implementation allows the client to establish communication with a configurable host through snmp versions 1 and 2. 

The configurable parameters are defined in the `manifest.json` and include **ip**, **port** for the snmp server. Currently snmp version 3 is *not* supported as it requires authentication. Most signal controllers only support NTCIP 1202 v2 currently.

The **TSC Service** has several responsibilies. The first is to make **SNMP GET** calls to the **TSC** on startup to get configuration information which includes:
- Phase Number (NTCIP) to Signal Group (J2735) Mapping for all phases (Pedestrian and Vehicle) (Found in the ChannelTable)
- Phase Sequence for Rings 1 and 2 (Vehicle Phases)(Found in SequenceTable)
- Phase red clearance , yellow change, and minimum/maximun green times (Found in PhaseTable)

This information is required for predicting traffic signal controller state and for interpeting the **NTCIP UDP SPaT** data that is broadcast by the TSC. The TSC can be configured to broadcast state information to a configurable host and port via UDP. Broadcasted messages contain information about phases, their current state, and time offset information about when they will change. One of the **TSC Services** other responsibities is to read these state messages and translate them to **J2735 SPaT** information. This requires the phase number to signal group mapping queried and startup as well as several unit/timing conversions from offsets to absolute times. The **TSC Service** will translate each incoming message and broadcast it, in JSON format, on the **CARMA-Streets** Kafka broker to a configurable topic( default : `modified_spat`). Other **CARMA-Streets** service can then subscribe to this topic and receive information about the current state of the **TSC**.

The final responsibility of the **TSC** is to make **SNMP SET** calls to the **TSC** to modify its default signal phase sequence and timing. This **TSC** will use the SET calls on the following **OIDs** to achieve this:
- HOLD
- OMIT
- FORCE_OFF

Modification of the **TSC** default signal phase sequence and timing is done based on the output of the **SO Service (Signal Optimization)**. This output is referred to as the **Desired Phase Plan** and consists of future desired phases and green timing intervals. The **TSC Service** will consume these messages from the **SO Service** on every phase transition (Yellow Change) and attempt to send the appropriate SNMP commands to NTCIP OIDs to make the **TSC** reflect this behavior. The **TSC Service** will also populate the JSON J2735 **SPaT** with **Desired Phase Plan** information as future state informat in the form of **MovementEvents** (See J2735 SPaT definition). 


## tsc_service
The main class in the **TSC Service** is the `tsc_service` class. This class is used to load configuration values from the `manifest.json` file, initialize any objects and launches any joined or detached threads of execution. Some important data stored in this class include the `spat` pointer (see streets_utils/streets_signal_phase_and_timing/README.md) and  `tsc_state`. Some important classes initialize here as well include the `snmp_client`, `intersection_client`, `spat_worker` and a `kafka_producer` for producing modified SPaT JSON messages (see kafka_clients README.md).

This information is requested and stored in the `tsc_state` object, which on intialization, uses an `snmp_client` to query this information from the TSC.

The `snmp_client` is a class which encapsulates the **net-snmp** connection logic and converts SNMP responses to their `std::string` or `int` equivalents. The constructor requires **host**, **port**, **version**, **community** and **timeout** information to initialize a connection. To make a request simply use the `process_snmp_request` method. It takes an **NTCIP OID** which describes the data you are querying/setting on the **TSC**, a **request_type** which is an enumeration describing whether you want to use SNMP SET/GET, and a `snmp_response_obj` which is a struct that will be populated by the metho with the SNMP server response.

The `intersection_client` is a REST client implemented using the `streets_utils/streets_api/intersection_client_api` library ( see README.md for further documentation). It is used to obtain information from the J2735 MAP message, mainly the intersection id and intersection name, to populate the outgoing SPaT message.

The `spat_worker` is a class which encapsulates a UDP socket listener. This socket listener, listens for UDP NTCIP data packets set from the **TSC** at 10 hz that provide traffic signal state information required for populating the **SPaT**. The `spat_worker` contains a method to consume a UDP datapacket and update the `spat` pointer which stores the most up-to-date information of the traffic signal controller state. The `tsc_service` `spat_thread` then continously consumes these messages and publishes the resulting **SPaT** JSON on the CARMA-Streets Kafka broker.

## tsc_service to MMITSS integration 
### Configuration parameter
The  `use_mmitss_mrp` parameter in manifest.json file is used to determine whether the TSC service consume [phase control schedule](https://github.com/mmitss/mmitss-az/tree/master/src/mrp/traffic-controller-interface) from external [MMTISS Roadside processor](https://github.com/mmitss/mmitss-az) or not. By default, this parameter is set to false to consume desired phase plan from internal signal optimization service. The MRP and carma-streets signal optimization service shall run exclusively, meaning there is no scenario where both MRP and signal optmization service are trying to manipulate traffic signal controller at the same time.


