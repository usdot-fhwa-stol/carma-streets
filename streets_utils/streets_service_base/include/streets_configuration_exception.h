#pragma once

#include <fstream>


class streets_configuration_exception : public std::runtime_error{
    public:
        ~streets_configuration_exception();
        streets_configuration_exception(const std::string msg );
};

