#pragma once

#include <net-snmp/net-snmp-config.h>
#include <net-snmp/net-snmp-includes.h>
#include <fstream>
# include "streets_configuration.h"

namespace traffic_signal_controller_service
{
class snmp_client
{
    private:

        /*variables to store an snmp session*/
        // struct that holds information about who we're going to be talking to
        // We need to declare 2 of these, one to fill info with and second which is 
        // a pointer returned by the library
        snmp_session session;
        snmp_session *ss;

        /*Structure to hold all of the information that we're going to send to the remote host*/
        snmp_pdu *pdu;

        
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

        /* Map between phase numbers(key) and signal group ids(value) for all vehicle phases in the Traffic Signal Controller*/
        std::unordered_map<int,int> phase_num_map_;

        /* Map between signal group ids(key) and phase numbers(value) for all vehicle phases in the Traffic Signal Controller*/
        std::unordered_map<int,int> signal_group_map_;

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
        snmp_client(const std::string& ip, const int& port, const std::string& community = "public", int snmp_version = 0, int timeout = 100);
        
        /*Disable default copy constructor*/
        snmp_client() = delete;

        /** @brief Returns true or false depending on whether the request could be processed for given input OID at the Traffic Signal Controller.
         *  @param input_oid The OID to request information for.
         *  @param request_type The request type for which the error is being logged. Accepted values are "GET" and "SET" only.
         *  @param value The integer value for the object returned by reference. For "SET" it is the value to be set. 
         *  For "GET", it is the value returned for the returned object by reference.
         *  @return Integer value at the oid, returns false if value cannot be set/requested or oid doesn't have an integer value to return.*/
        bool process_snmp_request(const std::string& input_oid, const std::string& request_type, int64_t& value);

        /** @brief Finds error type from status and logs an error.
         *  @param status The integer value corresponding to net-snmp defined errors. macros considered are STAT_SUCCESS(0) and STAT_TIMEOUT(2)
         *  @param request_type The request type for which the error is being logged (GET/SET).
         *  @param response The snmp_pdu struct */
        void log_error(const int& status, const std::string& request_type, snmp_pdu *response);

        /** @brief Constructs a map between phase number and signal group ids
        * According to the NTCIP 1202 v03 documentation signal group ids in the SPAT message are the channel numbers in the TSC
        * **/
        void get_phasenum_signalgroup_map();

        /** @brief Destructor for client. Closes the snmp session**/
        ~snmp_client(){
            SPDLOG_INFO("Closing snmp session");
            snmp_close(ss);
        }

};
} //namespace traffic_signal_controller_service
