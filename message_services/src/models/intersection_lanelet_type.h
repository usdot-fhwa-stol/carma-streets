
#ifndef INTERSECTION_LANELET_TYPE
#define INTERSECTION_LANELET_TYPE

#include <iostream>

namespace message_services
{
    namespace models
    {
        enum intersection_lanelet_type
        {
            unknown,    //A lanelet that is not relevant to any intersections (TSMO use case #1 use all_way_stop regulatory element to identify an intersection).
            entry,      //The lanelet where a vehicle enter the an intersection box
            link,       //The lanelet where a vehicle is inside an intersection box            
            departure   //The lanelet where a vehicle exit the an intersection box
        };
    }
}
#endif