#ifndef BSM_H
#define BSM_H

#include <spdlog/spdlog.h>
#include <boost/algorithm/string.hpp>
#include <iomanip>
#include <ctime>
#include "baseMessage.h"
#include "bsmCoreData.h"

namespace message_services
{
    namespace models
    {
        class bsm : public baseMessage
        {
            friend std::ostream &operator<<(std::ostream &out, bsm &bsm_obj);

        private:
            bsmCoreData_t core_data;

        public:
            //constructors
            bsm();

            //desctructor
            virtual ~bsm();
            void setCore_data(bsmCoreData_t core_data);
            bsmCoreData_t getCore_data() const;
            std::string generate_hash_bsm_msg_id( std::string temprary_id, long msg_count);

            //json string object converter with rapidjson
            virtual void fromJsonObject(const rapidjson::Value &obj);
            virtual bool asJsonObject(rapidjson::Writer<rapidjson::StringBuffer> *writer) const;
        };
    }

}

#endif