#ifndef HEADER_H
#define HEADER_H

#include <iostream>
namespace message_services
{
    namespace models
    {
        typedef struct header
        {
            long year = 0;
            long timestamp = 0;
            long sec_mark = 0;
        } header_t;
    }
}


#endif 