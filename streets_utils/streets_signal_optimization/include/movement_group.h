#pragma once
#include <list>

namespace streets_signal_optimization
{   
    /**
     * @brief A movement group is a combination of signal groups with non-conflicting directions.  The string name for movement_group 
     * is optional. The first entry in signal_groups pair will always be populated while the second will be 0 if the movement group 
     * only consists of a single signal group.
    */
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
         * @brief List of movement groups. Each movement group is expected to be unique combination. Also, no movement group can 
         * be a subset of another movement group.
        */
        std::list<movement_group> groups;
    };
}