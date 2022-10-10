#pragma once
#include <list>

namespace signal_opt_service
{   
    /**
     * @brief Movement group consists of up to two signal group ids which can concurrently be
     * given green in a NEMA 8 Phase Dual Ring Traffic Signal Controller setup. Each single group
     * will be from a different ring but both are inside the same barrier. A signal group pair with 
     * an entry of 0 indicates the movement group only consists of a single signal group (the non-zero 
     * value entry). 
     * 
     */
    struct movement_group
    {
        /**
         * @brief Optional parameter to identify movement group
         * 
         */
        std::string name;
        /**
         * @brief Up to two signal group ids from different rings but a common barrier in a Dual Ring 8 Phase TSC setup.
         * Entries where one item in the pair is 0 will be treated as movement groups with only a single single group. Entries
         * where both items in the pair are zero are invalid.
         * 
         */
        std::pair<int,int> signal_groups;
    };
    /**
     * @brief This struct is a list of movement groups. All the elements in this list should be unique. This
     * means that each signal group pair should be unique in this list. This is NOT enforced by the struct and 
     * should be ensured when populating the list.
     * 
     */
    struct movement_groups {
        std::list<movement_group> groups;
    };
} // namespace signal_opt_service


