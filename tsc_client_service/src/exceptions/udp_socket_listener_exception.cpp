#include "udp_socket_listener_exception.h"

namespace traffic_signal_controller_service {

    udp_socket_listener_exception::udp_socket_listener_exception(const std::string &msg): std::runtime_error(msg){};

    udp_socket_listener_exception::~udp_socket_listener_exception() = default;
}