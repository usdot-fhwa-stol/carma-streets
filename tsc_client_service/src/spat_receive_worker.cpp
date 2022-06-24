#include "spat_receive_worker.h"

using namespace std;

namespace traffic_signal_controller_service
{

SpatWorker::SpatWorker(const std::string& ip, const int& port, const int& socketTimeout) : ip_(ip), port_(port), socketTimeout_(socketTimeout)
{
    SPDLOG_DEBUG("Creating Spat Worker");
}

void SpatWorker::createSocket()
{
    struct addrinfo hints;
    struct addrinfo *result;
    struct timeval tv;

    int maxDataSize = 1000;

    //set addr info criteria for results to return
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_DGRAM;
    hints.ai_protocol = IPPROTO_UDP;

    //getting address info for streets, 0 indicates success
    int tscAddrInfo = getaddrinfo(ip_.c_str(), std::to_string(port_).c_str(), &hints, &result);
    if (tscAddrInfo != 0) {
        SPDLOG_DEBUG("Failed to get addr info for this streets instance");
        return;
    }

    //creating socket
    int sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (sock == -1) {
        SPDLOG_DEBUG("Failed to create socket");
        return;
    }

    //set socket options with timeout from manifest json
    tv.tv_sec = socketTimeout_;
    tv.tv_usec = 0;
    setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(struct timeval));

    //attempt to bind to socket
    if (bind(sock, result->ai_addr, result->ai_addrlen) == -1) {
        SPDLOG_DEBUG("Failed to bind to socket");
        return;
    }
    
    //if we reach here, the socket has been created successfully
    socketCreated = true;

    //reading data from socket
    while (true)
    {
        std::vector<char> spatBuf(maxDataSize);
        ssize_t bytesReceived = recv(sock, spatBuf.data(), spatBuf.size(), 0);

        if (bytesReceived != -1)
        {
            spatBuf.resize(bytesReceived);
            SPDLOG_DEBUG("Num bytes received: {0}", bytesReceived);

            std::stringstream ss;
            ss << std::hex << std::setfill('0');
            for (int i = 0; i < bytesReceived; i++)
            {
                ss << std::setw(2) << static_cast<unsigned>(spatBuf[i]);
            }
            std::string mystr = ss.str();
            SPDLOG_DEBUG("Buffer contains: {0}", mystr);

        }       
    }    
}

bool SpatWorker::getSocketStatus() const {
    return socketCreated;
}

}