#ifndef MOBILITYPATH_H
#define MOBILITYPATH_H

#include <spdlog/spdlog.h>
#include <iomanip>

#include "baseMessage.h"
#include "mobilityHeader.h"
#include "trajectory.h"

namespace message_services
{

    namespace models
    {

        class mobilitypath : public baseMessage
        {
            friend std::ostream &operator<<(std::ostream &out, mobilitypath &mobilitypath_obj);

        private:
            mobility_header_t header;
            trajectory_t trajectory;

        public:
            mobilitypath(/* args */);
            virtual ~mobilitypath();

            //Current timestamp in unit of milliseconds
            std::time_t msg_received_timestamp_ = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();

            virtual void fromJsonObject(const rapidjson::Value &obj);
            virtual bool asJsonObject(rapidjson::Writer<rapidjson::StringBuffer> *writer) const;
            std::string generate_hash_sender_timestamp_id( std::string sender_bsm_id, uint64_t timestamp );

            trajectory_t getTrajectory() const;
            void setTrajectory(trajectory_t trajectory);

            mobility_header_t getHeader() const;
            void setHeader(mobility_header_t header);
        };
    }

}

#endif