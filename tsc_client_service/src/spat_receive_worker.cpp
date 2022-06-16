#include "spat_receive_worker.h"

using namespace std;

SpatWorker::SpatWorker(std::string ip, int port) : ip_(ip), port_(port)
{
    SPDLOG_DEBUG("Creating Spat Worker");
    createSocket(ip_, port_);
}

void SpatWorker::createSocket(std::string ip, int port)
{
    struct addrinfo hints, *result;
    struct timeval tv;

    int rv;
    int broadcast = 1;
    int maxDataSize = 1000;

    //obtain addresses matching host/port
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_DGRAM;
    hints.ai_protocol = IPPROTO_UDP;

    //getting address info for streets, 0 indicates success
    if ((rv = getaddrinfo(ip.c_str(), std::to_string(port).c_str(), &hints, &result) != 0)) {
        SPDLOG_DEBUG("Failed to get addr info for this streets instance");
    }

    //creating socket
    int sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (sock == -1) {
        SPDLOG_DEBUG("Failed to create socket");
    }

    //set socket options with 10 second timeout
    tv.tv_sec = 10;
    tv.tv_usec = 0;
    setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, (struct timeval *)&tv, sizeof(struct timeval));

    //attempt to bind to socket
    if (bind(sock, result->ai_addr, result->ai_addrlen) == -1) {
        SPDLOG_DEBUG("Failed to bind to socket");
        return;
    }


    //reading data
    while (true)
    {
        char spatBuf[maxDataSize];       
        int bytesReceived = recv(sock, spatBuf, maxDataSize-1, 0);

        SPDLOG_DEBUG("Num bytes received: {0}", bytesReceived);

        std::string spat_buf_str(&spatBuf[0], bytesReceived-1);
        SPDLOG_DEBUG("Buffer contains: {0}", spat_buf_str);
    }
}
