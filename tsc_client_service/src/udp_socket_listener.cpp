#include "udp_socket_listener.h"

namespace traffic_signal_controller_service
{
    udp_socket_listener::udp_socket_listener(const std::string& ip, const int port, const int socket_timeout) 
                                                : ip_(ip), port_(port), socket_timeout_(socket_timeout) {

    }

    bool udp_socket_listener::initialize() {
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
            SPDLOG_ERROR("Failed to get addr info for the tsc_service instance");
            return false;
        }

        //creating socket
        sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
        if (sock == -1) {
            SPDLOG_ERROR("Failed to create socket");
            return false;
        }

        //set socket options with timeout from manifest json
        tv.tv_sec = socket_timeout_;
        tv.tv_usec = 0;
        setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(struct timeval));

        //attempt to bind to socket
        if (bind(sock, result->ai_addr, result->ai_addrlen) == -1) {
            SPDLOG_ERROR("Failed to bind to socket");
            return false;
        }
        socket_created_ = true;
        return true;
    }


    std::vector<char> udp_socket_listener::receive()  {
        std::vector<char> spat_buf(1024);
        if (socket_created_) {
            std::vector<char> spat_buf(1024);
            ssize_t bytes_received = recv(sock, spat_buf.data(), spat_buf.size(), 0);
            // see recv documentation https://man7.org/linux/man-pages/man2/recv.2.html#RETURN_VALUE
            if (bytes_received > 0)
            {
                spat_buf.resize(bytes_received);
            }
            else if (bytes_received == -1){
                // see recv documentation https://man7.org/linux/man-pages/man2/recv.2.html#ERRORS
                if (EAGAIN == errno){
                    throw udp_socket_listener_exception("Timeout of "+ std::to_string(socket_timeout_) + " seconds has elapsed. Closing SPaT Work UDP Socket");
                } else {
                    throw udp_socket_listener_exception(strerror(errno));
                }
            }
            // Should be impossible since UDP is connectionless communication protocol
            else if (bytes_received == 0){
                throw udp_socket_listener_exception("Connection terminated by server");
            }    
        }
        return spat_buf;   
    }
    
} // namespace traffic_signal_controller_service
