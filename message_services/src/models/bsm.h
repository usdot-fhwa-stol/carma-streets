#ifndef BSM_H
#define BSM_H

#include <spdlog/spdlog.h>
#include <boost/algorithm/string.hpp>
#include <iomanip>
#include <ctime>
#include "baseMessage.h"
#include "header.h"
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
            header_t header;

        public:
            //constructors
            bsm();

            //desctructor
            virtual ~bsm();
            header_t getHeader() const;
            void setHeader(header_t header);
            void setCore_data(bsmCoreData_t core_data);
            bsmCoreData_t getCore_data() const;

            //json string object converter with rapidjson
            virtual void fromJsonObject(const rapidjson::Value &obj);
            virtual bool asJsonObject(rapidjson::Writer<rapidjson::StringBuffer> *writer) const;
        };
    }

}

#endif