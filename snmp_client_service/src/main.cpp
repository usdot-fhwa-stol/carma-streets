# include <iostream>
#include "snmp_client.h"

int main()
{
    
    std::string setCustomMibsCommand = "export MIBS=ALL";
    system(setCustomMibsCommand.c_str()); 

    std::string ip = "172.17.0.4";
    int port = 161;
    SnmpClient worker(ip, port);
    std::string input_oid = ".1.3.6.1.4.1.1206.4.2.6.3.1";
    worker.process_snmp_get_request(input_oid);

    return 0;
}