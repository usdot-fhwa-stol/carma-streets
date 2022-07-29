#include "spat_worker.h"

using namespace std;

namespace traffic_signal_controller_service
{

    spat_worker::spat_worker(const std::string& ip, const int& port, const int& socket_timeout ) :
                          socket_ip_(ip), socket_port_(port), socket_timeout_(socket_timeout)
    {
        SPDLOG_DEBUG("Creating Spat Worker");
        
    }
    bool spat_worker::initialize() {
        if ( !request_intersection_info() ) {
            return false;
        }
        upd_socket_listener = upd_socket_listener( socket_ip_, socket_port_, socket_timeout_);
        if (!upd_socket_listener.initialize()) {
            SPDLOG_ERROR("Failed to initialize udp socket!");
            return false;
        }


        spat_ptr = make_shared<signal_phase_and_timing::spat>();
        spat_ptr->initialize_intersection(intersection_name_, intersection_id_, phase_number_to_signal_group_ );
        return true;
    }

    void spat_worker::listen_udp_spat() 
    {
        std::vector<char> spat_buf(max_data_size);
        ssize_t bytes_received = recv(sock, spat_buf.data(), spat_buf.size(), 0);
        // see recv documentation https://man7.org/linux/man-pages/man2/recv.2.html#RETURN_VALUE
        if (bytes_received > 0)
        {
            spat_buf.resize(bytes_received);
            ntcip::ntcip_1202_ext ntcip_1202_data;
            std::memcpy(&ntcip_1202_data, spat_buf.data(), bytes_received);
            spat_ptr->update(ntcip_1202_data, false);

        }
        else if (bytes_received == -1){
            // see recv documentation https://man7.org/linux/man-pages/man2/recv.2.html#ERRORS
            if (EAGAIN == errno){
                throw spat_worker_exception("Timeout of "+ std::to_string(socket_timeout_) + " seconds has elapsed. Closing SPaT Work UDP Socket");
            } else {
                throw spat_worker_exception(strerror(errno));
            }
        }
        // Should be impossible since UDP is connectionless communication protocol
        else if (bytes_received == 0){
            throw spat_worker_exception("Connection terminated by server");
        }       
           
    }

    bool spat_worker::request_intersection_info() {
        bool successful = false;
        OpenAPI::OAIDefaultApi apiInstance;
        QEventLoop loop;
        connect(&apiInstance, &OpenAPI::OAIDefaultApi::getIntersectionInfoSignal, [this, &successful, &loop](OpenAPI::OAIIntersection_info int_info){                   
            intersection_name_ = int_info.getName().toStdString();
            intersection_id_ = int_info.getId();
            successful = true;
            loop.quit(); 
        });

        connect(&apiInstance, &OpenAPI::OAIDefaultApi::getIntersectionInfoSignalE, [this, &signal_group_ids_valid, &invalid_signal_group_count, &loop](OpenAPI::OAIIntersection_info, QNetworkReply::NetworkError, QString error_str){ 
            SPDLOG_ERROR("Error happened while issuing intersection model GET information request : {0}",  error_str.toStdString());
            loop.quit();
         });

        apiInstance.getIntersectionInfo();

        QTimer::singleShot(5000, &loop, &QEventLoop::quit);
        loop.exec();
        return successful;
    }


}