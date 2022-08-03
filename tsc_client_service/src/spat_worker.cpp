#include "spat_worker.h"

using namespace std;

namespace traffic_signal_controller_service
{

    spat_worker::spat_worker(const std::string& ip, const int& port, const int& socket_timeout, const bool use_msg_timestamp ) :
                          socket_ip_(ip), socket_port_(port), socket_timeout_(socket_timeout), _use_msg_timestamp()
    {
        SPDLOG_DEBUG("Creating Spat Worker");
        
    }
    bool spat_worker::initialize() {
        spat_listener = std::make_unique<udp_socket_listener>( socket_ip_, socket_port_, socket_timeout_);
        if (!spat_listener->initialize()) {
            SPDLOG_ERROR("Failed to initialize udp socket!");
            return false;
        }
        return true;
    }

    void spat_worker::receive_spat(const shared_ptr<signal_phase_and_timing::spat> _spat_ptr) const
    {
        std::vector<char> spat_buf = spat_listener->receive();
        ntcip::ntcip_1202_ext ntcip_1202_data;
        std::memcpy(&ntcip_1202_data, spat_buf.data(), spat_buf.size());
        _spat_ptr->update(ntcip_1202_data, _use_msg_timestamp);

           
    }

}