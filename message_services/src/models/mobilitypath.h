#pragma once
#include <spdlog/spdlog.h>
#include <iomanip>

#include "objBase.h"
#include "mobilityHeader.h"
#include "trajectory.h"

namespace message_services
{

    namespace models
    {

        class mobilitypath : public objBase
        {
            friend std::ostream &operator<<(std::ostream &out, mobilitypath &mobilitypath_obj);

        private:
            mobility_header_t header;
            trajectory_t trajectory;

        public:
            mobilitypath(/* args */);
            virtual ~mobilitypath();

            virtual void fromJsonObject(const rapidjson::Value &obj);
            virtual bool asJsonObject(rapidjson::Writer<rapidjson::StringBuffer> *writer) const;

            trajectory_t getTrajectory() const;
            void setTrajectory(trajectory_t trajectory);

            mobility_header_t getHeader() const;
            void setHeader(mobility_header_t header);
        };
    }

}