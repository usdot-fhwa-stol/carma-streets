#pragma once


namespace streets_utils::messages{
    
    enum class equipment_type {
        UNKNOWN = 0,
        RSU = 1,
        OBU= 2,
        VRU= 3        
    };
    /**
     * @brief Function to convert integers to equiment type. Default to UNKNOWN.
     * @param i integer value of enum.
     * @return corresponding equipement type.
     */
    inline equipment_type equipment_type_from_int( const unsigned int  i ) {
        switch (i)
        {
        case 0:
            return equipment_type::UNKNOWN;
            break;
        case 1:
            return equipment_type::RSU;
            break;
        case 2:
            return equipment_type::OBU;
            break;
        case 3:
            return equipment_type::VRU;
            break;
        default:
            return equipment_type::UNKNOWN;
        }
    };
}