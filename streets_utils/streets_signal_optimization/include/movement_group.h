#pragma once
#include <list>

namespace streets_signal_optimization
{   
    struct movement_group
    {
        std::string name;
        /**
         * @brief List of signal groups. Each movement group contains a set of signal groups with non-conflicting directions.
        */
        std::pair<int,int> signal_groups;
    };

    
    struct movement_groups {
        /**
         * @brief List of movement groups
        */
        std::list<movement_group> groups;
    };
}