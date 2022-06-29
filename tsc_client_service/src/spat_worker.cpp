#include "spat_worker.h"

using namespace std;

namespace traffic_signal_controller_service
{

    spat_worker::spat_worker(const std::string& ip, const int& port, const int& socket_timeout) : ip_(ip), port_(port), socket_timeout_(socket_timeout)
    {
        SPDLOG_DEBUG("Creating Spat Worker");
    }

    void spat_worker::listen_udp_spat()
    {
        addrinfo hints;
        addrinfo *result;
        timeval tv;

        int max_data_size = 1000;

        //set addr info criteria for results to return
        memset(&hints, 0, sizeof(hints));
        hints.ai_family = AF_UNSPEC;
        hints.ai_socktype = SOCK_DGRAM;
        hints.ai_protocol = IPPROTO_UDP;

        //getting address info for streets, 0 indicates success
        int tsc_addr_info = getaddrinfo(ip_.c_str(), std::to_string(port_).c_str(), &hints, &result);
        if (tsc_addr_info != 0) {
            throw spat_worker_exception("Failed to get addr info for the tsc_service instance");
        }

        //creating socket
        int sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
        if (sock == -1) {
            throw spat_worker_exception("Failed to create socket");
        }

        //set socket options with timeout from manifest json
        tv.tv_sec = socket_timeout_;
        tv.tv_usec = 0;
        setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(struct timeval));

        //attempt to bind to socket
        if (bind(sock, result->ai_addr, result->ai_addrlen) == -1) {
            throw spat_worker_exception("Failed to bind to socket");
        }
        

        //reading data from socket
        while (true)
        {
            std::vector<char> spat_buf(max_data_size);
            ssize_t bytes_received = recv(sock, spat_buf.data(), spat_buf.size(), 0);

            if (bytes_received > 0)
            {
                spat_buf.resize(bytes_received);
                SPDLOG_DEBUG("Num bytes received: {0}", bytes_received);

                std::stringstream ss;
                ss << std::hex << std::setfill('0');
                for (int i = 0; i < bytes_received; i++)
                {
                    ss << std::setw(2) << static_cast<unsigned>(spat_buf[i]);
                }
                std::string mystr = ss.str();
                SPDLOG_DEBUG("Buffer contains: {0}", mystr);

            }
            else {
                throw spat_worker_exception("Timeout of " + std::to_string( socket_timeout_) + " seconds has elapsed. Closing SPaT Work UDP Socket" );
            }       
        }    
    }


}