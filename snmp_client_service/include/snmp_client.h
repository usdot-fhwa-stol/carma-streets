#pragma once

#include <net-snmp/net-snmp-config.h>
#include <net-snmp/net-snmp-includes.h>
#include <fstream>

#include <spdlog/spdlog.h>
#include <spdlog/async.h> //support for async logging.
#include <spdlog/sinks/daily_file_sink.h> // support for dailty file sink
#include <spdlog/sinks/stdout_color_sinks.h> // or "../stdout_sinks.h" if no colors needed
#include <rapidjson/document.h>
#include <rapidjson/istreamwrapper.h>
#include <fstream>
#include <mutex>
#include <map>
#include <boost/filesystem/operations.hpp>
# include "streets_configuration.h"

class SnmpClient
{
    private:
        /* String filepath to manifest.json configuration file */
        std::string filepath;

        // variable to store an snmp session
        // struct that holds information about who we're going to be talking to
        // We need to declare 2 of these, one to fill info with and second which is 
        // a pointer returned by the library
        struct snmp_session session, *ss;

        //Structure to hold all of the information that we're going to send to the remote host
        struct snmp_pdu *pdu;

        //Structure to hold response from the remote host
        struct snmp_pdu *response;

        // OID is going to hold the location of the information which we want to receive. It will need a size as well
        oid OID[MAX_OID_LEN];
        size_t OID_len = MAX_OID_LEN;
        int status;
        // variable list will hold the variables we want to manipulate via SNMP

        // Values from config
        std::string ip_ = "";
        int port_ = 0;
        std::string community_ = "public";
        int community_len_ = 3;
        int timeout_ = 10000;

        /* net-snmp version definition: SNMP_VERSION_1:0 SNMP_VERSION_2c:1 SNMP_VERSION_2u:2 SNMP_VERSION_3:3 
        https://github.com/net-snmp/net-snmp/blob/master/include/net-snmp/library/snmp.h */
        int snmp_version_ = 0;

    public:
        SnmpClient();
        
        ~SnmpClient();

        int process_snmp_request(std::string request_type, std::string input_oid, int value);

        void process_snmp_get_request(std::string input_oid);
    
    
};