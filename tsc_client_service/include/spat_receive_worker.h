#pragma once
#define SPDLOG_ACTIVE_LEVEL SPDLOG_LEVEL_TDEBUG

#include <fstream>
#include <spdlog/spdlog.h>
#include <spdlog/async.h> //support for async logging.
#include <spdlog/sinks/daily_file_sink.h> // support for dailty file sink
#include <spdlog/sinks/stdout_color_sinks.h> // or "../stdout_sinks.h" if no colors needed
#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#include <iostream>
#include <string>

using namespace std;

class SpatWorker 
{
    private:        

    public:
        SpatWorker(std::string ip, int port); 

        void createSocket(std::string ip, int port);

        std::string ip_;
        int port_;

};