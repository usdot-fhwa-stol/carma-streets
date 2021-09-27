#pragma once
#include <iostream>
#include <string>

#include "objBase.h"

namespace message_services
{
    namespace workers
    {
        class base_worker
        {
        private:
        public:
            base_worker(/* args */);
            ~base_worker();
            /***
            * @brief process incoming msg json string and create msg object.
              @param std::string json_string
            */
            virtual void process_incoming_msg(const std::string json_str) = 0;
        };
    }
}