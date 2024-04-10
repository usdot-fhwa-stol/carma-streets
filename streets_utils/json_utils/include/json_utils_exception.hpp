#pragma once

#include <stdexcept>



namespace streets_utils::json_utils {
    /**
     * @brief Runtime exception related to json_utils functions. Thrown when :
     *  - Passing Invalid JSON
     *  - Missing Required member
     *  - Passing rapidjson::Value that is not an object into parse functions
     * 
     * @author Paul Bourelly 
     */ 
    class json_parse_exception : public std::runtime_error{
        using std::runtime_error::runtime_error;
    };
}