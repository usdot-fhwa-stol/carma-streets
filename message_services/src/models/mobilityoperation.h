
#ifndef MOBILITYOPERATION_H
#define MOBILITYOPERATION_H

#include <spdlog/spdlog.h>
#include <iomanip>

#include "baseMessage.h"
#include "mobilityHeader.h"
namespace message_services
{

    namespace models
    {

        class mobilityoperation : public baseMessage
        {
            friend std::ostream &operator<<(std::ostream &out, mobilityoperation &mobilityoperation_obj);

        private:
            mobility_header_t header;
            std::string strategy = "";
            std::string strategy_params = "";

        public:
            mobilityoperation(/* args */);
            virtual ~mobilityoperation();

            //Current timestamp in unit of milliseconds
            std::time_t msg_received_timestamp_ = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();

            virtual void fromJsonObject(const rapidjson::Value &obj);
            virtual bool asJsonObject(rapidjson::Writer<rapidjson::StringBuffer> *writer) const;

            std::string get_value_from_strategy_params(std::string key) const;

            std::string generate_hash_bsm_msg_id( std::string temprary_id, long msg_count, long sec_mark);

            std::string generate_hash_sender_timestamp_id( std::string sender_bsm_id, uint64_t timestamp );

            std::string getStrategy_params() const;
            void setStrategy_params(std::string strategy_params);
            std::string getStrategy() const;
            void setStrategy(std::string strategy);

            mobility_header_t getHeader() const;
            void setHeader(mobility_header_t header);
        };
    }

}

#endif