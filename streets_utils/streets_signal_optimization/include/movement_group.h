#pragma once
#include <list>

namespace streets_signal_optimization
{
    struct movement_group
    {
        std::string name;

        std::pair<int,int> signal_groups;
    };

    struct movement_groups {
        std::list<movement_group> groups;
    };
} // namespace signal_opt_service