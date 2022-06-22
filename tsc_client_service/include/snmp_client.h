#pragma once
#define SPDLOG_ACTIVE_LEVEL SPDLOG_LEVEL_DEBUG

#include <net-snmp/net-snmp-config.h>
#include <net-snmp/net-snmp-includes.h>
#include <fstream>
# include "streets_configuration.h"

class TrafficSignalControllerService
{
    private:

        /*variables to store an snmp session*/
        // struct that holds information about who we're going to be talking to
        // We need to declare 2 of these, one to fill info with and second which is 
        // a pointer returned by the library
        struct snmp_session session;
        struct snmp_session *ss;

        /*Structure to hold all of the information that we're going to send to the remote host*/
        struct snmp_pdu *pdu;

        /*Structure to hold response from the remote host*/
        struct snmp_pdu *response;

        /*OID is going to hold the location of the information which we want to receive. It will need a size as well*/
        oid OID[MAX_OID_LEN];
        size_t OID_len = MAX_OID_LEN;

        // Values from config
        /*Target device IP address*/
        std::string ip_ = "";
        /*Target device NTCIP port*/
        int port_ = 0;
        /*Target community for establishing snmp communication*/
        std::string community_ = "public";
        /* net-snmp version definition: SNMP_VERSION_1:0 SNMP_VERSION_2c:1 SNMP_VERSION_2u:2 SNMP_VERSION_3:3 
        https://github.com/net-snmp/net-snmp/blob/master/include/net-snmp/library/snmp.h */
        int snmp_version_ = 0;
        /*Time after which the the snmp request times out*/
        int timeout_ = 10000;

    public:
        /** @brief Constructor for Traffic Signal Controller Service client.
         *  Uses the arguments provided to establish an snmp connection
         * @param ip The ip ,as a string, for the tsc_client_service to establish an snmp communication with.
         * @param port Target port as integer on the host for snmp communication.
         * @param community The community id as a string. Defaults to "public" if unassigned.
         * @param snmp_version The snmp_version as defined in net-snmp.Default to 0 if unassigned.
         *                      net-snmp version definition: SNMP_VERSION_1:0 SNMP_VERSION_2c:1 SNMP_VERSION_2u:2 SNMP_VERSION_3:3"
         * @param timeout The time in microseconds after which an snmp session request expires. Defaults to 100 if unassigned
         * **/
        TrafficSignalControllerService(const std::string& ip, const int& port, const std::string& community = "public", int snmp_version = 0, int timeout = 100);
        
        /*Disable default copy constructor*/
        TrafficSignalControllerService() = delete;

        /** @brief Returns boolean value for input OID at the Traffic Signal Controller.
         *  @param input_oid The OID to request information for.
         *  @param integer_response The integer value for the object returned by reference.
         *  @return Integer value at the oid, returns false if value cannot be requested or oid doesn't have an integer value to return.*/
        bool process_snmp_get_request(std::string input_oid, long& integer_response);

        /** @brief Sets integer value at input OID at the Traffic Signal Controller.
         *  @param input_oid The OID to set value for.
         *  @param value The value to set.
         *  @return boolean for whether value could be set, returns true if successful and false if value cannot be set.*/
        bool process_snmp_set_request(std::string input_oid, int value);

        void log_error(const int& status, const std::string& request_type);

        /** @brief Destructor for client. Closes the snmp session**/
        ~TrafficSignalControllerService(){
            SPDLOG_INFO("Closing snmp session");
            snmp_close(ss);
        }

};